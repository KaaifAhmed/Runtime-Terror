#include "pickup.h"
#include "tiles.h"
#include "constants.h"
#include <algorithm>
#include <vector>

// How many pixels above the tile surface the pickup floats
static const float PICKUP_HOVER = 40.0f;
// Chance (1-100) per tile that a pickup spawns above it
static const int PICKUP_CHANCE = 50;

// Define the static member — required for static variables
Sound Pickup::collectingSound = {};
float Pickup::collectingSoundvolumn=0.2f;

void Pickup::Update(float scrollSpeed, float baseSpeed)
{
    x -= baseSpeed + scrollSpeed;
}

// Call this after InitAudioDevice()
void Pickup::Init()
{
    collectingSound = LoadSound("sounds\\powerUp.wav");
    SetSoundVolume(collectingSound,collectingSoundvolumn);
}

// Call this before CloseAudioDevice()
void Pickup::Cleanup()
{
    if (collectingSound.frameCount > 0)
        UnloadSound(collectingSound);
}

#include "ui_system.h"

void Pickup::Draw() const
{
    if (collected)
        return;

    // VS Code styled widget (Accent Orange for Rewind time)
    DrawRectangleRounded({x, y, size, size}, 0.2f, 4, VSCodeTheme::ACCENT_ORANGE);
    DrawRectangleLinesEx({x, y, size, size}, 1, VSCodeTheme::BORDER);
    
    // Label above
    int tw = MeasureVSText("+1s", 14);
    DrawVSText("+1s", (int)(x + (size - tw) / 2), (int)(y - 18), 14, VSCodeTheme::ACCENT_ORANGE);
    
    // Inner icon (rewind symbol '<<')
    int tw2 = MeasureVSText("<<", 16);
    DrawVSText("<<", (int)(x + (size - tw2) / 2), (int)(y + (size - 16) / 2), 16, VSCodeTheme::BG_EDITOR);
}

bool Pickup::CheckCollect(Player &player)
{
    if (collected)
        return false;

    Rectangle pRect = {player.posX, player.posY, player.playerWidth, PLAYER_HEIGHT};
    Rectangle myRect = {x, y, size, size};

    if (CheckCollisionRecs(pRect, myRect))
    {
        collected = true;
        PlaySound(collectingSound);
        if (player.Rewind_time_left < REWIND_SECS)
        {
                        if(player.Rewind_time_left+1.0f>5.0)
            {
                player.Rewind_time_left=5.0f;
            }

            else player.Rewind_time_left+=1.0f;
        }

        return true;
    }
    return false;
}


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
