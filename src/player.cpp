#include <iostream>
#include "player.h"
#include "raylib.h"
#include "constants.h"

using namespace std;

void Player::Draw()
{
    float currentHeight = (dashFramesLeft > 0) ? playerHeight / 2 : playerHeight;
    DrawRectangle(posX, posY + (playerHeight - currentHeight), playerWidth, currentHeight, color);
}


// Draw the hitbox with upper and lower parts
void Player::Hitbox(Color c)
{
    float currentHeight = (dashFramesLeft > 0) ? playerHeight / 2 : playerHeight;
    float topRegion = currentHeight * HITBOX_SPLIT;
    float bottomRegion = currentHeight - topRegion;
    DrawRectangle(posX, posY + (playerHeight - currentHeight), playerWidth, topRegion, WHITE);
    DrawRectangle(posX, posY + (playerHeight - currentHeight) + topRegion, playerWidth, bottomRegion, c);
}

Rectangle Player::GetCollisionRect() const
{
    float currentHeight = (dashFramesLeft > 0) ? playerHeight / 2 : playerHeight;
    return {posX, posY + currentHeight * HITBOX_SPLIT, playerWidth, currentHeight * (1.0f - HITBOX_SPLIT)};
}

Rectangle Player::GetNonCollisionRect() const
{
    float currentHeight = (dashFramesLeft > 0) ? playerHeight / 2 : playerHeight;
    return {posX, posY, playerWidth, currentHeight * HITBOX_SPLIT};
}

// Update player physics: fall, jump, apply velocities
void Player::Update()
{
    // Update Recharge Timer (1 charge every 5 seconds, infinite capacity)
    if (!isGameOver) {
        timeSinceLastDashRecharge += GetFrameTime();
        if (timeSinceLastDashRecharge >= 5.0f) {
            dashCharges++;
            timeSinceLastDashRecharge = 0.0f;
        }
    }


    if (isGameOver)
        DrawText("NOOB! XD", SCREEN_WIDTH/3, 50, 100, WHITE);


    // 1. Record Inputs into the Queue
    if (IsKeyPressed(KEY_LEFT_SHIFT)) {
        if (dashCharges > 0) {
            inputQueue.push(Action::DASH);
        }
    }
    
    // Optional: Keep queue small so old inputs don't trigger 5 seconds later
    if (inputQueue.size() > 2) inputQueue.pop(); 

    // 2. Process Dash Physics if currently dashing
    if (dashFramesLeft > 0) {
        velY = 0; // Ignore gravity while dashing
        posX += dashSpeed;
        dashFramesLeft--;
    } 
    else {
        // Normal Physics
        Fall();
        Jump();
        posY += velY;
        posX += velX;
        
        // Return to center if pushed forward by dashing or collisions
        if (posX > PLAYER_START_X) {
            posX -= 4.0f; // Softly pull back (increased for faster return after double-dash)
            if (posX < PLAYER_START_X) posX = PLAYER_START_X;
        } else if (posX < PLAYER_START_X) {
             // In case they get stuck behind
            posX += 4.0f;
            if (posX > PLAYER_START_X) posX = PLAYER_START_X;
        }
        
        // 3. Check Queue when we are grounded and ready to act
        // We only allow a dash if we have returned to the starting position
        if (!inAir) {
            if (!inputQueue.empty()) {
                Action nextAction = inputQueue.front();
                
                // Keep popping until queue empty to prevent stale dashes buildup
                while (!inputQueue.empty()) inputQueue.pop();
                
                // Allow dashing as long as they have charges and aren't literally off the screen
                if (nextAction == Action::DASH && dashCharges > 0 && posX < SCREEN_WIDTH - playerWidth - 100) {
                    dashFramesLeft = 20; // Dash lasts for 20 frames
                    dashCharges--; // Consume a charge immediately
                }
            }
        }
    }
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

void Player::Reset()
{
    posX = PLAYER_START_X;
    posY = PLAYER_START_Y;
    velY = 0;
    velX = 0;
    jumpsAvailable = 0;
    delay = 0;
    inAir = true;
    isGameOver = false;
    dashFramesLeft = 0;
    dashCharges = 0;
    timeSinceLastDashRecharge = 0.0f;
    while (!inputQueue.empty()) {
        inputQueue.pop();
    }
}
