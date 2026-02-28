// tiles.h
#ifndef TILES_H
#define TILES_H
#include <vector>
#include <string>
#include "player.h"
#include "raylib.h"

enum TileType {
    SAFE_CODE,      // Normal platform (white/blue)
    SYNTAX_ERROR,   // Red jagged - instant death
    LOGIC_ERROR,    // Red background - slows player
    EXCEPTION,      // Moving hazard (throw e;)
    GAP             // Null gap - no platform
};

class Tile
{
public:
    float width_of_tile;
    float x_of_tile;
    float y_of_tile;
    TileType type;
    std::string codeText;
    
    // Exception movement
    float exceptionY;
    float exceptionPhase;
    
    static constexpr float height_of_tile = HEIGTH_TILE;
    static int tile_number;
    int normal_speed = TILE_SPEED;

    Tile(int startX, TileType t = SAFE_CODE);
    void Draw();
    bool Update(); // update position returns true if offscreen
    void Hitbox(Color c);
    static void Collision(Player &player, const std::vector<Tile *> &tiles);
    static void Cleanup(std::vector<Tile *> &tiles);
    static void Delete_And_Update(std::vector<Tile *> &tiles);
    static void New_tiles(std::vector<Tile *> &tiles);
    
    // Generate random code text based on type
    void GenerateCodeText();
    
    // Get color based on tile type
    Color GetTileColor();
    
    // Apply hazard effect to player
    void ApplyHazardEffect(Player &player);
};

#endif
