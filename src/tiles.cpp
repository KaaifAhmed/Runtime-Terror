#include "tiles.h"
#include "constants.h"
#include <string>
#include<iostream>
using namespace std;
int Tile::tile_number = 0;
int Tile::delay_count=0;
 Bhop_Buffer Tile:: spaceBuffer;

Tile::Tile(int startX)
{
    if (tile_number >= VARIENT_SPAWNER) // after the player has jumped 6 tiles then tiles become red
    {
        if (Chance(30)) // 30 percent chance to be red
        {
            this->variance = true;
        }
        
    }

    width_of_tile = GetRandomValue(200, 500);
    x_of_tile = startX;
    y_of_tile = GetRandomValue(4 * SCREEN_HEIGHT / 5, 4 * SCREEN_HEIGHT / 5 - 40);
}

void Tile::Draw(bool variance)
{
    if (!variance)
        DrawRectangle(x_of_tile, y_of_tile, width_of_tile, height_of_tile, WHITE);
    else
        DrawRectangle(x_of_tile, y_of_tile, width_of_tile, height_of_tile, RED);
}

bool Tile::Update(float game_speed)
{
    x_of_tile -= normal_speed + game_speed;  // move left
    return (x_of_tile + width_of_tile <= 0); // true if offscreen
}

void Tile::Delete_And_Update(std::vector<Tile *> &tiles, float game_speed)
{
    // updating the tiles
    for (int i = tiles.size() - 1; i >= 0; i--)
    {
        tiles[i]->Update(game_speed);
        // if (tiles[i]->Update(game_speed))
        // {
        //     delete tiles[i];                // delete tile if offscreen
        //     tiles.erase(tiles.begin() + i); // remove from vector
        // }
    }

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
    while (tiles.size() < 5 || tiles.back()->x_of_tile + tiles.back()->width_of_tile < SCREEN_WIDTH + 400)
    {
        // meaninng of the rigth cond:
        // Continue adding tiles as long as the right edge of the last tile
        // is less than SCREEN_WIDTH + 400, so there are always extra tiles
        // off-screen to the right for smooth scrolling
        int lastRight = tiles.empty() ? SCREEN_WIDTH : tiles.back()->x_of_tile + tiles.back()->width_of_tile;
        int gap_between_tiles = GetRandomValue(100, 150);
        tiles.push_back(new Tile(lastRight + gap_between_tiles));
    }
}

void Tile::Collision(Player &player, const std::vector<Tile *> &tiles)
{
            // --- 1. UPDATE BUFFER ---
        if (IsKeyPressed(KEY_R))
        {
            spaceBuffer.framesLeft = spaceBuffer.maxFrames;
            cout<<"Max frames aquired\n";
        }
        else if (spaceBuffer.framesLeft > 0)
        {
            spaceBuffer.framesLeft--;
            cout<<spaceBuffer.framesLeft<<endl;
        }

    bool collision = false;
    int overlap;
    int player_right = player.x + player.width;
    

    for (unsigned int i = 0; i < tiles.size(); i++)
    {
        Rectangle tile_top = {tiles[i]->x_of_tile, tiles[i]->y_of_tile, tiles[i]->width_of_tile, ((float)tiles[i]->height_of_tile) / 3};
        Rectangle tile = {tiles[i]->x_of_tile, tiles[i]->y_of_tile, tiles[i]->width_of_tile, tiles[i]->height_of_tile};

        Rectangle non_variant_player_bottom = {player.x, player.y + (float)(2 * player.height) / 3, player.width, player.height / 3};
        Rectangle non_variant_player_top    = {player.x, player.y, player.width, ((float)2 * player.height) / 3};

        if (!tiles[i]->variance)
        {

            if (CheckCollisionRecs(non_variant_player_bottom, tile_top))
            {
                player.inAir = false;
                tile_number = i + 1;
                player.y = tiles[i]->y_of_tile - player.height + 1;
                player.y_velocity = 0;
                collision = true;
                break;
            }
            else if (CheckCollisionRecs(non_variant_player_top, tile))
            {
                if (player_right >= tile.x)
                {
                    overlap = player_right - tile.x;
                    player.x = player.x - overlap + 1;
                    collision = true;
                    break;
                }
            }

            if (!collision)
            {
                player.inAir = true;
            }
        }
        else
        {

            if (CheckCollisionRecs(non_variant_player_bottom,tile_top))
            {
                cout<<"collided with tile"<<tile_number<<" ";
                
             //   if (player.y + player.height < tiles[i]->y_of_tile+ ((float)tiles[i]->height_of_tile) / 3) // bottom left corner  of player < less then bottom left corner of collidible part of tile
              //  {
                    if (spaceBuffer.framesLeft>0)
                    {
                         player.y_velocity -= JUMP_HEIGHT;
                    } 
                    else
                    {
                       player.isGameOver=true;
                      
                    }
               // }
            }
        }
    }

    std::string tileText = std::to_string(tile_number);
    const char *tile_cstr = tileText.c_str();
    DrawText(tile_cstr, 20, SCREEN_HEIGHT - 60, 45, WHITE);
}
void Tile::Cleanup(std::vector<Tile *> &tiles)
{
    // clean up remaining tiles
    for (Tile *t : tiles)
        delete t;
    tiles.clear();
}

void Tile::Hitbox(Color c,bool variance)
{
    if(!variance)
    {
         // upper part
    DrawRectangle(x_of_tile, y_of_tile, width_of_tile, (float)height_of_tile / 3, c);
    // lower part
    DrawRectangle(x_of_tile, y_of_tile + (float)height_of_tile / 3, width_of_tile, ((float)2 * height_of_tile) / 3, WHITE);
    }
    else
    {
    // upper part
    DrawRectangle(x_of_tile, y_of_tile, width_of_tile, (float)height_of_tile / 3, RED);
    // lower part
    DrawRectangle(x_of_tile, y_of_tile + (float)height_of_tile / 3, width_of_tile, ((float)2 * height_of_tile) / 3, WHITE);   
    }
   
}
bool Tile::Chance(int percent)
{
    return GetRandomValue(1, 100) <= percent;
}

void Tile::WarningText(int tile_number,Player player)
{
    if (tile_number >= VARIENT_SPAWNER)
    {
        static int start_time = -1;
        if (start_time == -1) start_time = GetTime(); // only set when we first hit VARIENT_SPAWNER

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