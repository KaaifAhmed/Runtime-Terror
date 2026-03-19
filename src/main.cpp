#include <raylib.h>
#include <string>
#include "constants.h"
#include "player.h"
#include <vector>
#include "tiles.h"
#include "rewind.h"
#include "pickup.h"              // ← ADD

using namespace std;

Player player;
vector<Tile *> tiles;
vector<Pickup> pickups;          // ← ADD
float scrollSpeed = 0;

// Function declarations
void drawHitbox();
void drawTiles();
void updateTiles(float scrollSpeed);
void drawRewindBar(float rewindTimeLeft);

int main()
{
    RewindBuffer rewindSys(REWIND_SECS * FPS);
    tiles.push_back(new Tile(TILES_START_X));
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Runtime Terror");
    SetTargetFPS(FPS);
    player.Rewind_time_left = 5;

    while (!WindowShouldClose())
    {
        BeginDrawing();

        // Use 0.05f instead of 0 — float subtraction never lands exactly on 0,
        // so we treat anything below this epsilon as empty
        if ((IsKeyDown(KEY_Z) && IsKeyDown(KEY_LEFT_CONTROL)) && !player.isGameOver && player.Rewind_time_left > 0.05f)
        {
            PlayerState restoredState;
            if (rewindSys.Rewind(restoredState))
            {
                Player::ReduceRewind(player.Rewind_time_left, REWIND_DURATION);
                cout << player.Rewind_time_left << endl;
                player.posX = restoredState.posX;
                player.posY = restoredState.posY;

                float reverseSpeed = (scrollSpeed + TILE_SPEED * 2) * -1;
                updateTiles(reverseSpeed);

                // Move pickups at the same reverse speed so they stay on their tiles
                for (Pickup &p : pickups)
                    p.Update(-TILE_SPEED, 0);

                player.isRewinding = true;

                ClearBackground(BLACK);
                player.Hitbox(ORANGE);
                drawTiles();
                Pickups::DrawAll(pickups);
                DrawText("CTRL + Z!", SCREEN_WIDTH / 2 - 300, 100, 100, WHITE);
                drawRewindBar(player.Rewind_time_left);
            }
        }
        else
        {
            PlayerState currentState = {player.posX, player.posY};
            rewindSys.Record(currentState);

            player.isRewinding = false;
            player.Update();

            Tile::WarningText(Tile::currentTileIndex, player);

            if (!player.isGameOver)
            {
                updateTiles(scrollSpeed);

                // ← ADD: spawn + update + draw pickups
                Pickups::SpawnIfNeeded(pickups, tiles);
                Pickups::UpdateAll(pickups, player, scrollSpeed, Tile::baseSpeed);
            }

            ClearBackground(BLACK);
            player.Hitbox(ORANGE);
            drawTiles();
            Pickups::DrawAll(pickups);
            drawRewindBar(player.Rewind_time_left);
        }

        EndDrawing();
    }

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

void drawRewindBar(float rewindTimeLeft)
{
    const int BAR_X      = 20;
    const int BAR_Y      = SCREEN_HEIGHT - 50;
    const int BAR_WIDTH  = 250;
    const int BAR_HEIGHT = 24;
    const int BORDER     = 2;

    // Background track
    DrawRectangle(BAR_X - BORDER, BAR_Y - BORDER,
                  BAR_WIDTH + BORDER * 2, BAR_HEIGHT + BORDER * 2,
                  { 60, 60, 60, 255 });
    DrawRectangle(BAR_X, BAR_Y, BAR_WIDTH, BAR_HEIGHT, { 20, 20, 20, 255 });

    // Filled portion — cyan when healthy, orange when low
    float fraction = rewindTimeLeft / (float)REWIND_SECS;
    if (fraction < 0) fraction = 0;
    if (fraction > 1) fraction = 1;
    int fillWidth = (int)(BAR_WIDTH * fraction);

    Color fillColor = (fraction > 0.3f) ? Color{ 0, 210, 210, 255 }
                                        : Color{ 255, 140, 0, 255 };
    if (fillWidth > 0)
        DrawRectangle(BAR_X, BAR_Y, fillWidth, BAR_HEIGHT, fillColor);

    // Label
    DrawText("REWIND", BAR_X, BAR_Y - 20, 16, { 180, 180, 180, 255 });

    // Seconds remaining
    string timeStr = to_string((int)rewindTimeLeft) + "s";
    DrawText(timeStr.c_str(), BAR_X + BAR_WIDTH + 8, BAR_Y + 4, 16, { 180, 180, 180, 255 });
}