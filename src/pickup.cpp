#include "pickup.h"
#include "tiles.h"
#include "constants.h"
#include <algorithm>
#include <vector>

// How many pixels above the tile surface the pickup floats
static const float PICKUP_HOVER = 40.0f;
// Chance (1-100) per tile that a pickup spawns above it
static const int PICKUP_CHANCE = 50;

void Pickup::Update(float scrollSpeed, float baseSpeed)
{
    x -= baseSpeed + scrollSpeed;
}

void Pickup::Draw() const
{
    if (collected)
        return;

    // Glowing cyan square
    DrawRectangle((int)x, (int)y, (int)size, (int)size, {0, 220, 220, 255});
    // Bright inner highlight
    DrawRectangle((int)x + 3, (int)y + 3, (int)size - 6, (int)size - 6, {180, 255, 255, 255});
    // Small label
    DrawText("+1s", (int)x - 2, (int)y - 16, 14, {0, 220, 220, 255});
}

bool Pickup::CheckCollect(Player &player)
{
    if (collected)
        return false;

    Rectangle pRect = {player.posX, player.posY, player.playerWidth, player.playerHeight};
    Rectangle myRect = {x, y, size, size};

    if (CheckCollisionRecs(pRect, myRect))
    {
        collected = true;

        if (player.Rewind_time_left < REWIND_SECS)
            player.Rewind_time_left += 1.0f;
        return true;
    }
    return false;
}

// ── Namespace helpers ────────────────────────────────────────────────────────

// Track which tiles have already been considered for a pickup, by pointer
static std::vector<Tile *> spawnedForTile;

void Pickups::SpawnIfNeeded(std::vector<Pickup> &pickups, const std::vector<Tile *> &tiles)
{
    // if (Tile::currentTileIndex < 30)   return;
      

    for (Tile *t : tiles)
    {
        if (t->tileType != Tile::TileType::NORMAL)
            continue;

        bool alreadySpawned = false;
        for (Tile *seen : spawnedForTile)
            if (seen == t)
            {
                alreadySpawned = true;
                break;
            }

        if (alreadySpawned)
            continue;
        spawnedForTile.push_back(t);

        if (GetRandomValue(1, 100) > PICKUP_CHANCE)
            continue;

        float px = t->tileX + t->tileWidth / 2.0f - 8;
        float py = t->tileY - PICKUP_HOVER;
        pickups.emplace_back(px, py);
    }

    // Remove stale pointers for tiles that have been deleted
    spawnedForTile.erase(
        std::remove_if(spawnedForTile.begin(), spawnedForTile.end(),
                       [&tiles](Tile *t)
                       {
                           for (Tile *active : tiles)
                               if (active == t)
                                   return false;
                           return true;
                       }),
        spawnedForTile.end());
}

void Pickups::UpdateAll(std::vector<Pickup> &pickups, Player &player,
                        float scrollSpeed, float baseSpeed)
{
    for (Pickup &p : pickups)
    {
        p.Update(scrollSpeed, baseSpeed);
        p.CheckCollect(player);
    }

    // Remove pickups that are off-screen or collected
    pickups.erase(
        std::remove_if(pickups.begin(), pickups.end(),
                       [](const Pickup &p)
                       { return p.collected || p.x + p.size < 0; }),
        pickups.end());
}

void Pickups::DrawAll(const std::vector<Pickup> &pickups)
{
    for (const Pickup &p : pickups)
        p.Draw();
}