#pragma once
#include <vector>
#include "player.h"
#include "raylib.h"
struct Bhop_Buffer
{
    float framesLeft;
    const float maxFrames = 30; // 30/60 = 0.5 secs
};
class Tile
{
public:


static Bhop_Buffer spaceBuffer;
    float width;
    float x;
    float y;
    static constexpr float height_of_tile = HEIGHT_TILE;
    static int tile_number; // this tells us on which tile the player is on (after the player reaches the 10th tile the first 2 tiles are to be deleted)
    int normal_speed = TILE_SPEED;
    bool variance; // tells if a tile is red or white
    static int delay_count;

    Tile(int start_X);
    void Draw(bool variance);
    bool Update(float game_speed); // update position returns true if offscreen
    void Hitbox(Color c,bool variance);
    static void Collision(Player &player, const std::vector<Tile *> &tiles);
    static void Cleanup(std::vector<Tile *> &tiles);
    static void Delete_And_Update(std::vector<Tile *> &tiles, float game_speed);
    static void New_tiles(std::vector<Tile *> &tiles);
    bool Chance(int percent);
    static void WarningText(int number_of_tile, Player player);
};
