#include <raylib.h>
#include <string>
#include "constants.h"
#include "player.h"
#include <vector>
#include "tiles.h"
#include "rewind.h"

using namespace std;

void drawHitbox();
void drawTiles();
void updateTiles();
void drawUI();

Player player;
vector<Tile *> tiles;
Camera2D camera = {0};

// Clock speed (auto-scroll) - represents CPU clock
float clockSpeed = INITIAL_CLOCK_SPEED;
int frameCount = 0;
float cameraTargetX = 0;

// Score - Lines of Code executed
int linesOfCode = 0;
int maxLinesOfCode = 0; // For high score tracking

// Rewind system state
bool isRewinding = false;
RewindBuffer rewindBuffer;
std::vector<GCToken> gcTokens;

// RAM system
const int MAX_RAM = 100;
int currentRAM = MAX_RAM;

int main()
{
    // Adding the initial tile
    tiles.push_back(new Tile(TILES_START_X));

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Runtime Terror");
    SetTargetFPS(FPS);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        BeginMode2D(camera);
        
        // Handle rewind input
        if (IsKeyDown(KEY_Z) && rewindBuffer.CanRewind() && currentRAM > 0) {
            isRewinding = true;
        } else {
            isRewinding = false;
        }
        
        // Rewind mode
        if (isRewinding && rewindBuffer.CanRewind()) {
            GameState prevState = rewindBuffer.Rewind();
            player.x = prevState.playerX;
            player.y = prevState.playerY;
            player.x_velocity = prevState.playerXVel;
            player.y_velocity = prevState.playerYVel;
            player.inAir = prevState.inAir;
            player.jumpCount = prevState.jumpCount;
            cameraTargetX = prevState.cameraTargetX;
            clockSpeed = prevState.clockSpeed;
            frameCount = prevState.frameCount;
            linesOfCode = prevState.linesOfCode;
            currentRAM -= 2; // Drain RAM while rewinding
            if (currentRAM < 0) currentRAM = 0;
        }
        // Normal gameplay recording
        else if (!player.isGameOver) {
            // Record state for rewind
            GameState currentState;
            currentState.playerX = player.x;
            currentState.playerY = player.y;
            currentState.playerXVel = player.x_velocity;
            currentState.playerYVel = player.y_velocity;
            currentState.cameraTargetX = cameraTargetX;
            currentState.clockSpeed = clockSpeed;
            currentState.frameCount = frameCount;
            currentState.linesOfCode = linesOfCode;
            currentState.inAir = player.inAir;
            currentState.jumpCount = player.jumpCount;
            rewindBuffer.Record(currentState);
        }
        
        // Clock speed overclocking - increase speed over time
        if (!player.isGameOver && !isRewinding) {
            frameCount++;
            if (frameCount % CLOCK_SPEED_INCREASE_INTERVAL == 0) {
                if (clockSpeed < MAX_CLOCK_SPEED) {
                    clockSpeed += CLOCK_SPEED_INCREMENT;
                }
            }
            
            // Auto-scroll: camera target moves right at clock speed
            cameraTargetX += clockSpeed;
            
            // Update lines of code score based on distance traveled
            linesOfCode = (int)(cameraTargetX / 10);
        }
        
        // Check for timeout (player falls off left side of screen)
        float screenLeftEdge = cameraTargetX - SCREEN_WIDTH/3.0f;
        if (player.x < screenLeftEdge - 100) {
            player.isGameOver = true;
        }
        
        // Spawn GC tokens randomly
        if (!player.isGameOver && !isRewinding && GetRandomValue(1, 600) == 1) {
            float tokenX = cameraTargetX + SCREEN_WIDTH/2 + GetRandomValue(0, 300);
            float tokenY = GetRandomValue(200, SCREEN_HEIGHT - 200);
            gcTokens.push_back(GCToken(tokenX, tokenY));
        }
        
        // Update and draw GC tokens
        for (auto& token : gcTokens) {
            // Move tokens with the world (same speed as tiles)
            if (!isRewinding) {
                token.x -= TILE_SPEED;
            }
            
            // Check collection
            if (token.CheckCollection(player)) {
                currentRAM = MAX_RAM; // Full replenish
            }
            
            // Draw if visible
            if (token.x > cameraTargetX - SCREEN_WIDTH/2 && 
                token.x < cameraTargetX + SCREEN_WIDTH &&
                !token.collected) {
                token.Draw();
            }
        }
        
        // Remove collected/offscreen tokens
        gcTokens.erase(
            std::remove_if(gcTokens.begin(), gcTokens.end(),
                [](const GCToken& t) { return t.collected || t.x < cameraTargetX - SCREEN_WIDTH; }),
            gcTokens.end()
        );
        
        player.Update();
        
        camera.offset = (Vector2){ SCREEN_WIDTH/3.0f, 2*SCREEN_HEIGHT/3.0f };
        // Camera follows auto-scroll target, not player directly
        camera.target = (Vector2) {cameraTargetX, player.y};
        camera.rotation = 0.0f;
        camera.zoom = 1.0f;

        if (!player.isGameOver) updateTiles();
        
        ClearBackground(BLACK);

        player.Draw();
        drawTiles();
        drawUI();

        EndMode2D();
        EndDrawing();
    }

    // clean up remaining tiles
    Tile::Cleanup(tiles);

    CloseWindow();
}

void drawUI()
{
    // Draw Lines of Code (Score) - Top Left
    std::string scoreText = "Line: " + std::to_string(linesOfCode);
    DrawText(scoreText.c_str(), 20, 20, 30, WHITE);
    
    // Draw Clock Speed - Top Center
    std::string clockText = "Clock: " + std::to_string((int)(clockSpeed * 100)) + " MHz";
    DrawText(clockText.c_str(), SCREEN_WIDTH/2 - 100, 20, 30, SKYBLUE);
    
    // Draw RAM Bar - Top Right
    int barWidth = 200;
    int barHeight = 25;
    int barX = SCREEN_WIDTH - barWidth - 30;
    int barY = 20;
    
    // RAM label
    DrawText("RAM:", barX - 60, barY, 25, WHITE);
    
    // Background bar
    DrawRectangle(barX, barY, barWidth, barHeight, DARKGRAY);
    
    // RAM fill color (green to red based on level)
    Color ramColor;
    if (currentRAM > 60) ramColor = GREEN;
    else if (currentRAM > 30) ramColor = YELLOW;
    else ramColor = RED;
    
    // Fill bar
    int fillWidth = (barWidth * currentRAM) / MAX_RAM;
    DrawRectangle(barX, barY, fillWidth, barHeight, ramColor);
    
    // Border
    DrawRectangleLines(barX, barY, barWidth, barHeight, WHITE);
    
    // Rewind indicator
    if (isRewinding) {
        DrawText("<< REWINDING <<", SCREEN_WIDTH/2 - 150, 60, 40, YELLOW);
    }
    
    // Draw GC token instruction
    DrawText("[Z] Rewind", 20, 60, 20, GRAY);
    DrawText("Collect GC tokens to refill RAM", 20, 85, 20, GREEN);
    
    // Game Over Screen
    if (player.isGameOver) {
        // Dark overlay
        DrawRectangle(cameraTargetX - SCREEN_WIDTH/2, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 
                      (Color){0, 0, 0, 200});
        
        // Terminal-style error box
        int boxX = cameraTargetX - 300;
        int boxY = SCREEN_HEIGHT/2 - 200;
        DrawRectangle(boxX, boxY, 600, 400, DARKGRAY);
        DrawRectangleLines(boxX, boxY, 600, 400, RED);
        DrawRectangle(boxX, boxY, 600, 40, RED);
        
        // Title bar
        DrawText("X", boxX + 570, boxY + 5, 30, WHITE);
        DrawText("FATAL ERROR", boxX + 20, boxY + 5, 30, WHITE);
        
        // Error message
        DrawText("System.Runtime.RuntimeException", boxX + 30, boxY + 60, 25, RED);
        DrawText("Process terminated with exit code 1.", boxX + 30, boxY + 100, 20, WHITE);
        
        std::string finalScore = "Lines Executed: " + std::to_string(linesOfCode);
        DrawText(finalScore.c_str(), boxX + 30, boxY + 150, 25, YELLOW);
        
        DrawText("Press [R] to restart or [ESC] to exit", boxX + 30, boxY + 350, 25, SKYBLUE);
        
        // Handle restart
        if (IsKeyPressed(KEY_R)) {
            // Reset game
            player.isGameOver = false;
            player.x = PLAYER_START_X;
            player.y = PLAYER_START_Y;
            player.x_velocity = 0;
            player.y_velocity = 0;
            player.inAir = true;
            player.jumpCount = 0;
            clockSpeed = INITIAL_CLOCK_SPEED;
            cameraTargetX = 0;
            frameCount = 0;
            linesOfCode = 0;
            currentRAM = MAX_RAM;
            rewindBuffer.Clear();
            Tile::Cleanup(tiles);
            tiles.push_back(new Tile(TILES_START_X));
            gcTokens.clear();
        }
    }
}

void drawHitbox()
{
    DrawText("The red is the hitbox!", SCREEN_WIDTH / 3, 30, 40, YELLOW);

    player.Hitbox(RED);
    for (Tile *t : tiles)
        t->Hitbox(RED);
}

void drawTiles()
{
    for (Tile *t : tiles)
        t->Draw();

    Tile::New_tiles(tiles); // this is a seperate line from the for loop for new tiles
}

void updateTiles()
{
    Tile::Delete_And_Update(tiles);

    // checks for collison
    Tile::Collision(player, tiles);
}