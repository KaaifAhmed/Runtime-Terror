// Enhanced screens with polished UX
#include "screens.h"
#include <cstdlib>
#include <cstring>
#include <cmath>

// Global player name storage (persistent across game overs in same session)
static std::string savedPlayerName = "";

// ============================================
// ENHANCED MAIN MENU SCREEN
// ============================================
MainMenuScreen::MainMenuScreen() 
    : logoY(100), logoPulse(0), selectedOption(0), animationTime(0) {
    menuOptions = {"Start Debugging (Play)", "View Leaderboard", "Credits", "Exit"};
}

MainMenuScreen::~MainMenuScreen() {}

void MainMenuScreen::SetCallbacks(std::function<void()> onPlay, 
                                  std::function<void()> onLeaderboard,
                                  std::function<void()> onCredits,
                                  std::function<void()> onExit) {
    callbacks = {onPlay, onLeaderboard, onCredits, onExit};
}

void MainMenuScreen::OnEnter() {
    logoY = 80;
    selectedOption = 0;
    animationTime = 0;
}

void MainMenuScreen::OnExit() {}

void MainMenuScreen::Update() {
    animationTime += GetFrameTime();
    
    // Smooth logo animation
    logoPulse = sin(animationTime * 2) * 5;
    logoY = 80 + logoPulse;
    
    // Keyboard navigation with debounce
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        selectedOption = (selectedOption + 1) % menuOptions.size();
    }
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        selectedOption = (selectedOption - 1 + menuOptions.size()) % menuOptions.size();
    }
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        if (selectedOption < (int)callbacks.size() && callbacks[selectedOption]) {
            callbacks[selectedOption]();
        }
    }
    
    // Mouse selection with hover effect
    for (size_t i = 0; i < menuOptions.size(); i++) {
        Rectangle btn = {
            (float)(GetScreenWidth() / 2 - 150),
            (float)(320 + i * 70),
            300, 55
        };
        if (UI::MouseInRect(btn)) {
            selectedOption = i;
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && callbacks[i]) {
                callbacks[i]();
            }
        }
    }
}

void MainMenuScreen::Draw() {
    // Animated gradient background
    DrawAnimatedBackground();
    
    // Draw main logo with glow
    DrawLogo();
    
    // Draw menu with hover animations
    DrawMenuOptions();
    
    // Draw bottom info bar
    DrawInfoBar();
}

void MainMenuScreen::DrawAnimatedBackground() {
    // Subtle animated background
    float t = animationTime * 0.5f;
    
    // Base background
    ClearBackground(VSCodeTheme::BG_EDITOR);
    
    // Animated grid lines
    for (int x = 0; x < GetScreenWidth(); x += 60) {
        float alpha = 15 + 10 * sin(t + x * 0.01f);
        DrawLine(x, 0, x, GetScreenHeight(), {60, 60, 60, (unsigned char)alpha});
    }
    for (int y = 0; y < GetScreenHeight(); y += 60) {
        float alpha = 15 + 10 * sin(t + y * 0.01f);
        DrawLine(0, y, GetScreenWidth(), y, {60, 60, 60, (unsigned char)alpha});
    }
}

void MainMenuScreen::DrawLogo() {
    const char* title = "Runtime Terror";
    const char* subtitle = "A Debugging Adventure";
    
    int titleSize = 72;
    int titleW = MeasureText(title, titleSize);
    int titleX = (GetScreenWidth() - titleW) / 2;
    
    // Glow effect behind title
    float glowIntensity = (sin(animationTime * 3) + 1) * 0.5f;
    for (int i = 5; i > 0; i--) {
        int alpha = (int)(30 * glowIntensity / i);
        Color glow = VSCodeTheme::ACCENT_CYAN;
        glow.a = (unsigned char)alpha;
        DrawText(title, titleX, (int)logoY + i, titleSize + i * 2, glow);
    }
    
    // Main title
    DrawText(title, titleX, (int)logoY, titleSize, VSCodeTheme::ACCENT_CYAN);
    
    // Blinking cursor
    int cursorX = titleX + titleW + 8;
    float cursorAlpha = (sin(animationTime * 8) + 1) * 0.5f;
    Color cursorColor = VSCodeTheme::ACCENT_CYAN;
    cursorColor.a = (unsigned char)(255 * cursorAlpha);
    DrawRectangle(cursorX, (int)logoY + 15, 4, 45, cursorColor);
    
    // Subtitle
    int subSize = 22;
    int subW = MeasureText(subtitle, subSize);
    DrawText(subtitle, (GetScreenWidth() - subW) / 2, (int)logoY + 90, subSize, VSCodeTheme::TEXT_MUTED);
}

void MainMenuScreen::DrawMenuOptions() {
    int startY = 320;
    int btnWidth = 300;
    int btnHeight = 55;
    int spacing = 70;
    int centerX = GetScreenWidth() / 2;
    
    for (size_t i = 0; i < menuOptions.size(); i++) {
        int btnX = centerX - btnWidth / 2;
        int btnY = startY + i * spacing;
        
        bool isSelected = (i == selectedOption);
        
        // Animated selection indicator
        float selectAnim = isSelected ? 1.0f : 0.0f;
        static float currentAnim[4] = {0, 0, 0, 0};
        currentAnim[i] += (selectAnim - currentAnim[i]) * 0.2f;
        
        // Button background with rounded feel
        Color bg = {
            (unsigned char)(40 + 20 * currentAnim[i]),
            (unsigned char)(45 + 20 * currentAnim[i]),
            (unsigned char)(55 + 25 * currentAnim[i]),
            255
        };
        
        // Draw rounded rectangle
        DrawRectangle(btnX, btnY, btnWidth, btnHeight, bg);
        
        // Left accent bar for selected item
        if (currentAnim[i] > 0.01f) {
            int barWidth = (int)(4 + 4 * currentAnim[i]);
            Color accent = VSCodeTheme::ACCENT_CYAN;
            accent.a = (unsigned char)(255 * currentAnim[i]);
            DrawRectangle(btnX, btnY, barWidth, btnHeight, accent);
        }
        
        // Border
        Color borderColor = {80, 80, 80, 255};
        if (isSelected) borderColor = VSCodeTheme::ACCENT_CYAN;
        DrawRectangleLinesEx({(float)btnX, (float)btnY, (float)btnWidth, (float)btnHeight}, 
                            isSelected ? 2 : 1, borderColor);
        
        // Text
        int textSize = 22;
        int textW = MeasureText(menuOptions[i].c_str(), textSize);
        Color textColor = isSelected ? WHITE : VSCodeTheme::TEXT_MUTED;
        DrawText(menuOptions[i].c_str(), btnX + (btnWidth - textW) / 2, 
                btnY + (btnHeight - textSize) / 2, textSize, textColor);
    }
}

void MainMenuScreen::DrawInfoBar() {
    int barY = GetScreenHeight() - 60;
    
    // Version info
    const char* version = "v1.0";
    DrawText(version, 20, barY, 14, VSCodeTheme::TEXT_MUTED);
    
    // Controls hint
    const char* hint = "UP/DOWN: Navigate  |  ENTER: Select  |  ESC: Back";
    int hintW = MeasureText(hint, 14);
    DrawText(hint, (GetScreenWidth() - hintW) / 2, barY, 14, VSCodeTheme::TEXT_MUTED);
    
    // Credits
    const char* credit = "Made with Raylib";
    int creditW = MeasureText(credit, 14);
    DrawText(credit, GetScreenWidth() - creditW - 20, barY, 14, VSCodeTheme::TEXT_MUTED);
}

void MainMenuScreen::DrawBackgroundCode() {
    // Simplified code background - just a few lines
    int editorX = 350;
    int y = 150;
    
    DrawText("// Runtime Terror", editorX, y, 16, VSCodeTheme::ACCENT_GREEN);
    y += 30;
    DrawText("#include <raylib.h>", editorX, y, 16, VSCodeTheme::ACCENT_PURPLE);
    y += 30;
    DrawText("int main() {", editorX, y, 16, VSCodeTheme::ACCENT_BLUE);
    y += 30;
    DrawText("  while (!gameOver)", editorX, y, 16, VSCodeTheme::ACCENT_YELLOW);
    y += 30;
    DrawText("    play();", editorX, y, 16, VSCodeTheme::ACCENT_CYAN);
}

// ============================================
// PAUSE SCREEN
// ============================================
PauseScreen::PauseScreen() 
    : overlayAlpha(0), selectedOption(0) {
    pauseOptions = {"Resume", "Restart", "Main Menu", "Quit"};
}

PauseScreen::~PauseScreen() {}

void PauseScreen::OnEnter() {
    overlayAlpha = 0;
    selectedOption = 0;
}

void PauseScreen::OnExit() {
    overlayAlpha = 0;
}

void PauseScreen::Update() {
    if (overlayAlpha < 0.7f) overlayAlpha += 0.05f;
    
    if (IsKeyPressed(KEY_ESCAPE)) {
        if (onResume) onResume();
    }
    
    if (IsKeyPressed(KEY_DOWN)) {
        selectedOption = (selectedOption + 1) % pauseOptions.size();
    }
    if (IsKeyPressed(KEY_UP)) {
        selectedOption = (selectedOption - 1 + pauseOptions.size()) % pauseOptions.size();
    }
    if (IsKeyPressed(KEY_ENTER)) {
        switch (selectedOption) {
            case 0: if (onResume) onResume(); break;
            case 1: if (onRestart) onRestart(); break;
            case 2: if (onMainMenu) onMainMenu(); break;
            case 3: if (onQuit) onQuit(); break;
        }
    }
}

void PauseScreen::Draw() {
    // Overlay
    Color overlay = VSCodeTheme::BG_DARK;
    overlay.a = (unsigned char)(overlayAlpha * 200);
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), overlay);
    
    // Panel
    int panelW = 400;
    int panelH = 350;
    int panelX = (GetScreenWidth() - panelW) / 2;
    int panelY = (GetScreenHeight() - panelH) / 2;
    
    DrawRectangle(panelX, panelY, panelW, panelH, VSCodeTheme::BG_SIDEBAR);
    DrawRectangleLinesEx({(float)panelX, (float)panelY, (float)panelW, (float)panelH}, 2, VSCodeTheme::BORDER);
    
    // Title
    const char* title = "PAUSED";
    int titleW = MeasureText(title, 36);
    DrawText(title, panelX + (panelW - titleW) / 2, panelY + 30, 36, VSCodeTheme::ACCENT_CYAN);
    
    // Options
    int startY = panelY + 100;
    for (size_t i = 0; i < pauseOptions.size(); i++) {
        int btnY = startY + i * 55;
        bool isSelected = (i == selectedOption);
        
        Color bg = isSelected ? VSCodeTheme::BG_HOVER : VSCodeTheme::BG_WIDGET;
        DrawRectangle(panelX + 50, btnY, panelW - 100, 45, bg);
        
        if (isSelected) {
            DrawRectangle(panelX + 50, btnY, 4, 45, VSCodeTheme::ACCENT_CYAN);
        }
        
        int textW = MeasureText(pauseOptions[i].c_str(), 22);
        Color textColor = isSelected ? WHITE : VSCodeTheme::TEXT_MUTED;
        DrawText(pauseOptions[i].c_str(), panelX + (panelW - textW) / 2, btnY + 10, 22, textColor);
    }
    
    // Help
    const char* help = "ESC to resume";
    int helpW = MeasureText(help, 16);
    DrawText(help, panelX + (panelW - helpW) / 2, panelY + panelH - 40, 16, VSCodeTheme::TEXT_MUTED);
}
