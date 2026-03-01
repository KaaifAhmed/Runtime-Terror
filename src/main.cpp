#include <raylib.h>
#include <string>
#include "constants.h"
#include "player.h"
#include <vector>
#include "tiles.h"

using namespace std;

struct PlayerState
{
    float x;
    float y;
};

// The Data Structure
class RewindBuffer
{
private:
    vector<PlayerState> buffer;
    int head;     // Where to write the next state
    int count;    // How many states are currently stored
    int capacity; // Maximum frames to be stored

public:
    RewindBuffer(int cap)
    {
        capacity = cap;
        buffer.resize(capacity);
        head = 0;
        count = 0;
    }

    // Called every frame during normal gameplay
    void Record(PlayerState state)
    {
        buffer[head] = state;         // Write data
        head = (head + 1) % capacity; // Move head forward, wrap around if at end

        if (count < capacity)
        {
            count++; // Increase count until full
        }
    }

    // Called every frame when the player holds 'Ctrl+Z'
    bool Rewind(PlayerState &outState)
    {
        if (count == 0)
        {
            return false; // Nothing left to rewind
        }

        // Move head backwards, wrapping around properly
        head = (head - 1 + capacity) % capacity;

        outState = buffer[head]; // Retrieve the state
        count--;

        return true;
    }

    int GetCount() { return count; }
};

void drawHitbox();
void drawTiles();
void updateTiles(float game_speed);

Player player;
vector<Tile *> tiles;
float game_speed = 0;

int main()
{
    RewindBuffer rewindSys(REWIND_SECS*FPS);

    // Adding the initial tile
    tiles.push_back(new Tile(TILES_START_X));

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Runtime Terror");
    SetTargetFPS(FPS);

    while (!WindowShouldClose())
    {
        BeginDrawing();

        if ((IsKeyDown(KEY_Z) && IsKeyDown(KEY_LEFT_CONTROL)) && !player.isGameOver)
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
            }
        }
        else
        {
            PlayerState currentState = {player.x, player.y};
            rewindSys.Record(currentState);

            player.Update();

            // if (GetRandomValue(0, 50) == 50)
            // {
            //     game_speed += 0.1;
            // }

            if (!player.isGameOver)
                updateTiles(game_speed);

            ClearBackground(BLACK);

            player.Draw();
            drawTiles();
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