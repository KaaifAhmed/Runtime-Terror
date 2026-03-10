#include <raylib.h>
#include <string>
#include "constants.h"
#include "player.h"
#include <vector>
#include "tiles.h"
#include "rewind.h"
#include "graph.h"
#include <fstream>
#include <algorithm>

using namespace std;


Player player;
vector<Tile *> tiles;
TeleportGraph portalNetwork;
extern bool inDarkWorld;
float scrollSpeed = 0;



// Function declarations
void drawHitbox();
void drawTiles();
void updateTiles(float scrollSpeed);

enum class GameState {
    MENU,
    PLAYING,
    HIGHSCORES
};

std::vector<int> loadScores() {
    std::vector<int> scores(3, 0);
    std::ifstream file("scores.json");
    if (!file.is_open()) return scores;
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    size_t start = content.find('[');
    size_t end = content.find(']', start);
    if (start != std::string::npos && end != std::string::npos) {
        std::string numbers = content.substr(start + 1, end - start - 1);
        size_t pos = 0;
        for (int i=0; i<3; i++) {
            try {
                scores[i] = std::stoi(numbers.substr(pos));
            } catch (...) { /* ignore parse errors */ }
            pos = numbers.find(',', pos);
            if (pos == std::string::npos) break;
            pos++;
        }
    }
    return scores;
}

void saveScore(int newScore) {
    if (newScore == 0) return;
    auto scores = loadScores();
    scores.push_back(newScore);
    std::sort(scores.begin(), scores.end(), std::greater<int>());
    if (scores.size() > 3) scores.resize(3);
    
    std::ofstream file("scores.json");
    file << "{\n  \"top_scores\": [" << scores[0] << ", " << scores[1] << ", " << scores[2] << "]\n}\n";
}

int main()
{
    // Initialize rewind system with capacity for REWIND_SECS seconds
    RewindBuffer rewindSys(REWIND_SECS * FPS);
    Tile* firstTile = new Tile(TILES_START_X);
    firstTile->tileWidth = 800; // Massive platform on game start
    tiles.push_back(firstTile);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Runtime Terror");
    SetExitKey(0); // Disable ESC from closing the window

    SetTargetFPS(FPS);
    GameState currentState = GameState::MENU;
    bool scoreSaved = false;

    // Main game loop
    while (!WindowShouldClose())
    {
        BeginDrawing();

        switch (currentState) {
            case GameState::MENU: {
                ClearBackground(BLACK);
                DrawText("RUNTIME TERROR", SCREEN_WIDTH / 2 - 340, SCREEN_HEIGHT / 4, 80, MAROON);
                DrawText("1. Play Game", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2, 30, WHITE);
                DrawText("2. Highest Scores", SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT / 2 + 50, 30, WHITE);
                DrawText("3. Exit", SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2 + 100, 30, WHITE);

                if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1)) {
                    currentState = GameState::PLAYING;
                    scoreSaved = false;
                    player.Reset();
                    rewindSys.Reset();
                    portalNetwork.Reset();
                    Tile::Cleanup(tiles);
                    Tile::currentTileIndex = 0;
                    Tile::baseSpeed = TILE_SPEED;
                    Tile* startTile = new Tile(TILES_START_X);
                    startTile->tileWidth = 800;
                    tiles.push_back(startTile);
                    scrollSpeed = 0;
                    inDarkWorld = false;
                } else if (IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2)) {
                    currentState = GameState::HIGHSCORES;
                } else if (IsKeyPressed(KEY_THREE) || IsKeyPressed(KEY_KP_3)) {
                    CloseWindow();
                    return 0;
                }
                break;
            }
            case GameState::HIGHSCORES: {
                ClearBackground(DARKGRAY);
                DrawText("HIGHEST SCORES", SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 4, 50, GOLD);
                auto scores = loadScores();
                for (int i=0; i<3; i++) {
                    DrawText(TextFormat("%d.   %d pts", i+1, scores[i]), SCREEN_WIDTH / 2 - 70, SCREEN_HEIGHT / 2 + (i*50), 40, WHITE);
                }
                DrawText("Press ESC to return to Menu", SCREEN_WIDTH / 2 - 250, SCREEN_HEIGHT - 100, 30, LIGHTGRAY);
                
                if (IsKeyPressed(KEY_ESCAPE)) {
                    currentState = GameState::MENU;
                }
                break;
            }
            case GameState::PLAYING: {
                // Check for rewind input (Ctrl+Z)
                if ((IsKeyDown(KEY_Z) && IsKeyDown(KEY_LEFT_CONTROL)) && !player.isGameOver)
                {
                    PlayerState restoredState;
                    if (rewindSys.Rewind(restoredState)) {
                        player.posX = restoredState.posX;
                        player.posY = restoredState.posY;
                        player.dashFramesLeft = restoredState.isSliding ? 1 : 0;
                        
                        // Rewind tiles at reverse speed
                        float reverseSpeed = (scrollSpeed + TILE_SPEED * 2) * -1;

                        for (Tile *t : tiles)
                            t->Update(reverseSpeed);

                        player.isRewinding = true;


                        // Draw rewind state
                        ClearBackground(inDarkWorld ? DARKPURPLE : BLACK);
                        portalNetwork.Draw();
                        player.Hitbox(ORANGE);
                        drawTiles();
                        DrawText("CTRL + Z!", SCREEN_WIDTH / 2 - 300, 100, 100, WHITE);
                    }

                }
                else if (player.isGameOver && IsKeyPressed(KEY_SPACE))
                {
                    player.Reset();
                    rewindSys.Reset();
                    portalNetwork.Reset();
                    
                    // Clean up and reset tiles
                    Tile::Cleanup(tiles);

                    Tile::currentTileIndex = 0;
                    Tile::baseSpeed = TILE_SPEED;
                    Tile* startTile = new Tile(TILES_START_X);
                    startTile->tileWidth = 800; // Force a massive platform on reset
                    tiles.push_back(startTile);
                    scrollSpeed = 0;
                    inDarkWorld = false; // Add reset hook for world color
                    scoreSaved = false;
                }
                else
                {
                    // Normal gameplay: record state and update
                    PlayerState currentStateInfo = {player.posX, player.posY, player.dashFramesLeft > 0};

                    rewindSys.Record(currentStateInfo);

                    player.isRewinding = false;


                    // Update player
                    player.Update();

                    // Show warning text if needed
                    Tile::WarningText(Tile::currentTileIndex, player);

                    // Update tiles if game not over
                    if (!player.isGameOver) {
                        updateTiles(scrollSpeed);
                        portalNetwork.Update(scrollSpeed + TILE_SPEED);
                        portalNetwork.CleanUp(-100.0f);
                    } else if (!scoreSaved) {
                        saveScore(Tile::currentTileIndex);
                        scoreSaved = true;
                    }

                    // Draw normal state
                    ClearBackground(inDarkWorld ? DARKPURPLE : BLACK);
                    portalNetwork.Draw();
                    player.Hitbox(ORANGE);

                    drawTiles();

                    
                    if (player.isGameOver) {
                        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.7f)); // Dims the background
                        DrawText("Press Space to Restart\n\nPress ESC for Menu", SCREEN_WIDTH / 2 - 300, SCREEN_HEIGHT / 2 - 50, 50, WHITE);
                        if (IsKeyPressed(KEY_ESCAPE)) {
                            currentState = GameState::MENU;
                            scoreSaved = false;
                        }
                    }
                    
                    // Draw Dash Charges
                    DrawText(TextFormat("Dashes: %i", player.dashCharges), 20, 20, 20, SKYBLUE);
                    DrawText(TextFormat("Recharging: %.1fs", 5.0f - player.timeSinceLastDashRecharge), 20, 45, 20, GRAY);
                }
                break;
            }
        }

        EndDrawing();
    }


    // Clean up tiles
    Tile::Cleanup(tiles);
    CloseWindow();
}

void drawHitbox()
{
    DrawText("The red is the hitbox!", SCREEN_WIDTH / 3, 30, 40, YELLOW);

    player.Hitbox(RED);
    for (Tile *t : tiles)
        t->Hitbox(ORANGE,t->tileType);
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
