#pragma once
#include "raylib.h"
#include "constants.h"

class Player
{
public:

    float x = PLAYER_START_X, y = PLAYER_START_Y;
    float y_velocity = 0, x_velocity = 0;
    int jumpsLeft = 0;

    float height = 100, width = 50;
    Color color = WHITE;

    bool isReversed = false; // true if player is currently rewinding

    int delay = 0;

    bool inAir = true;

    bool isGameOver = false;

    Rectangle GetCollisionRect() const;
    Rectangle GetNonCollisionRect() const;
    void Draw();
    void Hitbox(Color c);
    void Update();
    void Fall();
    void Jump();
   
};
