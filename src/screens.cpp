// screens.cpp — all screen implementations
#include "screens.h"
#include <cstdlib>
#include <cstring>
#include <cmath>

static std::string savedPlayerName = "";

// ============================================
// MAIN MENU SCREEN
// ============================================
MainMenuScreen::MainMenuScreen()
    : logoY(100), logoPulse(0), selectedOption(0), animationTime(0) {
    menuOptions = {"Start Debugging", "How to Play", "View Leaderboard", "Credits", "Exit"};
}

MainMenuScreen::~MainMenuScreen() {}

void MainMenuScreen::SetCallbacks(std::function<void()> onPlay,
                                  std::function<void()> onHowToPlay,
                                  std::function<void()> onLeaderboard,
                                  std::function<void()> onCredits,
                                  std::function<void()> onExit) {
    callbacks = {onPlay, onHowToPlay, onLeaderboard, onCredits, onExit};
}

void MainMenuScreen::OnEnter() { logoY = 80; selectedOption = 0; animationTime = 0; }
void MainMenuScreen::OnExit() {}

void MainMenuScreen::Update() {
    animationTime += GetFrameTime();
    logoPulse = sin(animationTime * 2) * 5;
    logoY = 80 + logoPulse;

    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
        selectedOption = (selectedOption + 1) % menuOptions.size();
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
        selectedOption = (selectedOption - 1 + menuOptions.size()) % menuOptions.size();
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        if (selectedOption < (int)callbacks.size() && callbacks[selectedOption])
            callbacks[selectedOption]();
    }

    int startY = 300, btnWidth = 220, btnHeight = 35, spacing = 50;
    int editorX = 120; // Aligned with the left padding in Draw()
    for (size_t i = 0; i < menuOptions.size(); i++) {
        Rectangle btn = {(float)editorX, (float)(startY + i * spacing), (float)btnWidth, (float)btnHeight};
        if (UI::MouseInRect(btn)) {
            selectedOption = i;
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && callbacks[i])
                callbacks[i]();
        }
    }
}

void MainMenuScreen::Draw() {
    UI::DrawVSCideBackground("Welcome");

    int editorX = 120; // Left align with Activity Bar padding
    DrawVSText("Welcome to Runtime Terror", editorX, 150, 32, WHITE);
    DrawVSText("A Debugging Adventure", editorX, 190, 16, VSCodeTheme::TEXT_MUTED);
    DrawVSText("Start", editorX, 260, 18, WHITE);

    int startY = 300, btnWidth = 220, btnHeight = 35, spacing = 50;
    for (size_t i = 0; i < menuOptions.size(); i++) {
        bool isSelected = (i == (size_t)selectedOption);
        Color bg = isSelected ? VSCodeTheme::BUTTON_HOVER : VSCodeTheme::BUTTON_BG;
        DrawRectangleRounded({(float)editorX, (float)(startY + i * spacing), (float)btnWidth, (float)btnHeight}, 0.1f, 4, bg);
        // Draw an indicator arrow if selected to improve UX
        if (isSelected) {
            DrawVSText(">", editorX - 20, startY + i * spacing + (btnHeight - 16) / 2, 16, VSCodeTheme::ACCENT_BLUE);
        }
        DrawVSText(menuOptions[i].c_str(), editorX + 15, startY + i * spacing + (btnHeight - 16) / 2, 16, WHITE);
    }
}

// ============================================
// PAUSE SCREEN
// ============================================
PauseScreen::PauseScreen() : overlayAlpha(0), selectedOption(0) {
    pauseOptions = {"Resume", "Restart", "Main Menu", "Quit"};
}

PauseScreen::~PauseScreen() {}
void PauseScreen::OnEnter() { overlayAlpha = 0; selectedOption = 0; }
void PauseScreen::OnExit()  { overlayAlpha = 0; }

void PauseScreen::Update() {
    if (overlayAlpha < 0.7f) overlayAlpha += 0.05f;

    if (IsKeyPressed(KEY_ESCAPE)) { if (onResume) onResume(); return; }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) selectedOption = (selectedOption + 1) % pauseOptions.size();
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))   selectedOption = (selectedOption - 1 + pauseOptions.size()) % pauseOptions.size();
    if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
        switch (selectedOption) {
            case 0: if (onResume)   onResume();   break;
            case 1: if (onRestart)  onRestart();  break;
            case 2: if (onMainMenu) onMainMenu(); break;
            case 3: if (onQuit)     onQuit();     break;
        }
    }
}

void PauseScreen::Draw() {
    Color overlay = VSCodeTheme::BG_DARK; overlay.a = (unsigned char)(overlayAlpha * 200);
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), overlay);

    int panelW = 400, panelH = 350;
    int panelX = (GetScreenWidth() - panelW) / 2;
    int panelY = (GetScreenHeight() - panelH) / 2;

    DrawRectangle(panelX, panelY, panelW, panelH, VSCodeTheme::BG_SIDEBAR);
    DrawRectangleLinesEx({(float)panelX, (float)panelY, (float)panelW, (float)panelH}, 2, VSCodeTheme::BORDER);

    // Title
    const char* title = "PAUSED";
    int titleW = MeasureVSText(title, 36);
    DrawVSText(title, panelX + (panelW - titleW) / 2, panelY + 30, 36, VSCodeTheme::ACCENT_CYAN);

    // Options
    int startY = panelY + 100;
    for (size_t i = 0; i < pauseOptions.size(); i++) {
        int btnY = startY + i * 55;
        bool isSelected = (i == (size_t)selectedOption);
        DrawRectangle(panelX + 50, btnY, panelW - 100, 45, isSelected ? VSCodeTheme::BG_HOVER : VSCodeTheme::BG_WIDGET);
        if (isSelected) DrawRectangle(panelX + 50, btnY, 4, 45, VSCodeTheme::ACCENT_CYAN);
        int textW = MeasureVSText(pauseOptions[i].c_str(), 22);
        DrawVSText(pauseOptions[i].c_str(), panelX + (panelW - textW) / 2, btnY + 10, 22,
                   isSelected ? WHITE : VSCodeTheme::TEXT_MUTED);
    }

    // Help
    const char* help = "ESC to resume";
    int helpW = MeasureVSText(help, 16);
    DrawVSText(help, panelX + (panelW - helpW) / 2, panelY + panelH - 40, 16, VSCodeTheme::TEXT_MUTED);
}

// ============================================
// GAME OVER SCREEN
// ============================================
GameOverScreen::GameOverScreen()
    : style(Style::TERMINAL_ERROR), displayTimer(0),
      finalScore(0), finalLines(0), finalTime(0),
      isHighScore(false), showNameInput(false), nameLength(0) {
    memset(playerName, 0, sizeof(playerName));
    errorMessage = "Segmentation Fault";
    errorDetail = "Memory access violation at 0x00000000";
    errorCode = "0xC0000005";
}

GameOverScreen::~GameOverScreen() {}

void GameOverScreen::SetResults(int score, int lines, int time, bool highScore) {
    finalScore = score; finalLines = lines; finalTime = time; isHighScore = highScore;
    GenerateRandomError();
}

void GameOverScreen::SetStyle(Style s) { style = Style::TERMINAL_ERROR; (void)s; }

void GameOverScreen::SetErrorOverride(const std::string& msg, const std::string& detail, const std::string& code) {
    errorMessage = msg; errorDetail = detail; errorCode = code;
}

void GameOverScreen::OnEnter() {
    doneTimer = displayTimer = 0;
    nameLength = 0;
    memset(playerName, 0, sizeof(playerName));
    typewriterTimer = typewriterCharIndex = typewriterLineIndex = 0;
    typewriterDone = false;
    shakeTimer = flashTimer = 0;

    if (isHighScore) {
        if (!savedPlayerName.empty()) {
            if (onSubmitScore) onSubmitScore(savedPlayerName);
            showNameInput = false;
        } else {
            strcpy(playerName, "Player"); nameLength = 6; showNameInput = true;
        }
    } else {
        showNameInput = false;
    }
}

void GameOverScreen::OnExit() { showNameInput = false; }

void GameOverScreen::Update() {
    float dt = GetFrameTime();
    displayTimer += dt;

    if (!typewriterDone) {
        typewriterTimer += dt;
        if (typewriterTimer >= 0.005f) { typewriterTimer = 0; typewriterCharIndex += 2; }
        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
            typewriterCharIndex = 99999; typewriterDone = true; doneTimer = 0; return;
        }
    } else {
        doneTimer += dt;
    }

    if (!showNameInput && typewriterDone && doneTimer > 0.2f) {
        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) { if (onRestart)  onRestart(); }
        if (IsKeyPressed(KEY_ESCAPE))                           { if (onMainMenu) onMainMenu(); }
    }

    if (shakeTimer > 0) shakeTimer -= dt;
    if (flashTimer > 0) flashTimer -= dt;

    if (showNameInput && nameLength < 20) {
        int key = GetCharPressed();
        while (key > 0) {
            // Allow printable characters except SPACE
            if (key >= 33 && key <= 125 && nameLength < 20) {
                playerName[nameLength++] = (char)key;
                playerName[nameLength] = '\0';
            }
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && nameLength > 0) playerName[--nameLength] = '\0';
        if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) && nameLength > 0) {
            savedPlayerName = std::string(playerName);
            if (onSubmitScore) onSubmitScore(savedPlayerName);
            showNameInput = false;
        }
        if (IsKeyPressed(KEY_ESCAPE)) showNameInput = false;
    }

    if (!showNameInput && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        int mouseY = GetMouseY(), centerY = GetScreenHeight() / 2;
        if      (mouseY > centerY + 80  && mouseY < centerY + 120) { if (onRestart)  onRestart(); }
        else if (mouseY > centerY + 130 && mouseY < centerY + 170) { if (onMainMenu) onMainMenu(); }
        else if (mouseY > centerY + 180 && mouseY < centerY + 220) { if (onQuit)     onQuit(); }
    }
}

void GameOverScreen::Draw() {
    DrawTerminalError();
    if (showNameInput) DrawNameInput();
}

void GameOverScreen::DrawTerminalError() {
    std::vector<TerminalLine> script = {
        {"runtime@terror:~$ ./play",                                   VSCodeTheme::TERMINAL_GREEN,  false},
        {"",                                                           VSCodeTheme::TEXT_MUTED,      false},
        {"[FATAL ERROR] Execution terminated unexpectedly",            VSCodeTheme::TERMINAL_RED,    true},
        {TextFormat("[ERROR CODE] %s", errorCode.c_str()),             VSCodeTheme::TERMINAL_RED,    false},
        {"",                                                           VSCodeTheme::TEXT_MUTED,      false},
        {TextFormat("Exception: %s", errorMessage.c_str()),            VSCodeTheme::TEXT_NORMAL,     false},
        {TextFormat("Details:   %s", errorDetail.c_str()),             VSCodeTheme::TEXT_MUTED,      false},
        {"",                                                           VSCodeTheme::TEXT_MUTED,      false},
        {"Stack trace:",                                               VSCodeTheme::ACCENT_YELLOW,   false},
        {"  at main.cpp:42    Player::Update()",                       VSCodeTheme::TEXT_MUTED,      false},
        {"  at tiles.cpp:156  Tile::Collision()",                      VSCodeTheme::TEXT_MUTED,      false},
        {"  at player.cpp:89  Player::Fall()",                        VSCodeTheme::TEXT_MUTED,      false},
        {"",                                                           VSCodeTheme::TEXT_MUTED,      false},
        {"=== EXECUTION STATISTICS ===",                               VSCodeTheme::TERMINAL_YELLOW, false},
        {TextFormat("Lines Compiled : %d", finalLines),                VSCodeTheme::TERMINAL_GREEN,  false},
        {TextFormat("Time Survived  : %ds", finalTime),                VSCodeTheme::TERMINAL_GREEN,  false},
        {TextFormat("Final Score    : %d", finalScore),                VSCodeTheme::ACCENT_CYAN,     false},
        {"",                                                           VSCodeTheme::TEXT_MUTED,      false},
        {"Process exited with code 1",                                 VSCodeTheme::TEXT_MUTED,      false},
    };

    ClearBackground((flashTimer > 0) ? Color{40, 0, 0, 255} : VSCodeTheme::TERMINAL_BG);
    int shakeX = (shakeTimer > 0) ? GetRandomValue(-4, 4) : 0;
    int shakeY = (shakeTimer > 0) ? GetRandomValue(-3, 3) : 0;
    int x = 50 + shakeX, y = 50 + shakeY, lineH = 26, totalChars = 0;

    for (int li = 0; li < (int)script.size(); li++) {
        const auto& line = script[li];
        int lineLen = (int)line.text.size();
        if (totalChars + lineLen <= typewriterCharIndex) {
            if (!line.text.empty()) {
                // Highlight the score line with a subtle background
                if (line.text.rfind("Final Score", 0) == 0) {
                    DrawRectangle(x - 6, y - 2, MeasureVSText(line.text.c_str(), 18) + 12, 22, {0, 122, 204, 40});
                    DrawRectangle(x - 6, y - 2, 3, 22, VSCodeTheme::ACCENT_CYAN);
                }
                DrawVSText(line.text.c_str(), x, y, 18, line.color);
            }
            if (line.isShakeTrigger && typewriterLineIndex == li) {
                shakeTimer = 0.25f; flashTimer = 0.15f; typewriterLineIndex = li + 1;
            }
            totalChars += lineLen + 1; y += line.text.empty() ? lineH / 2 : lineH;
        } else if (totalChars < typewriterCharIndex) {
            std::string partial = line.text.substr(0, typewriterCharIndex - totalChars);
            DrawVSText(partial.c_str(), x, y, 18, line.color);
            UI::DrawBlinkingCursor(x + MeasureVSText(partial.c_str(), 18) + 2, y, 20, line.color);
            y += lineH; break;
        } else { break; }
    }

    int total = 0; for (auto& l : script) total += (int)l.text.size() + 1;
    if (typewriterCharIndex >= total) {
        typewriterDone = true;
        // Prompt block — clean, no giant score
        DrawRectangle(x - 6, y + 6, GetScreenWidth() - x * 2 - 40, 32, {255,255,255, 8});
        DrawVSText("runtime@terror:~$ _", x, y + 14, 18, VSCodeTheme::TERMINAL_GREEN);
        y += 50;
        DrawVSText("[SPACE] Restart", x,        y, 18, VSCodeTheme::TEXT_NORMAL);
        DrawVSText("[ESC] Main Menu", x + 220,  y, 18, VSCodeTheme::TEXT_NORMAL);
    }
    if (!typewriterDone)
        DrawVSText("[SPACE] Skip animation", x, GetScreenHeight() - 40, 16, VSCodeTheme::TEXT_MUTED);
}

void GameOverScreen::DrawNameInput() {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), {0, 0, 0, 180});
    int panelW = 480, panelH = 230;
    int panelX = (GetScreenWidth() - panelW) / 2;
    int panelY = (GetScreenHeight() - panelH) / 2;

    // Panel background + left accent bar
    DrawRectangle(panelX, panelY, panelW, panelH, VSCodeTheme::BG_SIDEBAR);
    DrawRectangleLinesEx({(float)panelX, (float)panelY, (float)panelW, (float)panelH}, 1, VSCodeTheme::BORDER);
    DrawRectangle(panelX, panelY, 3, panelH, VSCodeTheme::ACCENT_BLUE);

    // Title row
    DrawRectangle(panelX, panelY, panelW, 44, VSCodeTheme::BG_PANEL);
    const char* title = "New High Score";
    DrawVSText(title, panelX + 16, panelY + 12, 22, VSCodeTheme::TEXT_NORMAL);
    const char* scoreTag = TextFormat("Score: %d", finalScore);
    DrawVSText(scoreTag, panelX + panelW - MeasureVSText(scoreTag, 18) - 16, panelY + 14, 18, VSCodeTheme::ACCENT_CYAN);

    // Label + input field
    DrawVSText("Enter your name:", panelX + 16, panelY + 62, 16, VSCodeTheme::TEXT_MUTED);
    int inputY = panelY + 84;
    DrawRectangle(panelX + 16, inputY, panelW - 32, 38, VSCodeTheme::BG_EDITOR);
    DrawRectangleLinesEx({(float)(panelX + 16), (float)inputY, (float)(panelW - 32), 38}, 1, VSCodeTheme::BORDER_FOCUS);
    DrawVSText(playerName, panelX + 26, inputY + 10, 20, VSCodeTheme::TEXT_NORMAL);
    UI::DrawBlinkingCursor(panelX + 26 + MeasureVSText(playerName, 20) + 2, inputY + 8, 22, VSCodeTheme::ACCENT_BLUE);

    // Hint
    const char* hint = "SPACE to save    ESC to skip";
    DrawVSText(hint, panelX + (panelW - MeasureVSText(hint, 14)) / 2, panelY + panelH - 30, 14, VSCodeTheme::TEXT_MUTED);
}

void GameOverScreen::GenerateRandomError() {
    const char* errors[]  = {"Segmentation Fault","Stack Overflow","Null Pointer Exception",
                              "Memory Access Violation","Unhandled Exception","Runtime Error",
                              "Buffer Overflow","Infinite Loop Detected","Deadlock","Race Condition"};
    const char* details[] = {"Attempted to read from address 0x00000000",
                              "Stack pointer exceeded maximum stack size",
                              "Object reference not set to an instance",
                              "Read of size 8 at address 0x7fff...",
                              "Exception thrown at 0x00402A11 in main.exe",
                              "Program terminated with exit code 1",
                              "Heap corruption detected at 0x...",
                              "Process exceeded time limit",
                              "Thread deadlock on mutex acquisition",
                              "Data race between threads"};
    int idx = GetRandomValue(0, 9);
    errorMessage = errors[idx]; errorDetail = details[idx];
    errorCode = TextFormat("0x%08X", GetRandomValue(0xC0000000, 0xCFFFFFFF));
}

// ============================================
// LEADERBOARD SCREEN
// ============================================
LeaderboardScreen::LeaderboardScreen() : scrollY(0), selectedIndex(-1), leaderboard(nullptr), showEmptyMessage(true) {}
LeaderboardScreen::~LeaderboardScreen() {}

void LeaderboardScreen::SetLeaderboard(LeaderboardLinkedList* lb) {
    leaderboard = lb;
    showEmptyMessage = (lb == nullptr || lb->IsEmpty());
}

void LeaderboardScreen::OnEnter() {
    scrollY = 0; selectedIndex = -1;
    if (leaderboard) showEmptyMessage = leaderboard->IsEmpty();
}
void LeaderboardScreen::OnExit() {}

void LeaderboardScreen::Update() {
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        if (leaderboard && !leaderboard->IsEmpty())
            selectedIndex = (selectedIndex + 1) % leaderboard->GetCount();
    }
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        if (leaderboard && !leaderboard->IsEmpty())
            selectedIndex = (selectedIndex - 1 + leaderboard->GetCount()) % leaderboard->GetCount();
    }
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) { if (onBack)   onBack(); }
    if (IsKeyPressed(KEY_R))                                      { if (onReset)  onReset(); }
    if (IsKeyPressed(KEY_E))                                      { if (onExport) onExport(); }

    float scroll = GetMouseWheelMove();
    scrollY += scroll * 30;
    if (scrollY > 0) scrollY = 0;
    if (leaderboard) {
        int maxScroll = leaderboard->GetCount() * 60 + 200 - GetScreenHeight() + 100;
        if (maxScroll > 0 && scrollY < -maxScroll) scrollY = -maxScroll;
    }
}

void LeaderboardScreen::Draw() {
    UI::DrawVSCideBackground("leaderboard.json");
    DrawHeader();
    if (showEmptyMessage || !leaderboard || leaderboard->IsEmpty()) DrawEmptyState();
    else DrawLeaderboardList();
    DrawFooter();
}

void LeaderboardScreen::DrawHeader() {
    // Uses VS Code chrome (top menu + activity bar + tab) drawn by DrawVSCideBackground
    // Add the column header row on top of the editor area
    const int colRank  = 70;
    const int colName  = 160;
    const int colScore = 420;
    const int colLines = 560;
    const int colTime  = 680;
    const int colDate  = 800;
    const int headerY  = 71;  // just below tab bar (36 + 35 = 71)

    DrawRectangle(56, headerY, GetScreenWidth() - 56, 50, VSCodeTheme::BG_PANEL);
    DrawLine(56, headerY + 50, GetScreenWidth(), headerY + 50, VSCodeTheme::BORDER);

    DrawVSText("RANK",   colRank,  headerY + 16, 14, VSCodeTheme::ACCENT_BLUE);
    DrawVSText("PLAYER", colName,  headerY + 16, 14, VSCodeTheme::ACCENT_BLUE);
    DrawVSText("SCORE",  colScore, headerY + 16, 14, VSCodeTheme::ACCENT_BLUE);
    DrawVSText("LINES",  colLines, headerY + 16, 14, VSCodeTheme::ACCENT_BLUE);
    DrawVSText("TIME",   colTime,  headerY + 16, 14, VSCodeTheme::ACCENT_BLUE);
    DrawVSText("DATE",   colDate,  headerY + 16, 14, VSCodeTheme::ACCENT_BLUE);

    // Page title in the menu bar area
    const char* title = "LEADERBOARD";
    DrawVSText(title, (GetScreenWidth() - MeasureVSText(title, 14)) / 2, 10, 14, VSCodeTheme::TEXT_NORMAL);
    if (leaderboard) {
        const char* stats = TextFormat("Total runs: %d", leaderboard->GetCount());
        DrawVSText(stats, GetScreenWidth() - MeasureVSText(stats, 14) - 16, 10, 14, VSCodeTheme::TEXT_MUTED);
    }
}

void LeaderboardScreen::DrawLeaderboardList() {
    if (!leaderboard) return;
    // Column X positions — aligned with DrawHeader labels
    const int colRank  = 70;
    const int colName  = 160;
    const int colScore = 420;
    const int colLines = 560;
    const int colTime  = 680;
    const int colDate  = 800;
    const int entryH   = 52;
    // startY must clear: topbar(36) + tabbar(35) + column header(50) = 121 → use 130
    int startY = 130 + (int)scrollY;

    RunDataNode* cur = leaderboard->GetHead();
    int index = 0;
    while (cur) {
        int y = startY + index * entryH;
        if (y > GetScreenHeight() - 50) break;   // off bottom
        if (y + entryH < 121) { cur = cur->next; index++; continue; } // off top

        bool isSelected = (index == selectedIndex);
        Color rowBg = isSelected ? VSCodeTheme::BG_HOVER : (index % 2 == 0 ? VSCodeTheme::BG_EDITOR : VSCodeTheme::BG_SIDEBAR);
        if      (cur->rank == 1) rowBg = {255, 215, 0,  25};
        else if (cur->rank == 2) rowBg = {192, 192, 192, 20};
        else if (cur->rank == 3) rowBg = {205, 127, 50,  20};
        DrawRectangle(56, y, GetScreenWidth() - 56, entryH, rowBg);
        if (isSelected) DrawRectangle(56, y, 3, entryH, VSCodeTheme::ACCENT_BLUE);
        DrawLine(56, y + entryH, GetScreenWidth(), y + entryH, VSCodeTheme::BORDER);

        DrawRankEntry(cur->rank, cur->playerName, cur->score, y);

        Color dc = isSelected ? VSCodeTheme::TEXT_NORMAL : VSCodeTheme::TEXT_MUTED;
        DrawVSText(TextFormat("%d",  cur->linesCompiled), colLines, y + (entryH - 16) / 2, 16, dc);
        DrawVSText(TextFormat("%ds", cur->timeSurvived),  colTime,  y + (entryH - 16) / 2, 16, dc);
        DrawVSText(cur->date.c_str(),                     colDate,  y + (entryH - 16) / 2, 14, dc);
        cur = cur->next; index++;
    }
}

void LeaderboardScreen::DrawRankEntry(int rank, const std::string& name, int score, int y) {
    const int colRank = 70, colName = 160, colScore = 420;
    const int entryH = 52;
    bool isSelected = (rank - 1 == selectedIndex);

    Color rankColor = VSCodeTheme::TEXT_MUTED;
    const char* rankStr = TextFormat("%dth", rank);
    if      (rank == 1) { rankColor = {255, 215, 0,   255}; rankStr = "1st"; }
    else if (rank == 2) { rankColor = {192, 192, 192, 255}; rankStr = "2nd"; }
    else if (rank == 3) { rankColor = {205, 127, 50,  255}; rankStr = "3rd"; }

    DrawVSText(rankStr,        colRank,   y + (entryH - 18) / 2, 18, rankColor);
    DrawVSText(name.c_str(),   colName,   y + (entryH - 18) / 2, 18,
               isSelected ? VSCodeTheme::TEXT_NORMAL : WHITE);
    DrawVSText(TextFormat("%d", score), colScore, y + (entryH - 24) / 2, 24,
               (rank <= 3) ? rankColor : VSCodeTheme::ACCENT_GREEN);
}

void LeaderboardScreen::DrawEmptyState() {
    const char* msg    = "No runs recorded yet!";
    const char* subMsg = "Play the game to see your scores here.";
    int centerY = GetScreenHeight() / 2;
    DrawVSText(msg,    (GetScreenWidth() - MeasureVSText(msg, 28)) / 2,    centerY - 30, 28, VSCodeTheme::TEXT_MUTED);
    DrawVSText(subMsg, (GetScreenWidth() - MeasureVSText(subMsg, 20)) / 2, centerY + 20, 20, VSCodeTheme::TEXT_MUTED);
}

void LeaderboardScreen::DrawFooter() {
    int footerY = GetScreenHeight() - 32;
    // Drawn on top of VS Code status bar area
    DrawVSText("ESC Back    R Reset    E Export CSV", 70, footerY + 8, 13, WHITE);
    const char* hint = "Scroll to view more";
    DrawVSText(hint, GetScreenWidth() - MeasureVSText(hint, 13) - 16, footerY + 8, 13, WHITE);
}

// ============================================
// CREDITS SCREEN
// ============================================
CreditsScreen::CreditsScreen() : scrollY(GetScreenHeight()), scrollSpeed(1.5f) {
    credits = {"RUNTIME TERROR","","A Programming Adventure","",
               "=== DEVELOPERS ===","Ahsan Zaheer","Kaaif Ahmed","Abdullah Khalid",
               "Ammar Ahmad","Muhammad Hamza","Arham Rehan","Haris Jawaid","",
               "=== DATA STRUCTURES ===","Circular Buffer (Rewind)","Linked List (Leaderboard)",
               "Insertion Sort","","Thank you for playing!","","Press ESC to return"};
}

CreditsScreen::~CreditsScreen() {}
void CreditsScreen::OnEnter() { scrollY = (float)GetScreenHeight(); }
void CreditsScreen::OnExit()  {}

void CreditsScreen::Update() {
    scrollY -= scrollSpeed;
    if (scrollY < -(float)(credits.size() * 40)) scrollY = (float)GetScreenHeight();
    if (IsKeyDown(KEY_SPACE)) scrollY -= scrollSpeed * 3;
    if (IsKeyPressed(KEY_ESCAPE)) { if (onBack) onBack(); }
}

void CreditsScreen::Draw() {
    ClearBackground(VSCodeTheme::BG_DARK);
    UI::DrawVSCideBackground("credits.txt");
    DrawScrollingCredits();

    // Fade top and bottom so credits scroll in/out cleanly
    Color fade = {VSCodeTheme::BG_DARK.r, VSCodeTheme::BG_DARK.g, VSCodeTheme::BG_DARK.b, 220};
    DrawRectangle(56, 71, GetScreenWidth() - 56, 60, fade);
    DrawRectangle(56, GetScreenHeight() - 60, GetScreenWidth() - 56, 60, fade);

    DrawVSText("ESC to return", (GetScreenWidth() - MeasureVSText("ESC to return", 14)) / 2,
               GetScreenHeight() - 44, 14, VSCodeTheme::TEXT_MUTED);
}

void CreditsScreen::DrawScrollingCredits() {
    int centerX = GetScreenWidth() / 2;
    int startY = (int)scrollY;
    for (size_t i = 0; i < credits.size(); i++) {
        int y = startY + (int)(i * 50);
        if (y < 71 || y > GetScreenHeight() - 40) continue; // clip to editor area
        if (credits[i].empty()) continue;

        int fontSize = 20;
        Color color = VSCodeTheme::TEXT_NORMAL;
        if      (credits[i].find("RUNTIME TERROR") != std::string::npos) { fontSize = 42; color = VSCodeTheme::ACCENT_RED; }
        else if (credits[i].find("===") != std::string::npos)            { fontSize = 14; color = VSCodeTheme::ACCENT_BLUE; }
        else if (credits[i].find("Thank you")      != std::string::npos) { fontSize = 24; color = VSCodeTheme::ACCENT_GREEN; }
        else if (credits[i].find("Press ESC")      != std::string::npos) continue; // handled separately
        else if (credits[i].find("A Programming")  != std::string::npos) { fontSize = 16; color = VSCodeTheme::TEXT_MUTED; }

        DrawVSText(credits[i].c_str(), centerX - MeasureVSText(credits[i].c_str(), fontSize) / 2,
                   y, fontSize, color);
    }
}

// ============================================
// HOW TO PLAY SCREEN
// ============================================
HowToPlayScreen::HowToPlayScreen() {}
HowToPlayScreen::~HowToPlayScreen() {}
void HowToPlayScreen::OnEnter() {}
void HowToPlayScreen::OnExit() {}

void HowToPlayScreen::Update() {
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) { 
        if (onBack) onBack(); 
    }
}

void HowToPlayScreen::Draw() {
    ClearBackground(VSCodeTheme::BG_DARK);
    UI::DrawVSCideBackground("how_to_play.md");

    int centerX = GetScreenWidth() / 2;
    int y = 100;
    
    DrawVSText("Controls", centerX - MeasureVSText("Controls", 36) / 2, y, 36, VSCodeTheme::ACCENT_CYAN);
    y += 60;

    int leftCol = centerX - 300;
    int rightCol = centerX + 50;

    auto DrawRule = [&](const char* key, const char* desc, Color color) {
        DrawVSText(key, leftCol, y, 20, color);
        DrawVSText(desc, rightCol, y, 20, VSCodeTheme::TEXT_NORMAL);
        y += 40;
    };

    DrawRule("SPACE", "Jump / Double Jump", VSCodeTheme::ACCENT_GREEN);
    DrawRule("CTRL + Z", "Rewind Time (Costs memory)", VSCodeTheme::ACCENT_YELLOW);
    DrawRule("R", "Deflect from Red Tile (Bhop)", VSCodeTheme::ACCENT_RED);
    DrawRule("SHIFT", "Dash forward (Recharges every 5s)", VSCodeTheme::ACCENT_BLUE);
    DrawRule("ESC", "Pause / Back", VSCodeTheme::TEXT_MUTED);

    y += 20;
    DrawVSText("Mechanics", centerX - MeasureVSText("Mechanics", 36) / 2, y, 36, VSCodeTheme::ACCENT_CYAN);
    y += 60;
    
    DrawRule("Grey Code:", "Normal paths. Build up your score.", VSCodeTheme::TEXT_MUTED);
    DrawRule("Blue Code:", "Logical error! Slows down your computer.", VSCodeTheme::ACCENT_BLUE);
    DrawRule("Red Code:", "Syntax error! Fatal crash on touch.", VSCodeTheme::ACCENT_RED);
    DrawRule("Graph Portals:", "Teleports you to the dark world (+4 score).", VSCodeTheme::ACCENT_CYAN);
    DrawRule("Dark World:", "Hardcore mode! Tiles become corrupted and scary.", {190, 0, 190, 255}); // Dark purple

    DrawVSText("ESC to return", centerX - MeasureVSText("ESC to return", 14) / 2, GetScreenHeight() - 44, 14, VSCodeTheme::TEXT_MUTED);
}

// ============================================
// UI SCREEN MANAGER
// ============================================
UIScreenManager::UIScreenManager()
    : currentScreen(ScreenType::NONE), previousScreen(ScreenType::NONE),
      mainMenu(nullptr), pauseScreen(nullptr), gameOverScreen(nullptr),
      leaderboardScreen(nullptr), creditsScreen(nullptr), howToPlayScreen(nullptr), leaderboard(nullptr) {}

UIScreenManager::~UIScreenManager() { Shutdown(); }

void UIScreenManager::Initialize() {
    mainMenu         = new MainMenuScreen();
    pauseScreen      = new PauseScreen();
    gameOverScreen   = new GameOverScreen();
    leaderboardScreen = new LeaderboardScreen();
    creditsScreen    = new CreditsScreen();
    howToPlayScreen  = new HowToPlayScreen();

    LeaderboardManager::GetInstance()->Initialize();
    leaderboard = LeaderboardManager::GetInstance()->GetLeaderboard();
    if (leaderboardScreen) leaderboardScreen->SetLeaderboard(leaderboard);
}

void UIScreenManager::Shutdown() {
    delete mainMenu; delete pauseScreen; delete gameOverScreen;
    delete leaderboardScreen; delete creditsScreen; delete howToPlayScreen;
    mainMenu         = nullptr;
    pauseScreen      = nullptr;
    gameOverScreen   = nullptr;
    leaderboardScreen = nullptr;
    creditsScreen    = nullptr;
    howToPlayScreen  = nullptr;
    LeaderboardManager::GetInstance()->Shutdown();
}

void UIScreenManager::SwitchTo(ScreenType screen) {
    previousScreen = currentScreen;
    currentScreen  = screen;

    auto callOnScreen = [&](ScreenType t, auto fn) {
        if (t == ScreenType::MAIN_MENU   && mainMenu)          (mainMenu->*fn)();
        else if (t == ScreenType::PAUSED && pauseScreen)        (pauseScreen->*fn)();
        else if (t == ScreenType::GAME_OVER && gameOverScreen)  (gameOverScreen->*fn)();
        else if (t == ScreenType::LEADERBOARD && leaderboardScreen) (leaderboardScreen->*fn)();
        else if (t == ScreenType::CREDITS && creditsScreen)     (creditsScreen->*fn)();
        else if (t == ScreenType::HOW_TO_PLAY && howToPlayScreen) (howToPlayScreen->*fn)();
    };
    callOnScreen(previousScreen, &Screen::OnExit);
    callOnScreen(currentScreen,  &Screen::OnEnter);
}

void UIScreenManager::GoBack() { SwitchTo(previousScreen); }

void UIScreenManager::UpdateCurrent() {
    switch (currentScreen) {
        case ScreenType::MAIN_MENU:   if (mainMenu)          mainMenu->Update();          break;
        case ScreenType::PAUSED:      if (pauseScreen)        pauseScreen->Update();       break;
        case ScreenType::GAME_OVER:   if (gameOverScreen)     gameOverScreen->Update();    break;
        case ScreenType::LEADERBOARD: if (leaderboardScreen)  leaderboardScreen->Update(); break;
        case ScreenType::CREDITS:     if (creditsScreen)      creditsScreen->Update();     break;
        case ScreenType::HOW_TO_PLAY: if (howToPlayScreen)    howToPlayScreen->Update();   break;
        default: break;
    }
}

void UIScreenManager::DrawCurrent() {
    switch (currentScreen) {
        case ScreenType::MAIN_MENU:   if (mainMenu)          mainMenu->Draw();          break;
        case ScreenType::PAUSED:      if (pauseScreen)        pauseScreen->Draw();       break;
        case ScreenType::GAME_OVER:   if (gameOverScreen)     gameOverScreen->Draw();    break;
        case ScreenType::LEADERBOARD: if (leaderboardScreen)  leaderboardScreen->Draw(); break;
        case ScreenType::CREDITS:     if (creditsScreen)      creditsScreen->Draw();     break;
        case ScreenType::HOW_TO_PLAY: if (howToPlayScreen)    howToPlayScreen->Draw();   break;
        default: break;
    }
}

void UIScreenManager::SetLeaderboard(LeaderboardLinkedList* lb) {
    leaderboard = lb;
    if (leaderboardScreen) leaderboardScreen->SetLeaderboard(lb);
}
