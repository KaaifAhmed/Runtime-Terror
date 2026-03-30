#include "leaderboard.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>

// ============================================
// LEADERBOARD LINKED LIST IMPLEMENTATION
// ============================================
LeaderboardLinkedList::LeaderboardLinkedList(int max) 
    : head(nullptr), count(0), maxSize(max) {}

LeaderboardLinkedList::~LeaderboardLinkedList() {
    Clear();
}

void LeaderboardLinkedList::Insert(RunDataNode* newNode) {
    if (!newNode) return;
    
    // Empty list case
    if (!head) {
        head = newNode;
        count++;
        UpdateRanks();
        return;
    }
    
    // Insert at head (highest score)
    if (newNode->score > head->score) {
        newNode->next = head;
        head = newNode;
        count++;
        UpdateRanks();
        
        // Remove excess if over max
        if (count > maxSize) {
            RemoveLast();
        }
        return;
    }
    
    // Find insertion point (descending order)
    RunDataNode* current = head;
    while (current->next && current->next->score >= newNode->score) {
        current = current->next;
    }
    
    // Insert after current
    newNode->next = current->next;
    current->next = newNode;
    count++;
    UpdateRanks();
    
    // Remove excess if over max
    if (count > maxSize) {
        RemoveLast();
    }
}

void LeaderboardLinkedList::InsertionSort() {
    if (!head || !head->next) return;
    
    RunDataNode* sorted = nullptr;
    RunDataNode* current = head;
    
    while (current) {
        RunDataNode* next = current->next;
        
        // Insert into sorted list
        if (!sorted || sorted->score < current->score) {
            current->next = sorted;
            sorted = current;
        } else {
            RunDataNode* search = sorted;
            while (search->next && search->next->score >= current->score) {
                search = search->next;
            }
            current->next = search->next;
            search->next = current;
        }
        
        current = next;
    }
    
    head = sorted;
    UpdateRanks();
}

void LeaderboardLinkedList::AddRun(const std::string& name, int score, int lines, int time, const std::string& date) {
    // Only add if it's a high score or list isn't full
    if (count < maxSize || score > GetAt(maxSize - 1)->score) {
        RunDataNode* newNode = new RunDataNode(name, score, lines, time, date);
        Insert(newNode);
    }
}

void LeaderboardLinkedList::RemoveLast() {
    if (!head) return;
    
    if (!head->next) {
        delete head;
        head = nullptr;
        count--;
        return;
    }
    
    RunDataNode* current = head;
    while (current->next && current->next->next) {
        current = current->next;
    }
    
    delete current->next;
    current->next = nullptr;
    count--;
}

void LeaderboardLinkedList::Clear() {
    RunDataNode* current = head;
    while (current) {
        RunDataNode* next = current->next;
        delete current;
        current = next;
    }
    head = nullptr;
    count = 0;
}

RunDataNode* LeaderboardLinkedList::GetAt(int index) const {
    if (index < 0 || index >= count) return nullptr;
    
    RunDataNode* current = head;
    for (int i = 0; i < index && current; i++) {
        current = current->next;
    }
    return current;
}

void LeaderboardLinkedList::UpdateRanks() {
    RunDataNode* current = head;
    int rank = 1;
    while (current) {
        current->rank = rank++;
        current = current->next;
    }
}

bool LeaderboardLinkedList::ShouldAddScore(int score) const {
    if (count < maxSize) return true;
    
    RunDataNode* last = GetAt(maxSize - 1);
    return last && score > last->score;
}

// ============================================
// FILE I/O IMPLEMENTATION
// ============================================
LeaderboardFileIO::LeaderboardFileIO(const std::string& fname) : filename(fname) {}

bool LeaderboardFileIO::SaveToFile(const LeaderboardLinkedList& list) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) return false;
    
    RunDataNode* current = list.GetHead();
    int count = list.GetCount();
    
    // Write count
    file.write(reinterpret_cast<const char*>(&count), sizeof(count));
    
    // Write each node
    while (current) {
        size_t nameLen = current->playerName.length();
        file.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
        file.write(current->playerName.c_str(), nameLen);
        
        file.write(reinterpret_cast<const char*>(&current->score), sizeof(current->score));
        file.write(reinterpret_cast<const char*>(&current->linesCompiled), sizeof(current->linesCompiled));
        file.write(reinterpret_cast<const char*>(&current->timeSurvived), sizeof(current->timeSurvived));
        
        size_t dateLen = current->date.length();
        file.write(reinterpret_cast<const char*>(&dateLen), sizeof(dateLen));
        file.write(current->date.c_str(), dateLen);
        
        current = current->next;
    }
    
    file.close();
    return true;
}

bool LeaderboardFileIO::LoadFromFile(LeaderboardLinkedList& list) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) return false;
    
    list.Clear();
    
    int count;
    file.read(reinterpret_cast<char*>(&count), sizeof(count));
    
    for (int i = 0; i < count; i++) {
        size_t nameLen;
        file.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        std::string name(nameLen, '\0');
        file.read(&name[0], nameLen);
        
        int score, lines, time;
        file.read(reinterpret_cast<char*>(&score), sizeof(score));
        file.read(reinterpret_cast<char*>(&lines), sizeof(lines));
        file.read(reinterpret_cast<char*>(&time), sizeof(time));
        
        size_t dateLen;
        file.read(reinterpret_cast<char*>(&dateLen), sizeof(dateLen));
        std::string date(dateLen, '\0');
        file.read(&date[0], dateLen);
        
        list.AddRun(name, score, lines, time, date);
    }
    
    file.close();
    return true;
}

bool LeaderboardFileIO::ExportToCSV(const LeaderboardLinkedList& list, const std::string& csvFilename) {
    std::ofstream file(csvFilename);
    if (!file.is_open()) return false;
    
    // Header
    file << "Rank,Player Name,Score,Lines Compiled,Time Survived (s),Date\n";
    
    // Data
    RunDataNode* current = list.GetHead();
    while (current) {
        file << current->rank << ","
             << current->playerName << ","
             << current->score << ","
             << current->linesCompiled << ","
             << current->timeSurvived << ","
             << current->date << "\n";
        current = current->next;
    }
    
    file.close();
    return true;
}

bool LeaderboardFileIO::FileExists() const {
    std::ifstream file(filename);
    return file.good();
}

bool LeaderboardFileIO::DeleteFile() {
    return std::remove(filename.c_str()) == 0;
}

std::string LeaderboardFileIO::GetCurrentDateString() const {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    
    std::stringstream ss;
    ss << 1900 + ltm->tm_year << "-"
       << std::setw(2) << std::setfill('0') << 1 + ltm->tm_mon << "-"
       << std::setw(2) << std::setfill('0') << ltm->tm_mday;
    return ss.str();
}

// ============================================
// LEADERBOARD MANAGER (SINGLETON)
// ============================================
LeaderboardManager* LeaderboardManager::instance = nullptr;

LeaderboardManager::LeaderboardManager() : leaderboard(nullptr), fileIO(nullptr) {}

LeaderboardManager* LeaderboardManager::GetInstance() {
    if (!instance) {
        instance = new LeaderboardManager();
    }
    return instance;
}

LeaderboardManager::~LeaderboardManager() {
    Shutdown();
}

void LeaderboardManager::Initialize() {
    leaderboard = new LeaderboardLinkedList(10);
    fileIO = new LeaderboardFileIO("leaderboard.dat");
    LoadLeaderboard();
}

void LeaderboardManager::Shutdown() {
    if (fileIO && leaderboard) {
        SaveLeaderboard();
    }
    delete leaderboard;
    delete fileIO;
    leaderboard = nullptr;
    fileIO = nullptr;
}

void LeaderboardManager::SubmitRun(const std::string& playerName, int score, int linesCompiled, int timeSurvived) {
    if (!leaderboard || !fileIO) return;
    
    std::string date = fileIO->GetCurrentDateString();
    leaderboard->AddRun(playerName, score, linesCompiled, timeSurvived, date);
    SaveLeaderboard();
}

bool LeaderboardManager::IsHighScore(int score) const {
    if (!leaderboard) return true; // If no leaderboard, any score is high
    return leaderboard->ShouldAddScore(score);
}

RunDataNode* LeaderboardManager::GetTopRuns(int count) const {
    if (!leaderboard) return nullptr;
    return leaderboard->GetHead();
}

void LeaderboardManager::SaveLeaderboard() {
    if (fileIO && leaderboard) {
        fileIO->SaveToFile(*leaderboard);
    }
}

void LeaderboardManager::LoadLeaderboard() {
    if (fileIO && leaderboard) {
        if (!fileIO->LoadFromFile(*leaderboard)) {
            // File doesn't exist or error - that's okay, start fresh
        }
    }
}

void LeaderboardManager::ResetLeaderboard() {
    if (leaderboard) {
        leaderboard->Clear();
    }
    if (fileIO) {
        fileIO->DeleteFile();
    }
}
