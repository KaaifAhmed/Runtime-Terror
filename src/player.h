#pragma once
#include "raylib.h"
#include "constants.h"

class Player
{
public:
    float posX = PLAYER_START_X;
    float posY = PLAYER_START_Y;
    float velY = 0;
    float velX = 0;
    float Rewind_time_left;

    int jumpsAvailable = 0;

    float playerHeight = 100;
    float playerWidth = 50;
    Color color = PLAYER_COLOR;

    bool isRewinding = false;

    int delay = 0;
    bool inAir = true;
    bool isGameOver = false;
    int linesCompiled = 0;  // Track lines of code successfully compiled

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
    void Hitbox(Color c);
    void Update();
    void Fall();
    void Jump();
    static void ReduceRewind(float &rewind_time_left, float total_rewind_time);
};