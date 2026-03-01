#include <raylib.h>
#include <string>
#include "constants.h"
#include "player.h"
#include <vector>
#include "tiles.h"

using namespace std;

void drawHitbox();
void drawTiles();
void updateTiles(float game_speed);

Player player;
vector<Tile *> tiles;
float game_speed = 0;
// Camera2D camera = {0};

int main()
{
    // Adding the initial tile
    tiles.push_back(new Tile(TILES_START_X));

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Runtime Terror");
    SetTargetFPS(FPS);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        // BeginMode2D(camera);

        player.Update();
        // camera.offset = (Vector2){ SCREEN_WIDTH/3.0f, 2*SCREEN_HEIGHT/3.0f };
        // camera.target = (Vector2) {player.x, 4*SCREEN_HEIGHT/5 - 40};
        // camera.rotation = 0.0f;
        // camera.zoom = 1.0f;

        if (GetRandomValue(0,50) == 50)
        {
            game_speed += 0.1;
        }
        

        if (!player.isGameOver) updateTiles(game_speed);
        
        ClearBackground(BLACK);

        player.Draw();
        drawTiles();
        

        // EndMode2D();
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