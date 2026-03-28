#pragma once
#include "raylib.h"
#include <string>
#include <vector>
#include <functional>

// ============================================
// VS CODE DARK+ THEME COLORS
// ============================================
namespace VSCodeTheme {
    // Background colors
    const Color BG_DARK = {9, 9, 9, 255};           // #090909 - Deepest background
    const Color BG_EDITOR = {30, 30, 30, 255};      // #1E1E1E - Editor background
    const Color BG_SIDEBAR = {37, 37, 38, 255};     // #252526 - Sidebar
    const Color BG_PANEL = {51, 51, 51, 255};        // #333333 - Panel/header
    const Color BG_WIDGET = {60, 60, 60, 255};      // #3C3C3C - Widget/button
    const Color BG_HOVER = {62, 62, 66, 255};        // #3E3E42 - Hover state
    const Color BG_ACTIVE = {55, 55, 61, 255};       // #37373D - Active/selected
    
    // Border colors
    const Color BORDER = {81, 81, 81, 255};          // #515151 - Borders
    const Color BORDER_FOCUS = {0, 122, 204, 255};   // #007ACC - Focus border (blue)
    
    // Text colors
    const Color TEXT_NORMAL = {204, 204, 204, 255};  // #CCCCCC - Normal text
    const Color TEXT_MUTED = {153, 153, 153, 255};   // #999999 - Muted/secondary
    const Color TEXT_DISABLED = {102, 102, 102, 255};// #666666 - Disabled
    
    // Syntax/Accent colors (VS Code Dark+)
    const Color ACCENT_BLUE = {0, 148, 255, 255};    // #0094FF - Keywords/Control
    const Color ACCENT_CYAN = {0, 210, 210, 255};     // #00D2D2 - Functions
    const Color ACCENT_GREEN = {96, 196, 96, 255};    // #60C460 - Strings/Success
    const Color ACCENT_YELLOW = {220, 205, 96, 255};  // #DCCD60 - Types
    const Color ACCENT_ORANGE = {255, 140, 0, 255};   // #FF8C00 - Warnings
    const Color ACCENT_RED = {255, 96, 96, 255};      // #FF6060 - Errors
    const Color ACCENT_PURPLE = {192, 128, 255, 255}; // #C080FF - Numbers
    const Color ACCENT_PINK = {255, 128, 192, 255};   // #FF80C0 - Special
    
    // Status bar colors
    const Color STATUS_BG = {0, 122, 204, 255};      // #007ACC - Normal status
    const Color STATUS_ERROR = {192, 48, 48, 255};    // #C03030 - Error status
    const Color STATUS_WARNING = {192, 128, 0, 255};  // #C08000 - Warning status
    
    // Terminal colors
    const Color TERMINAL_BG = {24, 24, 24, 255};      // #181818 - Terminal background
    const Color TERMINAL_RED = {205, 49, 49, 255};    // Terminal red
    const Color TERMINAL_GREEN = {13, 188, 121, 255}; // Terminal green
    const Color TERMINAL_YELLOW = {229, 229, 16, 255};// Terminal yellow
    const Color TERMINAL_BLUE = {36, 114, 200, 255}; // Terminal blue
}

// ============================================
// UI STYLE CONSTANTS
// ============================================
namespace UIStyle {
    const int PADDING_SMALL = 8;
    const int PADDING_MEDIUM = 16;
    const int PADDING_LARGE = 24;
    const int BUTTON_HEIGHT = 48;
    const int BUTTON_WIDTH = 280;
    const int BORDER_WIDTH = 2;
    const int CORNER_RADIUS = 4;
    const int FONT_SIZE_SMALL = 16;
    const int FONT_SIZE_MEDIUM = 20;
    const int FONT_SIZE_LARGE = 28;
    const int FONT_SIZE_TITLE = 48;
    const int FONT_SIZE_HUGE = 72;
}

// ============================================
// BUTTON COMPONENT
// ============================================
struct Button {
    Rectangle bounds;
    std::string text;
    bool hovered;
    bool pressed;
    bool disabled;
    Color bgColor;
    Color textColor;
    Color borderColor;
    std::function<void()> onClick;
    
    Button(float x, float y, float w, float h, const std::string& txt)
        : bounds{x, y, w, h}, text(txt), hovered(false), pressed(false), disabled(false),
          bgColor(VSCodeTheme::BG_WIDGET), textColor(VSCodeTheme::TEXT_NORMAL), 
          borderColor(VSCodeTheme::BORDER) {}
    
    void Update();
    void Draw();
    bool IsClicked() { return !disabled && pressed && IsMouseButtonReleased(MOUSE_LEFT_BUTTON); }
};

// ============================================
// PANEL/WINDOW COMPONENT
// ============================================
struct Panel {
    Rectangle bounds;
    std::string title;
    bool hasTitleBar;
    Color bgColor;
    Color borderColor;
    
    Panel(float x, float y, float w, float h, const std::string& ttl = "", bool titleBar = false)
        : bounds{x, y, w, h}, title(ttl), hasTitleBar(titleBar),
          bgColor(VSCodeTheme::BG_SIDEBAR), borderColor(VSCodeTheme::BORDER) {}
    
    void Draw();
    void DrawVSCodeWindow(); // VS Code style window with title bar
    void DrawTerminalWindow(); // Terminal style window
    void DrawErrorWindow(); // Windows BSOD/Error style
};

// ============================================
// MENU ITEM / LIST COMPONENT
// ============================================
struct MenuItem {
    std::string text;
    std::string shortcut;
    bool selected;
    Color color;
    
    MenuItem(const std::string& t, const std::string& sc = "", Color c = VSCodeTheme::TEXT_NORMAL)
        : text(t), shortcut(sc), selected(false), color(c) {}
};

// ============================================
// SCREEN BASE CLASS
// ============================================
class Screen {
protected:
    std::vector<Button> buttons;
    std::vector<Panel> panels;
    
public:
    virtual ~Screen() = default;
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual void OnEnter() {}  // Called when entering this screen
    virtual void OnExit() {}   // Called when exiting this screen
};

// ============================================
// UI HELPER FUNCTIONS
// ============================================
namespace UI {
    // Drawing helpers
    void DrawVSCideBackground();
    void DrawGridBackground(Color lineColor = {50, 50, 50, 100});
    void DrawCodeLine(float x, float y, const std::string& lineNumber, const std::string& code, 
                      Color lineNumColor = VSCodeTheme::TEXT_MUTED, 
                      Color codeColor = VSCodeTheme::TEXT_NORMAL);
    void DrawStatusBar(float y, const std::string& leftText, const std::string& rightText, 
                       Color bgColor = VSCodeTheme::STATUS_BG);
    void DrawActivityBar(float x, float y, float h, int selectedIndex);
    void DrawTabBar(float x, float y, float w, const std::vector<std::string>& tabs, int activeTab);
    void DrawBreadcrumbs(float x, float y, const std::vector<std::string>& crumbs);
    void DrawProgressBar(float x, float y, float w, float h, float progress, 
                         Color fillColor = VSCodeTheme::ACCENT_BLUE,
                         Color bgColor = VSCodeTheme::BG_WIDGET);
    void DrawBlinkingCursor(float x, float y, float h, Color color = VSCodeTheme::ACCENT_BLUE);
    
    // Text helpers
    void DrawTextAligned(const std::string& text, float x, float y, int fontSize, 
                         Color color, int align = 0); // 0=left, 1=center, 2=right
    void DrawMonospaceText(const std::string& text, float x, float y, int fontSize, Color color);
    
    // Animation helpers
    float PulseAlpha(float speed = 3.0f, float minAlpha = 0.4f, float maxAlpha = 1.0f);
    float SmoothStep(float t);
    
    // Utility
    Rectangle CenteredRect(float w, float h, float centerX, float centerY);
    bool MouseInRect(Rectangle rect);
}
