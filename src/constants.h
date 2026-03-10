#pragma once

// WORLD CONSTANTS
const int SCREEN_WIDTH = 1680;
const int SCREEN_HEIGHT = 900;
const int FPS = 60;
const int REWIND_SECS = 5;

const int GRAVITY = 5;
const int GRAVITY_DELAY = 3;

const int GROUND_POS = SCREEN_HEIGHT;

// PLAYER CONSTANTS
const int JUMP_HEIGHT = 25;

const float PLAYER_START_X = 100;
const float PLAYER_START_Y = 100;

// HITBOX SPLIT — fraction of player height where the bottom (collidable) part starts
// Change this one value and both Hitbox() and collision checks update automatically
const float HITBOX_SPLIT = 0.90f;  // bottom 2px out of 100px height

// TILE CONSTANTS
const int TILE_HEIGHT = 50;
const float TILE_SPEED = 7;
const float TILES_START_X = PLAYER_START_X - 50;
const int LEFT_TILES = 10;

const int TILE_Y = 4 * SCREEN_HEIGHT / 5;

const int VARIANT_TILE_INDEX = 2;
const int SCROLL_SPEED_DELAY = 50;
const float Speed_DELAY_DURATION = 1.0f;