#include "tiles.h"
#include "constants.h"
#include <string>
#include <iostream>
using namespace std;
int Tile::tile_number = 0;
int Tile::delay_count = 0;
int Tile::normal_speed = TILE_SPEED;
Bhop_Buffer Tile::spaceBuffer;

Tile::Tile(int startX)
{
    if (tile_number >= VARIANT_SPAWNER) // after the player has jumped some tiles, tiles can become red
    {
        if (Chance(30)) // 30% chance to be red
        {
            this->variance = true;
        }
    }

    width = GetRandomValue(200, 500);
    x = startX;
    y = 4 * SCREEN_HEIGHT / 5;
}

void Tile::Draw(bool variance)
{
    Color color = variance ? RED : WHITE;
    DrawRectangle(x, y, width, height_of_tile, color);
}

bool Tile::Update(float game_speed)
{
    x -= normal_speed + game_speed;  // Move left
    return (x + width <= 0); // True if offscreen
}

void Tile::Delete_And_Update(std::vector<Tile *> &tiles, float game_speed)
{
    // Update all tiles and mark for deletion if offscreen
    for (int i = tiles.size() - 1; i >= 0; i--)
    {
        tiles[i]->Update(game_speed);
        // if (tiles[i]->Update(game_speed))
        // {
        //     delete tiles[i];                // delete tile if offscreen
        //     tiles.erase(tiles.begin() + i); // remove from vector
        // }
    }

    // If too many tiles accumulated, delete oldest ones to save memory
    if (tile_number >= LEFT_TILES)
    {
        for (int i = 1; i >= 0; i--)
        {
            delete tiles[i];
            tiles.erase(tiles.begin() + i);
        }
        tile_number -= 2;
    }
}

void Tile::New_tiles(std::vector<Tile *> &tiles)
{
    // Ensure at least 5 tiles, and always have tiles extending off-screen to the right
    while (tiles.size() < 5 || tiles.back()->x + tiles.back()->width < SCREEN_WIDTH + 400)
    {
        int lastRight = tiles.empty() ? SCREEN_WIDTH : tiles.back()->x + tiles.back()->width;
        int gap = GetRandomValue(100, 150);
        if (!tiles.empty() && tiles.back()->variance)
        {
            gap = 0;  // No gap after a variance tile to prevent pits
        }
        tiles.push_back(new Tile(lastRight + gap));
    }
}

void Tile::Collision(Player &player, const std::vector<Tile *> &tiles)
{
    // Update bhop buffer for red tile deflection
    if (IsKeyPressed(KEY_R))
    {
        spaceBuffer.framesLeft = spaceBuffer.maxFrames;
        cout << "Max frames acquired\n";
    }
    else if (spaceBuffer.framesLeft > 0)
    {
        spaceBuffer.framesLeft--;
        cout << spaceBuffer.framesLeft << endl;
    }

    bool hasCollided = false;
    int playerRight = player.x + player.width;

    for (unsigned int i = 0; i < tiles.size(); i++)
    {
        // Define tile collision areas
        Rectangle tileTop = {tiles[i]->x, tiles[i]->y, tiles[i]->width, height_of_tile / 3};
        Rectangle tileRect = {tiles[i]->x, tiles[i]->y, tiles[i]->width, height_of_tile};

        // Define player collision areas
        Rectangle playerBottom = {player.x, player.y + (2 * player.height) / 3, player.width, player.height / 3};
        Rectangle playerTop = {player.x, player.y, player.width, (2 * player.height) / 3};

        if (!tiles[i]->variance)
        {

            if (CheckCollisionRecs(playerBottom, tileTop))
            {
                player.inAir = false;
                tile_number = i + 1;
                player.y = tiles[i]->y - player.height + 1;
                player.y_velocity = 0;
                hasCollided = true;
                break;
            }
            else if (CheckCollisionRecs(playerTop, tileRect))
            {
                if (playerRight >= tileRect.x)
                {
                    int overlapX = playerRight - tileRect.x;
                    player.x = player.x - overlapX + 1;
                    hasCollided = true;
                    break;
                }
            }

            if (!hasCollided)
            {
                player.inAir = true;
            }
        }
        else
        {

            if (CheckCollisionRecs(playerBottom, tileTop))
            {
                cout << "Collided with tile " << tile_number << endl;
                // If bhop buffer active, jump instead of game over
                if (spaceBuffer.framesLeft > 0)
                {
                    cout << "player jumped!" << endl;
                    player.y_velocity -= JUMP_HEIGHT *2;
                    spaceBuffer.framesLeft = 0;
                    player.inAir = true;
                }
                else
                {
                    player.isGameOver = true;
                }

                break;
            }
        }
    }

    // Display current tile number
    std::string tileText = std::to_string(tile_number);
    DrawText(tileText.c_str(), 20, SCREEN_HEIGHT - 60, 45, WHITE);
}
void Tile::Cleanup(std::vector<Tile *> &tiles)
{
    // clean up remaining tiles
    for (Tile *t : tiles)
        delete t;
    tiles.clear();
}

void Tile::Hitbox(Color c, bool variance)
{
    if (!variance)
    {
        // Upper part (non-collidable)
        DrawRectangle(x, y, width, height_of_tile / 3, c);
        // Lower part (collidable)
        DrawRectangle(x, y + height_of_tile / 3, width, (2 * height_of_tile) / 3, WHITE);
    }
    else
    {
        // Upper part (red for variant)
        DrawRectangle(x, y, width, height_of_tile / 3, RED);
        // Lower part
        DrawRectangle(x, y + height_of_tile / 3, width, (2 * height_of_tile) / 3, WHITE);
    }
}
bool Tile::Chance(int percent)
{
    return GetRandomValue(1, 100) <= percent;
}

void Tile::WarningText(int tile_number, Player player)
{
    if (tile_number >= VARIANT_SPAWNER)
    {
        static int start_time = -1;
        if (start_time == -1)
            start_time = GetTime(); // Set when first reaching variant spawner

        double elapsed = GetTime() - start_time;

        if (elapsed <= 2.0)
        {
            DrawText("Warning! Syntax errors detected", SCREEN_WIDTH / 4, 50, 100, RED);
        }
        else if (elapsed <= 4.0)
        {
            DrawText("Press \"R\" to deflect!", SCREEN_WIDTH / 6, 50, 100, RED);
        }
    }
}