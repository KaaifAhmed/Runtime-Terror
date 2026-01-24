#include "player.h"
#include "raylib.h"
#include "constants.h"

void Player::Draw()
{
    DrawRectangle(x, y, width, height, color);
}

void Player::Update()
{
    if (IsKeyDown(KEY_RIGHT))
    {
        x += PLAYER_SPEED;
    }

    if (IsKeyDown(KEY_LEFT))
    {
        x -= PLAYER_SPEED;
    }

    if (IsKeyDown(KEY_SPACE))
    {
        y -= JUMP_HEIGHT;
    }
    

    if ((y+height) < SCREEN_HEIGHT-10)
    {
        y += GRAVITY;
    }
}