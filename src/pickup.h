#pragma once
#include "raylib.h"
#include "player.h"
#include <vector>

class Tile;

struct Pickup
{
    float x, y;
    float size = 16;
    bool collected = false;
    static float collectingSoundvolumn;
    static Sound collectingSound;

    Pickup(float x, float y) : x(x), y(y) {}

    void Update(float scrollSpeed, float baseSpeed);
    void Draw() const;
    bool CheckCollect(Player &player); // returns true if just collected
    static void Init();
    static void Cleanup();
};

namespace Pickups
{
    void SpawnIfNeeded(std::vector<Pickup> &pickups, const std::vector<Tile *> &tiles);
    void UpdateAll(std::vector<Pickup> &pickups, Player &player, float scrollSpeed, float baseSpeed);
    void DrawAll(const std::vector<Pickup> &pickups);
}