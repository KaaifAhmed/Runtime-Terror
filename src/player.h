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
    float Rewind_time_left;

    // jump variables 
    int jumpsAvailable = 0;
    // Variable jump
    bool jumpHeld = false;
    int jumpHoldFrames = 0;

    // Fast fall
    bool isFastFalling = false;

    float playerWidth = 10;
    Color color = PLAYER_COLOR;

    bool isRewinding = false;

    int delay = 0;
    bool inAir = true;
    bool isGameOver = false;
    DeathCause causeOfDeath = DeathCause::NONE;
    int linesCompiled = 0; // Track lines of code successfully compiled

    std::queue<Action> inputQueue;
    // Dash variables
    int dashFramesLeft = 0;
    float dashSpeed = 15.0f;
    int dashCharges = 0;
    float timeSinceLastDashRecharge = 0.0f;

    Sound jumpSound;
    static float jumpSoundVolumn;

    Sound rewindSound;
    static float rewindSoundVolumn;

    Sound fallingDownSound;
    static float fallingDownSoundVolumn;

    void Init();    // call after InitAudioDevice()
    void Cleanup(); // call before CloseAudioDevice()

    Rectangle GetCollisionRect() const;
    Rectangle GetNonCollisionRect() const;
    void Draw();
    void Update();
    void Fall();
    void Jump();
    void Landed();
    static void ReduceRewind(float &rewind_time_left, float total_rewind_time);
};
