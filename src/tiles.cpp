#include "tiles.h"
#include "constants.h"
#include <string>
#include <iostream>
using namespace std;

int Tile::TotaltilesCreatedCount = 0;
int Tile::tilesLeft = 0;
float Tile::baseSpeed = TILE_SPEED;
BhopBuffer Tile::bhopBuffer;

Sound Tile::redTileCollisonSound = {};
float Tile::redTileCollisonSoundVolumn = 0.5f;

Sound Tile::LogicalTileCollisonSound = {};
float Tile::LogicalTileCollisonSoundVolumn = 1.0f;

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
    if (Tile::TotaltilesCreatedCount >= VARIANT_TILE_INDEX)
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
   int textWidth = MeasureCodeText(snippet, fontSize);
    tileWidth = (textWidth * 1.1f) + padding * 2 + 20;

    tileX = startX;
    tileY = TILE_Y;

    this->tileIndex = Tile::TotaltilesCreatedCount;
    Tile::TotaltilesCreatedCount++; // This NEVER goes down
    Tile::tilesLeft++;              // This can go down when deleted
}

Tile::Tile(int startX, float tilewidth, TileType type)
{
    this->tileType = type;
    this->tileWidth = tilewidth;
    snippetStartIndex = GetRandomValue(0, SNIPPET_COUNT - 1);
    tileX = startX;
    tileY = TILE_Y;

    this->tileIndex = Tile::TotaltilesCreatedCount;
    Tile::TotaltilesCreatedCount++; // This NEVER goes down
    Tile::tilesLeft++;              // This can go down when deleted
}

void Tile::Draw(TileType type)
{
    // Background colors based on tile type
    Color bgColor = VS_BG;
    if (type == TileType::SYNTAX)
        bgColor = { 79, 17, 21, 255 }; // #4F1115 Dark Red Highlight
    else if (type == TileType::LOGICAL)
        bgColor = { 26, 64, 93, 255 }; // #1A405D Dark Blue Highlight

    DrawRectangle(tileX, tileY, tileWidth, tileHeight, bgColor);

    // Subtle top highlight line ("lit edge" feel)
    Color topHighlight = {90, 90, 90, 180};
    if (type == TileType::SYNTAX)
        topHighlight = {200, 60, 60, 160};  // Red edge for syntax errors
    else if (type == TileType::LOGICAL)
        topHighlight = {60, 130, 200, 160}; // Blue edge for logical errors
    DrawRectangle(tileX, tileY, tileWidth, 2, topHighlight);

    // Subtle bottom border (separates from void below)
    DrawRectangle(tileX, tileY + tileHeight - 1, tileWidth, 1, {55, 55, 60, 200});

    const int fontSize = 24;
    const int padding = 6;

    const char *snippet = cppSnippets[snippetStartIndex % SNIPPET_COUNT];
    Color textColor = GetSnippetColor(snippet);

    // Vertically center the text
    int textY = (int)tileY + (tileHeight - fontSize) / 2;
    int textX = (int)tileX + padding;

    // Clip to tile width using scissor
    BeginScissorMode((int)tileX, (int)tileY, (int)tileWidth, (int)tileHeight);
    DrawCodeText(snippet, textX, textY, fontSize, textColor);
    EndScissorMode();
}

bool Tile::Update(float gameSpeed)
{
    tileX -= baseSpeed + gameSpeed;
    return (tileX + tileWidth <= 0); // True if offscreen
}

int Tile::CalculateRequiredLeftTiles(const std::vector<Tile *> &tiles, float scrollSpeed)
{
    const float requiredDistance = (Tile::baseSpeed + scrollSpeed) * REWIND_SECS * FPS;
    float accumulatedWidth = 0.0f;
    int count = 0;

    for (Tile *t : tiles)
    {
        float rightEdge = t->tileX + t->tileWidth;
        if (rightEdge < 0.0f)
        {
            accumulatedWidth += t->tileWidth;
            count++;
            if (accumulatedWidth >= requiredDistance)
                break;
        }
        else
        {
            // Once we hit screen or right side, we have enough of the left buffer
            break;
        }
    }

    return count;
}

void Tile::Delete_And_Update(std::vector<Tile *> &tiles, float gameSpeed, float scrollSpeed)
{
    // Update all tiles first.
    for (int i = (int)tiles.size() - 1; i >= 0; i--)
    {
        tiles[i]->Update(gameSpeed);
    }

    // Only prune during forward play; keep a minimum rewind buffer when scrolling normally.
    if (gameSpeed >= 0.0f)
    {
        const float requiredDistance = (Tile::baseSpeed + scrollSpeed) * REWIND_SECS * FPS;
        while (!tiles.empty())
        {
            Tile *t = tiles.front();
            float rightEdge = t->tileX + t->tileWidth;

            if (rightEdge <= -requiredDistance)
            {
                delete t;
                tiles.erase(tiles.begin());
            }
            else
            {
                break;
            }
        }
    }

    Tile::tilesLeft = (int)tiles.size();
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

    // this is so that it dosent check collison multiple times from same collison
    static float Syntax_collison_cooldown = 0.0f;
    if (Syntax_collison_cooldown > 0.0f)
        Syntax_collison_cooldown -= GetFrameTime();

    for (size_t i = 0; i < tiles.size(); i++)
    {
        Rectangle tileTop = {tiles[i]->tileX, tiles[i]->tileY, tiles[i]->tileWidth, tileHeight / 3};
        Rectangle tileRect = {tiles[i]->tileX, tiles[i]->tileY, tiles[i]->tileWidth, tileHeight};
        Rectangle playerBottom = {player.posX, player.posY + (2 * PLAYER_HEIGHT) / 3, player.playerWidth, PLAYER_HEIGHT / 3};
        Rectangle playerTop = {player.posX, player.posY, player.playerWidth, (2 * PLAYER_HEIGHT) / 3};

        if (tiles[i]->tileType == TileType::NORMAL)
        {
            if (CheckCollisionRecs(playerBottom, tileTop))
            {
                player.Landed();
                Tile::tilesLeft = (int)tiles.size();
                player.posY = tiles[i]->tileY - PLAYER_HEIGHT + 1;
                player.velY = 0;
                landedOnNormal = true;
                break;
            }
            // NORMAL tile
            else if (CheckCollisionRecs(playerTop, tileRect))
            {
                if (playerRightEdge >= tileRect.x)
                {
                    // Only push back if player is above the tile top (coming from the side)
                    // If player bottom is below tile top, they're falling onto/past it - let gravity handle it
                    float playerBottom = player.posY + PLAYER_HEIGHT;
                    float overlapY = playerBottom - tiles[i]->tileY;
                    float overlapX = playerRightEdge - tileRect.x;

                    if (overlapX < overlapY) // horizontal overlap is smaller = coming from the side
                    {
                        player.posX = player.posX - overlapX + 1;
                        landedOnNormal = true;
                    }
                    break;
                }
            }
        }
        else if (tiles[i]->tileType == TileType::SYNTAX && !player.isRewinding && Syntax_collison_cooldown <= 0.0f)
        {
            if (CheckCollisionRecs(playerBottom, tileTop))
            {
                if (bhopBuffer.framesLeft > 0)
                {

                    player.velY = -JUMP_HEIGHT;
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
                player.Landed();
                Tile::tilesLeft = (int)tiles.size();
                player.posY = tiles[i]->tileY - PLAYER_HEIGHT + 1;
                player.velY = 0;
                landedOnLogical = true;

                // Only play on first frame of landing, not every frame
                if (!IsSoundPlaying(LogicalTileCollisonSound))
                    PlaySound(LogicalTileCollisonSound);

                break;
            }

            // LOGICAL tile - same change
            else if (CheckCollisionRecs(playerTop, tileRect))
            {
                if (playerRightEdge >= tileRect.x)
                {
                    float playerBottom = player.posY + PLAYER_HEIGHT;
                    float overlapY = playerBottom - tiles[i]->tileY;
                    float overlapX = playerRightEdge - tileRect.x;

                    if (overlapX < overlapY)
                    {
                        player.posX = player.posX - overlapX + 1;
                        landedOnLogical = true;
                    }
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
    if (baseSpeed == 0) player.isGameOver = true;
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

bool Tile::Chance(int percent) { return GetRandomValue(1, 100) <= percent; }

void Tile::SmoothCountdown(float &value, float startValue) {
    value -= (startValue / Speed_DELAY_DURATION) * GetFrameTime();
    if (value < 0) value = 0;
}

void Tile::Init() {
    redTileCollisonSound = LoadSound("sounds\\red_tile_death_sound.wav");
    SetSoundVolume(redTileCollisonSound, redTileCollisonSoundVolumn);
    LogicalTileCollisonSound = LoadSound("sounds\\logical_tile_sound2.mp3");
    SetSoundVolume(LogicalTileCollisonSound, LogicalTileCollisonSoundVolumn);
}

float Tile::GetMaxTileWidth()
{
    const int fontSize = 24, padding = 6;
    float maxWidth = 0;
    for (int i = 0; i < SNIPPET_COUNT; i++) {
          float w = MeasureCodeText(cppSnippets[i], fontSize) + padding * 2 + 20; // was MeasureVSText
        if (w > maxWidth) maxWidth = w;
    }
    return maxWidth;
}