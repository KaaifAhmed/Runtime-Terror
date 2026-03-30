// Continuing screens.cpp - Game Over and Leaderboard implementations
#include "screens.h"
#include <cstdlib>
#include <cstring>

// Global player name storage (persistent across game overs in same session)
static std::string savedPlayerName = "";

// ============================================
// GAME OVER SCREEN IMPLEMENTATION
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
    finalScore = score;
    finalLines = lines;
    finalTime = time;
    isHighScore = highScore;
    GenerateRandomError();
}

void GameOverScreen::SetStyle(Style s) {
    // Always use terminal error style (most readable)
    style = Style::TERMINAL_ERROR;
}

void GameOverScreen::OnEnter() {
    displayTimer = 0;
    nameLength = 0;
    memset(playerName, 0, sizeof(playerName));
    
    // Only ask for name if it's a high score AND we haven't saved a name yet
    if (isHighScore && savedPlayerName.empty()) {
        showNameInput = true;
        strcpy(playerName, "Player"); // Default name
        nameLength = 6;
    } else if (isHighScore && !savedPlayerName.empty()) {
        // Use saved name automatically
        showNameInput = false;
        if (onSubmitScore) {
            onSubmitScore(savedPlayerName);
        }
    } else {
        showNameInput = false;
    }
}

void GameOverScreen::OnExit() {
    showNameInput = false;
}

void GameOverScreen::Update() {
    displayTimer += GetFrameTime();
    
    // Keyboard input for name (if high score)
    if (showNameInput && nameLength < 20) {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 32) && (key <= 125) && (nameLength < 20)) {
                playerName[nameLength] = (char)key;
                playerName[nameLength + 1] = '\0';
                nameLength++;
            }
            key = GetCharPressed();
        }
        
        if (IsKeyPressed(KEY_BACKSPACE) && nameLength > 0) {
            nameLength--;
            playerName[nameLength] = '\0';
        }
        
        if (IsKeyPressed(KEY_ENTER) && nameLength > 0) {
            if (onSubmitScore) {
                savedPlayerName = std::string(playerName);
                onSubmitScore(savedPlayerName);
            }
            showNameInput = false;
        }
        
        // ESC to skip/close name input
        if (IsKeyPressed(KEY_ESCAPE)) {
            showNameInput = false;
        }
    }
    
    // Standard navigation
    if (!showNameInput) {
        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
            if (onRestart) onRestart();
        }
        if (IsKeyPressed(KEY_M)) {
            if (onMainMenu) onMainMenu();
        }
        if (IsKeyPressed(KEY_Q)) {
            if (onQuit) onQuit();
        }
    }
    
    // Mouse buttons
    if (!showNameInput && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        int mouseY = GetMouseY();
        int centerY = GetScreenHeight() / 2;
        
        if (mouseY > centerY + 80 && mouseY < centerY + 120) {
            if (onRestart) onRestart();
        } else if (mouseY > centerY + 130 && mouseY < centerY + 170) {
            if (onMainMenu) onMainMenu();
        } else if (mouseY > centerY + 180 && mouseY < centerY + 220) {
            if (onQuit) onQuit();
        }
    }
}

void GameOverScreen::Draw() {
    // Always draw terminal error - clean and readable
    DrawTerminalError();
    
    if (showNameInput) {
        DrawNameInput();
    }
}

void GameOverScreen::DrawTerminalError() {
    // Black terminal background
    ClearBackground(VSCodeTheme::TERMINAL_BG);
    
    int x = 50;
    int y = 50;
    int lineHeight = 25;
    
    // Terminal header
    DrawText("runtime@terror:~$ ./play", x, y, 18, VSCodeTheme::TERMINAL_GREEN);
    y += lineHeight * 2;
    
    // Error output
    DrawText("[FATAL ERROR] Execution terminated unexpectedly", x, y, 18, VSCodeTheme::TERMINAL_RED);
    y += lineHeight;
    DrawText(TextFormat("[ERROR CODE] %s", errorCode.c_str()), x, y, 18, VSCodeTheme::TERMINAL_RED);
    y += lineHeight * 2;
    
    DrawText(TextFormat("Exception: %s", errorMessage.c_str()), x, y, 18, VSCodeTheme::TEXT_NORMAL);
    y += lineHeight;
    DrawText(TextFormat("Details: %s", errorDetail.c_str()), x, y, 18, VSCodeTheme::TEXT_MUTED);
    y += lineHeight * 2;
    
    // Stack trace
    DrawText("Stack trace:", x, y, 18, VSCodeTheme::ACCENT_YELLOW);
    y += lineHeight;
    DrawText("  at main.cpp:42 - Player::Update()", x, y, 16, VSCodeTheme::TEXT_MUTED);
    y += lineHeight;
    DrawText("  at tiles.cpp:156 - Tile::Collision()", x, y, 16, VSCodeTheme::TEXT_MUTED);
    y += lineHeight;
    DrawText("  at player.cpp:89 - Player::Fall()", x, y, 16, VSCodeTheme::TEXT_MUTED);
    y += lineHeight * 2;
    
    // Stats in terminal style
    DrawText("=== EXECUTION STATISTICS ===", x, y, 18, VSCodeTheme::TERMINAL_YELLOW);
    y += lineHeight;
    DrawText(TextFormat("Lines Compiled: %d", finalLines), x, y, 18, VSCodeTheme::TERMINAL_GREEN);
    y += lineHeight;
    DrawText(TextFormat("Time Survived: %ds", finalTime), x, y, 18, VSCodeTheme::TERMINAL_GREEN);
    y += lineHeight;
    DrawText(TextFormat("Final Score: %d", finalScore), x, y, 20, VSCodeTheme::ACCENT_CYAN);
    y += lineHeight * 3;
    
    // Options
    DrawText("[R] Restart    [M] Main Menu    [Q] Quit", x, y, 18, VSCodeTheme::TEXT_NORMAL);
    y += lineHeight;
    DrawText("_", x, y, 18, VSCodeTheme::TERMINAL_GREEN);
    UI::DrawBlinkingCursor(x, y, 18, VSCodeTheme::TERMINAL_GREEN);
}

void GameOverScreen::DrawWindowsBSOD() {
    // Classic Windows BSOD blue
    ClearBackground({0, 0, 170, 255}); // #0000AA
    
    int x = 50;
    int y = 50;
    
    // Sad face :(
    DrawText(":(", x, y, 150, WHITE);
    y += 180;
    
    // Main message
    DrawText("Your PC ran into a problem and needs to restart.", x, y, 24, WHITE);
    y += 40;
    DrawText("We're just collecting some error info, and then we'll restart for you.", 
             x, y, 24, WHITE);
    y += 60;
    
    // Progress percentage
    int progress = (int)((displayTimer / 5.0f) * 100) % 101;
    DrawText(TextFormat("%d%% complete", progress), x, y, 28, WHITE);
    y += 60;
    
    // QR code placeholder (just a square)
    DrawRectangle(x, y, 120, 120, WHITE);
    DrawRectangle(x + 10, y + 10, 100, 100, {0, 0, 170, 255});
    DrawText("Error", x + 35, y + 50, 16, WHITE);
    
    // Error details
    int detailsX = x + 150;
    int detailsY = y;
    DrawText(TextFormat("Stop code: %s", errorCode.c_str()), detailsX, detailsY, 20, WHITE);
    detailsY += 30;
    DrawText(TextFormat("What failed: %s", errorMessage.c_str()), detailsX, detailsY, 20, WHITE);
    detailsY += 30;
    DrawText(TextFormat("Score: %d", finalScore), detailsX, detailsY, 20, WHITE);
    
    y += 160;
    
    // Options
    DrawText("Press SPACE to restart, M for Menu, Q to Quit", x, y, 20, WHITE);
}

void GameOverScreen::DrawCompilerError() {
    // VS Code terminal style
    ClearBackground(VSCodeTheme::BG_EDITOR);
    
    // Terminal panel at bottom
    int panelY = GetScreenHeight() / 3;
    DrawRectangle(0, panelY, GetScreenWidth(), GetScreenHeight() - panelY, VSCodeTheme::TERMINAL_BG);
    DrawLine(0, panelY, GetScreenWidth(), panelY, VSCodeTheme::BORDER);
    
    // Terminal tabs
    UI::DrawTabBar(0, panelY - 36, 400, 
                   {"PROBLEMS", "OUTPUT", "DEBUG CONSOLE", "TERMINAL"}, 1);
    
    int x = 20;
    int y = panelY + 20;
    int lineHeight = 22;
    
    // Error header
    DrawText("EXECUTION FAILED", x, y, 20, VSCodeTheme::ACCENT_RED);
    y += lineHeight * 2;
    
    // Error list
    DrawText(TextFormat("ERROR in main.cpp (line 42, col 1): %s", errorMessage.c_str()), 
             x, y, 16, VSCodeTheme::ACCENT_RED);
    y += lineHeight;
    DrawText(TextFormat("  %s", errorDetail.c_str()), x, y, 16, VSCodeTheme::TEXT_MUTED);
    y += lineHeight * 2;
    
    DrawText("WARNING: Memory leak detected in player.cpp", x, y, 16, VSCodeTheme::ACCENT_ORANGE);
    y += lineHeight;
    DrawText("  45:45  delete player;  // Missing null check", x, y, 16, VSCodeTheme::TEXT_MUTED);
    y += lineHeight * 2;
    
    // Stats box
    int boxX = GetScreenWidth() - 350;
    int boxY = panelY + 20;
    DrawRectangle(boxX, boxY, 330, 200, VSCodeTheme::BG_SIDEBAR);
    DrawRectangleLinesEx({(float)boxX, (float)boxY, 330, 200}, 1, VSCodeTheme::BORDER);
    
    DrawText("BUILD STATISTICS", boxX + 10, boxY + 10, 16, VSCodeTheme::ACCENT_CYAN);
    DrawLine(boxX + 10, boxY + 35, boxX + 320, boxY + 35, VSCodeTheme::BORDER);
    
    DrawText(TextFormat("Lines: %d", finalLines), boxX + 10, boxY + 50, 16, VSCodeTheme::TEXT_NORMAL);
    DrawText(TextFormat("Time: %ds", finalTime), boxX + 10, boxY + 75, 16, VSCodeTheme::TEXT_NORMAL);
    DrawText(TextFormat("Score: %d", finalScore), boxX + 10, boxY + 110, 18, VSCodeTheme::ACCENT_CYAN);
    
    if (isHighScore) {
        DrawText("NEW HIGH SCORE!", boxX + 10, boxY + 140, 16, VSCodeTheme::ACCENT_GREEN);
    }
    
    // Options
    y = GetScreenHeight() - 60;
    DrawText("SPACE: Restart  |  M: Main Menu  |  Q: Quit", 
             x, y, 18, VSCodeTheme::TEXT_MUTED);
}

void GameOverScreen::DrawSyntaxError() {
    // Code editor view with syntax error highlighted
    ClearBackground(VSCodeTheme::BG_EDITOR);
    
    int lineNumWidth = 50;
    int editorX = lineNumWidth + 20;
    int y = 100;
    int lineHeight = 24;
    
    // Breadcrumbs
    UI::DrawBreadcrumbs(20, 30, {"src", "main.cpp"});
    DrawLine(0, 50, GetScreenWidth(), 50, VSCodeTheme::BORDER);
    
    // Code lines
    std::vector<std::string> code = {
        "void GameLoop() {",
        "    Player* player = new Player();",
        "    while (!gameOver) {",
        "        player->Update();",
        "        // Oops! Forgot to check bounds",
        "        if (player->y > SCREEN_HEIGHT) {",
        "            >>> SEGMENTATION FAULT <<<",
        "        }",
        "    }",
        "} // Game Over"
    };
    
    int errorLine = 6; // 0-indexed
    
    for (size_t i = 0; i < code.size(); i++) {
        // Line number
        DrawText(TextFormat("%d", i + 40), 15, y, 14, VSCodeTheme::TEXT_MUTED);
        
        // Error line highlighting
        if (i == (size_t)errorLine) {
            DrawRectangle(0, y - 2, GetScreenWidth(), lineHeight, {255, 0, 0, 30});
            DrawText(">>>", 50, y, 16, VSCodeTheme::ACCENT_RED);
        }
        
        // Code text
        Color textColor = (i == (size_t)errorLine) ? VSCodeTheme::ACCENT_RED : VSCodeTheme::TEXT_NORMAL;
        if (code[i].find("//") != std::string::npos) {
            textColor = VSCodeTheme::ACCENT_GREEN; // Comment
        }
        DrawText(code[i].c_str(), editorX, y, 16, textColor);
        
        y += lineHeight;
    }
    
    // Error squiggle on error line
    int squiggleY = 100 + errorLine * lineHeight + 18;
    for (int x = editorX; x < editorX + 200; x += 4) {
        int offset = (x % 8 == 0) ? 2 : -2;
        DrawPixel(x, squiggleY + offset, VSCodeTheme::ACCENT_RED);
    }
    
    // Error tooltip
    int tooltipY = squiggleY + 15;
    DrawRectangle(editorX, tooltipY, 400, 60, VSCodeTheme::BG_SIDEBAR);
    DrawRectangleLinesEx({(float)editorX, (float)tooltipY, 400, 60}, 1, VSCodeTheme::ACCENT_RED);
    DrawText("ERROR: Execution terminated", editorX + 10, tooltipY + 10, 16, VSCodeTheme::ACCENT_RED);
    DrawText(errorMessage.c_str(), editorX + 10, tooltipY + 35, 14, VSCodeTheme::TEXT_MUTED);
    
    // Stats
    y = GetScreenHeight() - 150;
    DrawRectangle(50, y, 300, 120, VSCodeTheme::BG_SIDEBAR);
    DrawText("EXECUTION STATS", 60, y + 10, 18, VSCodeTheme::ACCENT_CYAN);
    DrawText(TextFormat("Score: %d", finalScore), 60, y + 40, 20, VSCodeTheme::TEXT_NORMAL);
    DrawText(TextFormat("Lines: %d", finalLines), 60, y + 65, 16, VSCodeTheme::TEXT_MUTED);
    DrawText(TextFormat("Time: %ds", finalTime), 60, y + 85, 16, VSCodeTheme::TEXT_MUTED);
    
    // Options
    y = GetScreenHeight() - 40;
    DrawText("SPACE: Restart  |  M: Main Menu  |  Q: Quit", 
             50, y, 18, VSCodeTheme::TEXT_MUTED);
}

void GameOverScreen::DrawNameInput() {
    // Darken background
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), {0, 0, 0, 200});
    
    int panelW = 500;
    int panelH = 250;
    int panelX = (GetScreenWidth() - panelW) / 2;
    int panelY = (GetScreenHeight() - panelH) / 2;
    
    // Panel
    DrawRectangle(panelX, panelY, panelW, panelH, VSCodeTheme::BG_SIDEBAR);
    DrawRectangleLinesEx({(float)panelX, (float)panelY, (float)panelW, (float)panelH}, 
                         2, VSCodeTheme::ACCENT_GREEN);
    
    // Title
    const char* title = "NEW HIGH SCORE!";
    int titleW = MeasureText(title, 32);
    DrawText(title, panelX + (panelW - titleW) / 2, panelY + 20, 32, VSCodeTheme::ACCENT_GREEN);
    
    // Score display
    const char* scoreText = TextFormat("Score: %d", finalScore);
    int scoreW = MeasureText(scoreText, 24);
    DrawText(scoreText, panelX + (panelW - scoreW) / 2, panelY + 70, 24, VSCodeTheme::TEXT_NORMAL);
    
    // Input box
    int inputY = panelY + 120;
    DrawRectangle(panelX + 50, inputY, panelW - 100, 40, VSCodeTheme::BG_EDITOR);
    DrawRectangleLinesEx({(float)(panelX + 50), (float)inputY, (float)(panelW - 100), 40}, 
                         2, VSCodeTheme::BORDER_FOCUS);
    
    // Name text
    DrawText(playerName, panelX + 60, inputY + 10, 20, VSCodeTheme::TEXT_NORMAL);
    
    // Blinking cursor
    int nameW = MeasureText(playerName, 20);
    UI::DrawBlinkingCursor(panelX + 60 + nameW + 2, inputY + 8, 24, VSCodeTheme::ACCENT_BLUE);
    
    // Instructions
    const char* hint = "ENTER to save  |  ESC to skip";
    int hintW = MeasureText(hint, 16);
    DrawText(hint, panelX + (panelW - hintW) / 2, panelY + 180, 16, VSCodeTheme::TEXT_MUTED);
}

void GameOverScreen::GenerateRandomError() {
    const char* errors[] = {
        "Segmentation Fault",
        "Stack Overflow",
        "Null Pointer Exception",
        "Memory Access Violation",
        "Unhandled Exception",
        "Runtime Error",
        "Buffer Overflow",
        "Infinite Loop Detected",
        "Deadlock",
        "Race Condition"
    };
    
    const char* details[] = {
        "Attempted to read from address 0x00000000",
        "Stack pointer exceeded maximum stack size",
        "Object reference not set to an instance",
        "Read of size 8 at address 0x7fff...",
        "Exception thrown at 0x00402A11 in main.exe",
        "Program terminated with exit code 1",
        "Heap corruption detected at 0x...",
        "Process exceeded time limit",
        "Thread deadlock on mutex acquisition",
        "Data race between threads"
    };
    
    int idx = GetRandomValue(0, 9);
    errorMessage = errors[idx];
    errorDetail = details[idx];
    errorCode = TextFormat("0x%08X", GetRandomValue(0xC0000000, 0xCFFFFFFF));
}
