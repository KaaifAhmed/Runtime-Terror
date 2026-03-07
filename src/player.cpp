#include <iostream>
#include "player.h"
#include "raylib.h"
#include "constants.h"

using namespace std;

void Player::Draw()
{
    DrawRectangle(posX, posY, playerWidth, playerHeight, color);
}

// Draw the hitbox with upper and lower parts
void Player::Hitbox(Color c)
{
    float topRegion = playerHeight * HITBOX_SPLIT;
    float bottomRegion = playerHeight - topRegion;
    DrawRectangle(posX, posY, playerWidth, topRegion, WHITE);
    DrawRectangle(posX, posY + topRegion, playerWidth, bottomRegion, c);
}

Rectangle Player::GetCollisionRect() const
{
    return {posX, posY + playerHeight * HITBOX_SPLIT, playerWidth, playerHeight * (1.0f - HITBOX_SPLIT)};
}

Rectangle Player::GetNonCollisionRect() const
{
    return {posX, posY, playerWidth, playerHeight * HITBOX_SPLIT};
}

// Update player physics: fall, jump, apply velocities
void Player::Update()
{
    if (isGameOver)
        DrawText("NOOB! XD", SCREEN_WIDTH/3, 50, 100, WHITE);

    Fall();
    Jump();
    posY += velY;
    posX += velX;
}

void Player::Fall()
{
    if (posY >= GROUND_POS)
    {
        velY = 0;
        posY = GROUND_POS + 5;
        isGameOver = true;
    }

    // Apply gravity if in air
    if (inAir)
    {
        delay++;
        if (delay == GRAVITY_DELAY)
        {
            delay = 0;
            velY += GRAVITY;
        }
    }
}

// Handle jumping logic
void Player::Jump()
{
    if (IsKeyPressed(KEY_SPACE))
    {
        if (!inAir)
        {
            velY -= JUMP_HEIGHT;
            inAir = true;
            jumpsAvailable = 1;
        }
        else if (jumpsAvailable > 0)
        {
            velY = -JUMP_HEIGHT;
            jumpsAvailable--;
        }
    }
}
