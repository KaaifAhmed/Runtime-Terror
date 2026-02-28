#include "tiles.h"
#include "constants.h"
#include <string>

// Code snippets for different tile types
const char* safeCodeSnippets[] = {
    "int x = 0;",
    "float y;",
    "bool flag = true;",
    "return 0;",
    "if (valid)",
    "while (true)",
    "for (int i=0;)",
    "void* ptr;",
    "class Foo {}",
    "std::vector<>"
};

const char* syntaxErrorSnippets[] = {
    "int x = ;",
    "if () {}",
    "return ;",
    "void main(}",
    "int[ ]",
    "x ++ +"
};

const char* logicErrorSnippets[] = {
    "if (x = 0)",
    "while (i--)",
    "i++ + ++i",
    "null->call()",
    "arr[1000]",
    "x / 0"
};

const char* exceptionSnippets[] = {
    "throw e;",
    "catch(...)",
    "std::exception"
};

Tile::Tile(int startX, TileType t)
{
    type = t;
    
    if (type == GAP) {
        width_of_tile = GetRandomValue(80, 150);
        x_of_tile = startX;
        y_of_tile = SCREEN_HEIGHT + 100; // Offscreen
    } else {
        width_of_tile = GetRandomValue(200, 500);
        x_of_tile = startX;
        y_of_tile = GetRandomValue(4*SCREEN_HEIGHT/5, 4*SCREEN_HEIGHT/5 - 40);
    }
    
    exceptionY = 0;
    exceptionPhase = GetRandomValue(0, 360);
    
    GenerateCodeText();
}
int Tile::tile_number = 0;

void Tile::Draw()
{
    if (type == GAP) return;
    
    // Draw tile with appropriate color
    Color tileColor = GetTileColor();
    
    if (type == LOGIC_ERROR) {
        // Red background for logic error
        DrawRectangle(x_of_tile, y_of_tile, width_of_tile, height_of_tile, RED);
        DrawRectangle(x_of_tile + 5, y_of_tile + 5, width_of_tile - 10, height_of_tile - 10, DARKGRAY);
    } else {
        DrawRectangle(x_of_tile, y_of_tile, width_of_tile, height_of_tile, tileColor);
    }
    
    // Draw code text
    if (!codeText.empty()) {
        int fontSize = 20;
        int textX = x_of_tile + 10;
        int textY = y_of_tile + (height_of_tile - fontSize) / 2;
        
        Color textColor = (type == SAFE_CODE) ? BLACK : WHITE;
        DrawText(codeText.c_str(), textX, textY, fontSize, textColor);
    }
    
    // Draw exception (floating red ball)
    if (type == EXCEPTION) {
        exceptionPhase += 2;
        exceptionY = sin(exceptionPhase * PI / 180) * 30;
        DrawCircle(x_of_tile + width_of_tile/2, y_of_tile - 30 + exceptionY, 15, RED);
        DrawText("X", x_of_tile + width_of_tile/2 - 7, y_of_tile - 40 + exceptionY, 20, WHITE);
    }
    
    // Draw jagged underline for syntax error
    if (type == SYNTAX_ERROR) {
        for (int i = 0; i < width_of_tile; i += 10) {
            int yOffset = (i / 10) % 2 == 0 ? -5 : 5;
            DrawLine(x_of_tile + i, y_of_tile + height_of_tile + 5, 
                     x_of_tile + i + 5, y_of_tile + height_of_tile + 5 + yOffset, RED);
        }
    }
}

bool Tile::Update()
{
    x_of_tile -= normal_speed;               // move left
    return (x_of_tile + width_of_tile <= 0); // true if offscreen
}

void Tile::Delete_And_Update(std::vector<Tile *> &tiles)
{
    // updating the tiles
    for (int i = tiles.size() - 1; i >= 0; i--)
    {
        if (tiles[i]->Update())
        {
            delete tiles[i];                // delete tile if offscreen
            tiles.erase(tiles.begin() + i); // remove from vector
        }
    }

    if(tile_number>=3)
    {
       for(int i = 1; i >= 0; i--)
        {
            delete tiles[i];
            tiles.erase(tiles.begin() + i);
        }
        tile_number -= 2;

    }
}

void Tile::New_tiles(std::vector<Tile *> &tiles)
{
    while (tiles.size() < 5 || tiles.back()->x_of_tile + tiles.back()->width_of_tile < SCREEN_WIDTH + 400)
    {
        int lastRight = tiles.empty() ? SCREEN_WIDTH : tiles.back()->x_of_tile + tiles.back()->width_of_tile;
        int gap_between_tiles = GetRandomValue(100, 250);
        
        // Randomly choose tile type
        TileType newType = SAFE_CODE;
        int randType = GetRandomValue(1, 100);
        
        if (randType <= 60) {
            newType = SAFE_CODE; // 60% safe code
        } else if (randType <= 75) {
            newType = SYNTAX_ERROR; // 15% syntax error
        } else if (randType <= 85) {
            newType = LOGIC_ERROR; // 10% logic error
        } else if (randType <= 95) {
            newType = EXCEPTION; // 10% exception
        } else {
            newType = GAP; // 5% gap (null pointer)
        }
        
        tiles.push_back(new Tile(lastRight + gap_between_tiles, newType));
    }
}

void Tile::Collision(Player &player, const std::vector<Tile *> &tiles)
{
    bool collision = false;
    int overlap;
    int player_right = player.x + player.width;
  

    // checking collision
    for (long long unsigned int i=0;i<tiles.size();i++)
    {
        // checking of bottom of player with top of tile
        Rectangle tile_top = {tiles[i]->x_of_tile, tiles[i]->y_of_tile, tiles[i]->width_of_tile, ((float)tiles[i]->height_of_tile) / 3};
        Rectangle player_bottom = {player.x, player.y + (float)(2 * player.height) / 3, player.width, player.height / 3};

        // check the rest of player with the entire tile
        Rectangle player_top = {player.x, player.y, player.width, ((float)2 * player.height) / 3};
        Rectangle tile = {tiles[i]->x_of_tile, tiles[i]->y_of_tile, tiles[i]->width_of_tile, tiles[i]->height_of_tile};

        // Check collision with exception (dynamic hazard)
        if (tiles[i]->type == EXCEPTION) {
            float exX = tiles[i]->x_of_tile + tiles[i]->width_of_tile/2;
            float exY = tiles[i]->y_of_tile - 30 + tiles[i]->exceptionY;
            float playerCenterX = player.x + player.width/2;
            float playerCenterY = player.y + player.height/2;
            float dist = sqrt((exX - playerCenterX)*(exX - playerCenterX) + 
                              (exY - playerCenterY)*(exY - playerCenterY));
            if (dist < 35) { // 15 radius + player size
                player.isGameOver = true;
            }
        }

        if (CheckCollisionRecs(player_bottom, tile_top))
        {
            // Check if it's a hazard tile
            if (tiles[i]->type == SYNTAX_ERROR) {
                player.isGameOver = true;
                break;
            }
            if (tiles[i]->type == LOGIC_ERROR) {
                tiles[i]->ApplyHazardEffect(player);
            }
            
            player.inAir = false; // because he is on a solid block
            player.jumpCount = 0; // reset double jump
            tile_number=i+1;
            player.y = tiles[i]->y_of_tile - player.height + 1;
            player.y_velocity = 0;
            collision = true;
            break;
        }
        else if (CheckCollisionRecs(player_top, tile))
        {

            if (player_right >= tile.x)
            {
                overlap = player_right - tile.x; // how far it has gone inside
                player.x = player.x - overlap + 1;
                collision = true;
                break;
            }
        }
    }
    // convert int -> string (the string OWNS the memory)
 std::string tileText = std::to_string(tile_number);

// get const char* from the string
const char* tile_cstr = tileText.c_str();
DrawText(tile_cstr,20,SCREEN_HEIGHT-60,45,WHITE);

    if (!collision)
    {
        player.inAir = true;
    }
}

void Tile::Cleanup(std::vector<Tile *> &tiles)
{
    // clean up remaining tiles
    for (Tile *t : tiles)
        delete t;
    tiles.clear();
}

void Tile::Hitbox(Color c)
{
    // upper part
    DrawRectangle(x_of_tile, y_of_tile, width_of_tile, (float)height_of_tile / 3, c);
    // lower part
    DrawRectangle(x_of_tile, y_of_tile + (float)height_of_tile / 3, width_of_tile, ((float)2 * height_of_tile) / 3, WHITE);
}

void Tile::GenerateCodeText()
{
    int count;
    switch(type) {
        case SAFE_CODE:
            count = sizeof(safeCodeSnippets) / sizeof(safeCodeSnippets[0]);
            codeText = safeCodeSnippets[GetRandomValue(0, count - 1)];
            break;
        case SYNTAX_ERROR:
            count = sizeof(syntaxErrorSnippets) / sizeof(syntaxErrorSnippets[0]);
            codeText = syntaxErrorSnippets[GetRandomValue(0, count - 1)];
            break;
        case LOGIC_ERROR:
            count = sizeof(logicErrorSnippets) / sizeof(logicErrorSnippets[0]);
            codeText = logicErrorSnippets[GetRandomValue(0, count - 1)];
            break;
        case EXCEPTION:
            count = sizeof(exceptionSnippets) / sizeof(exceptionSnippets[0]);
            codeText = exceptionSnippets[GetRandomValue(0, count - 1)];
            break;
        default:
            codeText = "";
    }
}

Color Tile::GetTileColor()
{
    switch(type) {
        case SAFE_CODE: return WHITE;
        case SYNTAX_ERROR: return RED;
        case LOGIC_ERROR: return ORANGE;
        case EXCEPTION: return MAROON;
        default: return BLACK;
    }
}

void Tile::ApplyHazardEffect(Player &player)
{
    switch(type) {
        case SYNTAX_ERROR:
            player.isGameOver = true; // Instant crash
            break;
        case LOGIC_ERROR:
            // Slow down effect - push player left
            player.x -= 50;
            break;
        default:
            break;
    }
}