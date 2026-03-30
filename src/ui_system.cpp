#include "ui_system.h"
#include <cmath>

// ============================================
// BUTTON IMPLEMENTATION
// ============================================
void Button::Update() {
    Vector2 mousePos = GetMousePosition();
    hovered = CheckCollisionPointRec(mousePos, bounds);
    pressed = hovered && IsMouseButtonDown(MOUSE_LEFT_BUTTON);
    
    if (hovered && !disabled) {
        bgColor = pressed ? VSCodeTheme::BG_ACTIVE : VSCodeTheme::BG_HOVER;
        borderColor = VSCodeTheme::BORDER_FOCUS;
    } else {
        bgColor = disabled ? VSCodeTheme::BG_WIDGET : VSCodeTheme::BG_WIDGET;
        borderColor = VSCodeTheme::BORDER;
    }
}

void Button::Draw() {
    // Draw shadow
    DrawRectangle(bounds.x + 2, bounds.y + 2, bounds.width, bounds.height, {0, 0, 0, 50});
    
    // Draw background
    DrawRectangleRec(bounds, bgColor);
    
    // Draw border
    DrawRectangleLinesEx(bounds, UIStyle::BORDER_WIDTH, borderColor);
    
    // Draw text
    int textWidth = MeasureText(text.c_str(), UIStyle::FONT_SIZE_MEDIUM);
    float textX = bounds.x + (bounds.width - textWidth) / 2;
    float textY = bounds.y + (bounds.height - UIStyle::FONT_SIZE_MEDIUM) / 2;
    
    Color finalTextColor = disabled ? VSCodeTheme::TEXT_DISABLED : textColor;
    DrawText(text.c_str(), (int)textX, (int)textY, UIStyle::FONT_SIZE_MEDIUM, finalTextColor);
}

// ============================================
// PANEL IMPLEMENTATION
// ============================================
void Panel::Draw() {
    // Background
    DrawRectangleRec(bounds, bgColor);
    
    // Border
    DrawRectangleLinesEx(bounds, UIStyle::BORDER_WIDTH, borderColor);
}

void Panel::DrawVSCodeWindow() {
    // Main background
    DrawRectangleRec(bounds, bgColor);
    
    // Title bar
    float titleBarHeight = 36;
    Rectangle titleBar = {bounds.x, bounds.y, bounds.width, titleBarHeight};
    DrawRectangleRec(titleBar, VSCodeTheme::BG_PANEL);
    
    // Title bar bottom border
    DrawLineEx({bounds.x, bounds.y + titleBarHeight}, 
               {bounds.x + bounds.width, bounds.y + titleBarHeight}, 
               1, VSCodeTheme::BORDER);
    
    // Title text
    if (!title.empty()) {
        int titleWidth = MeasureText(title.c_str(), UIStyle::FONT_SIZE_SMALL);
        float titleX = bounds.x + (bounds.width - titleWidth) / 2;
        DrawText(title.c_str(), (int)titleX, (int)(bounds.y + 8), 
                 UIStyle::FONT_SIZE_SMALL, VSCodeTheme::TEXT_NORMAL);
    }
    
    // Window controls (dots on left like VS Code)
    float dotY = bounds.y + 14;
    DrawCircle((int)(bounds.x + 12), (int)dotY, 4, VSCodeTheme::ACCENT_RED);
    DrawCircle((int)(bounds.x + 24), (int)dotY, 4, VSCodeTheme::ACCENT_YELLOW);
    DrawCircle((int)(bounds.x + 36), (int)dotY, 4, VSCodeTheme::ACCENT_GREEN);
    
    // Main border
    DrawRectangleLinesEx(bounds, UIStyle::BORDER_WIDTH, borderColor);
}

void Panel::DrawTerminalWindow() {
    // Terminal background (darker)
    DrawRectangleRec(bounds, VSCodeTheme::TERMINAL_BG);
    
    // Title bar
    float titleBarHeight = 32;
    Rectangle titleBar = {bounds.x, bounds.y, bounds.width, titleBarHeight};
    DrawRectangleRec(titleBar, VSCodeTheme::BG_PANEL);
    
    // Terminal icon and title
    DrawText(">_", (int)(bounds.x + 12), (int)(bounds.y + 6), UIStyle::FONT_SIZE_MEDIUM, 
             VSCodeTheme::TEXT_NORMAL);
    
    if (!title.empty()) {
        DrawText(title.c_str(), (int)(bounds.x + 40), (int)(bounds.y + 7), 
                 UIStyle::FONT_SIZE_SMALL, VSCodeTheme::TEXT_NORMAL);
    }
    
    // Border
    DrawRectangleLinesEx(bounds, 1, VSCodeTheme::BORDER);
}

void Panel::DrawErrorWindow() {
    // White/light background for Windows error style
    DrawRectangleRec(bounds, {240, 240, 240, 255});
    
    // Blue title bar (classic Windows)
    float titleBarHeight = 30;
    Rectangle titleBar = {bounds.x, bounds.y, bounds.width, titleBarHeight};
    DrawRectangleRec(titleBar, {0, 0, 128, 255}); // Dark blue
    
    // Title text (white on blue)
    if (!title.empty()) {
        DrawText(title.c_str(), (int)(bounds.x + 8), (int)(bounds.y + 6), 
                 UIStyle::FONT_SIZE_SMALL, WHITE);
    }
    
    // 3D border effect
    DrawLineEx({bounds.x, bounds.y}, {bounds.x + bounds.width, bounds.y}, 2, WHITE);
    DrawLineEx({bounds.x, bounds.y}, {bounds.x, bounds.y + bounds.height}, 2, WHITE);
    DrawLineEx({bounds.x + bounds.width, bounds.y}, {bounds.x + bounds.width, bounds.y + bounds.height}, 2, {128, 128, 128, 255});
    DrawLineEx({bounds.x, bounds.y + bounds.height}, {bounds.x + bounds.width, bounds.y + bounds.height}, 2, {128, 128, 128, 255});
}

// ============================================
// UI HELPER FUNCTIONS
// ============================================
void UI::DrawVSCideBackground() {
    // Main editor background
    ClearBackground(VSCodeTheme::BG_EDITOR);
    
    // Activity bar on left (narrow)
    DrawRectangle(0, 0, 48, GetScreenHeight(), VSCodeTheme::BG_SIDEBAR);
    
    // Sidebar area
    DrawRectangle(48, 0, 250, GetScreenHeight(), VSCodeTheme::BG_SIDEBAR);
    
    // Status bar at bottom
    DrawRectangle(0, GetScreenHeight() - 22, GetScreenWidth(), 22, VSCodeTheme::BG_PANEL);
}

void UI::DrawGridBackground(Color lineColor) {
    const int gridSize = 40;
    for (int x = 0; x < GetScreenWidth(); x += gridSize) {
        DrawLine(x, 0, x, GetScreenHeight(), lineColor);
    }
    for (int y = 0; y < GetScreenHeight(); y += gridSize) {
        DrawLine(0, y, GetScreenWidth(), y, lineColor);
    }
}

void UI::DrawCodeLine(float x, float y, const std::string& lineNumber, 
                      const std::string& code, Color lineNumColor, Color codeColor) {
    // Line number
    DrawText(lineNumber.c_str(), (int)x, (int)y, UIStyle::FONT_SIZE_SMALL, lineNumColor);
    // Code (indented after line number)
    DrawText(code.c_str(), (int)(x + 50), (int)y, UIStyle::FONT_SIZE_SMALL, codeColor);
}

void UI::DrawStatusBar(float y, const std::string& leftText, const std::string& rightText, 
                       Color bgColor) {
    DrawRectangle(0, (int)y, GetScreenWidth(), 22, bgColor);
    DrawText(leftText.c_str(), 10, (int)y + 3, UIStyle::FONT_SIZE_SMALL, WHITE);
    int rightWidth = MeasureText(rightText.c_str(), UIStyle::FONT_SIZE_SMALL);
    DrawText(rightText.c_str(), GetScreenWidth() - rightWidth - 10, (int)y + 3, 
             UIStyle::FONT_SIZE_SMALL, WHITE);
}

void UI::DrawActivityBar(float x, float y, float h, int selectedIndex) {
    const char* icons[] = {"Files", "Search", "Git", "Debug", "Ext"};
    float itemHeight = h / 5;
    
    for (int i = 0; i < 5; i++) {
        Rectangle item = {x, y + i * itemHeight, 48, itemHeight};
        if (i == selectedIndex) {
            DrawRectangleRec(item, VSCodeTheme::BG_ACTIVE);
            DrawLineEx({x, item.y}, {x, item.y + item.height}, 2, VSCodeTheme::ACCENT_BLUE);
        }
        Color iconColor = (i == selectedIndex) ? VSCodeTheme::TEXT_NORMAL : VSCodeTheme::TEXT_MUTED;
        int textW = MeasureText(icons[i], UIStyle::FONT_SIZE_SMALL);
        DrawText(icons[i], (int)(x + (48 - textW) / 2), (int)(item.y + itemHeight / 2 - 8), 
                 UIStyle::FONT_SIZE_SMALL, iconColor);
    }
}

void UI::DrawTabBar(float x, float y, float w, const std::vector<std::string>& tabs, int activeTab) {
    float tabWidth = w / tabs.size();
    
    for (size_t i = 0; i < tabs.size(); i++) {
        Rectangle tab = {x + i * tabWidth, y, tabWidth, 36};
        Color bg = (i == (size_t)activeTab) ? VSCodeTheme::BG_EDITOR : VSCodeTheme::BG_SIDEBAR;
        DrawRectangleRec(tab, bg);
        
        // Active tab indicator (top border)
        if (i == (size_t)activeTab) {
            DrawRectangle(tab.x, tab.y, tab.width, 2, VSCodeTheme::ACCENT_BLUE);
        }
        
        // Tab text
        int textW = MeasureText(tabs[i].c_str(), UIStyle::FONT_SIZE_SMALL);
        DrawText(tabs[i].c_str(), (int)(tab.x + (tabWidth - textW) / 2), (int)(tab.y + 10), 
                 UIStyle::FONT_SIZE_SMALL, VSCodeTheme::TEXT_NORMAL);
    }
    
    // Bottom border
    DrawLineEx({x, y + 36}, {x + w, y + 36}, 1, VSCodeTheme::BORDER);
}

void UI::DrawBreadcrumbs(float x, float y, const std::vector<std::string>& crumbs) {
    float currentX = x;
    for (size_t i = 0; i < crumbs.size(); i++) {
        DrawText(crumbs[i].c_str(), (int)currentX, (int)y, UIStyle::FONT_SIZE_SMALL, 
                 VSCodeTheme::TEXT_NORMAL);
        currentX += MeasureText(crumbs[i].c_str(), UIStyle::FONT_SIZE_SMALL);
        
        if (i < crumbs.size() - 1) {
            DrawText(" > ", (int)currentX, (int)y, UIStyle::FONT_SIZE_SMALL, VSCodeTheme::TEXT_MUTED);
            currentX += MeasureText(" > ", UIStyle::FONT_SIZE_SMALL);
        }
    }
}

void UI::DrawProgressBar(float x, float y, float w, float h, float progress,
                         Color fillColor, Color bgColor) {
    // Background
    DrawRectangle(x, y, w, h, bgColor);
    
    // Fill
    int fillWidth = (int)(w * progress);
    if (fillWidth > 0) {
        DrawRectangle(x, y, fillWidth, h, fillColor);
    }
    
    // Border
    DrawRectangleLinesEx({x, y, w, h}, 1, VSCodeTheme::BORDER);
}

void UI::DrawBlinkingCursor(float x, float y, float h, Color color) {
    if ((int)(GetTime() * 2) % 2 == 0) {
        DrawRectangle((int)x, (int)y, 2, (int)h, color);
    }
}

void UI::DrawTextAligned(const std::string& text, float x, float y, int fontSize, 
                         Color color, int align) {
    int textW = MeasureText(text.c_str(), fontSize);
    float finalX = x;
    
    if (align == 1) { // Center
        finalX = x - textW / 2;
    } else if (align == 2) { // Right
        finalX = x - textW;
    }
    
    DrawText(text.c_str(), (int)finalX, (int)y, fontSize, color);
}

void UI::DrawMonospaceText(const std::string& text, float x, float y, int fontSize, Color color) {
    // Simulate monospace by drawing each character with fixed width
    float charWidth = fontSize * 0.6f;
    for (size_t i = 0; i < text.length(); i++) {
        std::string ch = text.substr(i, 1);
        DrawText(ch.c_str(), (int)(x + i * charWidth), (int)y, fontSize, color);
    }
}

float UI::PulseAlpha(float speed, float minAlpha, float maxAlpha) {
    float t = (sin(GetTime() * speed) + 1) / 2; // 0 to 1
    return minAlpha + t * (maxAlpha - minAlpha);
}

float UI::SmoothStep(float t) {
    return t * t * (3 - 2 * t);
}

Rectangle UI::CenteredRect(float w, float h, float centerX, float centerY) {
    return {centerX - w / 2, centerY - h / 2, w, h};
}

bool UI::MouseInRect(Rectangle rect) {
    return CheckCollisionPointRec(GetMousePosition(), rect);
}
