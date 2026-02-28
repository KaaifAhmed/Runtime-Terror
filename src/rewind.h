#ifndef REWIND_H
#define REWIND_H

#include "raylib.h"
#include "player.h"
#include <vector>

// State snapshot for rewind
struct GameState {
    float playerX, playerY;
    float playerXVel, playerYVel;
    float cameraTargetX;
    float clockSpeed;
    int frameCount;
    int linesOfCode;
    bool inAir;
    int jumpCount;
};

// Circular Buffer (Ring Buffer) for rewind system
class RewindBuffer {
public:
    static const int CAPACITY = 300; // 5 seconds at 60 FPS
    
    RewindBuffer();
    
    // O(1) insertion
    void Record(const GameState& state);
    
    // O(1) retrieval
    bool CanRewind();
    GameState Rewind();
    
    // Get current buffer fill level (0-100%)
    int GetFillPercentage();
    
    void Clear();
    
private:
    GameState buffer[CAPACITY];
    int head;  // Where we write next
    int count; // How many states stored
    bool full;
};

// Garbage Collector token
struct GCToken {
    float x, y;
    bool collected;
    
    GCToken(float xPos, float yPos);
    void Draw();
    bool CheckCollection(Player& player);
};

#endif
