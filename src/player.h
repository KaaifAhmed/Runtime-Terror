#ifndef PLAYER_H
#define PLAYER_H
#include "raylib.h"

class Player {
public:
    float x=50, y=30;
    float height=100, width=50;
    Color color=WHITE;

    void Draw();
    void Update();
};

#endif