#ifndef PLAYER_H
#define PLAYER_H
#include "raylib.h"
#include "constants.h"

class Player {
public:
    float x=PLAYER_START_X, y=PLAYER_START_Y;
    float height=100, width=50;
    Color color=WHITE;

    bool isJumping = false;
    bool inAir = false;

    void Draw();
    void Update();
};

#endif