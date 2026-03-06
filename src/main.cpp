#include <raylib.h>
#include <string>
#include "constants.h"
#include "player.h"
#include <vector>
#include "tiles.h"
#include "rewind.h"
#include <iostream>

using namespace std;

// Global player and tiles
Player player;
vector<Tile *> tiles;
float game_speed = 0; // Current game speed (affects tile movement)

// forward declarations for modularized main loop
void handleRewind(RewindBuffer &rewindSys);
void handleNormal(RewindBuffer &rewindSys);
// Function declarations
void drawHitbox();
void drawTiles();
void updateTiles(float game_speed);

int main()
{
    // Initialize rewind system with capacity for REWIND_SECS seconds
    RewindBuffer rewindSys(REWIND_SECS * FPS);

    // Add initial tile
    tiles.push_back(new Tile(TILES_START_X));

    // Initialize window and set FPS
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Runtime Terror");
    SetTargetFPS(FPS);

    // Main game loop
    while (!WindowShouldClose())
    {
        BeginDrawing();

        // Check for rewind input (Ctrl+Z)
        if ((IsKeyDown(KEY_Z) && IsKeyDown(KEY_LEFT_CONTROL)) && !player.isGameOver)
        {
            // Attempt to rewind player state
            PlayerState restoredState;
            if (rewindSys.Rewind(restoredState))
            {
                player.x = restoredState.x;
                player.y = restoredState.y;

                // Rewind tiles at reverse speed
                float reverseSpeed = (game_speed + TILE_SPEED * 2) * -1;
                updateTiles(reverseSpeed);

                ClearBackground(BLACK);
                player.isReversed = true;

                // Draw rewind state
                ClearBackground(BLACK);
                player.Hitbox(ORANGE);
                drawTiles();
                DrawText("CTRL + Z!", SCREEN_WIDTH / 2 - 300, 100, 100, WHITE);
            }
        }
        else
        {
            // Normal gameplay: record state and update
            PlayerState currentState = {player.x, player.y};
            rewindSys.Record(currentState);

            player.isReversed = false;

            // Show warning text if needed
            Tile::WarningText(Tile::tile_number, player);

            // Update tiles if game not over
            if (!player.isGameOver)
                updateTiles(game_speed);
            else
            {
                DrawText("LOL NOOB! XD", SCREEN_WIDTH / 4, 50, 100, WHITE);
            }

            // Update player
            player.Update();

            // Draw normal state
            ClearBackground(BLACK);
            player.Hitbox(ORANGE);
            drawTiles();
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
        t->Hitbox(ORANGE, t->variance);
}

void drawTiles()
{
    for (Tile *t : tiles)
        t->Draw(t->variance);

    Tile::New_tiles(tiles); // this is a seperate line from the for loop for new tiles
}

void updateTiles(float game_speed)
{
    Tile::Delete_And_Update(tiles, game_speed);


    // checks for collison only if player not reversing
    if(!player.isReversed)
    Tile::Collision(player, tiles);
}

// helper called when rewind key pressed
void handleRewind(RewindBuffer &rewindSys)
{
    PlayerState restoredState;
    if (rewindSys.Rewind(restoredState))
    {
        player.x = restoredState.x;
        player.y = restoredState.y;

        float reverseSpeed = (game_speed + TILE_SPEED * 2) * -1;
        updateTiles(reverseSpeed);

        ClearBackground(BLACK);

        player.Draw();
        drawTiles();
        DrawText("CTRL + Z!", SCREEN_WIDTH / 2 - 300, 100, 100, WHITE);
    }
    else
    {
        ClearBackground(BLACK);
        player.Draw();
        drawTiles();
        DrawText("Memory capacity reached!", SCREEN_WIDTH / 5, 100, 70, WHITE);
    }
}

// normal gameplay when rewind not active
void handleNormal(RewindBuffer &rewindSys)
{
    PlayerState currentState = {player.x, player.y};
    rewindSys.Record(currentState);

    player.Update();

    if (!player.isGameOver)
        updateTiles(game_speed);

    ClearBackground(BLACK);
    player.Draw();
    drawTiles();
}
