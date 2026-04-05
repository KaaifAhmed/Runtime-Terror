#pragma once
#include "ui_system.h"
#include "leaderboard.h"
#include <vector>
#include <functional>

enum class ScreenType { NONE, MAIN_MENU, PLAYING, PAUSED, GAME_OVER, LEADERBOARD, SETTINGS, CREDITS };

// ============================================
// MAIN MENU SCREEN
// ============================================
class MainMenuScreen : public Screen {
    float logoY, logoPulse;
    int selectedOption;
    float animationTime;
    std::vector<std::string> menuOptions;
    std::vector<std::function<void()>> callbacks;

public:
    MainMenuScreen();
    ~MainMenuScreen();
    void Update() override;
    void Draw() override;
    void OnEnter() override;
    void OnExit() override;
    void SetCallbacks(std::function<void()> onPlay, std::function<void()> onLeaderboard,
                      std::function<void()> onCredits, std::function<void()> onExit);
};

// ============================================
// PAUSE SCREEN
// ============================================
class PauseScreen : public Screen {
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

    std::function<void()> onResume, onRestart, onMainMenu, onQuit;
};

// ============================================
// GAME OVER SCREEN
// ============================================
class GameOverScreen : public Screen {
public:
    enum class Style { TERMINAL_ERROR, WINDOWS_BSOD, COMPILER_ERROR, SYNTAX_ERROR };

private:
    struct TerminalLine { std::string text; Color color; bool isShakeTrigger; };

    Style style;
    float displayTimer;
    std::string errorMessage, errorDetail, errorCode;
    int finalScore, finalLines, finalTime;
    bool isHighScore, showNameInput;
    char playerName[32];
    int nameLength;
    float typewriterTimer;
    int typewriterLineIndex, typewriterCharIndex;
    bool typewriterDone;
    float doneTimer, shakeTimer, flashTimer;

public:
    GameOverScreen();
    ~GameOverScreen();
    void Update() override;
    void Draw() override;
    void OnEnter() override;
    void OnExit() override;
    void SetResults(int score, int lines, int time, bool highScore);
    void SetStyle(Style s);
    void SetErrorOverride(const std::string& msg, const std::string& detail, const std::string& code);

    std::function<void()> onRestart, onMainMenu, onQuit;
    std::function<void(std::string)> onSubmitScore;

private:
    void DrawTerminalError();
    void DrawNameInput();
    void GenerateRandomError();
};

// ============================================
// LEADERBOARD SCREEN
// ============================================
class LeaderboardScreen : public Screen {
    float scrollY;
    int selectedIndex;
    LeaderboardLinkedList* leaderboard;
    bool showEmptyMessage;

public:
    LeaderboardScreen();
    ~LeaderboardScreen();
    void Update() override;
    void Draw() override;
    void OnEnter() override;
    void OnExit() override;
    void SetLeaderboard(LeaderboardLinkedList* lb);

    std::function<void()> onBack, onReset, onExport;

private:
    void DrawLeaderboardList();
    void DrawRankEntry(int rank, const std::string& name, int score, int y);
    void DrawEmptyState();
    void DrawHeader();
    void DrawFooter();
};

// ============================================
// CREDITS SCREEN
// ============================================
class CreditsScreen : public Screen {
    float scrollY, scrollSpeed;
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
class UIScreenManager {
    ScreenType currentScreen, previousScreen;
    MainMenuScreen*    mainMenu;
    PauseScreen*       pauseScreen;
    GameOverScreen*    gameOverScreen;
    LeaderboardScreen* leaderboardScreen;
    CreditsScreen*     creditsScreen;
    LeaderboardLinkedList* leaderboard;

public:
    UIScreenManager();
    ~UIScreenManager();
    void Initialize();
    void Shutdown();
    void SwitchTo(ScreenType screen);
    void GoBack();
    void UpdateCurrent();
    void DrawCurrent();
    void SetLeaderboard(LeaderboardLinkedList* lb);

    MainMenuScreen*    GetMainMenu()        { return mainMenu; }
    PauseScreen*       GetPauseScreen()     { return pauseScreen; }
    GameOverScreen*    GetGameOverScreen()  { return gameOverScreen; }
    LeaderboardScreen* GetLeaderboardScreen() { return leaderboardScreen; }
    CreditsScreen*     GetCreditsScreen()   { return creditsScreen; }
    ScreenType         GetCurrentScreen() const  { return currentScreen; }
    ScreenType         GetPreviousScreen() const { return previousScreen; }
    LeaderboardLinkedList* GetLeaderboard() const { return leaderboard; }
};
