#include "rewind.h"
#include "constants.h"
#include <cmath>

// GCToken implementation
GCToken::GCToken(float xPos, float yPos) : x(xPos), y(yPos), collected(false) {}

void GCToken::Draw() {
    if (collected) return;
    
    // Green recycle icon
    DrawCircle(x, y, 20, GREEN);
    DrawText("GC", x - 15, y - 10, 20, DARKGREEN);
    
    // Rotating effect
    static float rotation = 0;
    rotation += 2;
    float r1 = 25 + sin(rotation * 3.14159f / 180) * 3;
    float r2 = 25 + cos(rotation * 3.14159f / 180) * 3;
    DrawCircleLines(x, y, r1, LIME);
    DrawCircleLines(x, y, r2, LIME);
}

bool GCToken::CheckCollection(Player& player) {
    if (collected) return false;
    
    float dist = sqrt((x - (player.x + player.width/2)) * (x - (player.x + player.width/2)) +
                      (y - (player.y + player.height/2)) * (y - (player.y + player.height/2)));
    if (dist < 40) {
        collected = true;
        return true;
    }
    return false;
}

// RewindBuffer implementation
RewindBuffer::RewindBuffer() : head(0), count(0), full(false) {}

void RewindBuffer::Record(const GameState& state) {
    buffer[head] = state;
    head = (head + 1) % CAPACITY;
    
    if (count < CAPACITY) {
        count++;
    } else {
        full = true;
    }
}

bool RewindBuffer::CanRewind() {
    return count > 10; // Need at least some history
}

GameState RewindBuffer::Rewind() {
    // Rewind 30 frames (0.5 seconds)
    int rewindFrames = 30;
    int readIndex;
    
    if (full) {
        // Buffer is full, calculate from head
        readIndex = (head - rewindFrames + CAPACITY) % CAPACITY;
    } else {
        // Buffer not full yet
        readIndex = (head - rewindFrames);
        if (readIndex < 0) readIndex = 0;
    }
    
    // Remove future states
    if (full) {
        head = (readIndex + 1) % CAPACITY;
    } else {
        head = readIndex + 1;
        if (head > count) head = count;
    }
    
    // Clear the buffer to simulate "using" the RAM
    Clear();
    
    return buffer[readIndex];
}

int RewindBuffer::GetFillPercentage() {
    return (count * 100) / CAPACITY;
}

void RewindBuffer::Clear() {
    head = 0;
    count = 0;
    full = false;
}
