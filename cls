[1mdiff --git a/src/constants.h b/src/constants.h[m
[1mindex c3bd6ec..0a9211e 100644[m
[1m--- a/src/constants.h[m
[1m+++ b/src/constants.h[m
[36m@@ -1,4 +1,5 @@[m
 #pragma once[m
[32m+[m[32m#include <raylib.h>[m
 [m
 // GAME STATES[m
 enum GameState { MENU, PLAYING, GAME_OVER };[m
[36m@@ -12,6 +13,9 @@[m [mconst Color UI_TEXT_SECONDARY = {180, 180, 180, 255}; // Light grey[m
 const Color PLAYER_COLOR = {100, 200, 255, 255};      // Light blue[m
 const Color TILE_BG = {40, 50, 60, 255};              // Dark blue[m
 [m
[32m+[m[32m// VS Code Dark+ Theme Colors[m
[32m+[m[32mconst Color VS_BG = { 30, 30, 30, 255 };          // Editor Background[m
[32m+[m
 // WORLD CONSTANTS[m
 const int SCREEN_WIDTH = 1680;[m
 const int SCREEN_HEIGHT = 900;[m
[1mdiff --git a/src/main.cpp b/src/main.cpp[m
[1mindex 055bc87..5917ad4 100644[m
[1m--- a/src/main.cpp[m
[1m+++ b/src/main.cpp[m
[36m@@ -1,12 +1,12 @@[m
[31m-#include <raylib.h>[m
[31m-#include <string>[m
[31m-#include <cmath>[m
 #include "constants.h"[m
[32m+[m[32m#include "pickup.h"[m
 #include "player.h"[m
[31m-#include <vector>[m
[31m-#include "tiles.h"[m
 #include "rewind.h"[m
[31m-#include "pickup.h"[m
[32m+[m[32m#include "tiles.h"[m
[32m+[m[32m#include <cmath>[m
[32m+[m[32m#include <raylib.h>[m
[32m+[m[32m#include <string>[m
[32m+[m[32m#include <vector>[m
 [m
 using namespace std;[m
 [m
[36m@@ -27,362 +27,372 @@[m [mvoid drawGameOverScreen();[m
 void drawGameScore();[m
 void resetGame();[m
 [m
[31m-int main()[m
[31m-{[m
[31m-    RewindBuffer rewindSys(REWIND_SECS * FPS);[m
[31m-    tiles.push_back(new Tile(TILES_START_X));[m
[31m-    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Code Compiler");[m
[31m-[m
[31m-    InitAudioDevice(); //  1. Init audio first[m
[31m-    player.Init();     //  2. Then load sounds[m
[31m-    Pickup::Init();[m
[31m-    Tile::Init();[m
[31m-    gameMusic = LoadMusicStream("src\\game_music.mp3");[m
[31m-    PlayMusicStream(gameMusic);[m
[31m-[m
[31m-    SetTargetFPS(FPS);[m
[31m-    player.Rewind_time_left = 5;[m
[31m-[m
[31m-    while (!WindowShouldClose())[m
[31m-    {[m
[31m-        BeginDrawing();[m
[31m-        // IMPORTANT: Call every frame to keep music streaming[m
[31m-        UpdateMusicStream(gameMusic);[m
[31m-[m
[31m-        ClearBackground(UI_PRIMARY);[m
[31m-[m
[31m-        switch (gameState)[m
[31m-        {[m
[31m-        case MENU:[m
[31m-            drawMenuScreen();[m
[31m-            if (IsKeyPressed(KEY_SPACE))[m
[31m-            {[m
[31m-                gameState = PLAYING;[m
[31m-                PlayMusicStream(gameMusic);[m
[31m-            }[m
[31m-            break;[m
[31m-[m
[31m-        case PLAYING:[m
[31m-        {[m
[31m-            // Use 0.05f instead of 0 — float subtraction never lands exactly on 0,[m
[31m-            // so we treat anything below this epsilon as empty[m
[31m-            if ((IsKeyDown(KEY_Z) && IsKeyDown(KEY_LEFT_CONTROL)) && !player.isGameOver && player.Rewind_time_left > 0.05f)[m
[31m-            {[m
[31m-                // Only play when rewind just starts, not every frame[m
[31m-                if (!player.isRewinding)[m
[31m-                    PlaySound(player.rewindSound); //  fires once on first frame[m
[31m-[m
[31m-                PlayerState restoredState;[m
[31m-                if (rewindSys.Rewind(restoredState))[m
[31m-                {[m
[31m-                    Player::ReduceRewind(player.Rewind_time_left, REWIND_DURATION);[m
[31m-[m
[31m-                    player.posX = restoredState.posX;[m
[31m-                    player.posY = restoredState.posY;[m
[31m-[m
[31m-                    float reverseSpeed = (scrollSpeed + TILE_SPEED * 2) * -1;[m
[31m-                    updateTiles(reverseSpeed);[m
[31m-[m
[31m-                    // Move pickups at the same reverse speed so they stay on their tiles[m
[31m-                    for (Pickup &p : pickups)[m
[31m-                        p.Update(-TILE_SPEED, 0);[m
[31m-[m
[31m-                    player.isRewinding = true;[m
[31m-[m
[31m-                    player.Hitbox(UI_ACCENT);[m
[31m-                    drawTiles();[m
[31m-                    Pickups::DrawAll(pickups);[m
[31m-                    DrawText("CTRL + Z!", SCREEN_WIDTH / 2 - 300, 100, 100, UI_HIGHLIGHT);[m
[31m-                    drawRewindBar(player.Rewind_time_left);[m
[31m-                    drawGameScore();[m
[31m-                }[m
[31m-            }[m
[31m-            else[m
[31m-            {[m
[31m-                if (player.isRewinding)[m
[31m-                    StopSound(player.rewindSound);[m
[31m-[m
[31m-                PlayerState currentState = {player.posX, player.posY};[m
[31m-                rewindSys.Record(currentState);[m
[31m-[m
[31m-                player.isRewinding = false;[m
[31m-                player.Update();[m
[31m-[m
[31m-                // Tile::WarningText(Tile::currentTileIndex, player, tiles);[m
[31m-[m
[31m-                if (!player.isGameOver)[m
[31m-                {[m
[31m-                    updateTiles(scrollSpeed);[m
[31m-[m
[31m-                    // spawn + update + draw pickups[m
[31m-                    Pickups::SpawnIfNeeded(pickups, tiles);[m
[31m-                    Pickups::UpdateAll(pickups, player, scrollSpeed, Tile::baseSpeed);[m
[31m-[m
[31m-                    // Award lines for successfully passing tiles[m
[31m-                    static int lastTileIndex = 0;[m
[31m-                    if (Tile::currentTileIndex > lastTileIndex)[m
[31m-                    {[m
[31m-                        int tilesPassed = Tile::currentTileIndex - lastTileIndex;[m
[31m-                        for (int i = 0; i < tilesPassed; i++)[m
[31m-                        {[m
[31m-                            if (lastTileIndex + i < (int)tiles.size() && tiles[lastTileIndex + i]->tileType == Tile::TileType::NORMAL)[m
[31m-                            {[m
[31m-                                player.linesCompiled += 5; // 5 lines per normal tile[m
[31m-                            }[m
[31m-                            else if (lastTileIndex + i < (int)tiles.size() && tiles[lastTileIndex + i]->tileType == Tile::TileType::LOGICAL)[m
[31m-                            {[m
[31m-                                player.linesCompiled += 3; // 3 lines per logical tile (harder to navigate)[m
[31m-                            }[m
[31m-                        }[m
[31m-                        lastTileIndex = Tile::currentTileIndex;[m
[31m-                    }[m
[31m-                }[m
[31m-                else[m
[31m-                {[m
[31m-                    // Transition to game over screen[m
[31m-                    gameState = GAME_OVER;[m
[31m-                    PauseMusicStream(gameMusic);[m
[31m-                }[m
[31m-[m
[31m-                player.Hitbox(UI_ACCENT);[m
[31m-                drawTiles();[m
[31m-                Pickups::DrawAll(pickups);[m
[31m-                drawRewindBar(player.Rewind_time_left);[m
[31m-                drawGameScore();[m
[31m-            }[m
[31m-            break;[m
[32m+[m[32mint main() {[m
[32m+[m[32m  RewindBuffer rewindSys(REWIND_SECS * FPS);[m
[32m+[m[32m  tiles.push_back(new Tile(TILES_START_X));[m
[32m+[m[32m  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Code Compiler");[m
[32m+[m
[32m+[m[32m  InitAudioDevice(); //  1. Init audio first[m
[32m+[m[32m  player.Init();     //  2. Then load sounds[m
[32m+[m[32m  Pickup::Init();[m
[32m+[m[32m  Tile::Init();[m
[32m+[m[32m  gameMusic = LoadMusicStream("src\\game_music.mp3");[m
[32m+[m[32m  PlayMusicStream(gameMusic);[m
[32m+[m
[32m+[m[32m  SetTargetFPS(FPS);[m
[32m+[m[32m  player.Rewind_time_left = 5;[m
[32m+[m
[32m+[m[32m  while (!WindowShouldClose()) {[m
[32m+[m[32m    BeginDrawing();[m
[32m+[m[32m    // IMPORTANT: Call every frame to keep music streaming[m
[32m+[m[32m    UpdateMusicStream(gameMusic);[m
[32m+[m
[32m+[m[32m    // ClearBackground(UI_PRIMARY);[m
[32m+[m[32m    ClearBackground(VS_BG);[m
[32m+[m
[32m+[m[32m    switch (gameState) {[m
[32m+[m[32m    case MENU:[m
[32m+[m[32m      drawMenuScreen();[m
[32m+[m[32m      if (IsKeyPressed(KEY_SPACE)) {[m
[32m+[m[32m        gameState = PLAYING;[m
[32m+[m[32m        PlayMusicStream(gameMusic);[m
[32m+[m[32m      }[m
[32m+[m[32m      break;[m
[32m+[m
[32m+[m[32m    case PLAYING: {[m
[32m+[m[32m      // Use 0.05f instead of 0 — float subtraction never lands exactly on 0,[m
[32m+[m[32m      // so we treat anything below this epsilon as empty[m
[32m+[m[32m      if ((IsKeyDown(KEY_Z) && IsKeyDown(KEY_LEFT_CONTROL)) &&[m
[32m+[m[32m          !player.isGameOver && player.Rewind_time_left > 0.05f) {[m
[32m+[m[32m        // Only play when rewind just starts, not every frame[m
[32m+[m[32m        if (!player.isRewinding)[m
[32m+[m[32m          PlaySound(player.rewindSound); //  fires once on first frame[m
[32m+[m
[32m+[m[32m        PlayerState restoredState;[m
[32m+[m[32m        if (rewindSys.Rewind(restoredState)) {[m
[32m+[m[32m          Player::ReduceRewind(player.Rewind_time_left, REWIND_DURATION);[m
[32m+[m
[32m+[m[32m          player.posX = restoredState.posX;[m
[32m+[m[32m          player.posY = restoredState.posY;[m
[32m+[m
[32m+[m[32m          float reverseSpeed = (scrollSpeed + TILE_SPEED * 2) * -1;[m
[32m+[m[32m          updateTiles(reverseSpeed);[m
[32m+[m
[32m+[m[32m          // Move pickups at the same reverse speed so they stay on their tiles[m
[32m+[m[32m          for (Pickup &p : pickups)[m
[32m+[m[32m            p.Update(-TILE_SPEED, 0);[m
[32m+[m
[32m+[m[32m          player.isRewinding = true;[m
[32m+[m
[32m+[m[32m          // player.Hitbox(UI_ACCENT);[m
[32m+[m[32m          // In main.cpp switch case PLAYING:[m
[32m+[m[32m          player.Draw(); // This shows your blinking cursor[m
[32m+[m
[32m+[m[32m          // Only show the red/white hitboxes if you press a specific key (like[m
[32m+[m[32m          // H)[m
[32m+[m[32m          if (IsKeyDown(KEY_H)) {[m
[32m+[m[32m            player.Hitbox(RED);[m
[32m+[m[32m          }[m
[32m+[m[32m          drawTiles();[m
[32m+[m[32m          Pickups::DrawAll(pickups);[m
[32m+[m[32m          DrawText("CTRL + Z!", SCREEN_WIDTH / 2 - 300, 100, 100, UI_HIGHLIGHT);[m
[32m+[m[32m          drawRewindBar(player.Rewind_time_left);[m
[32m+[m[32m          drawGameScore();[m
         }[m
[31m-[m
[31m-        case GAME_OVER:[m
[31m-            drawGameOverScreen();[m
[31m-            if (IsKeyPressed(KEY_SPACE))[m
[31m-            {[m
[31m-                resetGame();[m
[31m-                gameState = MENU;[m
[32m+[m[32m      } else {[m
[32m+[m[32m        if (player.isRewinding)[m
[32m+[m[32m          StopSound(player.rewindSound);[m
[32m+[m
[32m+[m[32m        PlayerState currentState = {player.posX, player.posY};[m
[32m+[m[32m        rewindSys.Record(currentState);[m
[32m+[m
[32m+[m[32m        player.isRewinding = false;[m
[32m+[m[32m        player.Update();[m
[32m+[m
[32m+[m[32m        // Tile::WarningText(Tile::currentTileIndex, player, tiles);[m
[32m+[m
[32m+[m[32m        if (!player.isGameOver) {[m
[32m+[m[32m          updateTiles(scrollSpeed);[m
[32m+[m
[32m+[m[32m          // spawn + update + draw pickups[m
[32m+[m[32m          Pickups::SpawnIfNeeded(pickups, tiles);[m
[32m+[m[32m          Pickups::UpdateAll(pickups, player, scrollSpeed, Tile::baseSpeed);[m
[32m+[m
[32m+[m[32m          // Award lines for successfully passing tiles[m
[32m+[m[32m          static int lastTileIndex = 0;[m
[32m+[m[32m          if (Tile::currentTileIndex > lastTileIndex) {[m
[32m+[m[32m            int tilesPassed = Tile::currentTileIndex - lastTileIndex;[m
[32m+[m[32m            for (int i = 0; i < tilesPassed; i++) {[m
[32m+[m[32m              if (lastTileIndex + i < (int)tiles.size() &&[m
[32m+[m[32m                  tiles[lastTileIndex + i]->tileType ==[m
[32m+[m[32m                      Tile::TileType::NORMAL) {[m
[32m+[m[32m                player.linesCompiled += 5; // 5 lines per normal tile[m
[32m+[m[32m              } else if (lastTileIndex + i < (int)tiles.size() &&[m
[32m+[m[32m                         tiles[lastTileIndex + i]->tileType ==[m
[32m+[m[32m                             Tile::TileType::LOGICAL) {[m
[32m+[m[32m                player.linesCompiled +=[m
[32m+[m[32m                    3; // 3 lines per logical tile (harder to navigate)[m
[32m+[m[32m              }[m
             }[m
[31m-            break;[m
[32m+[m[32m            lastTileIndex = Tile::currentTileIndex;[m
[32m+[m[32m          }[m
[32m+[m[32m        } else {[m
[32m+[m[32m          // Transition to game over screen[m
[32m+[m[32m          gameState = GAME_OVER;[m
[32m+[m[32m          PauseMusicStream(gameMusic);[m
         }[m
 [m
[31m-        EndDrawing();[m
[32m+[m[32m        // player.Hitbox(UI_ACCENT);[m
[32m+[m[32m        // In main.cpp switch case PLAYING:[m
[32m+[m[32m        player.Draw(); // This shows your blinking cursor[m
[32m+[m
[32m+[m[32m        // Only show the red/white hitboxes if you press a specific key (like H)[m
[32m+[m[32m        if (IsKeyDown(KEY_H)) {[m
[32m+[m[32m          player.Hitbox(RED);[m
[32m+[m[32m        }[m
[32m+[m[32m        drawTiles();[m
[32m+[m[32m        Pickups::DrawAll(pickups);[m
[32m+[m[32m        drawRewindBar(player.Rewind_time_left);[m
[32m+[m[32m        drawGameScore();[m
[32m+[m[32m      }[m
[32m+[m[32m      break;[m
     }[m
 [m
[31m-    UnloadMusicStream(gameMusic);[m
[31m-    player.Cleanup(); //  Unload sounds before closing audio[m
[31m-    Pickup::Cleanup();[m
[31m-    Tile::CleanupTiles(tiles);[m
[31m-    CloseAudioDevice(); //  Close audio device[m
[31m-    CloseWindow();[m
[31m-}[m
[32m+[m[32m    case GAME_OVER:[m
[32m+[m[32m      drawGameOverScreen();[m
[32m+[m[32m      if (IsKeyPressed(KEY_SPACE)) {[m
[32m+[m[32m        resetGame();[m
[32m+[m[32m        gameState = MENU;[m
[32m+[m[32m      }[m
[32m+[m[32m      break;[m
[32m+[m[32m    }[m
 [m
[32m+[m[32m    EndDrawing();[m
[32m+[m[32m  }[m
 [m
[31m-void drawHitbox()[m
[31m-{[m
[31m-    DrawText("The red is the hitbox!", SCREEN_WIDTH / 3, 30, 40, YELLOW);[m
[31m-    player.Hitbox(RED);[m
[31m-    for (Tile *t : tiles)[m
[31m-        t->Hitbox(ORANGE, t->tileType);[m
[32m+[m[32m  UnloadMusicStream(gameMusic);[m
[32m+[m[32m  player.Cleanup(); //  Unload sounds before closing audio[m
[32m+[m[32m  Pickup::Cleanup();[m
[32m+[m[32m  Tile::CleanupTiles(tiles);[m
[32m+[m[32m  CloseAudioDevice(); //  Close audio device[m
[32m+[m[32m  CloseWindow();[m
 }[m
 [m
[31m-void drawTiles()[m
[31m-{[m
[31m-    for (Tile *t : tiles)[m
[31m-        t->Draw(t->tileType);[m
[31m-    Tile::New_tiles(tiles);[m
[32m+[m[32mvoid drawHitbox() {[m
[32m+[m[32m  DrawText("The red is the hitbox!", SCREEN_WIDTH / 3, 30, 40, YELLOW);[m
[32m+[m[32m  player.Hitbox(RED);[m
[32m+[m[32m  for (Tile *t : tiles)[m
[32m+[m[32m    t->Hitbox(ORANGE, t->tileType);[m
 }[m
 [m
[31m-void updateTiles(float scrollSpeed)[m
[31m-{[m
[31m-    Tile::Delete_And_Update(tiles, scrollSpeed);[m
[31m-    Tile::Collision(player, tiles);[m
[32m+[m[32mvoid drawTiles() {[m
[32m+[m[32m  for (Tile *t : tiles)[m
[32m+[m[32m    t->Draw(t->tileType);[m
[32m+[m[32m  Tile::New_tiles(tiles);[m
 }[m
 [m
[31m-void drawRewindBar(float rewindTimeLeft)[m
[31m-{[m
[31m-    const int BAR_X = 20;[m
[31m-    const int BAR_Y = SCREEN_HEIGHT - 50;[m
[31m-    const int BAR_WIDTH = 250;[m
[31m-    const int BAR_HEIGHT = 24;[m
[31m-    const int BORDER = 2;[m
[31m-[m
[31m-    // Background track[m
[31m-    DrawRectangle(BAR_X - BORDER, BAR_Y - BORDER,[m
[31m-                  BAR_WIDTH + BORDER * 2, BAR_HEIGHT + BORDER * 2,[m
[31m-                  {60, 60, 60, 255});[m
[31m-    DrawRectangle(BAR_X, BAR_Y, BAR_WIDTH, BAR_HEIGHT, {20, 20, 20, 255});[m
[31m-[m
[31m-    // Filled portion — cyan when healthy, orange when low[m
[31m-    float fraction = rewindTimeLeft / (float)REWIND_SECS;[m
[31m-    if (fraction < 0)[m
[31m-        fraction = 0;[m
[31m-    if (fraction > 1)[m
[31m-        fraction = 1;[m
[31m-    int fillWidth = (int)(BAR_WIDTH * fraction);[m
[31m-[m
[31m-    Color fillColor = (fraction > 0.3f) ? UI_ACCENT : UI_HIGHLIGHT;[m
[31m-    if (fillWidth > 0)[m
[31m-        DrawRectangle(BAR_X, BAR_Y, fillWidth, BAR_HEIGHT, fillColor);[m
[31m-[m
[31m-    // Label[m
[31m-    DrawText("REWIND", BAR_X, BAR_Y - 20, 16, UI_TEXT_SECONDARY);[m
[31m-[m
[31m-    // Seconds remaining[m
[31m-    char timeStr[16];[m
[31m-    sprintf(timeStr, "%.1f s", rewindTimeLeft);[m
[31m-    DrawText(timeStr, BAR_X + BAR_WIDTH + 8, BAR_Y + 4, 16, UI_TEXT_SECONDARY);[m
[32m+[m[32mvoid updateTiles(float scrollSpeed) {[m
[32m+[m[32m  Tile::Delete_And_Update(tiles, scrollSpeed);[m
[32m+[m[32m  Tile::Collision(player, tiles);[m
 }[m
 [m
[31m-void drawMenuScreen()[m
[31m-{[m
[31m-    // Title[m
[31m-    const char *title = "RUNTIME TERROR";[m
[31m-    int titleFontSize = 80;[m
[31m-    int titleWidth = MeasureText(title, titleFontSize);[m
[31m-    DrawText(title, (SCREEN_WIDTH - titleWidth) / 2, 100, titleFontSize, UI_ACCENT);[m
[31m-[m
[31m-    // Subtitle/Description[m
[31m-    const char *subtitle = "Navigate the code as the execution pointer";[m
[31m-    int subtitleFontSize = 30;[m
[31m-    int subtitleWidth = MeasureText(subtitle, subtitleFontSize);[m
[31m-    DrawText(subtitle, (SCREEN_WIDTH - subtitleWidth) / 2, 220, subtitleFontSize, UI_TEXT_SECONDARY);[m
[31m-[m
[31m-    // Instructions box[m
[31m-    int boxX = SCREEN_WIDTH / 2 - 400;[m
[31m-    int boxY = 350;[m
[31m-    int boxW = 800;[m
[31m-    int boxH = 300;[m
[31m-[m
[31m-    // Draw border with multiple rectangles[m
[31m-    DrawRectangle(boxX, boxY, boxW, boxH, {30, 30, 40, 200});[m
[31m-    DrawRectangle(boxX, boxY, boxW, 3, UI_ACCENT);[m
[31m-    DrawRectangle(boxX, boxY + boxH - 3, boxW, 3, UI_ACCENT);[m
[31m-    DrawRectangle(boxX, boxY, 3, boxH, UI_ACCENT);[m
[31m-    DrawRectangle(boxX + boxW - 3, boxY, 3, boxH, UI_ACCENT);[m
[31m-[m
[31m-    // Instructions text[m
[31m-    int instructionY = boxY + 30;[m
[31m-    int fontSize = 20;[m
[31m-    DrawText("EXECUTION POINTER CONTROLS:", boxX + 30, instructionY, 24, UI_ACCENT);[m
[31m-    instructionY += 40;[m
[31m-[m
[31m-    DrawText("SPACE - Jump / Double Jump", boxX + 40, instructionY, fontSize, UI_TEXT);[m
[31m-    instructionY += 30;[m
[31m-    DrawText("CTRL + Z - Rewind Time (5 seconds available)", boxX + 40, instructionY, fontSize, UI_TEXT);[m
[31m-    instructionY += 30;[m
[31m-    DrawText("Avoid RED syntax errors - they crash compilation!", boxX + 40, instructionY, fontSize, UI_TEXT);[m
[31m-    instructionY += 30;[m
[31m-    DrawText("BLUE logical errors slow compilation speed", boxX + 40, instructionY, fontSize, UI_TEXT);[m
[31m-    instructionY += 30;[m
[31m-    DrawText("GREEN code blocks compile successfully", boxX + 40, instructionY, fontSize, UI_TEXT);[m
[31m-[m
[31m-    // Start prompt[m
[31m-    const char *startText = "Press SPACE to Start";[m
[31m-    int startWidth = MeasureText(startText, 28);[m
[31m-    int blinkAlpha = (int)(128 + 127 * sin(GetTime() * 3)) + 128;[m
[31m-    DrawText(startText, (SCREEN_WIDTH - startWidth) / 2, SCREEN_HEIGHT - 100, 28, {UI_ACCENT.r, UI_ACCENT.g, UI_ACCENT.b, (unsigned char)blinkAlpha});[m
[32m+[m[32mvoid drawRewindBar(float rewindTimeLeft) {[m
[32m+[m[32m  const int BAR_X = 20;[m
[32m+[m[32m  const int BAR_Y = SCREEN_HEIGHT - 50;[m
[32m+[m[32m  const int BAR_WIDTH = 250;[m
[32m+[m[32m  const int BAR_HEIGHT = 24;[m
[32m+[m[32m  const int BORDER = 2;[m
[32m+[m
[32m+[m[32m  // Background track[m
[32m+[m[32m  DrawRectangle(BAR_X - BORDER, BAR_Y - BORDER, BAR_WIDTH + BORDER * 2,[m
[32m+[m[32m                BAR_HEIGHT + BORDER * 2, {60, 60, 60, 255});[m
[32m+[m[32m  DrawRectangle(BAR_X, BAR_Y, BAR_WIDTH, BAR_HEIGHT, {20, 20, 20, 255});[m
[32m+[m
[32m+[m[32m  // Filled portion — cyan when healthy, orange when low[m
[32m+[m[32m  float fraction = rewindTimeLeft / (float)REWIND_SECS;[m
[32m+[m[32m  if (fraction < 0)[m
[32m+[m[32m    fraction = 0;[m
[32m+[m[32m  if (fraction > 1)[m
[32m+[m[32m    fraction = 1;[m
[32m+[m[32m  int fillWidth = (int)(BAR_WIDTH * fraction);[m
[32m+[m
[32m+[m[32m  Color fillColor = (fraction > 0.3f) ? UI_ACCENT : UI_HIGHLIGHT;[m
[32m+[m[32m  if (fillWidth > 0)[m
[32m+[m[32m    DrawRectangle(BAR_X, BAR_Y, fillWidth, BAR_HEIGHT, fillColor);[m
[32m+[m
[32m+[m[32m  // Label[m
[32m+[m[32m  DrawText("REWIND", BAR_X, BAR_Y - 20, 16, UI_TEXT_SECONDARY);[m
[32m+[m
[32m+[m[32m  // Seconds remaining[m
[32m+[m[32m  char timeStr[16];[m
[32m+[m[32m  sprintf(timeStr, "%.1f s", rewindTimeLeft);[m
[32m+[m[32m  DrawText(timeStr, BAR_X + BAR_WIDTH + 8, BAR_Y + 4, 16, UI_TEXT_SECONDARY);[m
 }[m
 [m
[31m-void drawGameScore()[m
[31m-{[m
[31m-    // Score box in top-right[m
[31m-    int score = player.linesCompiled;[m
[31m-    char scoreStr[32];[m
[31m-    sprintf(scoreStr, "Lines Compiled: %d", score);[m
[31m-[m
[31m-    int boxX = SCREEN_WIDTH - 320;[m
[31m-    int boxY = 20;[m
[31m-    int boxW = 300;[m
[31m-    int boxH = 60;[m
[31m-[m
[31m-    // Draw background[m
[31m-    DrawRectangle(boxX, boxY, boxW, boxH, {40, 50, 60, 200});[m
[31m-    // Draw border[m
[31m-    DrawRectangle(boxX, boxY, boxW, 2, UI_ACCENT);[m
[31m-    DrawRectangle(boxX, boxY + boxH - 2, boxW, 2, UI_ACCENT);[m
[31m-    DrawRectangle(boxX, boxY, 2, boxH, UI_ACCENT);[m
[31m-    DrawRectangle(boxX + boxW - 2, boxY, 2, boxH, UI_ACCENT);[m
[31m-[m
[31m-    // Draw score text[m
[31m-    DrawText(scoreStr, boxX + 15, boxY + 18, 24, UI_ACCENT);[m
[32m+[m[32mvoid drawMenuScreen() {[m
[32m+[m[32m  // Title[m
[32m+[m[32m  const char *title = "RUNTIME TERROR";[m
[32m+[m[32m  int titleFontSize = 80;[m
[32m+[m[32m  int titleWidth = MeasureText(title, titleFontSize);[m
[32m+[m[32m  DrawText(title, (SCREEN_WIDTH - titleWidth) / 2, 100, titleFontSize,[m
[32m+[m[32m           UI_ACCENT);[m
[32m+[m
[32m+[m[32m  // Subtitle/Description[m
[32m+[m[32m  const char *subtitle = "Navigate the code as the execution pointer";[m
[32m+[m[32m  int subtitleFontSize = 30;[m
[32m+[m[32m  int subtitleWidth = MeasureText(subtitle, subtitleFontSize);[m
[32m+[m[32m  DrawText(subtitle, (SCREEN_WIDTH - subtitleWidth) / 2, 220, subtitleFontSize,[m
[32m+[m[32m           UI_TEXT_SECONDARY);[m
[32m+[m
[32m+[m[32m  // Instructions box[m
[32m+[m[32m  int boxX = SCREEN_WIDTH / 2 - 400;[m
[32m+[m[32m  int boxY = 350;[m
[32m+[m[32m  int boxW = 800;[m
[32m+[m[32m  int boxH = 300;[m
[32m+[m
[32m+[m[32m  // Draw border with multiple rectangles[m
[32m+[m[32m  DrawRectangle(boxX, boxY, boxW, boxH, {30, 30, 40, 200});[m
[32m+[m[32m  DrawRectangle(boxX, boxY, boxW, 3, UI_ACCENT);[m
[32m+[m[32m  DrawRectangle(boxX, boxY + boxH - 3, boxW, 3, UI_ACCENT);[m
[32m+[m[32m  DrawRectangle(boxX, boxY, 3, boxH, UI_ACCENT);[m
[32m+[m[32m  DrawRectangle(boxX + boxW - 3, boxY, 3, boxH, UI_ACCENT);[m
[32m+[m
[32m+[m[32m  // Instructions text[m
[32m+[m[32m  int instructionY = boxY + 30;[m
[32m+[m[32m  int fontSize = 20;[m
[32m+[m[32m  DrawText("EXECUTION POINTER CONTROLS:", boxX + 30, instructionY, 24,[m
[32m+[m[32m           UI_ACCENT);[m
[32m+[m[32m  instructionY += 40;[m
[32m+[m
[32m+[m[32m  DrawText("SPACE - Jump / Double Jump", boxX + 40, instructionY, fontSize,[m
[32m+[m[32m           UI_TEXT);[m
[32m+[m[32m  instructionY += 30;[m
[32m+[m[32m  DrawText("CTRL + Z - Rewind Time (5 seconds available)", boxX + 40,[m
[32m+[m[32m           instructionY, fontSize, UI_TEXT);[m
[32m+[m[32m  instructionY += 30;[m
[32m+[m[32m  DrawText("Avoid RED syntax errors - they crash compilation!", boxX + 40,[m
[32m+[m[32m           instructionY, fontSize, UI_TEXT);[m
[32m+[m[32m  instructionY += 30;[m
[32m+[m[32m  DrawText("BLUE logical errors slow compilation speed", boxX + 40,[m
[32m+[m[32m           instructionY, fontSize, UI_TEXT);[m
[32m+[m[32m  instructionY += 30;[m
[32m+[m[32m  DrawText("GREEN code blocks compile successfully", boxX + 40, instructionY,[m
[32m+[m[32m           fontSize, UI_TEXT);[m
[32m+[m
[32m+[m[32m  // Start prompt[m
[32m+[m[32m  const char *startText = "Press SPACE to Start";[m
[32m+[m[32m  int startWidth = MeasureText(startText, 28);[m
[32m+[m[32m  int blinkAlpha = (int)(128 + 127 * sin(GetTime() * 3)) + 128;[m
[32m+[m[32m  DrawText(startText, (SCREEN_WIDTH - startWidth) / 2, SCREEN_HEIGHT - 100, 28,[m
[32m+[m[32m           {UI_ACCENT.r, UI_ACCENT.g, UI_ACCENT.b, (unsigned char)blinkAlpha});[m
 }[m
 [m
[31m-void drawGameOverScreen()[m
[31m-{[m
[31m-    // Overlay[m
[31m-    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, {0, 0, 0, 150});[m
[31m-[m
[31m-    // Game Over box[m
[31m-    int boxX = SCREEN_WIDTH / 2 - 500;[m
[31m-    int boxY = SCREEN_HEIGHT / 2 - 250;[m
[31m-    int boxW = 1000;[m
[31m-    int boxH = 500;[m
[31m-[m
[31m-    // Draw background[m
[31m-    DrawRectangle(boxX, boxY, boxW, boxH, UI_PRIMARY);[m
[31m-    // Draw border[m
[31m-    DrawRectangle(boxX, boxY, boxW, 4, UI_HIGHLIGHT);[m
[31m-    DrawRectangle(boxX, boxY + boxH - 4, boxW, 4, UI_HIGHLIGHT);[m
[31m-    DrawRectangle(boxX, boxY, 4, boxH, UI_HIGHLIGHT);[m
[31m-    DrawRectangle(boxX + boxW - 4, boxY, 4, boxH, UI_HIGHLIGHT);[m
[31m-[m
[31m-    // Game Over Title[m
[31m-    const char *gameOverText = "GAME OVER";[m
[31m-    int gameOverWidth = MeasureText(gameOverText, 80);[m
[31m-    DrawText(gameOverText, (SCREEN_WIDTH - gameOverWidth) / 2, boxY + 40, 80, UI_HIGHLIGHT);[m
[31m-[m
[31m-    // Final Score[m
[31m-    int finalScore = player.linesCompiled;[m
[31m-    char scoreStr[64];[m
[31m-    sprintf(scoreStr, "Lines Compiled: %d", finalScore);[m
[31m-    int scoreWidth = MeasureText(scoreStr, 36);[m
[31m-    DrawText(scoreStr, (SCREEN_WIDTH - scoreWidth) / 2, boxY + 150, 36, UI_ACCENT);[m
[31m-[m
[31m-    // Additional stats (you can add more later like pickups collected, time survived, etc.)[m
[31m-    char timeStr[64];[m
[31m-    int seconds = (int)(finalScore / 5.0f); // Rough estimate based on 5 lines per tile at normal speed[m
[31m-    sprintf(timeStr, "Compilation Time: %d seconds", seconds);[m
[31m-    int timeWidth = MeasureText(timeStr, 24);[m
[31m-    DrawText(timeStr, (SCREEN_WIDTH - timeWidth) / 2, boxY + 210, 24, UI_TEXT_SECONDARY);[m
[31m-[m
[31m-    // Restart prompt[m
[31m-    const char *restartText = "Press SPACE to Return to Menu";[m
[31m-    int restartWidth = MeasureText(restartText, 28);[m
[31m-    DrawText(restartText, (SCREEN_WIDTH - restartWidth) / 2, boxY + 350, 28, UI_TEXT);[m
[31m-[m
[31m-    // Blinking effect[m
[31m-    if (((int)(GetTime() * 1.5f) % 2) == 0)[m
[31m-    {[m
[31m-        DrawRectangle((SCREEN_WIDTH - restartWidth) / 2 - 20, boxY + 345, MeasureText(restartText, 28) + 40, 40, {0, 0, 0, 0});[m
[31m-        DrawRectangle((SCREEN_WIDTH - restartWidth) / 2 - 22, boxY + 343, MeasureText(restartText, 28) + 44, 44, UI_ACCENT);[m
[31m-    }[m
[32m+[m[32mvoid drawGameScore() {[m
[32m+[m[32m  // Score box in top-right[m
[32m+[m[32m  int score = player.linesCompiled;[m
[32m+[m[32m  char scoreStr[32];[m
[32m+[m[32m  sprintf(scoreStr, "Lines Compiled: %d", score);[m
[32m+[m
[32m+[m[32m  int boxX = SCREEN_WIDTH - 320;[m
[32m+[m[32m  int boxY = 20;[m
[32m+[m[32m  int boxW = 300;[m
[32m+[m[32m  int boxH = 60;[m
[32m+[m
[32m+[m[32m  // Draw background[m
[32m+[m[32m  DrawRectangle(boxX, boxY, boxW, boxH, {40, 50, 60, 200});[m
[32m+[m[32m  // Draw border[m
[32m+[m[32m  DrawRectangle(boxX, boxY, boxW, 2, UI_ACCENT);[m
[32m+[m[32m  DrawRectangle(boxX, boxY + boxH - 2, boxW, 2, UI_ACCENT);[m
[32m+[m[32m  DrawRectangle(boxX, boxY, 2, boxH, UI_ACCENT);[m
[32m+[m[32m  DrawRectangle(boxX + boxW - 2, boxY, 2, boxH, UI_ACCENT);[m
[32m+[m
[32m+[m[32m  // Draw score text[m
[32m+[m[32m  DrawText(scoreStr, boxX + 15, boxY + 18, 24, UI_ACCENT);[m
 }[m
 [m
[31m-void resetGame()[m
[31m-{[m
[31m-    // Reset player[m
[31m-    player.posX = PLAYER_START_X;[m
[31m-    player.posY = PLAYER_START_Y;[m
[31m-    player.velY = 0;[m
[31m-    player.velX = 0;[m
[31m-    player.isGameOver = false;[m
[31m-    player.inAir = true;[m
[31m-    player.linesCompiled = 0;[m
[31m-    player.Rewind_time_left = REWIND_SECS;[m
[31m-    player.isRewinding = false;[m
[31m-[m
[31m-    // Reset tiles[m
[31m-    for (Tile *t : tiles)[m
[31m-    {[m
[31m-        delete t;[m
[31m-    }[m
[31m-    tiles.clear();[m
[31m-    tiles.push_back(new Tile(TILES_START_X));[m
[31m-[m
[31m-    // Reset pickups[m
[31m-    pickups.clear();[m
[31m-[m
[31m-    // Reset scroll speed[m
[31m-    scrollSpeed = 0;[m
[32m+[m[32mvoid drawGameOverScreen() {[m
[32m+[m[32m  // Overlay[m
[32m+[m[32m  DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, {0, 0, 0, 150});[m
[32m+[m
[32m+[m[32m  // Game Over box[m
[32m+[m[32m  int boxX = SCREEN_WIDTH / 2 - 500;[m
[32m+[m[32m  int boxY = SCREEN_HEIGHT / 2 - 250;[m
[32m+[m[32m  int boxW = 1000;[m
[32m+[m[32m  int boxH = 500;[m
[32m+[m
[32m+[m[32m  // Draw background[m
[32m+[m[32m  DrawRectangle(boxX, boxY, boxW, boxH, UI_PRIMARY);[m
[32m+[m[32m  // Draw border[m
[32m+[m[32m  DrawRectangle(boxX, boxY, boxW, 4, UI_HIGHLIGHT);[m
[32m+[m[32m  DrawRectangle(boxX, boxY + boxH - 4, boxW, 4, UI_HIGHLIGHT);[m
[32m+[m[32m  DrawRectangle(boxX, boxY, 4, boxH, UI_HIGHLIGHT);[m
[32m+[m[32m  DrawRectangle(boxX + boxW - 4, boxY, 4, boxH, UI_HIGHLIGHT);[m
[32m+[m
[32m+[m[32m  // Game Over Title[m
[32m+[m[32m  const char *gameOverText = "GAME OVER";[m
[32m+[m[32m  int gameOverWidth = MeasureText(gameOverText, 80);[m
[32m+[m[32m  DrawText(gameOverText, (SCREEN_WIDTH - gameOverWidth) / 2, boxY + 40, 80,[m
[32m+[m[32m           UI_HIGHLIGHT);[m
[32m+[m
[32m+[m[32m  // Final Score[m
[32m+[m[32m  int finalScore = player.linesCompiled;[m
[32m+[m[32m  char scoreStr[64];[m
[32m+[m[32m  sprintf(scoreStr, "Lines Compiled: %d", finalScore);[m
[32m+[m[32m  int scoreWidth = MeasureText(scoreStr, 36);[m
[32m+[m[32m  DrawText(scoreStr, (SCREEN_WIDTH - scoreWidth) / 2, boxY + 150, 36,[m
[32m+[m[32m           UI_ACCENT);[m
[32m+[m
[32m+[m[32m  // Additional stats (you can add more later like pickups collected, time[m
[32m+[m[32m  // survived, etc.)[m
[32m+[m[32m  char timeStr[64];[m
[32m+[m[32m  int seconds =[m
[32m+[m[32m      (int)(finalScore /[m
[32m+[m[32m            5.0f); // Rough estimate based on 5 lines per tile at normal speed[m
[32m+[m[32m  sprintf(timeStr, "Compilation Time: %d seconds", seconds);[m
[32m+[m[32m  int timeWidth = MeasureText(timeStr, 24);[m
[32m+[m[32m  DrawText(timeStr, (SCREEN_WIDTH - timeWidth) / 2, boxY + 210, 24,[m
[32m+[m[32m           UI_TEXT_SECONDARY);[m
[32m+[m
[32m+[m[32m  // Restart prompt[m
[32m+[m[32m  const char *restartText = "Press SPACE to Return to Menu";[m
[32m+[m[32m  int restartWidth = MeasureText(restartText, 28);[m
[32m+[m[32m  DrawText(restartText, (SCREEN_WIDTH - restartWidth) / 2, boxY + 350, 28,[m
[32m+[m[32m           UI_TEXT);[m
[32m+[m
[32m+[m[32m  // Blinking effect[m
[32m+[m[32m  if (((int)(GetTime() * 1.5f) % 2) == 0) {[m
[32m+[m[32m    DrawRectangle((SCREEN_WIDTH - restartWidth) / 2 - 20, boxY + 345,[m
[32m+[m[32m                  MeasureText(restartText, 28) + 40, 40, {0, 0, 0, 0});[m
[32m+[m[32m    DrawRectangle((SCREEN_WIDTH - restartWidth) / 2 - 22, boxY + 343,[m
[32m+[m[32m                  MeasureText(restartText, 28) + 44, 44, UI_ACCENT);[m
[32m+[m[32m  }[m
[32m+[m[32m}[m
 [m
[31m-    // Reset tile index tracking[m
[31m-    Tile::currentTileIndex = 0;[m
[32m+[m[32mvoid resetGame() {[m
[32m+[m[32m  // Reset player[m
[32m+[m[32m  player.posX = PLAYER_START_X;[m
[32m+[m[32m  player.posY = PLAYER_START_Y;[m
[32m+[m[32m  player.velY = 0;[m
[32m+[m[32m  player.velX = 0;[m
[32m+[m[32m  player.isGameOver = false;[m
[32m+[m[32m  player.inAir = true;[m
[32m+[m[32m  player.linesCompiled = 0;[m
[32m+[m[32m  player.Rewind_time_left = REWIND_SECS;[m
[32m+[m[32m  player.isRewinding = false;[m
[32m+[m
[32m+[m[32m  // Reset tiles[m
[32m+[m[32m  for (Tile *t : tiles) {[m
[32m+[m[32m    delete t;[m
[32m+[m[32m  }[m
[32m+[m[32m  tiles.clear();[m
[32m+[m[32m  tiles.push_back(new Tile(TILES_START_X));[m
[32m+[m
[32m+[m[32m  // Reset pickups[m
[32m+[m[32m  pickups.clear();[m
[32m+[m
[32m+[m[32m  // Reset scroll speed[m
[32m+[m[32m  scrollSpeed = 0;[m
[32m+[m
[32m+[m[32m  // Reset tile index tracking[m
[32m+[m[32m  Tile::currentTileIndex = 0;[m
 }[m
\ No newline at end of file[m
[1mdiff --git a/src/player.cpp b/src/player.cpp[m
[1mindex 27789d0..a96098a 100644[m
[1m--- a/src/player.cpp[m
[1m+++ b/src/player.cpp[m
[36m@@ -1,130 +1,129 @@[m
[31m-#include <iostream>[m
 #include "player.h"[m
[31m-#include "raylib.h"[m
 #include "constants.h"[m
[32m+[m[32m#include "raylib.h"[m
[32m+[m[32m#include <cmath>[m[41m [m
[32m+[m[32m#include <iostream>[m
 [m
 using namespace std;[m
 [m
[31m-[m
[31m-float Player::jumpSoundVolumn=0.1f;[m
[31m-float Player::rewindSoundVolumn=0.5f;[m
[31m-float Player::fallingDownSoundVolumn=1.0f;[m
[32m+[m[32mfloat Player::jumpSoundVolumn = 0.1f;[m
[32m+[m[32mfloat Player::rewindSoundVolumn = 0.5f;[m
[32m+[m[32mfloat Player::fallingDownSoundVolumn = 1.0f;[m
 // Call this after InitAudioDevice()[m
[31m-void Player::Init()[m
[31m-{[m
[31m-    jumpSound = LoadSound("sounds\\jump.wav");[m
[31m-    SetSoundVolume(jumpSound, jumpSoundVolumn);[m
[31m-    [m
[31m-    rewindSound=LoadSound("sounds\\Time_rewind_sound.mp3");[m
[31m-    SetSoundVolume(rewindSound,rewindSoundVolumn);[m
[32m+[m[32mvoid Player::Init() {[m
[32m+[m[32m  jumpSound = LoadSound("sounds\\jump.wav");[m
[32m+[m[32m  SetSoundVolume(jumpSound, jumpSoundVolumn);[m
 [m
[31m-    fallingDownSound=LoadSound("sounds\\falling_down_sound2.mp3");[m
[31m-     SetSoundVolume(fallingDownSound,fallingDownSoundVolumn);[m
[32m+[m[32m  rewindSound = LoadSound("sounds\\Time_rewind_sound.mp3");[m
[32m+[m[32m  SetSoundVolume(rewindSound, rewindSoundVolumn);[m
 [m
[32m+[m[32m  fallingDownSound = LoadSound("sounds\\falling_down_sound2.mp3");[m
[32m+[m[32m  SetSoundVolume(fallingDownSound, fallingDownSoundVolumn);[m
[32m+[m[32m}[m
 [m
[32m+[m[32mvoid Player::Cleanup() {[m
[32m+[m[32m  if (jumpSound.frameCount >[m
[32m+[m[32m      0) // frameCount checks if the sound was properly initialized,[m
[32m+[m[32m  {[m
[32m+[m[32m    UnloadSound(jumpSound); // otherwise we would be unloading a sound that[m
[32m+[m[32m                            // never existed[m
[32m+[m[32m  }[m
[32m+[m
[32m+[m[32m  if (rewindSound.frameCount > 0) {[m
[32m+[m[32m    UnloadSound(rewindSound);[m
[32m+[m[32m  }[m
[32m+[m
[32m+[m[32m  if (fallingDownSound.frameCount > 0) {[m
[32m+[m[32m    UnloadSound(fallingDownSound);[m
[32m+[m[32m  }[m
 }[m
[32m+[m[32mvoid Player::Draw() {[m
[32m+[m[32m  // Calculate Pulse[m
[32m+[m[32m  float blink = (sinf(GetTime() * 10.0f) + 1.0f) / 2.0f;[m
[32m+[m[32m  unsigned char alpha = (unsigned char)((blink * 180) + 75);[m
 [m
[31m-void Player::Cleanup()[m
[31m-{[m
[31m-    if (jumpSound.frameCount > 0) // frameCount checks if the sound was properly initialized,[m
[31m-    {[m
[31m-        UnloadSound(jumpSound); // otherwise we would be unloading a sound that never existed[m
[31m-    }[m
[32m+[m[32m  if (this->isRewinding || this->inAir)[m
[32m+[m[32m    alpha = 255;[m
 [m
[31m-    if (rewindSound.frameCount > 0)[m
[31m-    {[m
[31m-        UnloadSound(rewindSound);[m
[31m-    }[m
[32m+[m[32m  Color cursorColor = {UI_ACCENT.r, UI_ACCENT.g, UI_ACCENT.b, alpha};[m
 [m
[31m-    if(fallingDownSound.frameCount>0)[m
[31m-    {[m
[31m-        UnloadSound(fallingDownSound);[m
[31m-    }[m
[31m-}[m
[31m-void Player::Draw()[m
[31m-{[m
[31m-    DrawRectangle(posX, posY, playerWidth, playerHeight, color);[m
[32m+[m[32m  // Draw the Glow (Using the explicit Rectangle to stay safe)[m
[32m+[m[32m  Rectangle glowRect = {posX - 2, posY, 10, playerHeight};[m
[32m+[m[32m  DrawRectangleLinesEx(glowRect, 1.0f, ColorAlpha(UI_ACCENT, 0.3f));[m
[32m+[m
[32m+[m[32m  // Draw the Main Cursor[m
[32m+[m[32m  DrawRectangle(posX, posY, 6, playerHeight, cursols[m
[32m+[m[32m    rColor);[m
[32m+[m
[32m+[m[32m  // Draw the "I-Beam" tips[m
[32m+[m[32m  DrawRectangle(posX - 4, posY, 14, 2, cursorColor); // Top bar[m
[32m+[m[32m  DrawRectangle(posX - 4, posY + playerHeight - 2, 14, 2, cursorColor); // Bottom bar[m
 }[m
 [m
 // Draw the hitbox with upper and lower parts[m
[31m-void Player::Hitbox(Color c)[m
[31m-{[m
[31m-    float topRegion = playerHeight * HITBOX_SPLIT;[m
[31m-    float bottomRegion = playerHeight - topRegion;[m
[31m-    DrawRectangle(posX, posY, playerWidth, topRegion, WHITE);[m
[31m-    DrawRectangle(posX, posY + topRegion, playerWidth, bottomRegion, c);[m
[32m+[m[32mvoid Player::Hitbox(Color c) {[m
[32m+[m[32m  float topRegion = playerHeight * HITBOX_SPLIT;[m
[32m+[m[32m  float bottomRegion = playerHeight - topRegion;[m
[32m+[m[32m  DrawRectangle(posX, posY, playerWidth, topRegion, WHITE);[m
[32m+[m[32m  DrawRectangle(posX, posY + topRegion, playerWidth, bottomRegion, c);[m
 }[m
 [m
[31m-Rectangle Player::GetCollisionRect() const[m
[31m-{[m
[31m-    return {posX, posY + playerHeight * HITBOX_SPLIT, playerWidth, playerHeight * (1.0f - HITBOX_SPLIT)};[m
[32m+[m[32mRectangle Player::GetCollisionRect() const {[m
[32m+[m[32m  return {posX, posY + playerHeight * HITBOX_SPLIT, playerWidth,[m
[32m+[m[32m          playerHeight * (1.0f - HITBOX_SPLIT)};[m
 }[m
 [m
[31m-Rectangle Player::GetNonCollisionRect() const[m
[31m-{[m
[31m-    return {posX, posY, playerWidth, playerHeight * HITBOX_SPLIT};[m
[32m+[m[32mRectangle Player::GetNonCollisionRect() const {[m
[32m+[m[32m  return {posX, posY, playerWidth, playerHeight * HITBOX_SPLIT};[m
 }[m
 [m
 // Update player physics: fall, jump, apply velocities[m
[31m-void Player::Update()[m
[31m-{[m
[31m-    Fall();[m
[31m-    Jump();[m
[31m-    posY += velY;[m
[31m-    posX += velX;[m
[32m+[m[32mvoid Player::Update() {[m
[32m+[m[32m  Fall();[m
[32m+[m[32m  Jump();[m
[32m+[m[32m  posY += velY;[m
[32m+[m[32m  posX += velX;[m
 }[m
 [m
[31m-void Player::Fall()[m
[31m-{[m
[31m-    if (posY >= GROUND_POS)[m
[31m-    {[m
[31m-        velY = 0;[m
[31m-        posY = GROUND_POS + 5;[m
[31m-        if(!isGameOver)[m
[31m-        {[m
[31m-            PlaySound(fallingDownSound);[m
[31m-        }[m
[31m-        isGameOver = true;[m
[31m-       [m
[32m+[m[32mvoid Player::Fall() {[m
[32m+[m[32m  if (posY >= GROUND_POS) {[m
[32m+[m[32m    velY = 0;[m
[32m+[m[32m    posY = GROUND_POS + 5;[m
[32m+[m[32m    if (!isGameOver) {[m
[32m+[m[32m      PlaySound(fallingDownSound);[m
     }[m
[31m-[m
[31m-    // Apply gravity if in air[m
[31m-    if (inAir)[m
[31m-    {[m
[31m-        delay++;[m
[31m-        if (delay == GRAVITY_DELAY)[m
[31m-        {[m
[31m-            delay = 0;[m
[31m-            velY += GRAVITY;[m
[31m-        }[m
[32m+[m[32m    isGameOver = true;[m
[32m+[m[32m  }[m
[32m+[m
[32m+[m[32m  // Apply gravity if in air[m
[32m+[m[32m  if (inAir) {[m
[32m+[m[32m    delay++;[m
[32m+[m[32m    if (delay == GRAVITY_DELAY) {[m
[32m+[m[32m      delay = 0;[m
[32m+[m[32m      velY += GRAVITY;[m
     }[m
[32m+[m[32m  }[m
 }[m
 [m
 // Handle jumping logic[m
[31m-void Player::Jump()[m
[31m-{[m
[31m-    if (IsKeyPressed(KEY_SPACE))[m
[31m-    {[m
[31m-        if (!inAir)[m
[31m-        {[m
[31m-            velY -= JUMP_HEIGHT;[m
[31m-            inAir = true;[m
[31m-            jumpsAvailable = 1;[m
[31m-            PlaySound(jumpSound);[m
[31m-        }[m
[31m-        else if (jumpsAvailable > 0)[m
[31m-        {[m
[31m-            velY = -JUMP_HEIGHT;[m
[31m-            jumpsAvailable--;[m
[31m-            PlaySound(jumpSound);[m
[31m-        }[m
[32m+[m[32mvoid Player::Jump() {[m
[32m+[m[32m  if (IsKeyPressed(KEY_SPACE)) {[m
[32m+[m[32m    if (!inAir) {[m
[32m+[m[32m      velY -= JUMP_HEIGHT;[m
[32m+[m[32m      inAir = true;[m
[32m+[m[32m      jumpsAvailable = 1;[m
[32m+[m[32m      PlaySound(jumpSound);[m
[32m+[m[32m    } else if (jumpsAvailable > 0) {[m
[32m+[m[32m      velY = -JUMP_HEIGHT;[m
[32m+[m[32m      jumpsAvailable--;[m
[32m+[m[32m      PlaySound(jumpSound);[m
     }[m
[32m+[m[32m  }[m
 }[m
 [m
[31m-void Player::ReduceRewind(float &rewind_time_left, float total_rewind_time)[m
[31m-{[m
[31m-    float rate = total_rewind_time / REWIND_DURATION;[m
[31m-    rewind_time_left -= rate * GetFrameTime();[m
[31m-    if (rewind_time_left < 0)[m
[31m-        rewind_time_left = 0;  [m
[32m+[m[32mvoid Player::ReduceRewind(float &rewind_time_left, float total_rewind_time) {[m
[32m+[m[32m  float rate = total_rewind_time / REWIND_DURATION;[m
[32m+[m[32m  rewind_time_left -= rate * GetFrameTime();[m
[32m+[m[32m  if (rewind_time_left < 0)[m
[32m+[m[32m    rewind_time_left = 0;[m
 }[m
\ No newline at end of file[m
