#include <iostream>
#include "player.h"
#include "raylib.h"
#include "constants.h"

using namespace std;

void Player::Draw()
{
    DrawRectangle(x, y, width, height, color);
}

void Player::Update()
{
    Fall();
    Move();
    y += y_velocity;
    x += x_velocity;

}

void Player::Fall()
{
    // STOP FALLING IF ON GROUND
    // NEED TO REPLACE THIS AFTER COLLISION SETUP
    if ((y + height) >= GROUND_POS)
    {
        inAir = false;
        y_velocity = 0;
        y = GROUND_POS - height;
    }
    

    // FALL DOWNWARDS IF IN AIR
    if (inAir)
    {
        y_velocity += GRAVITY;
    }
}

void Player::Move() {
    // MOVE LEFT AND RIGHT USING LEFT/RIGHT KEYS
    if (IsKeyDown(KEY_RIGHT))
    {
        x_velocity = PLAYER_SPEED;
    }
    else if (IsKeyDown(KEY_LEFT))
    {
        x_velocity = -PLAYER_SPEED;
    }
    else {
        x_velocity = 0;
    }
    
    // JUMP WHEN SPACE KEY PRESSED
    if (IsKeyPressed(KEY_SPACE))
    {
        if (!inAir)
        {
            y_velocity -= JUMP_HEIGHT;
            inAir = true;
        }
    }
}