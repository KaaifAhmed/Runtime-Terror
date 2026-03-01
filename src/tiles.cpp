#include "tiles.h"
#include "constants.h"
#include <string>

Tile::Tile(int startX)
{
    width_of_tile = GetRandomValue(200, 500);
    x_of_tile = startX;
    y_of_tile = GetRandomValue(4*SCREEN_HEIGHT/5, 4*SCREEN_HEIGHT/5 - 40);
}
int Tile::tile_number = 0;

void Tile::Draw()
{
    DrawRectangle(x_of_tile, y_of_tile, width_of_tile, height_of_tile, WHITE);
}

bool Tile::Update(float game_speed)
{
    x_of_tile -= normal_speed + game_speed; // move left
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

    if(tile_number >= LEFT_TILES)
    {
       for(int i = 1; i >= 0; i--)
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
        int lastRight = tiles.empty() ? SCREEN_WIDTH : tiles.back()->x_of_tile + tiles.back()->width_of_tile;
        int gap_between_tiles = GetRandomValue(100, 150);
        tiles.push_back(new Tile(lastRight + gap_between_tiles));
    }
}

void Tile::Collision(Player &player, const std::vector<Tile *> &tiles)
{
    bool collision = false;
    int overlap;
    int player_right = player.x + player.width;
  

    // checking collision
    for (long long unsigned int i=0;i<tiles.size();i++)
    {
        // checking of bottom of player with top of tile
        Rectangle tile_top = {tiles[i]->x_of_tile, tiles[i]->y_of_tile, tiles[i]->width_of_tile, ((float)tiles[i]->height_of_tile) / 3};
        Rectangle player_bottom = {player.x, player.y + (float)(2 * player.height) / 3, player.width, player.height / 3};

        // check the rest of player with the entire tile
        Rectangle player_top = {player.x, player.y, player.width, ((float)2 * player.height) / 3};
        Rectangle tile = {tiles[i]->x_of_tile, tiles[i]->y_of_tile, tiles[i]->width_of_tile, tiles[i]->height_of_tile};

        if (CheckCollisionRecs(player_bottom, tile_top))
        {
            player.inAir = false; // because he is on a solid block
            tile_number=i+1;
            player.y = tiles[i]->y_of_tile - player.height + 1;
            player.y_velocity = 0;
            collision = true;
            break;
        }
        else if (CheckCollisionRecs(player_top, tile))
        {

            if (player_right >= tile.x)
            {
                overlap = player_right - tile.x; // how far it has gone inside
                player.x = player.x - overlap + 1;
                collision = true;
                break;
            }
        }
    }
    // convert int -> string (the string OWNS the memory)
 std::string tileText = std::to_string(tile_number);

// get const char* from the string
const char* tile_cstr = tileText.c_str();
DrawText(tile_cstr,20,SCREEN_HEIGHT-60,45,WHITE);

    if (!collision)
    {
        player.inAir = true;
    }
}

void Tile::Cleanup(std::vector<Tile *> &tiles)
{
    // clean up remaining tiles
    for (Tile *t : tiles)
        delete t;
    tiles.clear();
}

void Tile::Hitbox(Color c)
{
    // upper part
    DrawRectangle(x_of_tile, y_of_tile, width_of_tile, (float)height_of_tile / 3, c);
    // lower part
    DrawRectangle(x_of_tile, y_of_tile + (float)height_of_tile / 3, width_of_tile, ((float)2 * height_of_tile) / 3, WHITE);
}