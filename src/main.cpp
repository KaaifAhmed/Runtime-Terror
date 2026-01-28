#include <raylib.h>
#include <string>
#include "constants.h"
#include "player.h"
#include <vector>
#include "tiles.h"

using namespace std;

int main()
{
    Player player;
    vector<Tile *> tiles;

    // adding the initial tile
    tiles.push_back(new Tile(SCREEN_WIDTH));

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Runtime Terror");
    SetTargetFPS(FPS);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        player.Update();
        Tile::Delete_And_Update(tiles);
        DrawText("The red is the hitbox !",SCREEN_WIDTH/4,30,50,YELLOW);

        // checks for collison
        Tile::Collision(player, tiles);

        ClearBackground(BLACK);

        player.Hitbox(RED);
        // draw tiles
        for (Tile *t : tiles)
            t->Hitbox(RED);
        Tile::New_tiles(tiles); // this is a seperate line from the for loop for new tiles

        EndDrawing();
    }

    // clean up remaining tiles
    Tile::Cleanup(tiles);
    CloseWindow();
}