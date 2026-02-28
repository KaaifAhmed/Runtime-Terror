#include <iostream>
#include "player.h"
#include "raylib.h"
#include "constants.h"

using namespace std;

void Player::Draw()
{
    DrawRectangle(x, y, width, height, color);
}

void Player::Hitbox(Color c)
{
    // upper part (dont go on the platform)
    DrawRectangle(x, y, width, (float)(2 * height) / 3, WHITE);

    // lower part(go on the plat)
    DrawRectangle(x, y + (float)(2 * height) / 3, width, (float)(height) / 3, c);
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
    if (y >= GROUND_POS)
    {
        y_velocity = 0;
        y = GROUND_POS + 5;
        DrawText("LOL NOOB! XD", SCREEN_WIDTH/4, 50, 100, WHITE);
        isGameOver = true;
    }

    // FALL DOWNWARDS IF IN AIR
    if (inAir)
    {
        delay++;
        if (delay == GRAVITY_DELAY)
        {
            delay = 0;
            y_velocity += GRAVITY;
        }
        
    }
}

void Player::Move()
{
    // MOVE LEFT AND RIGHT USING LEFT/RIGHT KEYS
    if (IsKeyDown(KEY_RIGHT))
    {
        x_velocity = PLAYER_SPEED;
    }
    else if (IsKeyDown(KEY_LEFT))
    {
        x_velocity = -PLAYER_SPEED;
    }
    else
    {
        x_velocity = 0;
    }

    // JUMP WHEN SPACE KEY PRESSED
    if (IsKeyPressed(KEY_SPACE))
    {
        if (jumpCount < MAX_JUMPS)
        {
            y_velocity = -JUMP_HEIGHT;
            jumpCount++;
            inAir = true;
        }
    }
}