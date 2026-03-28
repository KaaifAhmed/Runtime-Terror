# Runtime Terror
An infinite runner inside a VS Code editor. You play as the Cursor trying to execute code while avoiding Bugs and Stack Overflow errors. The main mechanic is 'Ctrl+Z', which lets you rewind time using a Circular Buffer data structure. Now features a polished VS Code dark theme UI system with Main Menu, Pause Screen, Game Over screen, and Leaderboard.

## Prerequisites

1. **Download Raylib** from https://raysan5.itch.io/raylib/purchase?popup=1 (free, just enter $0)
2. **Extract** to `C:\raylib\` (must be this exact path)
3. **Verify** the structure:
   ```
   C:\raylib\
   ├── w64devkit\bin\      (contains g++.exe, mingw32-make.exe)
   └── src\                 (raylib headers and library)
   ```

## Build Commands

### Using Make (Recommended)
```powershell
cd c:\github-repos\Runtime-Terror
C:\raylib\w64devkit\bin\mingw32-make.exe clean
C:\raylib\w64devkit\bin\mingw32-make.exe
```

### Manual Compile (Alternative)
```powershell
cd c:\github-repos\Runtime-Terror\src
C:\raylib\w64devkit\bin\g++.exe -o ../game.exe main.cpp ui_system.cpp leaderboard.cpp screens.cpp screens_part2.cpp screens_part3.cpp player.cpp tiles.cpp pickup.cpp -std=c++14 -I. -IC:/raylib/src -LC:/raylib/src -lraylib -lopengl32 -lgdi32 -lwinmm
```

## Run Command

```powershell
cd c:\github-repos\Runtime-Terror
.\game.exe
```

## Game Controls

| Key | Action |
|-----|--------|
| **SPACE** | Jump / Double Jump |
| **CTRL + Z** | Rewind time (5 seconds max) |
| **ESC** | Pause game |
| **H** | Show hitboxes (debug) |

## UI Features

### Main Menu
- Animated gradient background with subtle grid effects
- Pulsing logo with glow effect and blinking cursor
- Smooth button hover animations with cyan accent bars
- Keyboard navigation (UP/DOWN/ENTER) + mouse support

### Pause Screen
- Clean dark overlay with fade-in animation
- Cyan accent color theme
- Simple menu: Resume, Restart, Main Menu, Quit
- ESC to resume

### Game Over Screen
- Clean terminal-style display
- Score and stats shown clearly
- Space to restart, M for menu, Q to quit
- Smart name input: default "Player" name, ESC to skip

### Leaderboard
- Linked list with insertion sort for top 10 scores
- Saves to file automatically
- E to export CSV, R to reset

## Troubleshooting

- **"mingw32-make not recognized"**: Use full path `C:\raylib\w64devkit\bin\mingw32-make.exe`
- **"Permission denied"**: Close the game first with `taskkill /f /im game.exe`
- **"raylib.h not found"**: Check `C:\raylib\` exists with correct structure
- **Audio warnings**: Game works without audio - these are just warnings
- **Window closes immediately**: Check `C:\raylib\w64devkit\bin\` is in your PATH or use full paths

## VS Code Workspace

Open `main.code-workspace` in VS Code for easy editing and building.

## Credits

Made with [Raylib](https://www.raylib.com/) - A simple and easy-to-use library to enjoy videogames programming.
