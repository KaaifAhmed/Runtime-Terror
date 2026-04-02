#include "ui_system.h"
#include <cmath>

// ============================================
// BUTTON
// ============================================
void Button::Update() {
    Vector2 mousePos = GetMousePosition();
    hovered = CheckCollisionPointRec(mousePos, bounds);
    pressed = hovered && IsMouseButtonDown(MOUSE_LEFT_BUTTON);
    bgColor = (hovered && !disabled) ? VSCodeTheme::BUTTON_HOVER
              : (disabled ? VSCodeTheme::BG_WIDGET : VSCodeTheme::BUTTON_BG);
    borderColor = bgColor;
}

void Button::Draw() {
    DrawRectangleRounded(bounds, 0.1f, 4, bgColor);
    int textWidth = MeasureVSText(text.c_str(), UIStyle::FONT_SIZE_MEDIUM);
    float textX = bounds.x + (bounds.width  - textWidth) / 2;
    float textY = bounds.y + (bounds.height - UIStyle::FONT_SIZE_MEDIUM) / 2;
    DrawVSText(text.c_str(), (int)textX, (int)textY, UIStyle::FONT_SIZE_MEDIUM,
               disabled ? VSCodeTheme::TEXT_DISABLED : WHITE);
}

// ============================================
// VS CODE BACKGROUND
// ============================================
void UI::DrawVSCideBackground(const char* activeTab) {
    // Top menu bar
    DrawRectangle(0, 0, GetScreenWidth(), 36, VSCodeTheme::BG_PANEL);
    DrawVSText("File    Edit    Selection    View    Go    Run    Terminal    Help",
               40, 10, 14, VSCodeTheme::TEXT_NORMAL);
    DrawTextAligned("Runtime-Terror - Visual Studio Code", GetScreenWidth() / 2, 10, 14,
                    VSCodeTheme::TEXT_NORMAL, 1);

    // Activity bar (left)
    DrawRectangle(0, 36, 56, GetScreenHeight() - 36, VSCodeTheme::BG_SIDEBAR);
    DrawActivityBar(0, 36, GetScreenHeight() - 36, 0);

    // Tab bar
    DrawRectangle(56, 36, GetScreenWidth() - 56, 35, VSCodeTheme::BG_PANEL);
    DrawRectangle(56, 36, 180, 35, VSCodeTheme::BG_EDITOR);
    DrawRectangle(56, 36, 180, 2, VSCodeTheme::ACCENT_BLUE);
    DrawVSText(activeTab, 78, 46, 14, VSCodeTheme::TEXT_NORMAL);
}

void UI::DrawActivityBar(float x, float y, float h, int selectedIndex) {
    if (sideBarTex.id != 0) {
        Rectangle source = {0, 0, (float)sideBarTex.width, (float)sideBarTex.height};
        Rectangle dest   = {x, y, 56, h};
        DrawTexturePro(sideBarTex, source, dest, {0, 0}, 0.0f, WHITE);
    } else {
        const char* icons[] = {"Files","Search","Git","Debug","Ext"};
        float itemH = h / 5;
        for (int i = 0; i < 5; i++) {
            Rectangle item = {x, y + i * itemH, 56, itemH};
            if (i == selectedIndex) {
                DrawRectangleRec(item, VSCodeTheme::BG_ACTIVE);
                DrawLineEx({x, item.y}, {x, item.y + item.height}, 2, VSCodeTheme::ACCENT_BLUE);
            }
            Color iconColor = (i == selectedIndex) ? VSCodeTheme::TEXT_NORMAL : VSCodeTheme::TEXT_MUTED;
            int textW = MeasureVSText(icons[i], UIStyle::FONT_SIZE_SMALL);
            DrawVSText(icons[i], (int)(x + (56 - textW) / 2), (int)(item.y + itemH / 2 - 8),
                       UIStyle::FONT_SIZE_SMALL, iconColor);
        }
    }
}

void UI::DrawProgressBar(float x, float y, float w, float h, float progress,
                         Color fillColor, Color bgColor) {
    DrawRectangle(x, y, w, h, bgColor);
    int fillW = (int)(w * progress);
    if (fillW > 0) DrawRectangle(x, y, fillW, h, fillColor);
    DrawRectangleLinesEx({x, y, w, h}, 1, VSCodeTheme::BORDER);
}

void UI::DrawBlinkingCursor(float x, float y, float h, Color color) {
    if ((int)(GetTime() * 2) % 2 == 0)
        DrawRectangle((int)x, (int)y, 2, (int)h, color);
}

void UI::DrawTextAligned(const std::string& text, float x, float y, int fontSize,
                         Color color, int align) {
    int textW = MeasureVSText(text.c_str(), fontSize);
    float finalX = (align == 1) ? x - textW / 2 : (align == 2) ? x - textW : x;
    DrawVSText(text.c_str(), (int)finalX, (int)y, fontSize, color);
}

float UI::PulseAlpha(float speed, float minAlpha, float maxAlpha) {
    float t = (sin(GetTime() * speed) + 1) / 2;
    return minAlpha + t * (maxAlpha - minAlpha);
}

float UI::SmoothStep(float t) { return t * t * (3 - 2 * t); }

Rectangle UI::CenteredRect(float w, float h, float centerX, float centerY) {
    return {centerX - w / 2, centerY - h / 2, w, h};
}

bool UI::MouseInRect(Rectangle rect) {
    return CheckCollisionPointRec(GetMousePosition(), rect);
}
