#pragma once
#include <raylib.h>

// GAME STATES
enum GameState { MENU, PLAYING, PAUSED, GAME_OVER, LEADERBOARD, CREDITS };

// UI COLORS (Updated for VS Code Theme)
const Color UI_PRIMARY = {30, 30, 30, 255};           // VS Code Editor BG
const Color UI_ACCENT = {0, 122, 204, 255};           // VS Code Focus Blue
const Color UI_HIGHLIGHT = {255, 140, 0, 255};        // VS Code Warning Orange
const Color UI_TEXT = {204, 204, 204, 255};           // VS Code Normal Text
const Color UI_TEXT_SECONDARY = {153, 153, 153, 255}; // VS Code Muted Text
const Color PLAYER_COLOR = {204, 204, 204, 255};      // Execution pointer color
const Color TILE_BG = {30, 30, 30, 255};              // Same as editor BG

// VS Code Dark+ Theme Colors
const Color VS_BG = { 30, 30, 30, 255 };          // Editor Background

// Global font declaration
extern Font vsFont;
extern Font codeFont;
extern Texture2D sideBarTex;

// Font Wrappers replacing Raylib's default DrawText/MeasureText
inline void DrawVSText(const char *text, int posX, int posY, int fontSize, Color color) {
    DrawTextEx(vsFont, text, {(float)posX, (float)posY}, (float)fontSize, 1.0f, color);
}

inline int MeasureVSText(const char *text, int fontSize) {
    return (int)MeasureTextEx(vsFont, text, (float)fontSize, 1.0f).x;
}

inline void DrawCodeText(const char *text, int posX, int posY, int fontSize, Color color) {
    DrawTextEx(codeFont, text, {(float)posX, (float)posY}, (float)fontSize, 1.0f, color);
}

inline int MeasureCodeText(const char *text, int fontSize) {
    return (int)MeasureTextEx(codeFont, text, (float)fontSize, 1.0f).x;
}

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
const int OFFSET=50; // to precisely control how far up or down the game starts
const int TILE_HEIGHT = 50;
const float TILE_SPEED = 7;
const float TILES_START_X = 100;
const int LEFT_TILES = 10;
const int TILE_Y = 4 * SCREEN_HEIGHT / 5 - OFFSET;

const int VARIANT_TILE_INDEX = 2;
const int SCROLL_SPEED_DELAY = 50;
const float Speed_DELAY_DURATION = 1.0f;


// PLAYER CONSTANTS

const float PLAYER_HEIGHT = 100.0f;
const float PLAYER_START_X =TILES_START_X;
const float PLAYER_START_Y = TILE_Y -PLAYER_HEIGHT;
const float REWIND_DURATION=5;
constexpr float FAST_FALL_MULTIPLIER=3.0f;
const int JUMP_HEIGHT=25;