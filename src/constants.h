#pragma once
#include <raylib.h>

// GAME STATES
enum GameState { MENU, PLAYING, GAME_OVER };

// UI COLORS
const Color UI_PRIMARY = {30, 30, 40, 255};           // Dark blue-grey
const Color UI_ACCENT = {0, 210, 210, 255};           // Cyan
const Color UI_HIGHLIGHT = {255, 140, 0, 255};        // Orange
const Color UI_TEXT = {255, 255, 255, 255};           // White
const Color UI_TEXT_SECONDARY = {180, 180, 180, 255}; // Light grey
const Color PLAYER_COLOR = {100, 200, 255, 255};      // Light blue
const Color TILE_BG = {40, 50, 60, 255};              // Dark blue

// VS Code Dark+ Theme Colors
const Color VS_BG = { 30, 30, 30, 255 };          // Editor Background

// WORLD CONSTANTS
const int SCREEN_WIDTH = 1680;
const int SCREEN_HEIGHT = 900;
const int FPS = 60;
const int REWIND_SECS = 5;

const int GRAVITY = 5;
const int GRAVITY_DELAY = 3;

const int GROUND_POS = SCREEN_HEIGHT;



// HITBOX SPLIT — fraction of player height where the bottom (collidable) part starts
// Change this one value and both Hitbox() and collision checks update automatically
const float HITBOX_SPLIT = 0.90f; // bottom 2px out of 100px height

// TILE CONSTANTS
const int TILE_HEIGHT = 50;
const float TILE_SPEED = 7;
const float TILES_START_X = 50;
const int LEFT_TILES = 10;
const int TILE_Y = 4 * SCREEN_HEIGHT / 5;

const int VARIANT_TILE_INDEX = 2;
const int SCROLL_SPEED_DELAY = 50;
const float Speed_DELAY_DURATION = 1.0f;


// PLAYER CONSTANTS
const int JUMP_HEIGHT = 25;
const float PLAYER_HEIGHT = 100.0f;
const float PLAYER_START_X =TILES_START_X;
const float PLAYER_START_Y = TILE_Y -PLAYER_HEIGHT;

const float REWIND_DURATION=5;