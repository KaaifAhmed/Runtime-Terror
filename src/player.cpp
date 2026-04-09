#include "player.h"
#include "constants.h"
#include "raylib.h"
#include <cmath> 
#include <iostream>

using namespace std;

float Player::jumpSoundVolumn = 0.1f;
float Player::rewindSoundVolumn = 0.5f;
float Player::fallingDownSoundVolumn = 1.0f;
// Call this after InitAudioDevice()
void Player::Init() {
  jumpSound = LoadSound("assets/audio/jump.wav");
  SetSoundVolume(jumpSound, jumpSoundVolumn);

  rewindSound = LoadSound("assets/audio/Time_rewind_sound.mp3");
  SetSoundVolume(rewindSound, rewindSoundVolumn);

  fallingDownSound = LoadSound("assets/audio/falling_down_sound2.mp3");
  SetSoundVolume(fallingDownSound, fallingDownSoundVolumn);
}

void Player::Cleanup() {
  if (jumpSound.frameCount >
      0) // frameCount checks if the sound was properly initialized,
  {
    UnloadSound(jumpSound); // otherwise we would be unloading a sound that
                            // never existed
  }

  if (rewindSound.frameCount > 0) {
    UnloadSound(rewindSound);
  }

  if (fallingDownSound.frameCount > 0) {
    UnloadSound(fallingDownSound);
  }
}
void Player::Draw() {
  // Calculate Pulse for blinking terminal block cursor
  float blink = (sinf(GetTime() * 10.0f) + 1.0f) / 2.0f;
  unsigned char alpha = (unsigned char)((blink * 200) + 55);

  if (this->isRewinding || this->inAir)
    alpha = 255;

  Color cursorColor = {PLAYER_COLOR.r, PLAYER_COLOR.g, PLAYER_COLOR.b, alpha};

  // Draw the Main Cursor as a thin typing line
  int cursorWidth = playerWidth;
  float currentHeight = (dashFramesLeft > 0) ? PLAYER_HEIGHT / 2 : PLAYER_HEIGHT;
  DrawRectangle(posX, posY + (PLAYER_HEIGHT - currentHeight), cursorWidth, currentHeight, cursorColor);
}

Rectangle Player::GetCollisionRect() const {
  float currentHeight = (dashFramesLeft > 0) ? PLAYER_HEIGHT / 2 : PLAYER_HEIGHT;
  return {posX, posY + currentHeight * HITBOX_SPLIT, playerWidth,
         currentHeight * (1.0f - HITBOX_SPLIT)};
}

Rectangle Player::GetNonCollisionRect() const {
  float currentHeight = (dashFramesLeft > 0) ? PLAYER_HEIGHT / 2 : PLAYER_HEIGHT;
  return {posX, posY, playerWidth, currentHeight * HITBOX_SPLIT};
}

// Update player physics: fall, jump, apply velocities
void Player::Update() {
  // Update Recharge Timer (1 charge every 5 seconds, infinite capacity)
  if (!isGameOver) {
      timeSinceLastDashRecharge += GetFrameTime();
      if (timeSinceLastDashRecharge >= 5.0f) {
          dashCharges++;
          timeSinceLastDashRecharge = 0.0f;
      }
  }

  // 1. Record Inputs into the Queue
  if (IsKeyPressed(KEY_LEFT_SHIFT)) {
      if (dashCharges > 0) {
          inputQueue.push(Action::DASH);
      }
  }
  
  if (inputQueue.size() > 2) inputQueue.pop(); 

  // 2. Process Dash Physics if currently dashing
  if (dashFramesLeft > 0) {
      velY = 0; // Ignore gravity while dashing
      posX += dashSpeed;
      dashFramesLeft--;
  } 
  else {
      Fall();
      Jump();
      posY += velY;
      posX += velX;
      
      // Return to center if pushed forward by dashing or collisions
      if (posX > PLAYER_START_X) {
          posX -= 4.0f; 
          if (posX < PLAYER_START_X) posX = PLAYER_START_X;
      } else if (posX < PLAYER_START_X) {
          posX += 4.0f;
          if (posX > PLAYER_START_X) posX = PLAYER_START_X;
      }
      
      // 3. Check Queue when we are grounded and ready to act
      if (!inAir) {
          if (!inputQueue.empty()) {
              Action nextAction = inputQueue.front();
              while (!inputQueue.empty()) inputQueue.pop();
              if (nextAction == Action::DASH && dashCharges > 0 && posX < SCREEN_WIDTH - playerWidth - 100) {
                  dashFramesLeft = 20; 
                  dashCharges--; 
              }
          }
      }
  }
}

void Player::Jump() {
    if (IsKeyPressed(KEY_SPACE)) {
        if (!inAir) {
            velY -= JUMP_HEIGHT;
            inAir = true;
            jumpsAvailable = 1;
            PlaySound(jumpSound);
        } else if (jumpsAvailable > 0) {
            velY = -JUMP_HEIGHT;
            jumpsAvailable--;
            PlaySound(jumpSound);
        }
    }
}

void Player::Fall() {
    if (posY >= GROUND_POS) {
        velY = 0;
        posY = GROUND_POS + 5;
        if (!isGameOver) PlaySound(fallingDownSound);
        isGameOver = true;
        causeOfDeath = DeathCause::VOID_FALL;
    }

    // Fast fall input handled here since it's downward physics
    if (IsKeyPressed(KEY_DOWN) && inAir) {
        isFastFalling = true;
        jumpHeld = false;
    }
    if (IsKeyReleased(KEY_DOWN)) isFastFalling = false;

    if (inAir) {
        delay++;
        if (delay == GRAVITY_DELAY) {
            delay = 0;
            velY += isFastFalling ? GRAVITY * FAST_FALL_MULTIPLIER : GRAVITY;
        }
    }
}
void Player::ReduceRewind(float &rewind_time_left, float total_rewind_time) {
  float rate = total_rewind_time / REWIND_DURATION;
  rewind_time_left -= rate * GetFrameTime();
  if (rewind_time_left < 0)
    rewind_time_left = 0;
}

void Player::Landed()
{
jumpHeld = false;
jumpHoldFrames = 0;
jumpsAvailable=0;
delay = 0;
inAir = false;
isFastFalling = false;
}
