#ifndef PLAYER_H
#define PLAYER_H
#include "raylib.h"
#include "constants.h"

class Player {
public:
    float x=PLAYER_START_X, y=PLAYER_START_Y;
    float y_velocity = 0, x_velocity = 0;
    
    float height=100, width=50;
    Color color=WHITE;

    int delay = 0;

    bool inAir = true;

    bool isGameOver = false;

    void Draw();
    void Hitbox(Color c);
    void Update();
    void Fall();
    void Move();
};

#endif