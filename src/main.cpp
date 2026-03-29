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

using namespace std;

// Game objects
Player player;
vector<Tile *> tiles;
vector<Pickup> pickups;
float scrollSpeed = 0;
GameState gameState = MENU;
Music gameMusic;
int lastTileIndex = 0;
RewindBuffer rewindSys{REWIND_SECS * FPS};
std::vector<FloatingText> FloatingTextSystem::popups;
bool shouldQuit = false;

// Audio availability flag
bool audioAvailable = false;

// UI System
UIScreenManager* uiManager = nullptr;
MainMenuScreen* mainMenu = nullptr;
PauseScreen* pauseScreen = nullptr;
GameOverScreen* gameOverScreen = nullptr;
LeaderboardScreen* leaderboardScreen = nullptr;
CreditsScreen* creditsScreen = nullptr;

// Game timing
float gameTimer = 0;
int finalScore = 0;
int finalLines = 0;
int finalTime = 0;

// Function declarations
void drawHitbox();
void drawTiles();
void updateTiles(float scrollSpeed);
void drawRewindBar(float rewindTimeLeft);
void drawGameScore();
void drawPlayingHUD();
void resetGame();
void setupUICallbacks();
void handlePauseInput();
void submitScore(const std::string& playerName);
int estimateTimeFromScore(int score);

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Runtime Terror - A Debugging Adventure");
    SetExitKey(KEY_NULL); // Prevent ESC from closing window
    
    // Check window creation
    if (!IsWindowReady()) {
        printf("ERROR: Failed to create window\n");
        return 1;
    }
    printf("DEBUG: Window created successfully\n");
    
    InitAudioDevice();
    audioAvailable = IsAudioDeviceReady();
    if (audioAvailable) {
        printf("DEBUG: Audio initialized successfully\n");
    } else {
        printf("WARNING: Audio device not available\n");
    }
    
    // Initialize systems
    player.Init();
    printf("DEBUG: Player initialized\n");
    
    Pickup::Init();
    printf("DEBUG: Pickup initialized\n");
    
    Tile::Init();
    printf("DEBUG: Tile initialized\n");
    
    // Load music only if audio is available
    if (audioAvailable) {
        gameMusic = LoadMusicStream("src\\game_music.mp3");
        if (gameMusic.frameCount > 0) {
            printf("DEBUG: Music loaded\n");
            SetMusicVolume(gameMusic, 0.7f);
        } else {
            printf("WARNING: Music not loaded\n");
        }
    } else {
        printf("WARNING: Skipping music load - audio not available\n");
    }
    
    // Initialize UI Manager
    printf("DEBUG: Creating UI Manager...\n");
    uiManager = new UIScreenManager();
    
    printf("DEBUG: Initializing UI Manager...\n");
    uiManager->Initialize();
    printf("DEBUG: UI Manager initialized\n");
    
    // Get screen instances
    mainMenu = uiManager->GetMainMenu();
    pauseScreen = uiManager->GetPauseScreen();
    gameOverScreen = uiManager->GetGameOverScreen();
    leaderboardScreen = uiManager->GetLeaderboardScreen();
    creditsScreen = uiManager->GetCreditsScreen();
    
    if (!mainMenu) printf("ERROR: mainMenu is null\n");
    if (!pauseScreen) printf("ERROR: pauseScreen is null\n");
    
    printf("DEBUG: Screens retrieved\n");
    
    // Setup callbacks
    printf("DEBUG: Setting up callbacks...\n");
    setupUICallbacks();
    printf("DEBUG: Callbacks set up\n");
    
    SetTargetFPS(FPS);
    player.Rewind_time_left = 5;
    
    // Initial state
    printf("DEBUG: Resetting game...\n");
    resetGame();
    
    printf("DEBUG: Switching to main menu...\n");
    uiManager->SwitchTo(ScreenType::MAIN_MENU);
    printf("DEBUG: Entering main loop...\n");
    
    // Main game loop
    int frameCount = 0;
    while (!WindowShouldClose())
    {
        if (frameCount == 0) printf("DEBUG: First frame\n");
        frameCount++;
        
        BeginDrawing();
        if (frameCount == 1) printf("DEBUG: After BeginDrawing\n");
        
        UpdateMusicStream(gameMusic);
        if (frameCount == 1) printf("DEBUG: After UpdateMusicStream\n");
        
        // Handle global input
        if (gameState == PLAYING && IsKeyPressed(KEY_ESCAPE)) {
            gameState = PAUSED;
            PauseMusicStream(gameMusic);
            uiManager->SwitchTo(ScreenType::PAUSED);
        }
        if (frameCount == 1) printf("DEBUG: After ESC check\n");
        
        // Update and draw based on state
        switch (gameState)
        {
        case MENU:
        case LEADERBOARD:
        case CREDITS:
            if (frameCount == 1) printf("DEBUG: In MENU/LEADERBOARD/CREDITS case\n");
            uiManager->UpdateCurrent();
            if (frameCount == 1) printf("DEBUG: After UpdateCurrent\n");
            uiManager->DrawCurrent();
            if (frameCount == 1) printf("DEBUG: After DrawCurrent\n");
            break;
            
        case PLAYING:
        {
            ClearBackground(VSCodeTheme::BG_EDITOR);
            
            // Update game timer
            gameTimer += GetFrameTime();
            
            // Handle rewind
            if ((IsKeyDown(KEY_Z) && IsKeyDown(KEY_LEFT_CONTROL)) &&
                !player.isGameOver && player.Rewind_time_left > 0.05f)
            {
                if (!player.isRewinding)
                    PlaySound(player.rewindSound);
                
                PlayerState restoredState;
                if (rewindSys.Rewind(restoredState))
                {
                    Player::ReduceRewind(player.Rewind_time_left, REWIND_DURATION);
                    player.posX = restoredState.posX;
                    player.posY = restoredState.posY;
                    
                    float reverseSpeed = (scrollSpeed + TILE_SPEED * 2) * -1;
                    updateTiles(reverseSpeed);
                    
                    for (Pickup &p : pickups)
                        p.Update(-TILE_SPEED, 0);
                    
                    player.isRewinding = true;
                    
                    // Draw during rewind
                    drawTiles();
                    Pickups::DrawAll(pickups);
                    FloatingTextSystem::DrawAll();
                    player.Draw();
                    
                    if (IsKeyDown(KEY_H))
                        player.Hitbox(RED);
                    
                    DrawText("CTRL + Z!", SCREEN_WIDTH / 2 - 300, 100, 100, VSCodeTheme::ACCENT_ORANGE);
                    drawRewindBar(player.Rewind_time_left);
                    drawPlayingHUD();
                }
            }
            else
            {
                if (player.isRewinding)
                    StopSound(player.rewindSound);
                
                PlayerState currentState = {player.posX, player.posY};
                rewindSys.Record(currentState);
                
                player.isRewinding = false;
                player.Update();
                
                if (!player.isGameOver)
                {
                    updateTiles(scrollSpeed);
                    Pickups::SpawnIfNeeded(pickups, tiles);
                    Pickups::UpdateAll(pickups, player, scrollSpeed, Tile::baseSpeed);
                    
                    // Scoring logic
                    Tile* standingOn = nullptr;
                    for (Tile *t : tiles) 
                    {
                        if (player.posX + player.playerWidth > t->tileX && 
                            player.posX < t->tileX + t->tileWidth) 
                        {
                            standingOn = t;
                            break;
                        }
                    }
                    
                    if (standingOn != nullptr && standingOn->tileIndex > lastTileIndex) 
                    {
                        int points = 0;
                        Color pColor = GREEN;
                        
                        if (standingOn->tileType == Tile::TileType::NORMAL) {
                            points = 5;
                            pColor = VSCodeTheme::ACCENT_GREEN;
                        }
                        else if (standingOn->tileType == Tile::TileType::LOGICAL) {
                            points = 3;
                            pColor = VSCodeTheme::ACCENT_BLUE;
                        }
                        
                        if (points > 0) {
                            player.linesCompiled += points;
                            FloatingTextSystem::Push("+" + std::to_string(points), 
                                                   {player.posX, player.posY - 30}, pColor);
                        }
                        
                        lastTileIndex = standingOn->tileIndex;
                    }
                }
                else
                {
                    // Game Over transition
                    finalScore = player.linesCompiled;
                    finalLines = player.linesCompiled;
                    finalTime = (int)gameTimer;
                    
                    bool isHigh = LeaderboardManager::GetInstance()->IsHighScore(finalScore);
                    gameOverScreen->SetResults(finalScore, finalLines, finalTime, isHigh);
                    
                    // Random error style each time
                    int style = GetRandomValue(0, 3);
                    gameOverScreen->SetStyle(static_cast<GameOverScreen::Style>(style));
                    
                    gameState = GAME_OVER;
                    PauseMusicStream(gameMusic);
                    uiManager->SwitchTo(ScreenType::GAME_OVER);
                }
                
                // Draw game
                drawTiles();
                Pickups::DrawAll(pickups);
                player.Draw();
                
                if (IsKeyDown(KEY_H))
                    player.Hitbox(RED);
                
                FloatingTextSystem::DrawAll();
                drawRewindBar(player.Rewind_time_left);
                drawPlayingHUD();
            }
            break;
        }
        
        case PAUSED:
            // Draw game in background (dimmed)
            ClearBackground(VSCodeTheme::BG_EDITOR);
            drawTiles();
            Pickups::DrawAll(pickups);
            player.Draw();
            drawRewindBar(player.Rewind_time_left);
            drawPlayingHUD();
            
            // Draw pause overlay
            uiManager->UpdateCurrent();
            uiManager->DrawCurrent();
            break;
            
        case GAME_OVER:
            uiManager->UpdateCurrent();
            uiManager->DrawCurrent();
            break;
        }
        
        EndDrawing();
        if (shouldQuit) break;
    }
    
    // Cleanup
    UnloadMusicStream(gameMusic);
    player.Cleanup();
    Pickup::Cleanup();
    Tile::CleanupTiles(tiles);
    
    uiManager->Shutdown();
    delete uiManager;
    
    CloseAudioDevice();
    CloseWindow();
    
    return 0;
}

void setupUICallbacks()
{
    // Main Menu callbacks
    mainMenu->SetCallbacks(
        // On Play
        []() {
            resetGame();
            gameState = PLAYING;
            PlayMusicStream(gameMusic);
            uiManager->SwitchTo(ScreenType::PLAYING);
        },
        // On Leaderboard
        []() {
            gameState = LEADERBOARD;
            uiManager->SwitchTo(ScreenType::LEADERBOARD);
        },
        // On Credits
        []() {
            gameState = CREDITS;
            uiManager->SwitchTo(ScreenType::CREDITS);
        },
        // On Exit
        []() {
            shouldQuit = true;
        }
    );
    
    // Pause Screen callbacks
    pauseScreen->onResume = []() {
        gameState = PLAYING;
        ResumeMusicStream(gameMusic);
        uiManager->SwitchTo(ScreenType::PLAYING);
    };
    
    pauseScreen->onRestart = []() {
        resetGame();
        gameState = PLAYING;
        PlayMusicStream(gameMusic);
        uiManager->SwitchTo(ScreenType::PLAYING);
    };
    
    pauseScreen->onMainMenu = []() {
        resetGame();
        gameState = MENU;
        StopMusicStream(gameMusic);
        uiManager->SwitchTo(ScreenType::MAIN_MENU);
    };
    
    pauseScreen->onQuit = []() {
       shouldQuit = true;
    };
    
    // Game Over callbacks
    gameOverScreen->onRestart = []() {
        resetGame();
        gameState = PLAYING;
        PlayMusicStream(gameMusic);
        uiManager->SwitchTo(ScreenType::PLAYING);
    };
    
    gameOverScreen->onMainMenu = []() {
        resetGame();
        gameState = MENU;
        StopMusicStream(gameMusic);
        uiManager->SwitchTo(ScreenType::MAIN_MENU);
    };
    
    gameOverScreen->onQuit = []() {
       shouldQuit = true;
    };
    
    gameOverScreen->onSubmitScore = [](std::string name) {
        submitScore(name);
    };
    
    // Leaderboard callbacks
    leaderboardScreen->onBack = []() {
        gameState = MENU;
        uiManager->SwitchTo(ScreenType::MAIN_MENU);
    };
    
    leaderboardScreen->onReset = []() {
        LeaderboardManager::GetInstance()->ResetLeaderboard();
        // Refresh the display
        leaderboardScreen->SetLeaderboard(LeaderboardManager::GetInstance()->GetLeaderboard());
    };
    
    leaderboardScreen->onExport = []() {
        LeaderboardFileIO fileIO;
        if (fileIO.ExportToCSV(*LeaderboardManager::GetInstance()->GetLeaderboard(), "leaderboard.csv")) {
            // Could show a confirmation here
        }
    };
    
    // Credits callback
    creditsScreen->onBack = []() {
        gameState = MENU;
        uiManager->SwitchTo(ScreenType::MAIN_MENU);
    };
}

void submitScore(const std::string& playerName)
{
    LeaderboardManager::GetInstance()->SubmitRun(
        playerName.empty() ? "Anonymous" : playerName,
        finalScore,
        finalLines,
        finalTime
    );
}

void drawTiles()
{
    for (Tile *t : tiles)
        t->Draw(t->tileType);
    Tile::New_tiles(tiles);
}

void updateTiles(float scrollSpeed)
{
    Tile::Delete_And_Update(tiles, scrollSpeed);
    Tile::Collision(player, tiles);
}

void drawRewindBar(float rewindTimeLeft)
{
    const int BAR_X = 20;
    const int BAR_Y = SCREEN_HEIGHT - 50;
    const int BAR_WIDTH = 250;
    const int BAR_HEIGHT = 24;
    const int BORDER = 2;
    
    // Background track
    DrawRectangle(BAR_X - BORDER, BAR_Y - BORDER, BAR_WIDTH + BORDER * 2,
                  BAR_HEIGHT + BORDER * 2, VSCodeTheme::BG_PANEL);
    DrawRectangle(BAR_X, BAR_Y, BAR_WIDTH, BAR_HEIGHT, VSCodeTheme::BG_DARK);
    
    // Filled portion
    float fraction = rewindTimeLeft / (float)REWIND_SECS;
    if (fraction < 0) fraction = 0;
    if (fraction > 1) fraction = 1;
    int fillWidth = (int)(BAR_WIDTH * fraction);
    
    Color fillColor = (fraction > 0.3f) ? VSCodeTheme::ACCENT_CYAN : VSCodeTheme::ACCENT_ORANGE;
    if (fillWidth > 0)
        DrawRectangle(BAR_X, BAR_Y, fillWidth, BAR_HEIGHT, fillColor);
    
    // Label
    DrawText("REWIND", BAR_X, BAR_Y - 20, 16, VSCodeTheme::TEXT_MUTED);
    
    // Seconds remaining
    char timeStr[16];
    sprintf(timeStr, "%.1f s", rewindTimeLeft);
    DrawText(timeStr, BAR_X + BAR_WIDTH + 8, BAR_Y + 4, 16, VSCodeTheme::TEXT_MUTED);
}

void drawGameScore()
{
    int score = player.linesCompiled;
    char scoreStr[32];
    sprintf(scoreStr, "Lines: %d", score);
    
    int boxX = SCREEN_WIDTH - 220;
    int boxY = 20;
    
    // Simple text display
    DrawRectangle(boxX - 10, boxY - 5, 200, 40, {30, 30, 30, 200});
    DrawRectangleLinesEx({(float)(boxX - 10), (float)(boxY - 5), 200, 40}, 1, VSCodeTheme::BORDER);
    DrawText(scoreStr, boxX, boxY + 5, 24, VSCodeTheme::ACCENT_CYAN);
}

void drawPlayingHUD()
{
    // Score
    drawGameScore();
    
    // Time display
    char timeStr[32];
    int minutes = (int)(gameTimer / 60);
    int seconds = (int)(gameTimer) % 60;
    sprintf(timeStr, "%02d:%02d", minutes, seconds);
    DrawText(timeStr, SCREEN_WIDTH - 100, 70, 20, VSCodeTheme::TEXT_MUTED);
    
    // Controls hint at bottom right
    DrawText("ESC: Pause", SCREEN_WIDTH - 120, SCREEN_HEIGHT - 30, 16, VSCodeTheme::TEXT_MUTED);
}

void resetGame()
{
    printf("DEBUG: resetGame - step 1 (player reset)...\n");
    // Player reset
    player.posX = TILES_START_X;
    player.posY = TILE_Y - PLAYER_HEIGHT - 5;
    player.velY = 0;
    player.velX = 0;
    player.inAir = false;
    player.jumpsAvailable = 1;
    player.isGameOver = false;
    player.linesCompiled = 0;
    player.isRewinding = false;
    
    printf("DEBUG: resetGame - step 2 (rewind reset)...\n");
    // Rewind system reset
    player.Rewind_time_left = REWIND_SECS;
    rewindSys.Reset();
    
    printf("DEBUG: resetGame - step 3 (tile reset)...\n");
    // Tile & scoring reset
    Tile::TotaltilesCreatedCount = 0;
    Tile::tilesLeft = 0;
    Tile::baseSpeed = TILE_SPEED;
    Tile::bhopBuffer.framesLeft = 0;
    lastTileIndex = -1;
    
    printf("DEBUG: resetGame - step 4 (cleanup tiles)...\n");
    // Clean up tiles
    for (Tile *t : tiles)
        delete t;
    tiles.clear();
    
    printf("DEBUG: resetGame - step 5 (spawn tile)...\n");
    // Spawn starting platform
    tiles.push_back(new Tile(TILES_START_X, Tile::GetMaxTileWidth(), Tile::TileType::NORMAL));
    
    printf("DEBUG: resetGame - step 6 (audio/world reset)...\n");
    // World & audio reset
    pickups.clear();
    scrollSpeed = 0;
    gameTimer = 0;
    
    // Only stop music if audio is available
    if (audioAvailable && gameMusic.frameCount > 0) {
        StopMusicStream(gameMusic);
        SeekMusicStream(gameMusic, 0.0f);
    }
    
    printf("DEBUG: resetGame - step 7 (floating text reset)...\n");
    // Floating text reset
    FloatingTextSystem::Clear();
    printf("DEBUG: resetGame - complete!\n");
}
