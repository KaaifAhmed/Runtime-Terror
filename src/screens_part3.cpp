// Continuing screens.cpp - Leaderboard and Credits implementations
#include "screens.h"
#include <cstdlib>
#include <cstring>

// ============================================
// LEADERBOARD SCREEN IMPLEMENTATION
// ============================================
LeaderboardScreen::LeaderboardScreen() 
    : scrollY(0), selectedIndex(-1), leaderboard(nullptr), showEmptyMessage(true) {}

LeaderboardScreen::~LeaderboardScreen() {}

void LeaderboardScreen::SetLeaderboard(LeaderboardLinkedList* lb) {
    leaderboard = lb;
    showEmptyMessage = (lb == nullptr || lb->IsEmpty());
}

void LeaderboardScreen::OnEnter() {
    scrollY = 0;
    selectedIndex = -1;
    if (leaderboard) {
        showEmptyMessage = leaderboard->IsEmpty();
    }
}

void LeaderboardScreen::OnExit() {}

void LeaderboardScreen::Update() {
    // Keyboard navigation
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        if (leaderboard && !leaderboard->IsEmpty()) {
            int count = leaderboard->GetCount();
            selectedIndex = (selectedIndex + 1) % count;
        }
    }
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        if (leaderboard && !leaderboard->IsEmpty()) {
            int count = leaderboard->GetCount();
            selectedIndex = (selectedIndex - 1 + count) % count;
        }
    }
    
    // ESC or Backspace to go back
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) {
        if (onBack) onBack();
    }
    
    // R to reset
    if (IsKeyPressed(KEY_R)) {
        if (onReset) onReset();
    }
    
    // E to export
    if (IsKeyPressed(KEY_E)) {
        if (onExport) onExport();
    }
    
    // Mouse scrolling
    float scroll = GetMouseWheelMove();
    scrollY += scroll * 30;
    if (scrollY > 0) scrollY = 0;
    
    // Limit scroll to content
    if (leaderboard) {
        int contentHeight = leaderboard->GetCount() * 60 + 200;
        int maxScroll = contentHeight - GetScreenHeight() + 100;
        if (maxScroll > 0 && scrollY < -maxScroll) scrollY = -maxScroll;
    }
}

void LeaderboardScreen::Draw() {
    // VS Code IDE background
    UI::DrawVSCideBackground();
    
    // Draw header
    DrawHeader();
    
    // Draw leaderboard content
    if (showEmptyMessage || !leaderboard || leaderboard->IsEmpty()) {
        DrawEmptyState();
    } else {
        DrawLeaderboardList();
    }
    
    // Draw footer
    DrawFooter();
}

void LeaderboardScreen::DrawHeader() {
    // Top bar with title
    DrawRectangle(0, 0, GetScreenWidth(), 60, VSCodeTheme::BG_PANEL);
    DrawLine(0, 60, GetScreenWidth(), 60, VSCodeTheme::BORDER);
    
    // Title
    const char* title = "LEADERBOARD - Top Runs";
    int titleW = MeasureText(title, 32);
    DrawText(title, (GetScreenWidth() - titleW) / 2, 15, 32, VSCodeTheme::ACCENT_CYAN);
    
    // Back button hint
    DrawText("ESC: Back", 20, 20, 16, VSCodeTheme::TEXT_MUTED);
    
    // Stats
    if (leaderboard) {
        const char* stats = TextFormat("Total runs: %d", leaderboard->GetCount());
        int statsW = MeasureText(stats, 16);
        DrawText(stats, GetScreenWidth() - statsW - 20, 20, 16, VSCodeTheme::TEXT_MUTED);
    }
}

void LeaderboardScreen::DrawLeaderboardList() {
    if (!leaderboard) return;
    
    int startY = 100 + (int)scrollY;
    int entryHeight = 60;
    int visibleCount = 0;
    
    // Table header
    int tableX = 100;
    int colRank = tableX;
    int colName = tableX + 80;
    int colScore = tableX + 400;
    int colLines = tableX + 550;
    int colTime = tableX + 700;
    int colDate = tableX + 850;
    
    // Header background
    DrawRectangle(50, 80, GetScreenWidth() - 100, 50, VSCodeTheme::BG_PANEL);
    DrawLine(50, 130, GetScreenWidth() - 50, 130, VSCodeTheme::BORDER);
    
    DrawText("RANK", colRank + 10, 95, 18, VSCodeTheme::ACCENT_BLUE);
    DrawText("PLAYER", colName, 95, 18, VSCodeTheme::ACCENT_BLUE);
    DrawText("SCORE", colScore, 95, 18, VSCodeTheme::ACCENT_BLUE);
    DrawText("LINES", colLines, 95, 18, VSCodeTheme::ACCENT_BLUE);
    DrawText("TIME", colTime, 95, 18, VSCodeTheme::ACCENT_BLUE);
    DrawText("DATE", colDate, 95, 18, VSCodeTheme::ACCENT_BLUE);
    
    // Draw entries
    RunDataNode* current = leaderboard->GetHead();
    int index = 0;
    
    while (current && visibleCount < 15) {
        int y = startY + index * entryHeight;
        
        // Skip if off screen
        if (y < 130 || y > GetScreenHeight() - 80) {
            current = current->next;
            index++;
            continue;
        }
        
        DrawRankEntry(current->rank, current->playerName, current->score, y);
        
        // Additional data
        Color dataColor = (index == selectedIndex) ? VSCodeTheme::TEXT_NORMAL : VSCodeTheme::TEXT_MUTED;
        DrawText(TextFormat("%d", current->linesCompiled), colLines, y + 15, 16, dataColor);
        DrawText(TextFormat("%ds", current->timeSurvived), colTime, y + 15, 16, dataColor);
        DrawText(current->date.c_str(), colDate, y + 15, 14, dataColor);
        
        current = current->next;
        index++;
        visibleCount++;
    }
}

void LeaderboardScreen::DrawRankEntry(int rank, const std::string& name, int score, int y) {
    bool isSelected = (rank - 1 == selectedIndex);
    
    // Background
    Color bg = isSelected ? VSCodeTheme::BG_HOVER : VSCodeTheme::BG_WIDGET;
    if (rank == 1) bg = {255, 215, 0, 40};      // Gold tint for 1st
    else if (rank == 2) bg = {192, 192, 192, 40}; // Silver tint for 2nd
    else if (rank == 3) bg = {205, 127, 50, 40};  // Bronze tint for 3rd
    
    int entryHeight = 60;
    DrawRectangle(50, y, GetScreenWidth() - 100, entryHeight, bg);
    
    // Border for selected
    if (isSelected) {
        DrawRectangleLinesEx({50, (float)y, (float)(GetScreenWidth() - 100), (float)entryHeight}, 
                             2, VSCodeTheme::ACCENT_BLUE);
    }
    
    // Rank with medal colors
    Color rankColor = VSCodeTheme::TEXT_NORMAL;
    const char* rankPrefix = "";
    if (rank == 1) { rankColor = {255, 215, 0, 255}; rankPrefix = "1st"; }
    else if (rank == 2) { rankColor = {192, 192, 192, 255}; rankPrefix = "2nd"; }
    else if (rank == 3) { rankColor = {205, 127, 50, 255}; rankPrefix = "3rd"; }
    else rankPrefix = TextFormat("%dth", rank);
    
    DrawText(rankPrefix, 110, y + 15, 20, rankColor);
    
    // Name
    Color nameColor = isSelected ? VSCodeTheme::ACCENT_CYAN : VSCodeTheme::TEXT_NORMAL;
    DrawText(name.c_str(), 180, y + 15, 20, nameColor);
    
    // Score
    Color scoreColor = VSCodeTheme::ACCENT_GREEN;
    if (rank <= 3) scoreColor = rankColor;
    DrawText(TextFormat("%d", score), 500, y + 15, 24, scoreColor);
}

void LeaderboardScreen::DrawEmptyState() {
    const char* message = "No runs recorded yet!";
    const char* subMessage = "Play the game to see your scores here.";
    
    int msgW = MeasureText(message, 28);
    int subW = MeasureText(subMessage, 20);
    
    int centerY = GetScreenHeight() / 2;
    
    DrawText(message, (GetScreenWidth() - msgW) / 2, centerY - 30, 28, VSCodeTheme::TEXT_MUTED);
    DrawText(subMessage, (GetScreenWidth() - subW) / 2, centerY + 20, 20, VSCodeTheme::TEXT_MUTED);
}

void LeaderboardScreen::DrawFooter() {
    int footerY = GetScreenHeight() - 40;
    
    // Bottom bar
    DrawRectangle(0, footerY, GetScreenWidth(), 40, VSCodeTheme::BG_PANEL);
    DrawLine(0, footerY, GetScreenWidth(), footerY, VSCodeTheme::BORDER);
    
    // Controls
    DrawText("ESC: Back    R: Reset    E: Export CSV", 20, footerY + 10, 16, VSCodeTheme::TEXT_MUTED);
    
    // Mouse hint
    const char* hint = "Scroll to view more";
    int hintW = MeasureText(hint, 16);
    DrawText(hint, GetScreenWidth() - hintW - 20, footerY + 10, 16, VSCodeTheme::TEXT_MUTED);
}

// ============================================
// CREDITS SCREEN IMPLEMENTATION
// ============================================
CreditsScreen::CreditsScreen() : scrollY(GetScreenHeight()), scrollSpeed(1.5f) {
    credits = {
        "RUNTIME TERROR",
        "",
        "A Programming Adventure",
        "",
        "=== DEVELOPMENT ===",
        "Code: Runtime Terror Team",
        "Engine: Raylib",
        "Theme: VS Code Dark+",
        "",
        "=== GAME DESIGN ===",
        "Infinite Runner Concept",
        "Rewind Mechanic",
        "Circular Buffer System",
        "",
        "=== DATA STRUCTURES ===",
        "Circular Buffer (Rewind)",
        "Linked List (Leaderboard)",
        "Insertion Sort",
        "",
        "=== SPECIAL THANKS ===",
        "Raylib by Ramon Santamaria",
        "VS Code by Microsoft",
        "",
        "Thank you for playing!",
        "",
        "Press ESC to return"
    };
}

CreditsScreen::~CreditsScreen() {}

void CreditsScreen::OnEnter() {
    scrollY = (float)GetScreenHeight();
}

void CreditsScreen::OnExit() {}

void CreditsScreen::Update() {
    // Auto scroll
    scrollY -= scrollSpeed;
    
    // Reset if scrolled past
    int totalHeight = credits.size() * 40;
    if (scrollY < -totalHeight) {
        scrollY = (float)GetScreenHeight();
    }
    
    // Speed up with space
    if (IsKeyDown(KEY_SPACE)) {
        scrollY -= scrollSpeed * 3;
    }
    
    // Back
    if (IsKeyPressed(KEY_ESCAPE)) {
        if (onBack) onBack();
    }
}

void CreditsScreen::Draw() {
    // Dark background
    ClearBackground(VSCodeTheme::BG_DARK);
    
    // VS Code-style activity bar on left
    DrawRectangle(0, 0, 48, GetScreenHeight(), VSCodeTheme::BG_SIDEBAR);
    
    // Stars in background
    for (int i = 0; i < 50; i++) {
        int x = (i * 137) % GetScreenWidth();
        int y = (i * 89 + (int)(GetTime() * 10)) % GetScreenHeight();
        DrawPixel(x, y, {255, 255, 255, (unsigned char)(50 + (i % 100))});
    }
    
    // Draw scrolling credits
    DrawScrollingCredits();
    
    // Gradient overlays
    DrawRectangle(0, 0, GetScreenWidth(), 100, 
                  {VSCodeTheme::BG_DARK.r, VSCodeTheme::BG_DARK.g, VSCodeTheme::BG_DARK.b, 200});
    DrawRectangle(0, GetScreenHeight() - 100, GetScreenWidth(), 100,
                  {VSCodeTheme::BG_DARK.r, VSCodeTheme::BG_DARK.g, VSCodeTheme::BG_DARK.b, 200});
}

void CreditsScreen::DrawScrollingCredits() {
    int centerX = GetScreenWidth() / 2;
    int startY = (int)scrollY;
    
    for (size_t i = 0; i < credits.size(); i++) {
        int y = startY + i * 50;
        
        // Skip if off screen
        if (y < -50 || y > GetScreenHeight() + 50) continue;
        
        const std::string& line = credits[i];
        
        // Style based on content
        int fontSize = 24;
        Color color = VSCodeTheme::TEXT_NORMAL;
        
        if (line.empty()) continue;
        
        if (line.find("RUNTIME TERROR") != std::string::npos) {
            fontSize = 48;
            color = VSCodeTheme::ACCENT_RED;
        } else if (line.find("===") != std::string::npos) {
            fontSize = 20;
            color = VSCodeTheme::ACCENT_BLUE;
        } else if (line.find("") == 0 && line.length() == 0) {
            continue;
        } else if (line.find("Thank you") != std::string::npos) {
            fontSize = 28;
            color = VSCodeTheme::ACCENT_GREEN;
        } else if (line.find("Press ESC") != std::string::npos) {
            fontSize = 18;
            color = VSCodeTheme::TEXT_MUTED;
        }
        
        int textW = MeasureText(line.c_str(), fontSize);
        DrawText(line.c_str(), centerX - textW / 2, y, fontSize, color);
    }
}

// ============================================
// UI SCREEN MANAGER IMPLEMENTATION
// ============================================
UIScreenManager::UIScreenManager() 
    : currentScreen(ScreenType::NONE), previousScreen(ScreenType::NONE),
      mainMenu(nullptr), pauseScreen(nullptr), gameOverScreen(nullptr),
      leaderboardScreen(nullptr), creditsScreen(nullptr), leaderboard(nullptr) {}

UIScreenManager::~UIScreenManager() {
    Shutdown();
}

void UIScreenManager::Initialize() {
    mainMenu = new MainMenuScreen();
    pauseScreen = new PauseScreen();
    gameOverScreen = new GameOverScreen();
    leaderboardScreen = new LeaderboardScreen();
    creditsScreen = new CreditsScreen();
    
    // Initialize leaderboard
    LeaderboardManager::GetInstance()->Initialize();
    leaderboard = LeaderboardManager::GetInstance()->GetLeaderboard();
    
    if (leaderboardScreen) {
        leaderboardScreen->SetLeaderboard(leaderboard);
    }
}

void UIScreenManager::Shutdown() {
    delete mainMenu;
    delete pauseScreen;
    delete gameOverScreen;
    delete leaderboardScreen;
    delete creditsScreen;
    
    mainMenu = nullptr;
    pauseScreen = nullptr;
    gameOverScreen = nullptr;
    leaderboardScreen = nullptr;
    creditsScreen = nullptr;
    
    LeaderboardManager::GetInstance()->Shutdown();
}

void UIScreenManager::SwitchTo(ScreenType screen) {
    previousScreen = currentScreen;
    currentScreen = screen;
    
    // Call exit on previous
    switch (previousScreen) {
        case ScreenType::MAIN_MENU: if (mainMenu) mainMenu->OnExit(); break;
        case ScreenType::PAUSED: if (pauseScreen) pauseScreen->OnExit(); break;
        case ScreenType::GAME_OVER: if (gameOverScreen) gameOverScreen->OnExit(); break;
        case ScreenType::LEADERBOARD: if (leaderboardScreen) leaderboardScreen->OnExit(); break;
        case ScreenType::CREDITS: if (creditsScreen) creditsScreen->OnExit(); break;
        default: break;
    }
    
    // Call enter on new
    switch (currentScreen) {
        case ScreenType::MAIN_MENU: if (mainMenu) mainMenu->OnEnter(); break;
        case ScreenType::PAUSED: if (pauseScreen) pauseScreen->OnEnter(); break;
        case ScreenType::GAME_OVER: if (gameOverScreen) gameOverScreen->OnEnter(); break;
        case ScreenType::LEADERBOARD: if (leaderboardScreen) leaderboardScreen->OnEnter(); break;
        case ScreenType::CREDITS: if (creditsScreen) creditsScreen->OnEnter(); break;
        default: break;
    }
}

void UIScreenManager::GoBack() {
    SwitchTo(previousScreen);
}

void UIScreenManager::UpdateCurrent() {
    switch (currentScreen) {
        case ScreenType::MAIN_MENU: if (mainMenu) mainMenu->Update(); break;
        case ScreenType::PAUSED: if (pauseScreen) pauseScreen->Update(); break;
        case ScreenType::GAME_OVER: if (gameOverScreen) gameOverScreen->Update(); break;
        case ScreenType::LEADERBOARD: if (leaderboardScreen) leaderboardScreen->Update(); break;
        case ScreenType::CREDITS: if (creditsScreen) creditsScreen->Update(); break;
        default: break;
    }
}

void UIScreenManager::DrawCurrent() {
    switch (currentScreen) {
        case ScreenType::MAIN_MENU: if (mainMenu) mainMenu->Draw(); break;
        case ScreenType::PAUSED: if (pauseScreen) pauseScreen->Draw(); break;
        case ScreenType::GAME_OVER: if (gameOverScreen) gameOverScreen->Draw(); break;
        case ScreenType::LEADERBOARD: if (leaderboardScreen) leaderboardScreen->Draw(); break;
        case ScreenType::CREDITS: if (creditsScreen) creditsScreen->Draw(); break;
        default: break;
    }
}

void UIScreenManager::SetLeaderboard(LeaderboardLinkedList* lb) {
    leaderboard = lb;
    if (leaderboardScreen) {
        leaderboardScreen->SetLeaderboard(lb);
    }
}
