#pragma once
#include "raylib.h"
#include "constants.h"

class Player
{
public:
    float posX = PLAYER_START_X;
    float posY = PLAYER_START_Y;
    float velY = 0;
    float velX = 0;
    int jumpsAvailable = 0;

    float playerHeight = 100;
    float playerWidth = 50;
    Color color = WHITE;

    bool isRewinding = false;

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
