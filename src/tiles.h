// tiles.h
#ifndef TILES_H
#define TILES_H
#include<vector>
#include "player.h"
#include "raylib.h"

class Tile
{
public:
    float width_of_tile;
    float x_of_tile;
    float y_of_tile;
    static constexpr int HEIGTH_TILE = 50;
    static const int normal_speed = 4;

    Tile(int start_X); 
    void Draw();         
    bool Update();       // update position returns true if offscreen
   static void Collision(Player &player, const std::vector<Tile*>& tiles);
   static void Cleanup(std::vector<Tile*>& tiles);
   static void Delete_And_Update(std::vector<Tile*>& tiles);
   static void New_tiles(std::vector<Tile*>& tiles);

};

#endif
