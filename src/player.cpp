#include <iostream>
#include "player.h"
#include "raylib.h"
#include "constants.h"

void Player::Draw()
{
    DrawRectangle(x, y, width, height, color);
}

void Player::Update()
{
    y += y_velocity;
    Fall();
    Move();    
}

void Player::Fall()
{
    // STOP FALLING IF ON GROUND
    // NEED TO REPLACE THIS AFTER COLLISION SETUP
    if ((y + height) >= SCREEN_HEIGHT - 10)
    {
        inAir = false;
        y_velocity = 0;
        y = SCREEN_HEIGHT - 10 - height;
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
        x += PLAYER_SPEED;
    }
    else if (IsKeyDown(KEY_LEFT))
    {
        x -= PLAYER_SPEED;
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