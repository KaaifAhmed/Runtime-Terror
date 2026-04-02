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
  jumpSound = LoadSound("sounds\\jump.wav");
  SetSoundVolume(jumpSound, jumpSoundVolumn);

  rewindSound = LoadSound("sounds\\Time_rewind_sound.mp3");
  SetSoundVolume(rewindSound, rewindSoundVolumn);

  fallingDownSound = LoadSound("sounds\\falling_down_sound2.mp3");
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
  DrawRectangle(posX, posY, cursorWidth, PLAYER_HEIGHT, cursorColor);
}

Rectangle Player::GetCollisionRect() const {
  return {posX, posY +PLAYER_HEIGHT * HITBOX_SPLIT, playerWidth,
         PLAYER_HEIGHT * (1.0f - HITBOX_SPLIT)};
}

Rectangle Player::GetNonCollisionRect() const {
  return {posX, posY, playerWidth,PLAYER_HEIGHT * HITBOX_SPLIT};
}

// Update player physics: fall, jump, apply velocities
void Player::Update() {
  Fall();
  Jump();
  posY += velY;
  posX += velX;
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
