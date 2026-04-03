#pragma once
#include <vector>
#include "player.h"
#include "raylib.h"
#include <iostream>

struct BhopBuffer
{
    float framesLeft;
    const float maxFrames = 30;
};

class Tile
{
public:
    static BhopBuffer bhopBuffer;
    
    static int TotaltilesCreatedCount; 
    static int tilesLeft;  
    int tileIndex;                // The individual tile's ID
    float tileWidth;
    float tileX;
    float tileY;
    int snippetStartIndex = 0;
    static constexpr float tileHeight = TILE_HEIGHT;
    static float baseSpeed;

    static Sound redTileCollisonSound;
    static float redTileCollisonSoundVolumn;

    static Sound LogicalTileCollisonSound;
    static float LogicalTileCollisonSoundVolumn;

    enum class TileType
    {
        NORMAL,
        SYNTAX,
        LOGICAL
    };

    TileType tileType = TileType::NORMAL;

    Tile(int startX);
    Tile(int startX, float tilewidth, TileType type);
    void Draw(TileType type);
    bool Update(float gameSpeed);
    static void Collision(Player &player, const std::vector<Tile *> &tiles);
    static void CleanupTiles(std::vector<Tile *> &tiles);
    static int CalculateRequiredLeftTiles(const std::vector<Tile *> &tiles, float scrollSpeed);
    static void Delete_And_Update(std::vector<Tile *> &tiles, float gameSpeed, float scrollSpeed);
    static void New_tiles(std::vector<Tile *> &tiles);
    static void Init();
    static float GetMaxTileWidth();


private:
    static void SmoothCountdown(float &value, float startValue);
    bool Chance(int percent);
};
