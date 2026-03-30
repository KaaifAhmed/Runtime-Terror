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
  // Calculate Pulse
  float blink = (sinf(GetTime() * 10.0f) + 1.0f) / 2.0f;
  unsigned char alpha = (unsigned char)((blink * 180) + 75);

  if (this->isRewinding || this->inAir)
    alpha = 255;

  Color cursorColor = {UI_ACCENT.r, UI_ACCENT.g, UI_ACCENT.b, alpha};

  // Draw the Glow (Using the explicit Rectangle to stay safe)
  Rectangle glowRect = {posX - 2, posY, 10,PLAYER_HEIGHT};
  DrawRectangleLinesEx(glowRect, 1.0f, ColorAlpha(UI_ACCENT, 0.3f));

  // Draw the Main Cursor
  DrawRectangle(posX, posY, 6,PLAYER_HEIGHT, cursorColor);

  // Draw the "I-Beam" tips
  DrawRectangle(posX - 4, posY, 14, 2, cursorColor); // Top bar
<<<<<<< HEAD
  DrawRectangle(posX - 4, posY + playerHeight - 2, 14, 2,
                cursorColor); // Bottom bar
=======
  DrawRectangle(posX - 4, posY +PLAYER_HEIGHT - 2, 14, 2, cursorColor); // Bottom bar
>>>>>>> 0433f93f98ad94049afdab3310909ff9383af4a3
}

// Draw the hitbox with upper and lower parts
void Player::Hitbox(Color c) {
  float topRegion =PLAYER_HEIGHT * HITBOX_SPLIT;
  float bottomRegion =PLAYER_HEIGHT - topRegion;
  DrawRectangle(posX, posY, playerWidth, topRegion, WHITE);
  DrawRectangle(posX, posY + topRegion, playerWidth, bottomRegion, c);
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

void Player::Fall() {
  if (posY >= GROUND_POS) {
    velY = 0;
    posY = GROUND_POS + 5;
    if (!isGameOver) {
      PlaySound(fallingDownSound);
    }
    isGameOver = true;
  }

  // Apply gravity if in air
  if (inAir) {
    delay++;
    if (delay == GRAVITY_DELAY) {
      delay = 0;
      velY += GRAVITY;
    }
  }
}

// Handle jumping logic
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

void Player::ReduceRewind(float &rewind_time_left, float total_rewind_time) {
  float rate = total_rewind_time / REWIND_DURATION;
  rewind_time_left -= rate * GetFrameTime();
  if (rewind_time_left < 0)
    rewind_time_left = 0;
}