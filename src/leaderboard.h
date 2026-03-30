#pragma once
#include <string>
#include <ctime>

// ============================================
// RUN DATA NODE (Linked List Node)
// ============================================
struct RunDataNode {
    std::string playerName;
    int score;
    int linesCompiled;
    int timeSurvived;
    std::string date;
    int rank;
    
    RunDataNode* next;
    
    RunDataNode(const std::string& name, int scr, int lines, int time, const std::string& dt)
        : playerName(name), score(scr), linesCompiled(lines), timeSurvived(time), 
          date(dt), rank(0), next(nullptr) {}
};

// ============================================
// LINKED LIST FOR LEADERBOARD
// ============================================
class LeaderboardLinkedList {
private:
    RunDataNode* head;
    int count;
    int maxSize;
    
public:
    LeaderboardLinkedList(int max = 10);
    ~LeaderboardLinkedList();
    
    // Core operations
    void Insert(RunDataNode* newNode);           // Insert at appropriate position (sorted)
    void InsertionSort();                        // Sort using Insertion Sort
    void AddRun(const std::string& name, int score, int lines, int time, const std::string& date);
    void RemoveLast();                           // Remove last node (lowest score)
    void Clear();
    
    // Getters
    RunDataNode* GetHead() const { return head; }
    int GetCount() const { return count; }
    bool IsEmpty() const { return head == nullptr; }
    bool IsFull() const { return count >= maxSize; }
    
    // Access
    RunDataNode* GetAt(int index) const;
    RunDataNode* GetBestRun() const { return head; }
    
    // Utility
    void UpdateRanks();                        // Update rank numbers after sorting
    bool ShouldAddScore(int score) const;        // Check if score qualifies for top 10
};

// ============================================
// FILE I/O FOR LEADERBOARD
// ============================================
class LeaderboardFileIO {
private:
    std::string filename;
    
public:
    LeaderboardFileIO(const std::string& fname = "leaderboard.dat");
    
    // Save/Load
    bool SaveToFile(const LeaderboardLinkedList& list);
    bool LoadFromFile(LeaderboardLinkedList& list);
    bool ExportToCSV(const LeaderboardLinkedList& list, const std::string& csvFilename);
    
    // Utility
    bool FileExists() const;
    bool DeleteFile();
    std::string GetCurrentDateString() const;
};

// ============================================
// LEADERBOARD MANAGER (Singleton pattern)
// ============================================
class LeaderboardManager {
private:
    static LeaderboardManager* instance;
    LeaderboardLinkedList* leaderboard;
    LeaderboardFileIO* fileIO;
    
    LeaderboardManager();
    
public:
    static LeaderboardManager* GetInstance();
    ~LeaderboardManager();
    
    // Core operations
    void Initialize();
    void Shutdown();
    void SubmitRun(const std::string& playerName, int score, int linesCompiled, int timeSurvived);
    bool IsHighScore(int score) const;
    
    // Getters
    LeaderboardLinkedList* GetLeaderboard() const { return leaderboard; }
    RunDataNode* GetTopRuns(int count = 10) const;
    
    // File operations
    void SaveLeaderboard();
    void LoadLeaderboard();
    void ResetLeaderboard();
};
