#include "tiles.h"
#include "constants.h"

Tile::Tile(int startX)
{
    width_of_tile = GetRandomValue(200, 500);
    x_of_tile = startX;
    y_of_tile = GetRandomValue(4*SCREEN_HEIGHT/5, 4*SCREEN_HEIGHT/5 - 40);
}

void Tile::Draw()
{
    DrawRectangle(x_of_tile, y_of_tile, width_of_tile, height_of_tile, WHITE);
}

bool Tile::Update()
{
    x_of_tile -= normal_speed;               // move left
    return (x_of_tile + width_of_tile <= 0); // true if offscreen
}

void Tile::Delete_And_Update(std::vector<Tile *> &tiles)
{
    // updating the tiles
    for (int i = tiles.size() - 1; i >= 0; i--)
    {
        if (tiles[i]->Update())
        {
            delete tiles[i];                // delete tile if offscreen
            tiles.erase(tiles.begin() + i); // remove from vector
        }
    }
}
void Tile::New_tiles(std::vector<Tile *> &tiles)
{
    //  spawn new tiles
    if (tiles.empty() || tiles.back()->x_of_tile + tiles.back()->width_of_tile < SCREEN_WIDTH)
    {
        int lastRight = tiles.empty() ? SCREEN_WIDTH : tiles.back()->x_of_tile + tiles.back()->width_of_tile;
        tiles.push_back(new Tile(lastRight + GetRandomValue(100, 200))); // this is the gap between the new values
    }
}

void Tile::Collision(Player &player, const std::vector<Tile *> &tiles)
{
    bool collision = false;
    int overlap;
    int player_right = player.x + player.width;

    // checking collision
    for (Tile *t : tiles)
    {
        // checking of bottom of player with top of tile
        Rectangle tile_top = {t->x_of_tile, t->y_of_tile, t->width_of_tile, ((float)t->height_of_tile) / 3};
        Rectangle player_bottom = {player.x, player.y + (float)(2 * player.height) / 3, player.width, player.height / 3};

        // check the rest of player with the entire tile
        Rectangle player_top = {player.x, player.y, player.width, ((float)2 * player.height) / 3};
        Rectangle tile = {t->x_of_tile, t->y_of_tile, t->width_of_tile, t->height_of_tile};

        if (CheckCollisionRecs(player_bottom, tile_top))
        {
            player.inAir = false; // because he is on a solid block

            player.y = t->y_of_tile - player.height + 1;
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