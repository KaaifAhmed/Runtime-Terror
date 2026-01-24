#include <raylib.h>
#include <string>
#include "constants.h"
#include "player.h"

using namespace std;

int FPS = 60;

int main()
{
    Player player;
    
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Runtime Terror");
    SetTargetFPS(FPS);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        player.Update();
        

        ClearBackground(BLACK);
        
        player.Draw();

        EndDrawing();
    }

    CloseWindow();
}