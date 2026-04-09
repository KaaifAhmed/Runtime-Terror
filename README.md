# Runtime Terror
Runtime Terror is a Windows-native infinite runner built in C++ using [Raylib](https://www.raylib.com/). Players control a Cursor navigating a stylized VS Code-inspired environment, avoiding Bugs, Stack Overflow errors, and corrupted code tiles while using a rewind mechanic to recover from mistakes.

## Project Overview
- Genre: Infinite runner
- Engine: Raylib
- Language: C++ (C++14)
- Platform: Windows
- Development environment: VS Code

## Key Features
- Player character is the Cursor, racing through code-themed obstacles
- Rewind mechanic via **Ctrl+Z**, implemented with a circular buffer for time restoration
- Distinct hazard types: normal code, logical errors, syntax errors, and corrupted dark world tiles
- Polished UI system with Main Menu, Pause Screen, Game Over screen, and Leaderboard
- Smooth input handling, keyboard navigation, and custom game state management

## Prerequisites
1. Install Raylib for Windows
   - Download from https://raysan5.itch.io/raylib/purchase?popup=1
   - Raylib is free; enter `$0` when prompted
2. Extract Raylib to `C:\raylib\`
3. Confirm the directory structure:
   ```text
   C:\raylib\
   ├── w64devkit\bin\      (contains g++.exe, mingw32-make.exe)
   └── src\                 (raylib headers and library)
   ```

## Build Instructions
### Recommended: Use VS Code build tasks
This repository includes VS Code tasks for debug and release builds using the installed MinGW toolchain.

### Manual build
From the repository root:
```powershell
cd C:\Users\kaaif\Documents\Github\Runtime-Terror
C:\raylib\w64devkit\bin\g++.exe -std=c++14 -g -I C:/raylib/raylib/src src/*.cpp -L C:/raylib/raylib/src -lraylib -lopengl32 -lgdi32 -lwinmm -luser32 -o main.exe
```

## Run the Game
From the repository root:
```powershell
cd C:\Users\kaaif\Documents\Github\Runtime-Terror
.\main.exe
```

## Controls
| Key | Action |
|-----|--------|
| `SPACE` | Jump / double jump |
| `CTRL + Z` | Rewind time for up to 5 seconds |
| `R` | Use Bhop buffer to deflect off syntax error tiles |
| `LEFT SHIFT` | Dash forward (recharges over time) |
| `ESC` | Pause / back |
| `H` | Toggle hitbox debug view |

## Gameplay Mechanics
- **Normal tiles** represent safe code execution paths.
- **Logical errors** slow the player and require precise timing.
- **Syntax errors** are lethal unless deflected with the `R` buffer.
- **Graph portals** transition the player into an alternate dark world and reward successful progress.
- **Dark World** tiles are more dangerous and visually corrupted for a high-intensity challenge.

## Project Structure
- `src/` — game source code and headers
- `src/main.cpp` — application entry point
- `src/player.cpp`, `player.h` — player movement and abilities
- `src/tiles.cpp`, `tiles.h` — tile generation and obstacle behavior
- `src/ui_system.cpp`, `ui_system.h` — menu and HUD rendering
- `src/leaderboard.cpp`, `leaderboard.h` — scoring and persistent leaderboard
- `src/screens.cpp`, `screens.h` — menu, pause, and game over screens

## Notes
- The codebase is designed to be edited and rebuilt from within VS Code.
- The game currently targets Windows and Raylib's MinGW toolchain.

## License
This repository does not currently specify a license. Add a `LICENSE` file if you wish to permit reuse or distribution.
