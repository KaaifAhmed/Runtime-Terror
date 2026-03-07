#pragma once
#include <vector>
#include "player.h"
#include "raylib.h"
#include <iostream>

struct BhopBuffer
{
    float framesLeft;
    const float maxFrames = 30; // 30/60 = 0.5 secs
};

class Tile
{
public:
    static BhopBuffer bhopBuffer;
    float tileWidth;
    float tileX;
    float tileY;
    static constexpr float tileHeight = TILE_HEIGHT;
    static int currentTileIndex; // Tells which tile the player is currently on
    static float baseSpeed;
    enum class TileType
    {
        NORMAL,
        SYNTAX,
        LOGICAL
    };

    TileType tileType = TileType::NORMAL;

    Tile(int startX);
    void Draw(TileType type);
    bool Update(float gameSpeed);
    void Hitbox(Color c, TileType type);
    static void Collision(Player &player, const std::vector<Tile *> &tiles);
    static void Cleanup(std::vector<Tile *> &tiles);
    static void Delete_And_Update(std::vector<Tile *> &tiles, float gameSpeed);
    static void New_tiles(std::vector<Tile *> &tiles);
    static void WarningText(int tileIndex, Player player);

private:
    static void SmoothCountdown(float &value, float startValue);
    bool Chance(int percent);
};
