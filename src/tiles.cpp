#include "tiles.h"
#include "constants.h"
#include "graph.h"
#include <string>
#include <iostream>
using namespace std;

extern TeleportGraph portalNetwork;
bool inDarkWorld = false;

int Tile::currentTileIndex = 0;

float Tile::baseSpeed = TILE_SPEED;
BhopBuffer Tile::bhopBuffer;

Tile::Tile(int startX)
{
    if (currentTileIndex >= VARIANT_TILE_INDEX)
    {
        if (Chance(30))
            tileType = TileType::SYNTAX;
        else if (Chance(20))
            tileType = TileType::LOGICAL;
    }
    tileWidth = GetRandomValue(200, 500);
    tileX = startX;
    tileY = TILE_Y;
}

void Tile::Draw(TileType type)
{
    Color tileColor = WHITE;
    if (type == TileType::SYNTAX)
        tileColor = RED;
    else if (type == TileType::LOGICAL)
        tileColor = BLUE;
    DrawRectangle(tileX, tileY, tileWidth, tileHeight, tileColor);
}

bool Tile::Update(float gameSpeed)
{
    tileX -= baseSpeed + gameSpeed;
    return (tileX + tileWidth <= 0); // True if offscreen
}

void Tile::Delete_And_Update(std::vector<Tile *> &tiles, float gameSpeed)
{
    for (int i = tiles.size() - 1; i >= 0; i--)
    {
        tiles[i]->Update(gameSpeed);
        // if (tiles[i]->Update(game_speed))
        // {
        //     delete tiles[i];                // delete tile if offscreen
        //     tiles.erase(tiles.begin() + i); // remove from vector
        // }
    }

    // If too many tiles accumulated, delete old ones to save memory
    if (tiles.size() > LEFT_TILES)
    {
        for (int i = 1; i >= 0; i--)
        {
            delete tiles[i];

            tiles.erase(tiles.begin() + i);
        }
    }
}

void Tile::New_tiles(std::vector<Tile *> &tiles)
{
    // Ensure there are always more than 5 tiles extending off screen to right
    while (tiles.size() < 5 || tiles.back()->tileX + tiles.back()->tileWidth < SCREEN_WIDTH + 400)
    {
        int lastRight = tiles.empty() ? SCREEN_WIDTH : tiles.back()->tileX + tiles.back()->tileWidth;
        int gap = GetRandomValue(100, 150);

        if (!tiles.empty())
        {
            if (tiles.back()->tileType == TileType::SYNTAX)
                gap = 0;
            else if (tiles.back()->tileType == TileType::LOGICAL)
                gap = GetRandomValue(40, 60);
            
            // 5% chance to spawn a portal network connection
            else if (GetRandomValue(1, 100) <= 5 && tiles.back()->tileType == TileType::NORMAL) {
                 float heightA = TILE_Y - GetRandomValue(150, 300);
                 float heightB = TILE_Y - GetRandomValue(150, 400);
                 portalNetwork.SpawnPair(lastRight + gap, heightA, heightB);
            }
        }
        tiles.push_back(new Tile(lastRight + gap));
    }
}

void Tile::Collision(Player &player, const std::vector<Tile *> &tiles)
{
    // Update bhop buffer for red tile deflection
    if (IsKeyDown(KEY_R))
    {
        bhopBuffer.framesLeft = bhopBuffer.maxFrames;
    }

    else if (bhopBuffer.framesLeft > 0)
    {
        bhopBuffer.framesLeft--;
    }
    bool landedOnNormal = false;
    bool landedOnLogical = false;
    int playerRightEdge = player.posX + player.playerWidth;

    // Check Graph Teleportation first
    for (auto it = portalNetwork.GetNodes().begin(); it != portalNetwork.GetNodes().end(); ++it) {
        Rectangle nodeRect = {it->posX, it->posY, it->width, it->height};
        if (CheckCollisionRecs(player.GetCollisionRect(), nodeRect) || 
            CheckCollisionRecs(player.GetNonCollisionRect(), nodeRect)) {
            
            // RNG 50/50 Roll
            if (GetRandomValue(1, 100) <= 50) {
                // Success!
                currentTileIndex += 4; // Add 4 score
                
                // Change world color
                inDarkWorld = !inDarkWorld;
                
                // Pop player up slightly so they don't hit the ground immediately
                player.velY = -JUMP_HEIGHT; 

                player.dashFramesLeft = 0; 
                
                // Despawn this specific portal so it can't be hit twice
                // (Using a quick hack to push it offscreen for CleanUp to catch it)
                const_cast<TerminalNode&>(*it).posX = -1000.0f; 
                
            } else {
                // Failure - Fall into the void
                player.isGameOver = true;
            }
            break; // Only process one portal collision per frame
        }
    }


    for (size_t i = 0; i < tiles.size(); i++)
    {
        Rectangle tileTop = {tiles[i]->tileX, tiles[i]->tileY, tiles[i]->tileWidth, tileHeight / 3};

        Rectangle tileRect = {tiles[i]->tileX, tiles[i]->tileY, tiles[i]->tileWidth, tileHeight};
        Rectangle playerBottom = {player.posX, player.posY + (2 * player.playerHeight) / 3, player.playerWidth, player.playerHeight / 3};
        Rectangle playerTop = {player.posX, player.posY, player.playerWidth, (2 * player.playerHeight) / 3};

        if (tiles[i]->tileType == TileType::NORMAL)
        {
            if (CheckCollisionRecs(playerBottom, tileTop))
            {
                player.inAir = false;
                player.jumpsAvailable = 0;
                
                // Only increment score if moving forward to new tiles
                static Tile* lastLandedTile = nullptr;
                if (lastLandedTile != tiles[i]) {
                    currentTileIndex++;
                    lastLandedTile = tiles[i];
                }

                player.posY = tiles[i]->tileY - player.playerHeight + 1;
                player.velY = 0;
                landedOnNormal = true;
                break;
            }

            else if (CheckCollisionRecs(playerTop, tileRect))
            {
                if (playerRightEdge >= tileRect.x)
                {
                    int overlapX = playerRightEdge - tileRect.x;
                    player.posX = player.posX - overlapX + 1;
                    landedOnNormal = true;
                    break;
                }
            }
        }
        else if (tiles[i]->tileType == TileType::SYNTAX)
        {
            if (CheckCollisionRecs(playerBottom, tileTop))
            {
                if (bhopBuffer.framesLeft > 0)
                {
                    // Instead of subtracting, explicitly set height so spamming doesn't stack
                    player.velY = -JUMP_HEIGHT * 1.5f;
                    bhopBuffer.framesLeft = 0;
                    player.inAir = true;
                }
                else
                {
                    player.isGameOver = true;
                }
                break;
            }
        }
        else if (tiles[i]->tileType == TileType::LOGICAL)
        {
            if (CheckCollisionRecs(playerBottom, tileTop))
            {
                player.inAir = false;
                player.jumpsAvailable = 0;

                static Tile* lastLandedTileLog = nullptr;
                if (lastLandedTileLog != tiles[i]) {
                    currentTileIndex++;
                    lastLandedTileLog = tiles[i];
                }

                player.posY = tiles[i]->tileY - player.playerHeight + 1;
                player.velY = 0;
                landedOnLogical = true;
                break;
            }

            else if (CheckCollisionRecs(playerTop, tileRect))
            {
                if (playerRightEdge >= tileRect.x)
                {
                    int overlapX = playerRightEdge - tileRect.x;
                    player.posX = player.posX - overlapX + 1;
                    landedOnLogical = true;
                    break;
                }
            }
        }
    }
    if (!landedOnLogical && !landedOnNormal)
    {
        player.inAir = true;
        if (baseSpeed != TILE_SPEED)
            baseSpeed = TILE_SPEED;
    }
    if (landedOnLogical)
        SmoothCountdown(baseSpeed, TILE_SPEED);
    if (baseSpeed == 0)
        player.isGameOver = true;
    std::string tileText = std::to_string(currentTileIndex);
    DrawText(tileText.c_str(), 20, SCREEN_HEIGHT - 60, 45, WHITE);
}

void Tile::Cleanup(std::vector<Tile *> &tiles)
{
    for (Tile *t : tiles)
        delete t;
    tiles.clear();
}

void Tile::Hitbox(Color c, TileType type)
{    
    DrawRectangle(tileX, tileY, tileWidth, tileHeight / 3, c);
    DrawRectangle(tileX, tileY + tileHeight / 3, tileWidth, (2 * tileHeight) / 3, WHITE);
}

bool Tile::Chance(int percent)
{
    return GetRandomValue(1, 100) <= percent;
}

void Tile::WarningText(int tileIndex, Player player)
{
    if (tileIndex >= VARIANT_TILE_INDEX)
    {
        static int start_time = -1;
        if (start_time == -1)
            start_time = GetTime();
        double elapsed = GetTime() - start_time;

        if (elapsed <= 2.0)
            DrawText("Warning! Syntax errors detected", SCREEN_WIDTH / 4, 50, 100, RED);

        else if (elapsed <= 4.0)
            DrawText("Press \"R\" to deflect!", SCREEN_WIDTH / 6, 50, 100, RED);
    }
}

void Tile::SmoothCountdown(float &value, float startValue)
{
    float rate = startValue / Speed_DELAY_DURATION;
    value -= rate * GetFrameTime();
    if (value < 0)
        value = 0;
}