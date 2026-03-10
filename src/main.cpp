#include <raylib.h>
#include <string>
#include "constants.h"
#include "player.h"
#include <vector>
#include "tiles.h"
#include "rewind.h"

using namespace std;

Player player;
vector<Tile *> tiles;
float scrollSpeed = 0;

// Function declarations
void drawHitbox();
void drawTiles();
void updateTiles(float scrollSpeed);

int main()
{
    // Initialize rewind system with capacity for REWIND_SECS seconds
    RewindBuffer rewindSys(REWIND_SECS * FPS);
    tiles.push_back(new Tile(TILES_START_X));
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Runtime Terror");
    SetTargetFPS(FPS);

    // Main game loop
    while (!WindowShouldClose())
    {
        BeginDrawing();

        // Check for rewind input (Ctrl+Z)
        if ((IsKeyDown(KEY_Z) && IsKeyDown(KEY_LEFT_CONTROL)) && !player.isGameOver)
        {
            PlayerState restoredState;
            if (rewindSys.Rewind(restoredState))
            {
                player.posX = restoredState.posX;
                player.posY = restoredState.posY;

                // Rewind tiles at reverse speed
                float reverseSpeed = (scrollSpeed + TILE_SPEED * 2) * -1;
                updateTiles(reverseSpeed);

                player.isRewinding = true;

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
            PlayerState currentState = {player.posX, player.posY};
            rewindSys.Record(currentState);

            player.isRewinding = false;

            // Update player
            player.Update();

            // Show warning text if needed
            Tile::WarningText(Tile::currentTileIndex, player);

            // Update tiles if game not over
            if (!player.isGameOver)
                updateTiles(scrollSpeed);

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
