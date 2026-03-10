# Runtime Terror
An infinite runner inside a VS Code editor. You play as the Cursor trying to execute code while avoiding Bugs and Stack Overflow errors. The main mechanic is 'Ctrl+Z', which lets you rewind time using a Circular Buffer data structure. It's funny, relatable to every student, and technically complex enough to get us a 4.0 GPA.

## Recent Feature Additions

**What we added and why:**

1. **Uncapped Infinite Dash Cooldown**
   * **What:** The player now starts with 0 dashes, but dynamically earns a Dash Charge every `5.0` seconds indefinitely. These are tracked on a GUI. The player can rapidly use these back-to-back without locking the camera.
   * **Why:** The original queue structure got "jammed" with stale inputs and the fixed camera auto-centering made double-dashing impossible. This fix turns the dash into a tactical resource management mechanic.
2. **50/50 RNG Portals (Graph Network)**
   * **What:** Spawning a simple `TeleportGraph` node using Adjacency coordinates. Hitting one triggers a 50% chance to either die instantly, or gain +4 score, pop into the air, and completely invert the world's color palette (simulating a warp to a new dimension).
   * **Why:** We needed a secondary Data Structure (Graph) to satisfy the prompt, but traditional Point-A to Point-B teleporters felt clunky. An RNG-based "Risk versus Reward" portal is much more fun in a fast-paced infinite runner. 
3. **Red Tile Bounce Cap**
   * **What:** Hardcoded the "R" key deflection to only grant `-JUMP_HEIGHT * 1.5f` velocity.
   * **Why:** Because collisions trigger every frame, holding R multiplied the player's upward velocity instantly causing them to launch out of the map bounds. This limits bounding to a set height.
4. **Game State Switch & Start Menu**
   * **What:** Rewrote the `main.cpp` loop to use a `GameState` Enum to handle transitions between MENU, PLAYING, and HIGHSCORES. Built a retro Start Menu to interact with.
   * **Why:** The game originally dropped the player in instantly upon running `game.exe`. This provides a polished Video Game feel and allows the player time to prepare.
5. **Persistent JSON High Scores**
   * **What:** Built a custom parser utilizing `<fstream>` to open, parse, sort, and save the top 3 scores into `scores.json` whenever the user dies.
   * **Why:** Having high scores incentivizes replayability, and building a manual JSON string parser adds an extra layer of C++ complexity.
6. **Spawn Platform Guarantees**
   * **What:** Hard-locked the very first tile generated on boot/restart to a massive 800px wide platform.
   * **Why:** Spawns used random width generation, occasionally dropping the player into a 60px wide pit the moment the game loaded! 

## How to Build & Run
If you want to modify the code or work on the game, you must compile it via the provided `Makefile` using the Raylib compiler environment.

### Prerequisites
- Download and install [Raylib](https://raysan5.itch.io/) (Assumes it is installed at `C:/raylib/`)
- Ensure the `w64devkit` compiler is accessible.

### Compiling on Windows
To build a Debug binary of the game, open your terminal (e.g. VS Code Integrated Terminal) in the root `Runtime-Terror` directory and execute the following command:

```powershell
C:/raylib/w64devkit/bin/mingw32-make.exe RAYLIB_PATH=C:/raylib/raylib PROJECT_NAME=game OBJS=src/*.cpp BUILD_MODE=DEBUG
```

### Running the Game
After a successful compilation, a `game.exe` binary will be generated in the root directory. You can launch the game via:

```powershell
.\game.exe
```