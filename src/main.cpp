#include "constants.h"
#include "pickup.h"
#include "player.h"
#include "rewind.h"
#include "tiles.h"
#include "floatingtext.h"
#include "ui_system.h"
#include "leaderboard.h"
#include "screens.h"
#include <cmath>
#include <raylib.h>
#include <string>
#include <vector>
#include <functional>

using namespace std;

// Globals required by rendering helpers in constants.h
Font vsFont = {0};
Font codeFont = {0};
Texture2D sideBarTex = {0};
vector<FloatingText> FloatingTextSystem::popups;

struct Game
{
    Player player;
    vector<Tile*> tiles;
    vector<Pickup> pickups;
    RewindBuffer rewindSys{REWIND_SECS * FPS};

    UIScreenManager* uiManager = nullptr;
    MainMenuScreen* mainMenu = nullptr;
    PauseScreen* pauseScreen = nullptr;
    GameOverScreen* gameOverScreen = nullptr;
    LeaderboardScreen* leaderboardScreen = nullptr;
    CreditsScreen* creditsScreen = nullptr;

    Music gameMusic = {0};
    bool audioAvailable = false;
    bool shouldQuit = false;

    GameState state = MENU;

    float gameTimer = 0;
    int finalScore = 0;
    int finalLines = 0;
    int finalTime = 0;
    float scrollSpeed = 0;
    int lastTileIndex = -1;

    // Status bar Ln/Col tracking
    int currentLn = 1;
    int currentCol = 1;
    float colTimer = 0.0f;       // time since last col increment
    float colInterval = 0.18f;  // seconds per column step (mimics cursor movement speed)
    int lastLinesCompiled = 0;

    void Init()
    {
        SetExitKey(KEY_NULL); // prevent ESC from closing window
        // Do not force fullscreen (can cause mode-switch frame jitter/glitching):
        // ToggleFullscreen();

        InitAudioDevice();
        audioAvailable = IsAudioDeviceReady();

        vsFont = LoadFontEx("C:\\Windows\\Fonts\\segoeui.ttf", 48, 0, 256);
        SetTextureFilter(vsFont.texture, TEXTURE_FILTER_BILINEAR);
        codeFont = LoadFontEx("C:\\Windows\\Fonts\\consola.ttf", 48, 0, 256);
        SetTextureFilter(codeFont.texture, TEXTURE_FILTER_BILINEAR);
        sideBarTex = LoadTexture("src\\side-bar.png");
        SetTextureFilter(sideBarTex, TEXTURE_FILTER_BILINEAR);

        player.Init();
        Pickup::Init();
        Tile::Init();

        if (audioAvailable) {
            gameMusic = LoadMusicStream("src\\game_music.mp3");
            if (gameMusic.frameCount > 0) SetMusicVolume(gameMusic, 0.7f);
        }

        uiManager = new UIScreenManager();
        uiManager->Initialize();

        mainMenu = uiManager->GetMainMenu();
        pauseScreen = uiManager->GetPauseScreen();
        gameOverScreen = uiManager->GetGameOverScreen();
        leaderboardScreen = uiManager->GetLeaderboardScreen();
        creditsScreen = uiManager->GetCreditsScreen();

        SetupUI();
        Reset();
        ChangeState(MENU);
    }

    void SetupUI()
    {
        if (!mainMenu || !pauseScreen || !gameOverScreen || !leaderboardScreen || !creditsScreen)
            return;

        mainMenu->SetCallbacks(
            [&]() { Reset(); ChangeState(PLAYING); },
            [&]() { ChangeState(LEADERBOARD); },
            [&]() { ChangeState(CREDITS); },
            [&]() { shouldQuit = true; }
        );

        pauseScreen->onResume = [&]() { ChangeState(PLAYING); };
        pauseScreen->onRestart = [&]() { Reset(); ChangeState(PLAYING); };
        pauseScreen->onMainMenu = [&]() { Reset(); ChangeState(MENU); PauseMusicStream(gameMusic); };
        pauseScreen->onQuit = [&]() { shouldQuit = true; };

        gameOverScreen->onRestart = [&]() { Reset(); ChangeState(PLAYING); };
        gameOverScreen->onMainMenu = [&]() { Reset(); ChangeState(MENU); PauseMusicStream(gameMusic); };
        gameOverScreen->onQuit = [&]() { shouldQuit = true; };
        gameOverScreen->onSubmitScore = [&](const string& name) {
            SubmitScore(name);
        };

        leaderboardScreen->onBack = [&]() { ChangeState(MENU); };
        leaderboardScreen->onReset = [&]() { LeaderboardManager::GetInstance()->ResetLeaderboard(); leaderboardScreen->SetLeaderboard(LeaderboardManager::GetInstance()->GetLeaderboard()); };
        leaderboardScreen->onExport = [&]() { LeaderboardFileIO fileIO; fileIO.ExportToCSV(*LeaderboardManager::GetInstance()->GetLeaderboard(), "leaderboard.csv"); };
        creditsScreen->onBack = [&]() { ChangeState(MENU); };
    }

    void Reset()
    {
        player.posX = TILES_START_X;
        player.posY = TILE_Y - PLAYER_HEIGHT - 5;
        player.velX = player.velY = 0;
        player.inAir = false;
        player.jumpsAvailable = 1;
        player.isGameOver = false;
        player.isRewinding = false;
        player.linesCompiled = 0;

        player.Rewind_time_left = REWIND_SECS;
        rewindSys.Reset();

        Tile::TotaltilesCreatedCount = 0;
        Tile::tilesLeft = 0;
        Tile::baseSpeed = TILE_SPEED;
        Tile::bhopBuffer.framesLeft = 0;
        lastTileIndex = -1;

        for (Tile* t : tiles) delete t;
        tiles.clear();

        tiles.push_back(new Tile(TILES_START_X, Tile::GetMaxTileWidth(), Tile::TileType::NORMAL));
        pickups.clear();
        scrollSpeed = 0;
        gameTimer = 0;

        if (audioAvailable && gameMusic.frameCount > 0) {
            StopMusicStream(gameMusic);
            SeekMusicStream(gameMusic, 0.0f);
        }

        currentLn = 1; currentCol = 1; colTimer = 0.0f; lastLinesCompiled = 0;
        FloatingTextSystem::Clear();
    }

    void ChangeState(GameState newState)
    {
        state = newState;
        switch (state) {
            case MENU: uiManager->SwitchTo(ScreenType::MAIN_MENU); break;
            case PLAYING: uiManager->SwitchTo(ScreenType::PLAYING); if (audioAvailable) PlayMusicStream(gameMusic); break;
            case PAUSED: uiManager->SwitchTo(ScreenType::PAUSED); break;
            case GAME_OVER: uiManager->SwitchTo(ScreenType::GAME_OVER); break;
            case LEADERBOARD: uiManager->SwitchTo(ScreenType::LEADERBOARD); break;
            case CREDITS: uiManager->SwitchTo(ScreenType::CREDITS); break;
        }
    }

    void SubmitScore(const string& playerName)
    {
        LeaderboardManager::GetInstance()->SubmitRun(playerName.empty() ? "Anonymous" : playerName,
            finalScore, finalLines, finalTime);
    }

    void UpdatePlaying() {
        const float dt = GetFrameTime();
        UpdateMusicStream(gameMusic);

        // ── REWIND BRANCH ──────────────────────────────────────────────────
        bool rewindHeld = IsKeyDown(KEY_LEFT_CONTROL) && IsKeyDown(KEY_Z) && player.Rewind_time_left > 0.05f;
        if (rewindHeld) {
            // Do NOT call player.Update() — gravity/jump must not fight position restore
            if (!player.isRewinding) PlaySound(player.rewindSound);

            PlayerState rest;
            if (rewindSys.Rewind(rest)) {
                Player::ReduceRewind(player.Rewind_time_left, REWIND_DURATION);

                player.posX = rest.posX;
                player.posY = rest.posY;
                player.velX = 0;
                player.velY = 0;

                // Tile::Update does:  tileX -= (baseSpeed + gameSpeed)
                // To move tiles right at baseSpeed
                float reverseSpeed = -2.0f * Tile::baseSpeed;
                // During rewind move tiles backward — do NOT spawn new tiles
                Tile::Delete_And_Update(tiles, reverseSpeed, scrollSpeed);
                Tile::Collision(player, tiles);
                for (auto &p : pickups) p.Update(-Tile::baseSpeed, 0);

                player.isRewinding = true;
            } else {
                // Buffer exhausted — stop cleanly even if key still held
                StopSound(player.rewindSound);
                player.isRewinding = false;
                player.velX = 0;
                player.velY = 0;
                player.inAir = false;
                player.jumpsAvailable = 0;
                scrollSpeed = 0;
            }
            return; // skip normal-play branch this frame
        }

        // ── NORMAL PLAY BRANCH ─────────────────────────────────────────────
        // Clean up rewind state the frame the key is released
        if (player.isRewinding) {
            StopSound(player.rewindSound);
            player.isRewinding = false;
            player.velX = 0;
            player.velY = 0;
            player.inAir = false;
            player.jumpsAvailable = 0;
            scrollSpeed = 0;
        }

        player.Update(); // gravity, jump, position

        // Game-over check (fall off screen is set inside player.Update -> Fall())
        if (state == PLAYING && player.isGameOver) {
            finalScore = player.linesCompiled;
            finalLines = player.linesCompiled;
            finalTime = (int)gameTimer;
            bool isHigh = LeaderboardManager::GetInstance()->IsHighScore(finalScore);
            gameOverScreen->SetResults(finalScore, finalLines, finalTime, isHigh);
            gameOverScreen->SetStyle(GameOverScreen::Style::TERMINAL_ERROR);
            ChangeState(GAME_OVER);
            PauseMusicStream(gameMusic);
            return;
        }

        if (state != PLAYING) return;

        gameTimer += dt;
        UpdateLnCol(dt);

        rewindSys.Record({player.posX, player.posY});
        updateTiles(scrollSpeed);
        Pickups::SpawnIfNeeded(pickups, tiles);
        Pickups::UpdateAll(pickups, player, scrollSpeed, Tile::baseSpeed);

        Tile* standingOn = nullptr;
        for (Tile* t : tiles) {
            if (player.posX + player.playerWidth > t->tileX && player.posX < t->tileX + t->tileWidth) {
                standingOn = t;
                break;
            }
        }

        if (standingOn && standingOn->tileIndex > lastTileIndex) {
            int points = 0;
            Color pColor = GREEN;
            if (standingOn->tileType == Tile::TileType::NORMAL) { points=5; pColor=VSCodeTheme::ACCENT_GREEN; }
            else if (standingOn->tileType == Tile::TileType::LOGICAL) { points=3; pColor=VSCodeTheme::ACCENT_BLUE; }
            if (points>0) {
                player.linesCompiled += points;
                FloatingTextSystem::Push("+" + to_string(points), {player.posX, player.posY - 30}, pColor);
            }
            lastTileIndex = standingOn->tileIndex;
        }

        // Game over via SYNTAX tile collision
        if (player.isGameOver) {
            finalScore = player.linesCompiled;
            finalLines = player.linesCompiled;
            finalTime = (int)gameTimer;
            bool isHigh = LeaderboardManager::GetInstance()->IsHighScore(finalScore);
            gameOverScreen->SetResults(finalScore, finalLines, finalTime, isHigh);
            gameOverScreen->SetStyle(GameOverScreen::Style::TERMINAL_ERROR);
            ChangeState(GAME_OVER);
            PauseMusicStream(gameMusic);
        }
    }

    void Update() {
        // Global ESC handlers (before per-screen dispatch so they take priority)
        if (state == PLAYING && IsKeyPressed(KEY_ESCAPE)) {
            ChangeState(PAUSED);
            PauseMusicStream(gameMusic);
            return; // prevent PauseScreen from seeing same KEY_ESCAPE
        }
        if (state == MENU && IsKeyPressed(KEY_ESCAPE)) {
            shouldQuit = true;
            return;
        }
        if (state == GAME_OVER && IsKeyPressed(KEY_ESCAPE)) {
            Reset(); ChangeState(MENU); PauseMusicStream(gameMusic);
            return;
        }

        switch (state) {
            case MENU:
            case LEADERBOARD:
            case CREDITS:
            case GAME_OVER:
            case PAUSED:
                uiManager->UpdateCurrent();
                break;
            case PLAYING:
                UpdatePlaying();
                break;
        }
    }

    void UpdateLnCol(float dt) {
        int newLn = player.linesCompiled + 1;
        if (newLn != lastLinesCompiled) {
            // Player compiled a new line — reset col counter
            currentCol = 1;
            colTimer = 0.0f;
            lastLinesCompiled = newLn;
        }
        currentLn = newLn;

        // Increment col counter over time to simulate cursor moving through words
        colTimer += dt;
        if (colTimer >= colInterval) {
            colTimer -= colInterval;
            currentCol++;
            if (currentCol > 80) currentCol = 1; // wrap at col 80
        }
    }

    void DrawHUD() {
        int barHeight = 32;
        int barY = GetScreenHeight() - barHeight;
        DrawRectangle(0, barY, GetScreenWidth(), barHeight,
                      player.isRewinding ? VSCodeTheme::ACCENT_ORANGE : VSCodeTheme::STATUS_BG);

        char leftText[256];
        sprintf(leftText, "  >< Runtime-Terror  |  0 Errors  0 Warnings  |  Ln %d, Col %d", currentLn, currentCol);
        DrawVSText(leftText, 10, barY + 9, 14, WHITE);

        int minutes = (int)(gameTimer / 60), seconds = (int)gameTimer % 60;
        char rightText[256];
        sprintf(rightText, "Uptime %02d:%02d | Rewind %.1fs", minutes, seconds, player.Rewind_time_left);
        int rightW = MeasureVSText(rightText, 14);
        DrawVSText(rightText, GetScreenWidth() - rightW - 10, barY + 9, 14, WHITE);

        float fraction = fmax(0.0f, fmin(1.0f, player.Rewind_time_left / REWIND_SECS));
        int pbW = 100, px = GetScreenWidth() - rightW - 20 - pbW;
        DrawRectangle(px, barY + 10, pbW, 12, {255,255,255,60});
        DrawRectangle(px, barY + 10, (int)(pbW*fraction), 12, WHITE);

        if (player.isRewinding) {
            // Toast notification
            int toastW = 380, toastH = 72;
            int toastX = GetScreenWidth() - toastW - 16;
            int toastY = GetScreenHeight() - barHeight - toastH - 12;
            float pulse = (sinf(GetTime() * 6.0f) + 1.0f) / 2.0f;
            unsigned char ba = (unsigned char)(180 + pulse * 75);
            DrawRectangle(toastX, toastY, toastW, toastH, {37, 37, 38, 230});
            DrawRectangleLinesEx({(float)toastX, (float)toastY, (float)toastW, (float)toastH}, 1,
                                  {VSCodeTheme::ACCENT_ORANGE.r, VSCodeTheme::ACCENT_ORANGE.g, VSCodeTheme::ACCENT_ORANGE.b, ba});
            DrawRectangle(toastX, toastY, 4, toastH, VSCodeTheme::ACCENT_ORANGE);
            DrawVSText("Rewinding Timeline", toastX + 14, toastY + 14, 18, VSCodeTheme::ACCENT_ORANGE);
            char sub[64]; sprintf(sub, "Hold Ctrl+Z  |  %.1fs remaining", player.Rewind_time_left);
            DrawVSText(sub, toastX + 14, toastY + 42, 14, VSCodeTheme::TEXT_MUTED);
            int pbX2 = toastX + 14, pbY2 = toastY + toastH - 8, pbW2 = toastW - 28;
            DrawRectangle(pbX2, pbY2, pbW2, 4, {60, 60, 60, 255});
            DrawRectangle(pbX2, pbY2, (int)(pbW2 * fraction), 4, VSCodeTheme::ACCENT_ORANGE);
        }
    }

    void DrawTiles() {
        for (Tile* t : tiles) t->Draw(t->tileType);
    }

    void updateTiles(float speed) {
        // Normal play: spawn new tiles, move existing, check collisions
        Tile::New_tiles(tiles);
        Tile::Delete_And_Update(tiles, speed, scrollSpeed);
        Tile::Collision(player, tiles);
    }

    void Draw() {
        BeginDrawing();
        ClearBackground(VSCodeTheme::BG_EDITOR);
        switch (state) {
            case PAUSED:
            case PLAYING:
                DrawTiles();
                Pickups::DrawAll(pickups);
                player.Draw();
                UI::DrawVSCideBackground("main.cpp");
                if (state == PLAYING) FloatingTextSystem::DrawAll();
                DrawHUD();
                if (state == PAUSED) uiManager->DrawCurrent();
                break;
            default:
                uiManager->DrawCurrent();
                break;
        }
        EndDrawing();
    }

    void Cleanup() {
        if (gameMusic.frameCount > 0) UnloadMusicStream(gameMusic);
        player.Cleanup();
        Pickup::Cleanup();
        Tile::CleanupTiles(tiles);

        if (uiManager) { uiManager->Shutdown(); delete uiManager; uiManager = nullptr; }

        UnloadFont(vsFont);
        UnloadFont(codeFont);
        UnloadTexture(sideBarTex);

        if (IsAudioDeviceReady()) CloseAudioDevice();
        CloseWindow();
    }
};

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Runtime Terror");
    if (!IsWindowReady()) return 1;

    Game game;
    game.Init();
    SetTargetFPS(FPS);

    while (!WindowShouldClose() && !game.shouldQuit)
    {
        game.Update();
        game.Draw();
    }

    game.Cleanup();
    return 0;
}
