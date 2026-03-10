#include "menu.h"
#include "constants.h"
#include <string>

// Check if mouse is over button
bool IsMouseOverButton(Button& btn) {
    Vector2 mousePos = GetMousePosition();
    return (mousePos.x >= btn.x && mousePos.x <= btn.x + btn.width &&
            mousePos.y >= btn.y && mousePos.y <= btn.y + btn.height);
}

// Check if button is clicked
bool IsButtonClicked(Button& btn) {
    return IsMouseOverButton(btn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

// Draw a simple button
void DrawButton(Button& btn) {
    Color bgColor = IsMouseOverButton(btn) ? btn.hoverColor : btn.color;
    
    // Draw button background
    DrawRectangle(btn.x, btn.y, btn.width, btn.height, bgColor);
    
    // Draw button border
    DrawRectangleLines(btn.x, btn.y, btn.width, btn.height, VSCode_TEXT);
    
    // Draw button text (centered)
    int textWidth = MeasureText(btn.text, 30);
    int textX = btn.x + (btn.width - textWidth) / 2;
    int textY = btn.y + (btn.height - 30) / 2;
    DrawText(btn.text, textX, textY, 30, btn.textColor);
}

// Draw main menu
void DrawMainMenu(Button& playBtn, Button& quitBtn) {
    // Clear background with VS Code dark theme
    ClearBackground(VSCode_BACKGROUND);
    
    // Draw title
    const char* title = "RUNTIME TERROR";
    int titleWidth = MeasureText(title, 60);
    DrawText(title, (SCREEN_WIDTH - titleWidth) / 2, 200, 60, VSCode_BLUE);
    
    // Draw subtitle
    const char* subtitle = "A Programming Adventure";
    int subWidth = MeasureText(subtitle, 30);
    DrawText(subtitle, (SCREEN_WIDTH - subWidth) / 2, 280, 30, VSCode_TEXT);
    
    // Draw buttons
    DrawButton(playBtn);
    DrawButton(quitBtn);
    
    // Draw instructions at bottom
    const char* instructions = "SPACE to jump    |    ESC for pause menu    |    CTRL+Z to rewind";
    int instWidth = MeasureText(instructions, 20);
    DrawText(instructions, (SCREEN_WIDTH - instWidth) / 2, SCREEN_HEIGHT - 100, 20, VSCode_GREEN);
}

// Draw pause menu
void DrawPauseMenu(Button& resumeBtn, Button& quitBtn) {
    // Semi-transparent overlay
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, {0, 0, 0, 200});
    
    // Draw pause title
    const char* title = "PAUSED";
    int titleWidth = MeasureText(title, 50);
    DrawText(title, (SCREEN_WIDTH - titleWidth) / 2, 250, 50, VSCode_YELLOW);
    
    // Draw buttons
    DrawButton(resumeBtn);
    DrawButton(quitBtn);
}

// Draw game over screen
void DrawGameOverScreen(int score, Button& menuBtn, Button& quitBtn) {
    // Clear background
    ClearBackground(VSCode_SIDEBAR);
    
    // Draw error message box
    int boxWidth = 600;
    int boxHeight = 400;
    int boxX = (SCREEN_WIDTH - boxWidth) / 2;
    int boxY = (SCREEN_HEIGHT - boxHeight) / 2;
    
    // Red title bar
    DrawRectangle(boxX, boxY, boxWidth, 50, VSCode_RED);
    DrawText("X", boxX + boxWidth - 40, boxY + 10, 30, WHITE);
    DrawText("ERROR", boxX + 20, boxY + 10, 30, WHITE);
    
    // Gray body
    DrawRectangle(boxX, boxY + 50, boxWidth, boxHeight - 50, VSCode_SIDEBAR);
    DrawRectangleLines(boxX, boxY, boxWidth, boxHeight, VSCode_RED);
    
    // Error message
    DrawText("FATAL EXCEPTION", boxX + 50, boxY + 80, 35, VSCode_RED);
    DrawText("Segmentation Fault: Player crashed!", boxX + 50, boxY + 130, 25, VSCode_TEXT);
    
    // Score
    std::string scoreText = "Lines of Code: " + std::to_string(score);
    DrawText(scoreText.c_str(), boxX + 50, boxY + 180, 30, VSCode_GREEN);
    
    // Buttons
    DrawButton(menuBtn);
    DrawButton(quitBtn);
}
