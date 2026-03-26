#include "constants.h"
#include "pickup.h"
#include "player.h"
#include "rewind.h"
#include "tiles.h"
#include <cmath>
#include <raylib.h>
#include <string>
#include <vector>

using namespace std;

Player player;
vector<Tile *> tiles;
vector<Pickup> pickups;
float scrollSpeed = 0;
GameState gameState = MENU;
Music gameMusic;
int lastTileIndex = 0;
RewindBuffer rewindSys{REWIND_SECS * FPS};
 
// Function declarations
void drawHitbox();
void drawTiles();
void updateTiles(float scrollSpeed);
void drawRewindBar(float rewindTimeLeft);
void drawMenuScreen();
void drawGameOverScreen();
void drawGameScore();
void resetGame();

int main()
{
 

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Code Compiler");

  InitAudioDevice(); //  1. Init audio first
  player.Init();     //  2. Then load sounds
  Pickup::Init();
  Tile::Init();
  gameMusic = LoadMusicStream("src\\game_music.mp3");
  PlayMusicStream(gameMusic);

  SetTargetFPS(FPS);
  player.Rewind_time_left = 5;
  tiles.push_back(new Tile(TILES_START_X, Tile::GetMaxTileWidth(), Tile::TileType::NORMAL));

  while (!WindowShouldClose())
  {
    BeginDrawing();
    // IMPORTANT: Call every frame to keep music streaming
    UpdateMusicStream(gameMusic);

    // ClearBackground(UI_PRIMARY);
    ClearBackground(VS_BG);

    switch (gameState)
    {
    case MENU:
      drawMenuScreen();
      if (IsKeyPressed(KEY_SPACE))
      {
        gameState = PLAYING;
        PlayMusicStream(gameMusic);
      }
      break;

    case PLAYING:
    {
      // Use 0.05f instead of 0 — float subtraction never lands exactly on 0,
      // so we treat anything below this epsilon as empty
      if ((IsKeyDown(KEY_Z) && IsKeyDown(KEY_LEFT_CONTROL)) &&
          !player.isGameOver && player.Rewind_time_left > 0.05f)
      {
        // Only play when rewind just starts, not every frame
        if (!player.isRewinding)
          PlaySound(player.rewindSound); //  fires once on first frame

        PlayerState restoredState;
        if (rewindSys.Rewind(restoredState))
        {
          Player::ReduceRewind(player.Rewind_time_left, REWIND_DURATION);

          player.posX = restoredState.posX;
          player.posY = restoredState.posY;

          float reverseSpeed = (scrollSpeed + TILE_SPEED * 2) * -1;
          updateTiles(reverseSpeed);

          // Move pickups at the same reverse speed so they stay on their tiles
          for (Pickup &p : pickups)
            p.Update(-TILE_SPEED, 0);

          player.isRewinding = true;

          // player.Hitbox(UI_ACCENT);
          // In main.cpp switch case PLAYING:
          player.Draw(); // This shows your blinking cursor

          // Only show the red/white hitboxes if you press a specific key (like
          // H)
          if (IsKeyDown(KEY_H))
          {
            player.Hitbox(RED);
          }
          drawTiles();
          Pickups::DrawAll(pickups);
          DrawText("CTRL + Z!", SCREEN_WIDTH / 2 - 300, 100, 100, UI_HIGHLIGHT);
          drawRewindBar(player.Rewind_time_left);
          drawGameScore();
        }
      }
      else
      {
        if (player.isRewinding)
          StopSound(player.rewindSound);

        PlayerState currentState = {player.posX, player.posY};
        rewindSys.Record(currentState);

        player.isRewinding = false;
        player.Update();

        // Tile::WarningText(Tile::currentTileIndex, player, tiles);

        if (!player.isGameOver)
        {
          updateTiles(scrollSpeed);

          // spawn + update + draw pickups
          Pickups::SpawnIfNeeded(pickups, tiles);
          Pickups::UpdateAll(pickups, player, scrollSpeed, Tile::baseSpeed);

          // Award lines for successfully passing tiles

          if (Tile::currentTileIndex > lastTileIndex)
          {
            // Score the tile we just LEFT (lastTileIndex - 1), not the one we're on
            int justLeft = lastTileIndex; // this is the tile index before we moved
            if (justLeft > 0 && justLeft - 1 < (int)tiles.size())
            {
              if (tiles[justLeft - 1]->tileType == Tile::TileType::NORMAL)
                player.linesCompiled += 5;
              else if (tiles[justLeft - 1]->tileType == Tile::TileType::LOGICAL)
                player.linesCompiled += 3;
            }
            lastTileIndex = Tile::currentTileIndex;
          }
        }
        else
        {
          // Transition to game over screen
          gameState = GAME_OVER;
          PauseMusicStream(gameMusic);
        }

        // player.Hitbox(UI_ACCENT);
        // In main.cpp switch case PLAYING:
        player.Draw(); // This shows your blinking cursor

        // Only show the red/white hitboxes if you press a specific key (like H)
        if (IsKeyDown(KEY_H))
        {
          player.Hitbox(RED);
        }
        drawTiles();
        Pickups::DrawAll(pickups);
        drawRewindBar(player.Rewind_time_left);
        drawGameScore();
      }
      break;
    }

    case GAME_OVER:
      drawGameOverScreen();
      if (IsKeyPressed(KEY_SPACE))
      {
        resetGame();
        gameState = MENU;
      }
      break;
    }

    EndDrawing();
  }

  UnloadMusicStream(gameMusic);
  player.Cleanup(); //  Unload sounds before closing audio
  Pickup::Cleanup();
  Tile::CleanupTiles(tiles);
  CloseAudioDevice(); //  Close audio device
  CloseWindow();
}

void drawHitbox()
{
  DrawText("The red is the hitbox!", SCREEN_WIDTH / 3, 30, 40, YELLOW);
  player.Hitbox(RED);
  for (Tile *t : tiles)
    t->Hitbox(ORANGE, t->tileType);
}

void drawTiles()
{
  for (Tile *t : tiles)
    t->Draw(t->tileType);
  Tile::New_tiles(tiles);
}

void updateTiles(float scrollSpeed)
{
  Tile::Delete_And_Update(tiles, scrollSpeed);
  Tile::Collision(player, tiles);
}

void drawRewindBar(float rewindTimeLeft)
{
  const int BAR_X = 20;
  const int BAR_Y = SCREEN_HEIGHT - 50;
  const int BAR_WIDTH = 250;
  const int BAR_HEIGHT = 24;
  const int BORDER = 2;

  // Background track
  DrawRectangle(BAR_X - BORDER, BAR_Y - BORDER, BAR_WIDTH + BORDER * 2,
                BAR_HEIGHT + BORDER * 2, {60, 60, 60, 255});
  DrawRectangle(BAR_X, BAR_Y, BAR_WIDTH, BAR_HEIGHT, {20, 20, 20, 255});

  // Filled portion — cyan when healthy, orange when low
  float fraction = rewindTimeLeft / (float)REWIND_SECS;
  if (fraction < 0)
    fraction = 0;
  if (fraction > 1)
    fraction = 1;
  int fillWidth = (int)(BAR_WIDTH * fraction);

  Color fillColor = (fraction > 0.3f) ? UI_ACCENT : UI_HIGHLIGHT;
  if (fillWidth > 0)
    DrawRectangle(BAR_X, BAR_Y, fillWidth, BAR_HEIGHT, fillColor);

  // Label
  DrawText("REWIND", BAR_X, BAR_Y - 20, 16, UI_TEXT_SECONDARY);

  // Seconds remaining
  char timeStr[16];
  sprintf(timeStr, "%.1f s", rewindTimeLeft);
  DrawText(timeStr, BAR_X + BAR_WIDTH + 8, BAR_Y + 4, 16, UI_TEXT_SECONDARY);
}

void drawMenuScreen()
{
  // Title
  const char *title = "RUNTIME TERROR";
  int titleFontSize = 80;
  int titleWidth = MeasureText(title, titleFontSize);
  DrawText(title, (SCREEN_WIDTH - titleWidth) / 2, 100, titleFontSize,
           UI_ACCENT);

  // Subtitle/Description
  const char *subtitle = "Navigate the code as the execution pointer";
  int subtitleFontSize = 30;
  int subtitleWidth = MeasureText(subtitle, subtitleFontSize);
  DrawText(subtitle, (SCREEN_WIDTH - subtitleWidth) / 2, 220, subtitleFontSize,
           UI_TEXT_SECONDARY);

  // Instructions box
  int boxX = SCREEN_WIDTH / 2 - 400;
  int boxY = 350;
  int boxW = 800;
  int boxH = 300;

  // Draw border with multiple rectangles
  DrawRectangle(boxX, boxY, boxW, boxH, {30, 30, 40, 200});
  DrawRectangle(boxX, boxY, boxW, 3, UI_ACCENT);
  DrawRectangle(boxX, boxY + boxH - 3, boxW, 3, UI_ACCENT);
  DrawRectangle(boxX, boxY, 3, boxH, UI_ACCENT);
  DrawRectangle(boxX + boxW - 3, boxY, 3, boxH, UI_ACCENT);

  // Instructions text
  int instructionY = boxY + 30;
  int fontSize = 20;
  DrawText("EXECUTION POINTER CONTROLS:", boxX + 30, instructionY, 24,
           UI_ACCENT);
  instructionY += 40;

  DrawText("SPACE - Jump / Double Jump", boxX + 40, instructionY, fontSize,
           UI_TEXT);
  instructionY += 30;
  DrawText("CTRL + Z - Rewind Time (5 seconds available)", boxX + 40,
           instructionY, fontSize, UI_TEXT);
  instructionY += 30;
  DrawText("Avoid RED syntax errors - they crash compilation!", boxX + 40,
           instructionY, fontSize, UI_TEXT);
  instructionY += 30;
  DrawText("BLUE logical errors slow compilation speed", boxX + 40,
           instructionY, fontSize, UI_TEXT);
  instructionY += 30;
  DrawText("GREEN code blocks compile successfully", boxX + 40, instructionY,
           fontSize, UI_TEXT);

  // Start prompt
  const char *startText = "Press SPACE to Start";
  int startWidth = MeasureText(startText, 28);
  int blinkAlpha = (int)(128 + 127 * sin(GetTime() * 3)) + 128;
  DrawText(startText, (SCREEN_WIDTH - startWidth) / 2, SCREEN_HEIGHT - 100, 28,
           {UI_ACCENT.r, UI_ACCENT.g, UI_ACCENT.b, (unsigned char)blinkAlpha});
}

void drawGameScore()
{
  // Score box in top-right
  int score = player.linesCompiled;
  char scoreStr[32];
  sprintf(scoreStr, "Lines Compiled: %d", score);

  int boxX = SCREEN_WIDTH - 320;
  int boxY = 20;
  int boxW = 300;
  int boxH = 60;

  // Draw background
  DrawRectangle(boxX, boxY, boxW, boxH, {40, 50, 60, 200});
  // Draw border
  DrawRectangle(boxX, boxY, boxW, 2, UI_ACCENT);
  DrawRectangle(boxX, boxY + boxH - 2, boxW, 2, UI_ACCENT);
  DrawRectangle(boxX, boxY, 2, boxH, UI_ACCENT);
  DrawRectangle(boxX + boxW - 2, boxY, 2, boxH, UI_ACCENT);

  // Draw score text
  DrawText(scoreStr, boxX + 15, boxY + 18, 24, UI_ACCENT);
}

void drawGameOverScreen()
{
  // Overlay
  DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, {0, 0, 0, 150});

  // Game Over box
  int boxX = SCREEN_WIDTH / 2 - 500;
  int boxY = SCREEN_HEIGHT / 2 - 250;
  int boxW = 1000;
  int boxH = 500;

  // Draw background
  DrawRectangle(boxX, boxY, boxW, boxH, UI_PRIMARY);
  // Draw border
  DrawRectangle(boxX, boxY, boxW, 4, UI_HIGHLIGHT);
  DrawRectangle(boxX, boxY + boxH - 4, boxW, 4, UI_HIGHLIGHT);
  DrawRectangle(boxX, boxY, 4, boxH, UI_HIGHLIGHT);
  DrawRectangle(boxX + boxW - 4, boxY, 4, boxH, UI_HIGHLIGHT);

  // Game Over Title
  const char *gameOverText = "GAME OVER";
  int gameOverWidth = MeasureText(gameOverText, 80);
  DrawText(gameOverText, (SCREEN_WIDTH - gameOverWidth) / 2, boxY + 40, 80,
           UI_HIGHLIGHT);

  // Final Score
  int finalScore = player.linesCompiled;
  char scoreStr[64];
  sprintf(scoreStr, "Lines Compiled: %d", finalScore);
  int scoreWidth = MeasureText(scoreStr, 36);
  DrawText(scoreStr, (SCREEN_WIDTH - scoreWidth) / 2, boxY + 150, 36,
           UI_ACCENT);

  // Additional stats (you can add more later like pickups collected, time
  // survived, etc.)
  char timeStr[64];
  int seconds =
      (int)(finalScore /
            5.0f); // Rough estimate based on 5 lines per tile at normal speed
  sprintf(timeStr, "Compilation Time: %d seconds", seconds);
  int timeWidth = MeasureText(timeStr, 24);
  DrawText(timeStr, (SCREEN_WIDTH - timeWidth) / 2, boxY + 210, 24,
           UI_TEXT_SECONDARY);

  // Restart prompt
  const char *restartText = "Press SPACE to Return to Menu";
  int restartWidth = MeasureText(restartText, 28);
  DrawText(restartText, (SCREEN_WIDTH - restartWidth) / 2, boxY + 350, 28,
           UI_TEXT);

  // Blinking effect
  if (((int)(GetTime() * 1.5f) % 2) == 0)
  {
    DrawRectangle((SCREEN_WIDTH - restartWidth) / 2 - 20, boxY + 345,
                  MeasureText(restartText, 28) + 40, 40, {0, 0, 0, 0});
    DrawRectangle((SCREEN_WIDTH - restartWidth) / 2 - 22, boxY + 343,
                  MeasureText(restartText, 28) + 44, 44, UI_ACCENT);
  }
}

void resetGame()
{
  // Reset player — anchor to start of first tile
  player.posX = TILES_START_X;          // left edge of tile + padding
  player.posY = TILE_Y - PLAYER_HEIGHT; // sitting on top of tile
  player.velY = 0;
  player.velX = 0;
  player.inAir = false; // <-- also fix this, was true
  player.jumpsAvailable = 1;
  player.isGameOver = false;
  player.linesCompiled = 0;
  player.Rewind_time_left = REWIND_SECS;
  player.isRewinding = false;
  lastTileIndex = 0;
  rewindSys.Reset();

  for (Tile *t : tiles)
    delete t;
  tiles.clear();
  Tile::currentTileIndex = 0;
  Tile::baseSpeed = TILE_SPEED;
  tiles.push_back(new Tile(TILES_START_X, Tile::GetMaxTileWidth(), Tile::TileType::NORMAL));
  pickups.clear();
  scrollSpeed = 0;

  StopMusicStream(gameMusic);
  SeekMusicStream(gameMusic, 0.0f);
}
