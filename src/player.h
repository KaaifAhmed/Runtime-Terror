#pragma once
#include "raylib.h"
#include "constants.h"
#include <queue>

// Define possible buffered actions
enum class Action {
    NONE,
    DASH,
    JUMP
};

enum class DeathCause {
    NONE,
    VOID_FALL,
    SYNTAX_ERROR,
    PORTAL_GAMBLE
};

class Player

{
public:
    float posX = PLAYER_START_X;
    float posY = PLAYER_START_Y;
    float velY = 0;
    float velX = 0;
    int jumpsAvailable = 0;

    float playerHeight = 100;
    float playerWidth = 50;
    Color color = WHITE;

    bool isRewinding = false;

    int delay = 0;
    bool inAir = true;
    bool isGameOver = false;
    DeathCause causeOfDeath = DeathCause::NONE;

    std::queue<Action> inputQueue;
    // Dash variables
    int dashFramesLeft = 0;
    float dashSpeed = 15.0f;
    int dashCharges = 0;
    float timeSinceLastDashRecharge = 0.0f;



    Rectangle GetCollisionRect() const;
    Rectangle GetNonCollisionRect() const;
    void Draw();
    void Hitbox(Color c);
    void Update();
    void Fall();
    void Jump();
    void Reset();
};
