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
    float topHeight    = height * HITBOX_SPLIT;
    float bottomHeight = height * (1.0f - HITBOX_SPLIT);

    // upper part (dont go on the platform)
    DrawRectangle(x, y, width, topHeight, WHITE);

    // lower part (go on the plat) — change HITBOX_SPLIT and both halves adjust
    DrawRectangle(x, y + topHeight, width, bottomHeight, c);
}

// Returns the bottom (collidable) portion of the player as a Rectangle
// Always in sync with Hitbox() — both driven by HITBOX_SPLIT
Rectangle Player::GetCollisionRect() const
 {
    return { x, y + height * HITBOX_SPLIT, width, height * (1.0f - HITBOX_SPLIT) };
 }

 Rectangle Player::GetNonCollisionRect() const
{
    return { x, y, width, height * HITBOX_SPLIT };
}
void Player::Update()
{
    Fall();
    Jump();
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

void Player::Jump()
{
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
