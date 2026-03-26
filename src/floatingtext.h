#pragma once
#include "raylib.h"
#include <vector>
#include <string>

struct FloatingText {
    std::string message;
    Vector2 position;
    float alpha;
    float speedY;
    Color color;
};

class FloatingTextSystem {
public:
    // Adds a new popup to the list
    static void Push(std::string text, Vector2 pos, Color color) {
        popups.push_back({ text, pos, 1.0f, -1.5f, color });
    }

    // Updates transparency/position and draws them
    static void DrawAll() {
        for (int i = popups.size() - 1; i >= 0; i--) {
            // Update
            popups[i].position.y += popups[i].speedY;
            popups[i].alpha -= 0.02f; // Fade speed

            // Remove if invisible
            if (popups[i].alpha <= 0) {
                popups.erase(popups.begin() + i);
                continue;
            }

            // Draw
            Color c = popups[i].color;
            c.a = (unsigned char)(popups[i].alpha * 255);
            DrawText(popups[i].message.c_str(), (int)popups[i].position.x, (int)popups[i].position.y, 25, c);
        }
    }

    // Clear all (for resetGame)
    static void Clear() {
        popups.clear();
    }

private:
    static std::vector<FloatingText> popups;
};

// Note: You must initialize the static vector in a .cpp file 
// or use 'inline static' if using C++17 or newer.