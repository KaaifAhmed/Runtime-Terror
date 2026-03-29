#pragma once
#include "ui_system.h"
#include "leaderboard.h"
#include <vector>
#include <functional>

// ============================================
// SCREEN TYPES
// ============================================
enum class ScreenType
{
    NONE,
    MAIN_MENU,
    PLAYING,
    PAUSED,
    GAME_OVER,
    LEADERBOARD,
    SETTINGS,
    CREDITS
};

// ============================================
// MAIN MENU SCREEN
// ============================================
class MainMenuScreen : public Screen
{
private:
    float logoY;
    float logoPulse;
    int selectedOption;
    float animationTime;
    std::vector<std::string> menuOptions;
    std::vector<std::function<void()>> callbacks;
    float *scrollOffset; // For parallax effect

public:
    MainMenuScreen();
    ~MainMenuScreen();

    void Update() override;
    void Draw() override;
    void OnEnter() override;
    void OnExit() override;

    void SetCallbacks(std::function<void()> onPlay,
                      std::function<void()> onLeaderboard,
                      std::function<void()> onCredits,
                      std::function<void()> onExit);

private:
    void DrawLogo();
    void DrawMenuOptions();
    void DrawBackgroundCode();
    void DrawSidebar();
    void DrawStatusBar();
    void DrawAnimatedBackground();
    void DrawInfoBar();
};

// ============================================
// PAUSE SCREEN
// ============================================
class PauseScreen : public Screen
{
private:
    float overlayAlpha;
    int selectedOption;
    std::vector<std::string> pauseOptions;

public:
    PauseScreen();
    ~PauseScreen();

    void Update() override;
    void Draw() override;
    void OnEnter() override;
    void OnExit() override;

    std::function<void()> onResume;
    std::function<void()> onRestart;
    std::function<void()> onMainMenu;
    std::function<void()> onQuit;

private:
    void DrawOverlay();
    void DrawPausePanel();
    void DrawCurrentStats(int score, int time);
};


// ============================================
// GAME OVER SCREEN (Windows/Terminal Error Style)
// ============================================
class GameOverScreen : public Screen
{
public:
    enum class Style
    {
        TERMINAL_ERROR,
        WINDOWS_BSOD,
        COMPILER_ERROR,
        SYNTAX_ERROR
    };

private:
    struct TerminalLine
    {
        std::string text;
        Color color;
        bool isShakeTrigger;
    };

    // Style & timing
    Style style;
    float displayTimer;

    // Error content
    std::string errorMessage;
    std::string errorDetail;
    std::string errorCode;

    // Results
    int finalScore;
    int finalLines;
    int finalTime;
    bool isHighScore;

    // Name input
    bool showNameInput;
    char playerName[32];
    int nameLength;

    // Typewriter effect
    float typewriterTimer;
    int typewriterLineIndex;
    int typewriterCharIndex;
    bool typewriterDone;
    float doneTimer; // how long since typewriter finished

    // Screen shake & flash
    float shakeTimer;
    float flashTimer;

public:
    GameOverScreen();
    ~GameOverScreen();

    void Update() override;
    void Draw() override;
    void OnEnter() override;
    void OnExit() override;

    void SetResults(int score, int lines, int time, bool highScore);
    void SetStyle(Style s);
    std::string GetPlayerName() const { return std::string(playerName); }

    std::function<void()> onRestart;
    std::function<void()> onMainMenu;
    std::function<void()> onQuit;
    std::function<void(std::string)> onSubmitScore;

private:
    void DrawTerminalError();
    void DrawWindowsBSOD();
    void DrawCompilerError();
    void DrawSyntaxError();
    void DrawNameInput();
    void GenerateRandomError();
};

// ============================================
// LEADERBOARD SCREEN
// ============================================
class LeaderboardScreen : public Screen
{
private:
    float scrollY;
    int selectedIndex;
    LeaderboardLinkedList *leaderboard;
    bool showEmptyMessage;

public:
    LeaderboardScreen();
    ~LeaderboardScreen();

    void Update() override;
    void Draw() override;
    void OnEnter() override;
    void OnExit() override;

    void SetLeaderboard(LeaderboardLinkedList *lb);

    std::function<void()> onBack;
    std::function<void()> onReset;
    std::function<void()> onExport;

private:
    void DrawLeaderboardList();
    void DrawRankEntry(int rank, const std::string &name, int score, int y);
    void DrawEmptyState();
    void DrawHeader();
    void DrawFooter();
};

// ============================================
// CREDITS SCREEN
// ============================================
class CreditsScreen : public Screen
{
private:
    float scrollY;
    float scrollSpeed;
    std::vector<std::string> credits;

public:
    CreditsScreen();
    ~CreditsScreen();

    void Update() override;
    void Draw() override;
    void OnEnter() override;
    void OnExit() override;

    std::function<void()> onBack;

private:
    void DrawScrollingCredits();
};

// ============================================
// UI SCREEN MANAGER
// ============================================
class UIScreenManager
{
private:
    ScreenType currentScreen;
    ScreenType previousScreen;

    MainMenuScreen *mainMenu;
    PauseScreen *pauseScreen;
    GameOverScreen *gameOverScreen;
    LeaderboardScreen *leaderboardScreen;
    CreditsScreen *creditsScreen;

    LeaderboardLinkedList *leaderboard;

public:
    UIScreenManager();
    ~UIScreenManager();

    void Initialize();
    void Shutdown();

    // State transitions
    void SwitchTo(ScreenType screen);
    void GoBack();

    // Screen access
    MainMenuScreen *GetMainMenu() { return mainMenu; }
    PauseScreen *GetPauseScreen() { return pauseScreen; }
    GameOverScreen *GetGameOverScreen() { return gameOverScreen; }
    LeaderboardScreen *GetLeaderboardScreen() { return leaderboardScreen; }
    CreditsScreen *GetCreditsScreen() { return creditsScreen; }

    // Current screen operations
    void UpdateCurrent();
    void DrawCurrent();
    ScreenType GetCurrentScreen() const { return currentScreen; }
    ScreenType GetPreviousScreen() const { return previousScreen; }

    // Leaderboard access
    void SetLeaderboard(LeaderboardLinkedList *lb);
    LeaderboardLinkedList *GetLeaderboard() const { return leaderboard; }
};
