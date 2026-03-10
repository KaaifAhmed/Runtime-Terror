#include <raylib.h>
#include <string>
#include "constants.h"
#include "player.h"
#include <vector>
#include "tiles.h"
#include "rewind.h"
#include "menu.h"

using namespace std;

Player player;
vector<Tile *> tiles;
float scrollSpeed = 0;

// Function declarations
void drawHitbox();
void drawTiles();
void updateTiles(float scrollSpeed);
void ResetGame(RewindBuffer& rewindSys);

int main()
{
    // Initialize window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Runtime Terror");
    SetTargetFPS(FPS);
    
    // Initialize rewind system
    RewindBuffer rewindSys(REWIND_SECS * FPS);
    
    // Game state
    GameState currentState = MENU;
    
    // Create buttons for main menu
    Button playBtn = {
        (SCREEN_WIDTH - 200) / 2.0f,  // x
        400,                          // y
        200,                          // width
        60,                           // height
        "PLAY",                       // text
        VSCode_SIDEBAR,               // color
        VSCode_BLUE,                  // hoverColor
        VSCode_TEXT                   // textColor
    };
    
    Button quitBtn = {
        (SCREEN_WIDTH - 200) / 2.0f,  // x
        500,                          // y
        200,                          // width
        60,                           // height
        "QUIT",                       // text
        VSCode_SIDEBAR,               // color
        VSCode_RED,                   // hoverColor
        VSCode_TEXT                   // textColor
    };
    
    // Create buttons for pause menu
    Button resumeBtn = {
        (SCREEN_WIDTH - 200) / 2.0f,
        350,
        200,
        60,
        "RESUME",
        VSCode_SIDEBAR,
        VSCode_GREEN,
        VSCode_TEXT
    };
    
    // Buttons for game over screen
    Button menuBtn = {
        (SCREEN_WIDTH - 400) / 2.0f,
        550,
        180,
        50,
        "MAIN MENU",
        VSCode_SIDEBAR,
        VSCode_BLUE,
        VSCode_TEXT
    };
    
    Button quitBtn2 = {
        (SCREEN_WIDTH + 40) / 2.0f,
        550,
        180,
        50,
        "QUIT",
        VSCode_SIDEBAR,
        VSCode_RED,
        VSCode_TEXT
    };
    
    // Main game loop
    while (!WindowShouldClose())
    {
        BeginDrawing();
        
        // State machine
        switch (currentState) {
            case MENU:
                // Draw main menu
                DrawMainMenu(playBtn, quitBtn);
                
                // Check button clicks
                if (IsButtonClicked(playBtn)) {
                    ResetGame(rewindSys);
                    currentState = PLAYING;
                }
                if (IsButtonClicked(quitBtn)) {
                    CloseWindow();
                    return 0;
                }
                break;
                
            case PLAYING:
                // Check for pause (ESC key)
                if (IsKeyPressed(KEY_ESCAPE)) {
                    currentState = PAUSED;
                    break;
                }
                
                // Check for rewind input (Ctrl+Z)
                if ((IsKeyDown(KEY_Z) && IsKeyDown(KEY_LEFT_CONTROL)) && !player.isGameOver)
                {
                    PlayerState restoredState;
                    if (rewindSys.Rewind(restoredState)) {
                        player.posX = restoredState.posX;
                        player.posY = restoredState.posY;
                        
                        // Rewind tiles at reverse speed
                        float reverseSpeed = (scrollSpeed + TILE_SPEED * 2) * -1;
                        updateTiles(reverseSpeed);
                        
                        player.isRewinding = true;
                        
                        // Draw rewind state
                        ClearBackground(VSCode_BACKGROUND);
                        player.Hitbox(ORANGE);
                        drawTiles();
                        DrawText("CTRL + Z!", SCREEN_WIDTH / 2 - 300, 100, 100, VSCode_BLUE);
                    }
                }
                else
                {
                    // Normal gameplay: record state and update
                    PlayerState currentStateData = {player.posX, player.posY};
                    rewindSys.Record(currentStateData);
                    
                    player.isRewinding = false;
                    
                    // Update player
                    player.Update();
                    
                    // Show warning text if needed
                    Tile::WarningText(Tile::currentTileIndex, player);
                    
                    // Update tiles if game not over
                    if (!player.isGameOver)
                        updateTiles(scrollSpeed);
                    
                    // Check game over
                    if (player.isGameOver) {
                        currentState = GAMEOVER;
                    }
                    
                    // Draw normal state
                    ClearBackground(VSCode_BACKGROUND);
                    player.Hitbox(ORANGE);
                    drawTiles();
                }
                break;
                
            case PAUSED:
                // Draw paused game in background
                ClearBackground(VSCode_BACKGROUND);
                player.Hitbox(ORANGE);
                drawTiles();
                
                // Draw pause menu on top
                DrawPauseMenu(resumeBtn, quitBtn);
                
                // Check button clicks
                if (IsButtonClicked(resumeBtn)) {
                    currentState = PLAYING;
                }
                if (IsButtonClicked(quitBtn)) {
                    currentState = MENU;
                }
                // Also resume on ESC
                if (IsKeyPressed(KEY_ESCAPE)) {
                    currentState = PLAYING;
                }
                break;
                
            case GAMEOVER:
                // Draw game over screen
                DrawGameOverScreen(Tile::currentTileIndex, menuBtn, quitBtn2);
                
                // Check button clicks
                if (IsButtonClicked(menuBtn)) {
                    currentState = MENU;
                }
                if (IsButtonClicked(quitBtn2)) {
                    CloseWindow();
                    return 0;
                }
                // Also allow R to restart
                if (IsKeyPressed(KEY_R)) {
                    ResetGame(rewindSys);
                    currentState = PLAYING;
                }
                break;
        }
        
        EndDrawing();
    }
    
    // Clean up tiles
    Tile::Cleanup(tiles);
    CloseWindow();
    return 0;
}

void ResetGame(RewindBuffer& rewindSys) {
    // Clean up old tiles
    Tile::Cleanup(tiles);
    
    // Reset player
    player.posX = PLAYER_START_X;
    player.posY = PLAYER_START_Y;
    player.velY = 0;
    player.velX = 0;
    player.jumpsAvailable = 0;
    player.inAir = true;
    player.isGameOver = false;
    player.isRewinding = false;
    
    // Reset tiles
    Tile::currentTileIndex = 0;
    Tile::baseSpeed = TILE_SPEED;
    tiles.push_back(new Tile(TILES_START_X));
    
    // Clear rewind buffer
    while (rewindSys.GetCount() > 0) {
        PlayerState dummy;
        rewindSys.Rewind(dummy);
    }
}

void drawHitbox()
{
    DrawText("The red is the hitbox!", SCREEN_WIDTH / 3, 30, 40, YELLOW);
    
    player.Hitbox(RED);
    for (Tile *t : tiles)
        t->Hitbox(ORANGE, t->tileType);
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
