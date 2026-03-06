#pragma once
#include <vector>
#include "player.h"
#include "raylib.h"
#include <iostream>
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
    static float normal_speed;
    enum class TileType
    {
        NORMAL,
        SYNTAX,
        LOGICAL
    };

    TileType variance = TileType::NORMAL;

    Tile(int start_X);
    void Draw(TileType variance);
    bool Update(float game_speed); // update position returns true if offscreen
    void Hitbox(Color c, TileType variance);
    static void Collision(Player &player, const std::vector<Tile *> &tiles);
    static void Cleanup(std::vector<Tile *> &tiles);
    static void Delete_And_Update(std::vector<Tile *> &tiles, float game_speed);
    static void New_tiles(std::vector<Tile *> &tiles);
    static void WarningText(int number_of_tile, Player player);
    
    private:
    static void SmoothCountdown(float &value, float startValue);
    
     bool Chance(int percent);
};
