#include <raylib.h>
#include <string>
#include "constants.h"
#include "player.h"
#include <vector>
#include "tiles.h"

using namespace std;

void drawHitbox();
void drawTiles();
void updateTiles();

Player player;
vector<Tile *> tiles;

int main()
{
    // Adding the initial tile
    tiles.push_back(new Tile(TILES_START_X));

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Runtime Terror");
    SetTargetFPS(FPS);

    while (!WindowShouldClose())
    {
        BeginDrawing();

        player.Update();
        if (!player.isGameOver) updateTiles();

        ClearBackground(BLACK);

        player.Draw();
        drawTiles();
        // drawHitbox();

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

void updateTiles()
{
    Tile::Delete_And_Update(tiles);

    // checks for collison
    Tile::Collision(player, tiles);
}