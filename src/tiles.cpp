#include "tiles.h"
#include "constants.h"

Tile::Tile(int startX)
{
    width_of_tile = GetRandomValue(200, 500);
    x_of_tile = startX;
    y_of_tile = GetRandomValue(SCREEN_HEIGHT - 32 - 40, SCREEN_HEIGHT - 32);
}

void Tile::Draw()
{
    DrawRectangle(x_of_tile, y_of_tile, width_of_tile, HEIGTH_TILE, WHITE);
}

bool Tile::Update()
{
    x_of_tile -= normal_speed;               // move left
    return (x_of_tile + width_of_tile <= 0); // true if offscreen
}

void Tile::Collision(Player &player, const std::vector<Tile *> &tiles)
{
    bool collision = false;
    // checking collision
    for (Tile *t : tiles)
    {
        // first need to convert them both to rect
        Rectangle tileRect = {t->x_of_tile, t->y_of_tile, t->width_of_tile, Tile::HEIGTH_TILE};
        Rectangle player_copy = {player.x, player.y, player.width, player.height};
        if (CheckCollisionRecs(player_copy, tileRect))
        {
            player.inAir = false; // because he is on a solid block
            player.y = t->y_of_tile - player.height + 1;
            player.y_velocity = 0;
            collision = true;
            break;
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
        tiles.push_back(new Tile(lastRight + GetRandomValue(50, 150))); // this is the gap between the new values
    }
}
