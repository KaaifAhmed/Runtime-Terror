#include <iostream>
#include "player.h"
#include "raylib.h"
#include "constants.h"

using namespace std;

// Draw the player as a simple rectangle
void Player::Draw()
{
    DrawRectangle(x, y, width, height, color);
}

// Draw the hitbox visualization with upper and lower parts
void Player::Hitbox(Color c)
{
    float topHeight = height * HITBOX_SPLIT;
    float bottomHeight = height * (1.0f - HITBOX_SPLIT);

    // Upper part (non-collidable)
    DrawRectangle(x, y, width, topHeight, WHITE);

    // Lower part (collidable)
    DrawRectangle(x, y + topHeight, width, bottomHeight, c);
}

// Get the collidable bottom rectangle of the player
Rectangle Player::GetCollisionRect() const
{
    return { x, y + height * HITBOX_SPLIT, width, height * (1.0f - HITBOX_SPLIT) };
}

// Get the non-collidable top rectangle of the player
Rectangle Player::GetNonCollisionRect() const
{
    return { x, y, width, height * HITBOX_SPLIT };
}

// Update player physics: fall, jump, apply velocities
void Player::Update()
{
    Fall();
    Jump();
    y += y_velocity;
    x += x_velocity;
}

// Handle falling logic
void Player::Fall()
{
    if (!isReversed)
    {
        x_velocity = TILE_SPEED; // No horizontal movement for now, can be expanded later
    } else {
        x_velocity = -1 * TILE_SPEED;
    }
    
    // Stop falling if on ground
    if (y >= GROUND_POS)
    {
        y_velocity = 0;
        y = GROUND_POS + 5;  // Slight offset to prevent clipping
        DrawText("LOL NOOB! XD", SCREEN_WIDTH/4, 50, 100, WHITE);
        isGameOver = true;
    }

    // Apply gravity if in air
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

// Handle jumping logic
void Player::Jump()
{
    // Jump when space is pressed and not in air
    if (IsKeyPressed(KEY_SPACE))
    {
        if (!inAir)
        {
            y_velocity -= JUMP_HEIGHT;
            inAir = true;
        }
    }
}
