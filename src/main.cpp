#include <raylib.h>
#include "ball.h"

int main() 
{    
    const int screenWidth = 1280;
    const int screenHeight = 860;
    
    Ball ball;
    
    InitWindow(screenWidth, screenHeight, "Runtime Terror");
    SetTargetFPS(60);
    
    while (!WindowShouldClose())
    {
        BeginDrawing();
        EndDrawing();
    }
    
    CloseWindow();
}