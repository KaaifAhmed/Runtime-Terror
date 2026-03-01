#include <raylib.h>
#include <string>
#include "constants.h"
#include "player.h"
#include <vector>
#include "tiles.h"
#include "rewind.h"

using namespace std;

void drawHitbox();
void drawTiles();
void updateTiles(float game_speed);

Player player;
vector<Tile *> tiles;
float game_speed = 0;

// forward declarations for modularized main loop
void handleRewind(RewindBuffer &rewindSys);
void handleNormal(RewindBuffer &rewindSys);

int main()
{
    RewindBuffer rewindSys(REWIND_SECS*FPS);

    // Adding the initial tile
    tiles.push_back(new Tile(TILES_START_X));

    // int delay = 0;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Runtime Terror");
    SetTargetFPS(FPS);

    while (!WindowShouldClose())
    {
        BeginDrawing();

        if ((IsKeyDown(KEY_Z) && IsKeyDown(KEY_LEFT_CONTROL)) && !player.isGameOver)
        {
            handleRewind(rewindSys);
        }
        else
        {
            handleNormal(rewindSys);
        }

        EndDrawing();
    }

    // clean up remaining tiles
    Tile::Cleanup(tiles);

    CloseWindow();
}

void drawHitbox()
{
    DrawText("The red is the hitbox!", SCREEN_WIDTH / 3, 30, 40, YELLOW);

    player.Hitbox(RED);
    for (Tile *t : tiles)
        t->Hitbox(RED);
}

void drawTiles()
{
    for (Tile *t : tiles)
        t->Draw();

    Tile::New_tiles(tiles); // this is a seperate line from the for loop for new tiles
}

void updateTiles(float game_speed)
{
    Tile::Delete_And_Update(tiles, game_speed);

    // checks for collison
    Tile::Collision(player, tiles);
}

// helper called when rewind key pressed
void handleRewind(RewindBuffer &rewindSys)
{
    PlayerState restoredState;
    if (rewindSys.Rewind(restoredState)) {
        player.x = restoredState.x;
        player.y = restoredState.y;

        float reverseSpeed = (game_speed + TILE_SPEED*2)*-1;
        updateTiles(reverseSpeed);

        ClearBackground(BLACK);

        player.Draw();
        drawTiles();
        DrawText("CTRL + Z!", SCREEN_WIDTH/2-300, 100, 100, WHITE);
    } else {
        ClearBackground(BLACK);
        player.Draw();
        drawTiles();
        DrawText("Memory capacity reached!", SCREEN_WIDTH/5, 100, 70, WHITE);
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
