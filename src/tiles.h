// tiles.h
#ifndef TILES_H
#define TILES_H
#include<vector>
#include "player.h"
#include "raylib.h"

class Tile
{
public:
    int width_of_tile;
    int x_of_tile;
    int y_of_tile;
    static const int HEIGTH_TILE = 20;
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
