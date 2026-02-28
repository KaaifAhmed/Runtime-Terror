#ifndef CONSTANTS_H
#define CONSTANTS_H

// WORLD CONSTANTS
const int SCREEN_WIDTH = 1680;
const int SCREEN_HEIGHT = 900;
const int FPS = 60;

const int GRAVITY = 5;
const int GRAVITY_DELAY = 3;

const int GROUND_POS = SCREEN_HEIGHT;

// PLAYER CONSTANTS
const int PLAYER_SPEED = 6;
const int JUMP_HEIGHT = 25;

const float PLAYER_START_X = 50;
const float PLAYER_START_Y = 100;

// TILE CONSTANTS
const int HEIGTH_TILE = 50;
const int TILE_SPEED = 5;
const float TILES_START_X = 50;

// CLOCK SPEED (Auto-scroll) - increases over time
const float INITIAL_CLOCK_SPEED = 2.0f;
const float MAX_CLOCK_SPEED = 8.0f;
const float CLOCK_SPEED_INCREMENT = 0.1f;
const int CLOCK_SPEED_INCREASE_INTERVAL = 600; // Every 10 seconds (at 60 FPS)

#endif