#pragma once
#include "raylib.h"
#include <vector>
#include <string>

struct FloatingText {
    std::string message;
    Vector2 position;  // Screen coordinates (x, y)
    float alpha;       // Normalized transparency: 1.0 (solid) to 0.0 (invisible)
    float speedY;      // Vertical drift velocity (negative moves text UP)
    Color color;
};

class FloatingTextSystem {
public:
//@param: The keyword telling the editor "I am about to describe a parameter."
    /**
     * Spawns a new floating text instance.
     * @param text  The string to display (e.g., "+5 Lines")
     * @param pos   The starting position on screen
     * @param color The base color of the text
     */
    static void Push(std::string text, Vector2 pos, Color color) {
        // Initial state: Fully visible (1.0f) and drifting upward (-1.5f)
        popups.push_back({ text, pos, 1.0f, -1.5f, color });
    }

    /**
     * Updates text physics and transparency, then renders to screen.
     * Should be called once per frame in the main draw loop.
     */
    static void DrawAll() {
        // Iterate backwards to safely erase elements while looping
        for (int i = popups.size() - 1; i >= 0; i--) {
            
            // 1. Update Physics: Move text up and reduce visibility
            popups[i].position.y += popups[i].speedY;
            popups[i].alpha -= 0.02f; 

            // 2. Lifecycle Check: Remove if the text has fully faded out
            if (popups[i].alpha <= 0) {
                popups.erase(popups.begin() + i);
                continue;
            }

            // 3. Render: Map normalized alpha (0.0-1.0) to Raylib's byte alpha (0-255)
            Color c = popups[i].color;
            c.a = (unsigned char)(popups[i].alpha * 255); 
            
            DrawVSText(
                popups[i].message.c_str(), 
                (int)popups[i].position.x, 
                (int)popups[i].position.y, 
                25, 
                c
            );
        }
    }

    // Resets the system (useful for Game Over or Level Restart)
    static void Clear() {
        popups.clear();
    }

private:
    // Storage for all active floating text objects
    static std::vector<FloatingText> popups;
};
