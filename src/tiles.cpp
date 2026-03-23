#include "tiles.h"
#include "constants.h"
#include <string>
#include <iostream>
using namespace std;

int Tile::currentTileIndex = 0;
float Tile::baseSpeed = TILE_SPEED;
BhopBuffer Tile::bhopBuffer;

Sound Tile::redTileCollisonSound={};
float Tile::redTileCollisonSoundVolumn=0.5f;

Sound Tile::LogicalTileCollisonSound={};
float Tile::LogicalTileCollisonSoundVolumn=1.0f;

struct CodeToken
{
    const char *text;
    int type;
};

static const char *cppSnippets[] = {

    "int maxVal = arr[0];",
    "for (int i=1; i<n; i++) {",
    "if (arr[i] > maxVal){ maxVal = arr[i]}",
    "return maxVal;",

    "freq = {}",
    "for ch in string:",
    "  if ch in freq:",
    "    freq[ch] += 1",
    "  else: freq[ch] = 1",
    // JavaScript - flatten array
    "let flat = [];",
    "for (let i=0; i<arr.length; i++){",
    "  for (let j=0; j<arr[i].length; j++)",
    "     {flat.push(arr[i][j])}",
    // Java - prefix sum
    "int[] prefix = new int[n+1];",
    "prefix[0] = 0;",
    "for (int i=0; i<n; i++) {",
    "  prefix[i+1] = prefix[i] + arr[i] }",

    // C# - reverse array
    "int left = 0;",
    "int right = arr.Length - 1;",
    "while (left < right) {",
    "  int tmp = arr[left];",
    "  arr[left++] = arr[right];",
    "  arr[right--] = tmp; }",

    "int lo=0, hi=n-1, mid;",
    "while (lo <= hi) {",
    "  mid = (lo+hi) / 2;",
    "  if (arr[mid]==target) return mid;",
    "  else if (arr[mid]<target) lo=mid+1;",
    "  else hi = mid - 1; }",

    "lo, hi = 0, len(arr)-1",
    "while lo <= hi:",
    "  mid = (lo + hi) // 2",
    "  if arr[mid] == target: return mid",
    "  lo = mid+1 if arr[mid]<target else hi-1",

    "let i = 0;",
    "while (i < arr.length) {",
    "  if (arr[i] === arr[i+1])",
    "    arr.splice(i+1, 1);",
    "  else i++;",

    "int left=0, right=n-1;",
    "while (left < right) {",
    "  int sum = arr[left]+arr[right];",
    "  if (sum == target) return true;",
    "  else if (sum < target) left++;",
    "  else right--; }",

    "if (val < minVal) {",
    "  val = minVal;",
    "} else if (val > maxVal) {",
    "  val = maxVal;",
    "} else {val = val; }",
    // Python - fizzbuzz
    "if n % 15 == 0:",
    "  print('FizzBuzz')",
    "elif n % 3 == 0:",
    "  print('Fizz')",
    "elif n % 5 == 0:",
    "  print('Buzz')",
    // JavaScript - grade check
    "if (score >= 90) {",
    "  grade = 'A';",
    "} else if (score >= 80) {",
    "  grade = 'B';",
    "} else if (score >= 70) {",
    "  grade = 'C'; }",
    // Java - null safe access
    "if (map == null) {return -1;",
    "} else if (map.containsKey(k)) {",
    "  return map.get(k);} else {",
    "  return 0; }",
    // C# - state machine
    "if (state == State.IDLE) {",
    "  if (Input.move) state=State.RUN;",
    "} else if (state == State.RUN) {",
    "  if (Input.jump) state=State.JUMP;",
    "} else if (state == State.JUMP) {",
    "  if (grounded) state=State.IDLE; }",

    // ===== SWITCH / CASE BLOCKS (5-6 lines) =====
    // C++
    "switch (direction) {",
    "  case UP:    posY--; break;",
    "  case DOWN:  posY++; break;",
    "  case LEFT:  posX--; break;",
    "  case RIGHT: posX++; break;",
    "  default: break; }",
    // Python match
    "match command:",
    "  case 'quit':  running=False",
    "  case 'up':    y -= 1",
    "  case 'down':  y += 1",
    "  case 'left':  x -= 1",
    "  case 'right': x += 1",
    // JavaScript
    "switch (event.key) {",
    "  case 'ArrowUp':    y--; break;",
    "  case 'ArrowDown':  y++; break;",
    "  case 'ArrowLeft':  x--; break;",
    "  case 'ArrowRight': x++; break;",
    "  default: break; }",
    // Java
    "switch (tokenType) {",
    "  case INT:    parseInt(); break;",
    "  case STRING: parseStr(); break;",
    "  case BOOL:   parseBool(); break;",
    "  case NULL:   parseNull(); break;",
    "  default:     skip(); break; }",
    // C#
    "switch (phase) {",
    "  case Phase.MENU:  DrawMenu(); break;",
    "  case Phase.PLAY:  Update();   break;",
    "  case Phase.PAUSE: DrawPause();break;",
    "  case Phase.OVER:  DrawOver(); break;",
    "  default: break; }",

    // ===== BUBBLE SORT (5-6 lines) =====
    // C++
    "for (int i=0; i<n-1; i++) {",
    "  for (int j=0; j<n-i-1; j++) {",
    "    if (arr[j] > arr[j+1]) {",
    "      int t = arr[j];",
    "      arr[j] = arr[j+1];",
    "      arr[j+1] = t; } } }",
    // Python
    "for i in range(n-1):",
    "  for j in range(n-i-1):",
    "    if arr[j] > arr[j+1]:",
    "      arr[j], arr[j+1] =",
    "        arr[j+1], arr[j]",
    // JavaScript
    "for (let i=0; i<n-1; i++) {",
    "  for (let j=0; j<n-i-1; j++) {",
    "    if (a[j] > a[j+1]) {",
    "      let t = a[j];",
    "      a[j] = a[j+1];",
    "      a[j+1] = t; } } }",

    // ===== MERGE SORT MERGE STEP (5-6 lines) =====
    // C++
    "int i=0, j=0, k=0;",
    "while (i<lLen && j<rLen) {",
    "  if (L[i] <= R[j])",
    "    arr[k++] = L[i++];",
    "  else arr[k++] = R[j++]; }",
    // Python
    "i = j = k = 0",
    "while i<len(L) and j<len(R):",
    "  if L[i] <= R[j]:",
    "    arr[k] = L[i]; i+=1",
    "  else: arr[k] = R[j]; j+=1",
    // Java
    "int i=0, j=0, k=left;",
    "while (i<n1 && j<n2) {",
    "  if (L[i] <= R[j])",
    "    arr[k++] = L[i++];",
    "  else arr[k++] = R[j++]; }",

    // ===== QUICK SORT PARTITION (5-6 lines) =====
    // C++
    "int pivot = arr[hi];",
    "int i = lo - 1;",
    "for (int j=lo; j<hi; j++) {",
    "  if (arr[j] <= pivot) {",
    "    swap(arr[++i], arr[j]);",
    "} } swap(arr[i+1], arr[hi]);",
    // Python
    "pivot = arr[high]",
    "i = low - 1",
    "for j in range(low, high):",
    "  if arr[j] <= pivot:",
    "    i += 1",
    "    arr[i], arr[j] = arr[j], arr[i]",
    // JavaScript
    "const pivot = arr[hi];",
    "let i = lo - 1;",
    "for (let j=lo; j<hi; j++) {",
    "  if (arr[j] <= pivot) {",
    "    i++;",
    "    [arr[i],arr[j]]=[arr[j],arr[i]]; } }",

    // ===== LINKED LIST (5-6 lines) =====
    // C++ - traverse and print
    "Node* curr = head;",
    "while (curr != nullptr) {",
    "  cout << curr->data;",
    "  if (curr->next) cout<<'->';",
    "  curr = curr->next}",
    // C++ - delete node
    "Node* curr = head;",
    "while (curr->next != nullptr) {",
    "  if (curr->next->data == val) {",
    "    Node* tmp = curr->next;",
    "    curr->next = tmp->next;",
    "    delete tmp; break; } curr=curr->next;}",
    // Python - reverse linked list
    "prev = None",
    "curr = head",
    "while curr:",
    "  nxt = curr.next",
    "  curr.next = prev",
    "  prev = curr; curr = nxt",
    // JavaScript - find middle
    "let slow = head;",
    "let fast = head;",
    "while (fast && fast.next) {",
    "  slow = slow.next;",
    "  fast = fast.next.next}",
    // Java - detect cycle
    "ListNode slow = head;",
    "ListNode fast = head;",
    "while (fast!=null && fast.next!=null){",
    "  slow = slow.next;",
    "  fast = fast.next.next;",
    "  if(slow==fast) return true; }",

    // ===== DYNAMIC PROGRAMMING (5-6 lines) =====
    // C++ - fibonacci
    "vector<int> dp(n+1, 0);",
    "dp[0] = 0; dp[1] = 1;",
    "for (int i=2; i<=n; i++) {",
    "  dp[i] = dp[i-1] + dp[i-2]}",
    "return dp[n];",
    // C++ - 0/1 knapsack
    "for (int i=1; i<=n; i++) {",
    "  for (int w=0; w<=W; w++) {",
    "    dp[i][w] = dp[i-1][w];",
    "    if (wt[i]<=w)",
    "      dp[i][w]=max(dp[i][w],",
    "        dp[i-1][w-wt[i]]+val[i]); } }",
    // Python - LCS
    "dp=[[0]*(n+1) for _ in range(m+1)]",
    "for i in range(1,m+1):",
    "  for j in range(1,n+1):",
    "    if a[i-1]==b[j-1]:",
    "      dp[i][j]=dp[i-1][j-1]+1",
    "    else: dp[i][j]=max(dp[i-1][j],dp[i][j-1])",
    // JavaScript - coin change
    "const dp = new Array(amt+1).fill(Inf);",
    "dp[0] = 0;",
    "for (const c of coins) {",
    "  for (let i=c; i<=amt; i++) {",
    "    dp[i] = Math.min(dp[i],",
    "      dp[i-c] + 1); } }",
    // Java - max subarray (Kadane)
    "int maxSum = arr[0];",
    "int curr = arr[0];",
    "for (int i=1; i<n; i++) {",
    "  curr = Math.max(arr[i], curr+arr[i]);",
    "  maxSum = Math.max(maxSum, curr)}",
    // C# - longest increasing subsequence
    "int[] dp = new int[n];",
    "Array.Fill(dp, 1);",
    "for (int i=1; i<n; i++)",
    "  for (int j=0; j<i; j++)",
    "    if (arr[j]<arr[i])",
    "      dp[i]=Math.Max(dp[i],dp[j]+1);",
};
static const int SNIPPET_COUNT = sizeof(cppSnippets) / sizeof(cppSnippets[0]);

static Color GetSnippetColor(const char *line)
{
    std::string s(line);

    if (s.find("\"") != std::string::npos || s.find("'") != std::string::npos)
        return {206, 145, 120, 255};

    if (s.find("for ") != std::string::npos || s.find("for(") != std::string::npos ||
        s.find("while") != std::string::npos || s.find("do {") != std::string::npos ||
        s.find("if (") != std::string::npos || s.find("if(") != std::string::npos ||
        s.find("else") != std::string::npos || s.find("elif") != std::string::npos ||
        s.find("switch") != std::string::npos || s.find("case ") != std::string::npos ||
        s.find("default:") != std::string::npos || s.find("break;") != std::string::npos ||
        s.find("return") != std::string::npos || s.find("foreach") != std::string::npos)
        return {86, 156, 214, 255}; // blue

    if (s.find("vector") != std::string::npos || s.find("map") != std::string::npos ||
        s.find("stack") != std::string::npos || s.find("queue") != std::string::npos ||
        s.find("set") != std::string::npos || s.find("list") != std::string::npos ||
        s.find("List") != std::string::npos || s.find("Map") != std::string::npos ||
        s.find("Stack") != std::string::npos || s.find("Queue") != std::string::npos ||
        s.find("Dict") != std::string::npos || s.find("Array") != std::string::npos ||
        s.find("HashMap") != std::string::npos || s.find("ArrayList") != std::string::npos)
        return {78, 201, 176, 255};

    if (s.find("int ") != std::string::npos || s.find("float ") != std::string::npos ||
        s.find("bool ") != std::string::npos || s.find("void ") != std::string::npos ||
        s.find("let ") != std::string::npos || s.find("const ") != std::string::npos ||
        s.find("var ") != std::string::npos || s.find("new ") != std::string::npos ||
        s.find("static") != std::string::npos)
        return {197, 134, 192, 255}; // purple

    // Indented lines (continuations) → dimmer grey
    if (s.size() > 0 && s[0] == ' ')
        return {170, 170, 170, 255};

    return {220, 220, 220, 255}; // default light grey
}

Tile::Tile(int startX)
{
    if (currentTileIndex >= VARIANT_TILE_INDEX)
    {
        if (Chance(30))
            tileType = TileType::SYNTAX;
        else if (Chance(20))
            tileType = TileType::LOGICAL;
    }
    snippetStartIndex = GetRandomValue(0, SNIPPET_COUNT - 1);

    const int fontSize = 24;
    const int padding = 6;
    const char *snippet = cppSnippets[snippetStartIndex % SNIPPET_COUNT];
    int textWidth = MeasureText(snippet, fontSize);
    tileWidth = textWidth + padding * 2 + 20;

    tileX = startX;
    tileY = TILE_Y;
}

void Tile::Draw(TileType type)
{
    // Dark editor background
    Color bgColor = {28, 28, 28, 255};
    if (type == TileType::SYNTAX)
        bgColor = {45, 12, 12, 255};
    else if (type == TileType::LOGICAL)
        bgColor = {12, 18, 45, 255};
    DrawRectangle(tileX, tileY, tileWidth, tileHeight, bgColor);

    // Colored top border
    Color borderColor = {80, 80, 80, 255};
    if (type == TileType::SYNTAX)
        borderColor = {220, 50, 50, 255};
    else if (type == TileType::LOGICAL)
        borderColor = {80, 120, 255, 255};
    DrawRectangle(tileX, tileY, tileWidth, 3, borderColor);

    const int fontSize = 24;
    const int padding = 6;

    const char *snippet = cppSnippets[snippetStartIndex % SNIPPET_COUNT];
    Color textColor = GetSnippetColor(snippet);

    // Vertically center the text
    int textY = (int)tileY + (tileHeight - fontSize) / 2;
    int textX = (int)tileX + padding;

    // Clip to tile width using scissor
    BeginScissorMode((int)tileX, (int)tileY, (int)tileWidth, (int)tileHeight);
    DrawText(snippet, textX, textY, fontSize, textColor);
    EndScissorMode();
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
    }

    if (currentTileIndex >= LEFT_TILES)
    {
        for (int i = 1; i >= 0; i--)
        {
            delete tiles[i];
            tiles.erase(tiles.begin() + i);
        }
        currentTileIndex -= 2;
    }
}

void Tile::New_tiles(std::vector<Tile *> &tiles)
{
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
        }
        tiles.push_back(new Tile(lastRight + gap));
    }
}

void Tile::Collision(Player &player, const std::vector<Tile *> &tiles)
{
    // Update bhop buffer for red tile deflection
    if (IsKeyPressed(KEY_R))
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
                currentTileIndex = i + 1;
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
        else if (tiles[i]->tileType == TileType::SYNTAX && !player.isRewinding)
        {
            if (CheckCollisionRecs(playerBottom, tileTop))
            {
                if (bhopBuffer.framesLeft > 0)
                {
                    player.velY -= JUMP_HEIGHT * 2;
                    bhopBuffer.framesLeft = 0;
                    player.inAir = true;
                }
                else
                {
                    PlaySound(redTileCollisonSound);
                    player.isGameOver = true;
                }
                break;
            }
        }
        else if (tiles[i]->tileType == TileType::LOGICAL && !player.isRewinding)
        {
            if (CheckCollisionRecs(playerBottom, tileTop))
            {
                player.inAir = false;
                player.jumpsAvailable = 0;
                currentTileIndex = i + 1;
                player.posY = tiles[i]->tileY - player.playerHeight + 1;
                player.velY = 0;
                landedOnLogical = true;

                // Only play on first frame of landing, not every frame
                if (!IsSoundPlaying(LogicalTileCollisonSound))
                    PlaySound(LogicalTileCollisonSound);

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

        // Stop logical tile sound the moment player leaves the tile
        if (IsSoundPlaying(LogicalTileCollisonSound))
            StopSound(LogicalTileCollisonSound);
    }

    if (landedOnLogical)
        SmoothCountdown(baseSpeed, TILE_SPEED);
    if (baseSpeed == 0)
        player.isGameOver = true;
    std::string tileText = std::to_string(currentTileIndex);
    DrawText(tileText.c_str(), 20, SCREEN_HEIGHT - 60, 45, WHITE);
}

void Tile::CleanupTiles(std::vector<Tile *> &tiles)
{
    // Unload sounds first before audio device closes
    if (redTileCollisonSound.frameCount > 0)
        UnloadSound(redTileCollisonSound);

    if (LogicalTileCollisonSound.frameCount > 0)
        UnloadSound(LogicalTileCollisonSound);

    // Then delete tile pointers
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

void Tile::WarningText(int tileIndex, Player player, const std::vector<Tile *> &tiles)
{
    if (tileIndex >= VARIANT_TILE_INDEX)
    {
        static int start_time = -1;
        if (start_time == -1)
            start_time = GetTime();
        double elapsed = GetTime() - start_time;

        // Check for upcoming dangerous tiles
        bool syntaxWarning = false;
        bool logicalWarning = false;

        for (size_t i = 0; i < tiles.size(); i++)
        {
            if (tiles[i]->tileX > player.posX + 200 && tiles[i]->tileX < player.posX + 600) // Tiles approaching
            {
                if (tiles[i]->tileType == TileType::SYNTAX)
                    syntaxWarning = true;
                else if (tiles[i]->tileType == TileType::LOGICAL)
                    logicalWarning = true;
            }
        }

        if (syntaxWarning && elapsed <= 2.0)
        {
            DrawText("RED SYNTAX ERROR APPROACHING!", SCREEN_WIDTH / 4, 50, 60, UI_HIGHLIGHT);
            DrawText("Press R to deflect or compilation crashes!", SCREEN_WIDTH / 6, 120, 30, UI_TEXT);
        }
        else if (logicalWarning && elapsed <= 2.0)
        {
            DrawText("BLUE LOGICAL ERROR APPROACHING!", SCREEN_WIDTH / 4, 50, 60, UI_ACCENT);
            DrawText("Navigate carefully - slows compilation speed!", SCREEN_WIDTH / 6, 120, 30, UI_TEXT);
        }
        else if (elapsed <= 4.0)
        {
            DrawText("GREEN CODE BLOCKS = SUCCESSFUL COMPILATION", SCREEN_WIDTH / 6, 50, 40, {0, 180, 0, 255});
        }
    }
}

void Tile::SmoothCountdown(float &value, float startValue)
{
    float rate = startValue / Speed_DELAY_DURATION;
    value -= rate * GetFrameTime();
    if (value < 0)
        value = 0;
}

// Call this after InitAudioDevice()
void Tile::Init()
{
    redTileCollisonSound = LoadSound("sounds\\red_tile_death_sound.wav");
    SetSoundVolume(redTileCollisonSound, redTileCollisonSoundVolumn);

    LogicalTileCollisonSound = LoadSound("sounds\\logical_tile_sound2.mp3");
    SetSoundVolume(LogicalTileCollisonSound, LogicalTileCollisonSoundVolumn);
}