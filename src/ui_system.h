#pragma once
#include "raylib.h"
#include <string>
#include <vector>
#include <functional>
#include "constants.h"

// ============================================
// VS CODE DARK+ THEME
// ============================================
namespace VSCodeTheme {
    const Color BG_DARK     = {30, 30, 30, 255};
    const Color BG_EDITOR   = {30, 30, 30, 255};
    const Color BG_SIDEBAR  = {37, 37, 38, 255};
    const Color BG_PANEL    = {45, 45, 48, 255};
    const Color BG_WIDGET   = {45, 45, 48, 255};
    const Color BG_HOVER    = {42, 45, 46, 255};
    const Color BG_ACTIVE   = {55, 55, 61, 255};
    const Color BORDER      = {60, 60, 60, 255};
    const Color BORDER_FOCUS= {0, 122, 204, 255};
    const Color TEXT_NORMAL = {212, 212, 212, 255};
    const Color TEXT_MUTED  = {133, 133, 133, 255};
    const Color TEXT_DISABLED={133, 133, 133, 255};
    const Color ACCENT_BLUE = {0, 122, 204, 255};
    const Color ACCENT_CYAN = {0, 122, 204, 255};
    const Color ACCENT_GREEN= {106, 153, 85, 255};
    const Color ACCENT_YELLOW={220, 205, 96, 255};
    const Color ACCENT_ORANGE={255, 140, 0, 255};
    const Color ACCENT_RED  = {241, 76, 76, 255};
    const Color ACCENT_PURPLE={192, 128, 255, 255};
    const Color STATUS_BG   = {0, 122, 204, 255};
    const Color TERMINAL_BG = {30, 30, 30, 255};
    const Color TERMINAL_RED= {241, 76, 76, 255};
    const Color TERMINAL_GREEN={106, 153, 85, 255};
    const Color TERMINAL_YELLOW={220, 205, 96, 255};
    const Color SELECTION   = {38, 79, 120, 255};
    const Color BUTTON_BG   = {14, 99, 156, 255};
    const Color BUTTON_HOVER= {17, 119, 187, 255};
}

// ============================================
// UI STYLE CONSTANTS
// ============================================
namespace UIStyle {
    const int PADDING_SMALL  = 8;
    const int PADDING_MEDIUM = 16;
    const int PADDING_LARGE  = 24;
    const int BUTTON_HEIGHT  = 48;
    const int BUTTON_WIDTH   = 280;
    const int BORDER_WIDTH   = 2;
    const int FONT_SIZE_SMALL  = 16;
    const int FONT_SIZE_MEDIUM = 20;
    const int FONT_SIZE_LARGE  = 28;
    const int FONT_SIZE_TITLE  = 48;
}

// ============================================
// BUTTON COMPONENT
// ============================================
struct Button {
    Rectangle bounds;
    std::string text;
    bool hovered = false, pressed = false, disabled = false;
    Color bgColor, textColor, borderColor;
    std::function<void()> onClick;

    Button(float x, float y, float w, float h, const std::string& txt)
        : bounds{x, y, w, h}, text(txt),
          bgColor(VSCodeTheme::BG_WIDGET), textColor(VSCodeTheme::TEXT_NORMAL),
          borderColor(VSCodeTheme::BORDER) {}

    void Update();
    void Draw();
    bool IsClicked() { return !disabled && pressed && IsMouseButtonReleased(MOUSE_LEFT_BUTTON); }
};

// ============================================
// SCREEN BASE CLASS
// ============================================
class Screen {
protected:
    std::vector<Button> buttons;

public:
    virtual ~Screen() = default;
    virtual void Update() = 0;
    virtual void Draw()   = 0;
    virtual void OnEnter() {}
    virtual void OnExit()  {}
};

// ============================================
// UI HELPER FUNCTIONS
// ============================================
namespace UI {
    void DrawVSCideBackground(const char* activeTab = "main.cpp");
    void DrawActivityBar(float x, float y, float h, int selectedIndex);
    void DrawProgressBar(float x, float y, float w, float h, float progress,
                         Color fillColor = VSCodeTheme::ACCENT_BLUE,
                         Color bgColor   = VSCodeTheme::BG_WIDGET);
    void DrawBlinkingCursor(float x, float y, float h, Color color = VSCodeTheme::ACCENT_BLUE);
    void DrawTextAligned(const std::string& text, float x, float y, int fontSize, Color color, int align = 0);
    float PulseAlpha(float speed = 3.0f, float minAlpha = 0.4f, float maxAlpha = 1.0f);
    float SmoothStep(float t);
    Rectangle CenteredRect(float w, float h, float centerX, float centerY);
    bool MouseInRect(Rectangle rect);
}
