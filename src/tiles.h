// tiles.h
#ifndef TILES_H
#define TILES_H
#include <vector>
#include "player.h"
#include "raylib.h"

class Tile
{
public:
    float width_of_tile;
    float x_of_tile;
    float y_of_tile;
    static constexpr float height_of_tile = HEIGTH_TILE;
    static int tile_number; // this tells us on which tile the player is on (after the player reaches the 10th tile the first 2 tiles are to be deleted)
    int normal_speed = TILE_SPEED;

    Tile(int start_X);
    void Draw();
    bool Update(float game_speed); // update position returns true if offscreen
    void Hitbox(Color c);
    static void Collision(Player &player, const std::vector<Tile *> &tiles);
    static void Cleanup(std::vector<Tile *> &tiles);
    static void Delete_And_Update(std::vector<Tile *> &tiles, float game_speed);
    static void New_tiles(std::vector<Tile *> &tiles);
};

#endif
