#include "tiles.h"
#include "constants.h"
#include <string>
#include <iostream>
using namespace std;
int Tile::tile_number = 0;
float Tile::normal_speed = TILE_SPEED;
Bhop_Buffer Tile::spaceBuffer;

Tile::Tile(int startX)
{
    if (tile_number >= VARIANT_SPAWNER) // after the player has jumped some tiles, tiles can become red
    {
        if (Chance(30)) // 30% chance to be red
        {
            this->variance = TileType::SYNTAX;
        }
        else if (Chance(20))
        {
            this->variance = TileType::LOGICAL;
        }
    }

    width = GetRandomValue(200, 500);
    x = startX;
    y = 4 * SCREEN_HEIGHT / 5;
}

void Tile::Draw(TileType variance)
{
    Color color_of_tile;
    if (variance == TileType::NORMAL)
        color_of_tile = WHITE;
    else if (variance == TileType::SYNTAX)
        color_of_tile = RED;
    else if (variance == TileType::LOGICAL)
        color_of_tile = BLUE;

    DrawRectangle(x, y, width, height_of_tile, color_of_tile);
}

bool Tile::Update(float game_speed)
{
    x -= normal_speed + game_speed; // Move left
    return (x + width <= 0);        // True if offscreen
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
    while (tiles.size() < 5 || tiles.back()->x + tiles.back()->width < SCREEN_WIDTH + 400)
    {
        int lastRight = tiles.empty() ? SCREEN_WIDTH : tiles.back()->x + tiles.back()->width;
        int gap = GetRandomValue(100, 150);

        if (!tiles.empty())
        {
            if (tiles.back()->variance == TileType::SYNTAX)
                gap = 0;
            else if (tiles.back()->variance == TileType::LOGICAL)
                gap = GetRandomValue(40, 60);
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
        // cout << "Max frames acquired\n";
    }
    else if (spaceBuffer.framesLeft > 0)
    {
        spaceBuffer.framesLeft--;
        //  cout << spaceBuffer.framesLeft << endl;
    }

    bool hasCollidedWhite = false;
    bool hasCollidedBlue = false;
    int playerRight = player.x + player.width;

    for (unsigned int i = 0; i < tiles.size(); i++)
    {
        // Define tile collision areas
        Rectangle tileTop = {tiles[i]->x, tiles[i]->y, tiles[i]->width, height_of_tile / 3};
        Rectangle tileRect = {tiles[i]->x, tiles[i]->y, tiles[i]->width, height_of_tile};

        // Define player collision areas
        Rectangle playerBottom = {player.x, player.y + (2 * player.height) / 3, player.width, player.height / 3};
        Rectangle playerTop = {player.x, player.y, player.width, (2 * player.height) / 3};

        if (tiles[i]->variance == TileType::NORMAL)
        {

            if (CheckCollisionRecs(playerBottom, tileTop))
            {
                player.inAir = false;
                player.jumpsLeft = 0; // reset on landing
                tile_number = i + 1;
                player.y = tiles[i]->y - player.height + 1;
                player.y_velocity = 0;
                hasCollidedWhite = true;
                break;
            }
            else if (CheckCollisionRecs(playerTop, tileRect))
            {
                if (playerRight >= tileRect.x)
                {
                    int overlapX = playerRight - tileRect.x;
                    player.x = player.x - overlapX + 1;
                    hasCollidedWhite = true;
                    break;
                }
            }
        }
        else if (tiles[i]->variance == TileType::SYNTAX)
        {

            if (CheckCollisionRecs(playerBottom, tileTop))
            {
                cout << "Collided with tile " << tile_number << endl;
                // If bhop buffer active, jump instead of game over
                if (spaceBuffer.framesLeft > 0)
                {
                    cout << "player jumped!" << endl;
                    player.y_velocity -= JUMP_HEIGHT * 2;
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
        else if (tiles[i]->variance == TileType::LOGICAL)
        {

            if (CheckCollisionRecs(playerBottom, tileTop))
            {
                player.inAir = false;
                player.jumpsLeft = 0; // reset on landing
                tile_number = i + 1;
                player.y = tiles[i]->y - player.height + 1;
                player.y_velocity = 0;
                hasCollidedBlue = true;
                break;
            }
            else if (CheckCollisionRecs(playerTop, tileRect))
            {
                if (playerRight >= tileRect.x)
                {
                    int overlapX = playerRight - tileRect.x;
                    player.x = player.x - overlapX + 1;
                    hasCollidedBlue = true;
                    break;
                }
            }
        }
    }

    // OUTSIDE the loop:
    if (!hasCollidedBlue && !hasCollidedWhite)
    {
        player.inAir = true;
        if (normal_speed != TILE_SPEED)
        {
            cout << "resetted" << endl;
            normal_speed = TILE_SPEED;
        }
    }

    if (hasCollidedBlue)
    {
        cout << "value going down";
        SmoothCountdown(normal_speed, TILE_SPEED);
    }
    if (normal_speed == 0)
    {
        player.isGameOver = true;
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

void Tile::Hitbox(Color c, TileType variance)
{
    if (variance == TileType::NORMAL)
    {
        // Upper part (non-collidable)
        DrawRectangle(x, y, width, height_of_tile / 3, c);
        // Lower part (collidable)
        DrawRectangle(x, y + height_of_tile / 3, width, (2 * height_of_tile) / 3, WHITE);
    }

    else if (variance == TileType::LOGICAL)
    {
        // Upper part (non-collidable)
        DrawRectangle(x, y, width, height_of_tile / 3, c);
        // Lower part (collidable)
        DrawRectangle(x, y + height_of_tile / 3, width, (2 * height_of_tile) / 3, BLUE);
    }
    else if (variance == TileType::SYNTAX)
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

void Tile::SmoothCountdown(float &value, float startValue)
{
    float rate = startValue / Speed_DELAY_DURATION;
    value -= rate * GetFrameTime();
    if (value < 0)
        value = 0;
}