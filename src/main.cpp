#include "constants.h"
#include "pickup.h"
#include "player.h"
#include "rewind.h"
#include "tiles.h"
#include "floatingtext.h"
#include "ui_system.h"
#include "leaderboard.h"
#include "screens.h"
#include "graph.h"
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
extern bool inDarkWorld;
TeleportGraph teleportNetwork;

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

    // Instruction toasts
    struct InstructionToast {
        std::string title;
        std::vector<std::string> lines;
        float timeLeft;
    };
    std::vector<InstructionToast> activeToasts;
    int toastIndex = 0;
    float nextToastTime = 0.0f;
    
    float portalSpawnTimer = 0.0f;
    float targetPortalSpawn = 0.0f;

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
        Tile::ResetPatternState();

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
        teleportNetwork.Reset();
        inDarkWorld = false;

        Tile::TotaltilesCreatedCount = 0;
        Tile::tilesLeft = 0;
        Tile::baseSpeed = TILE_SPEED;
        Tile::bhopBuffer.framesLeft = 0;
        Tile::ResetPatternState();
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

        // Onboarding toasts - sequential
        toastIndex = 0;
        nextToastTime = 0.5f; // Show first tip after 0.5 seconds
        
        portalSpawnTimer = 0.0f;
        targetPortalSpawn = (float)GetRandomValue(5, 15);
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

    void PushInstructionToast(const std::string& title, const std::vector<std::string>& lines, float duration = 6.0f) {
        activeToasts.push_back({title, lines, duration});
    }

    void UpdateInstructionToasts(float dt) {
        for (auto it = activeToasts.begin(); it != activeToasts.end(); ) {
            it->timeLeft -= dt;
            if (it->timeLeft <= 0) {
                it = activeToasts.erase(it);
            } else {
                ++it;
            }
        }
    }

    void DrawInstructionToasts() {
        if (activeToasts.empty()) return;

        const InstructionToast& toast = activeToasts.back(); // Show the most recent

        const int panelWidth = 450;
        const int panelHeight = 90 + (toast.lines.size() - 1) * 26; // Adjust height for bigger text
        const int panelX = SCREEN_WIDTH - panelWidth - 16;
        const int panelY = SCREEN_HEIGHT - panelHeight - 44; // Above status bar

        // Background with slight transparency
        DrawRectangle(panelX, panelY, panelWidth, panelHeight, {37, 37, 38, 230});

        // Border
        DrawRectangleLinesEx({(float)panelX, (float)panelY, (float)panelWidth, (float)panelHeight}, 1, VSCodeTheme::ACCENT_CYAN);

        // Accent bar
        DrawRectangle(panelX, panelY, 4, panelHeight, VSCodeTheme::ACCENT_CYAN);

        // Title
        DrawVSText(toast.title.c_str(), panelX + 14, panelY + 14, 28, VSCodeTheme::ACCENT_CYAN);

        // Lines
        for (size_t i = 0; i < toast.lines.size(); ++i) {
            DrawVSText(toast.lines[i].c_str(), panelX + 14, panelY + 50 + i * 26, 20, VSCodeTheme::TEXT_MUTED);
        }
    }

    void UpdatePlaying() {
        const float dt = GetFrameTime();
        UpdateMusicStream(gameMusic);
        UpdateInstructionToasts(dt);

        // Sequential onboarding toasts
        if (toastIndex <= 5 && nextToastTime > 0) {
            nextToastTime -= dt;
            if (nextToastTime <= 0) {
                switch (toastIndex) {
                    case 0:
                        PushInstructionToast("Compilation Tip", {"Press SPACE to jump over null voids","Press ESC to pause compilation."}, 6.0f);
                        nextToastTime = 5.0f;
                        break;
                    case 1:
                        PushInstructionToast("Compilation Tip", {"Hold CTRL+Z to rewind"}, 6.0f);
                        nextToastTime = 5.0f;
                        break;
                    case 2:
                        PushInstructionToast("Compilation Tip", {"Collect memory tokens to make space for rewind.","Memory space is indicated by the white bar below."}, 6.0f);
                        nextToastTime = 5.0f;
                        break;
                    case 3:
                        PushInstructionToast("Compilation Tip", {"Compile normal code without errors"}, 6.0f);
                        nextToastTime = 5.5f;
                        break;
                    case 4:
                        PushInstructionToast("Compilation Tip", {"Blue codes are logical errors.","Jump quickly to avoid slowdown"}, 6.0f);
                        nextToastTime = 5.5f;
                        break;
                    case 5:
                        PushInstructionToast("Compilation Tip", {"Red codes are runtime errors.","Press R to rebound and avoid crashing"}, 6.0f);
                        nextToastTime = 0.0f; // No more
                        break;
                }
                toastIndex++;
            }
        }

        if (Tile::phaseChanged) {
            Tile::phaseChanged = false;
            if (Tile::currentPhase == TilePatternPhase::LOGICAL) {
                PushInstructionToast("Logical Error Phase", {
                    "Entering logical error codes",
                    "Blue codes lag your computer - jump quickly",
                    "Avoid staying too long or crash"
                }, 6.0f);
            } else if (Tile::currentPhase == TilePatternPhase::MIXED) {
                PushInstructionToast("Mixed Error Phase", {
                    "Normal and erroneous codes mixed",
                    "Watch for logical and runtime errors",
                    "Use R to rebound from red codes"
                }, 6.0f);
            } else if (Tile::currentPhase == TilePatternPhase::SYNTAX_SPIKE) {
                PushInstructionToast("Runtime Error Spike", {
                    "Runtime error spike incoming",
                    "Red tiles ahead - press R to rebound",
                    "Stay ready for fast error handling"
                }, 6.0f);
            }
        }

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
            
            if (player.causeOfDeath == DeathCause::PORTAL_GAMBLE) {
                gameOverScreen->SetErrorOverride("PORTAL GAMBLE FAILED", "You lost the 50/50 teleport risk.", "0xGAMBLEFAIL");
            } else if (player.causeOfDeath == DeathCause::SYNTAX_ERROR) {
                gameOverScreen->SetErrorOverride("SYNTAX ERROR: FATAL COLLISION", "Geometry collision caused a fatal exception.", "0xSYNTAXERR");
            } else if (player.causeOfDeath == DeathCause::VOID_FALL) {
                gameOverScreen->SetErrorOverride("VOID EXCEPTION", "Player fell below valid memory boundaries.", "0xVOID_FALL");
            }
            
            ChangeState(GAME_OVER);
            PauseMusicStream(gameMusic);
            return;
        }

        if (state != PLAYING) return;

        gameTimer += dt;
        UpdateLnCol(dt);

        rewindSys.Record({player.posX, player.posY});
        updateTiles(scrollSpeed);
        
        portalSpawnTimer += dt;
        if (targetPortalSpawn > 0 && portalSpawnTimer >= targetPortalSpawn) {
            teleportNetwork.SpawnPair(GetScreenWidth() + 200, TILE_Y - 150, TILE_Y - 350);
            portalSpawnTimer = 0.0f;
            targetPortalSpawn = (float)GetRandomValue(5, 15); // Re-roll next portal spawn 
        }
        
        teleportNetwork.Update(Tile::baseSpeed + scrollSpeed);
        if (teleportNetwork.CleanUp(-100.0f)) {
            // Re-roll portal spawn to within 5 seconds if missed
            portalSpawnTimer = 0.0f;
            targetPortalSpawn = (float)GetRandomValue(1, 5);
        }
        
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
            
            if (player.causeOfDeath == DeathCause::PORTAL_GAMBLE) {
                gameOverScreen->SetErrorOverride("PORTAL GAMBLE FAILED", "You lost the 50/50 teleport risk.", "0xGAMBLEFAIL");
            } else if (player.causeOfDeath == DeathCause::SYNTAX_ERROR) {
                gameOverScreen->SetErrorOverride("SYNTAX ERROR: FATAL COLLISION", "Geometry collision caused a fatal exception.", "0xSYNTAXERR");
            } else if (player.causeOfDeath == DeathCause::VOID_FALL) {
                gameOverScreen->SetErrorOverride("VOID EXCEPTION", "Player fell below valid memory boundaries.", "0xVOID_FALL");
            }
            
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
            DrawVSText("Rewinding Execution", toastX + 14, toastY + 14, 18, VSCodeTheme::ACCENT_ORANGE);
            char sub[64]; sprintf(sub, "Hold Ctrl+Z  |  %.1fs remaining", player.Rewind_time_left);
            DrawVSText(sub, toastX + 14, toastY + 42, 14, VSCodeTheme::TEXT_MUTED);
            int pbX2 = toastX + 14, pbY2 = toastY + toastH - 8, pbW2 = toastW - 28;
            DrawRectangle(pbX2, pbY2, pbW2, 4, {60, 60, 60, 255});
            DrawRectangle(pbX2, pbY2, (int)(pbW2 * fraction), 4, VSCodeTheme::ACCENT_ORANGE);
        }

        DrawInstructionToasts();
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
        ClearBackground(inDarkWorld ? DARKPURPLE : VSCodeTheme::BG_EDITOR);
        switch (state) {
            case PAUSED:
            case PLAYING:
                DrawTiles();
                teleportNetwork.Draw();
                Pickups::DrawAll(pickups);
                player.Draw();
                UI::DrawVSCideBackground("main.cpp");
                
                // Draw Dash Charges prominently
                DrawVSText(TextFormat("Dashes: %i  (%.1fs Recharge)", player.dashCharges, 5.0f - player.timeSinceLastDashRecharge), 110, 85, 20, player.dashCharges > 0 ? VSCodeTheme::ACCENT_CYAN : VSCodeTheme::ACCENT_RED);

                FloatingTextSystem::DrawAll();
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
