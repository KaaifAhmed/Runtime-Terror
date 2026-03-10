#pragma once
#include "raylib.h"

// VS Code Dark Theme Colors
const Color VSCode_BACKGROUND = {30, 30, 30, 255};      // #1E1E1E
const Color VSCode_SIDEBAR = {37, 37, 38, 255};         // #252526
const Color VSCode_TEXT = {212, 212, 212, 255};         // #D4D4D4
const Color VSCode_BLUE = {0, 122, 204, 255};           // #007ACC
const Color VSCode_GREEN = {78, 201, 176, 255};         // #4EC9B0
const Color VSCode_RED = {244, 71, 71, 255};            // #F44747
const Color VSCode_YELLOW = {220, 220, 170, 255};       // #DCDCAA

// Game states
enum GameState {
    MENU,
    PLAYING,
    PAUSED,
    GAMEOVER
};

// Simple button structure
struct Button {
    float x, y;
    float width, height;
    const char* text;
    Color color;
    Color hoverColor;
    Color textColor;
};

// Menu functions
void DrawMainMenu(Button& playBtn, Button& quitBtn);
void DrawPauseMenu(Button& resumeBtn, Button& quitBtn);
void DrawGameOverScreen(int score, Button& menuBtn, Button& quitBtn);
bool IsButtonClicked(Button& btn);
bool IsMouseOverButton(Button& btn);
void DrawButton(Button& btn);
