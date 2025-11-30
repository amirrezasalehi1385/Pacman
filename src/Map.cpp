#include "Map.h"
#include "TextureManager.h"
#include <random>
#include <ctime>

void Map::spawnDots() {
    std::vector<std::pair<int,int>> emptyTiles;

    for (int y = 0; y < mapGrid.size(); ++y) {
        for (int x = 0; x < mapGrid[y].size(); ++x) {
            if (mapGrid[y][x] == 0) {
                if (!(y >= 9 && y <= 22 && x >= 7 && x <= 20) && !(y == 14 && (x <= 5 || x >= 22)) && !(x == 14 && y == 23) ) {
                    tileGrid[y][x] = 21;
                    totalDots++;
                    emptyTiles.emplace_back(x,y);
                }
            }
        }
    }

    std::vector<std::pair<int,int>> bigDotPositions = {
            {1,3}, {26,3}, {1,23}, {26,23}
    };

    for (auto [x,y] : bigDotPositions) {
        tileGrid[y][x] = 22;
    }
}

void Map::resetDots() {
    totalDots = 0;

    for (int y = 0; y < tileGrid.size(); ++y) {
        for (int x = 0; x < tileGrid[y].size(); ++x) {
            // اگه dot بود، پاک کن
            if (tileGrid[y][x] == 21 || tileGrid[y][x] == 22) {
                tileGrid[y][x] = 0;
            }
        }
    }

    // دوباره spawn کن
    spawnDots();
}

Map::Map(TextureManager* texManager, SDL_Renderer* rend)
        : textureManager(texManager), renderer(rend)
{
    // تکسچرهای آبی
    std::vector<std::string> texturePathsBlue = {
            "",
            "assets/walls/1.png",
            "assets/walls/2.png",
            "assets/walls/3.png",
            "assets/walls/4.png",
            "assets/walls/5.png",
            "assets/walls/6.png",
            "assets/walls/7.png",
            "assets/walls/8.png",
            "assets/walls/9.png",
            "assets/walls/10.png",
            "assets/walls/11.png",
            "assets/walls/12.png",
            "assets/walls/13.png",
            "assets/walls/14.png",
            "assets/walls/15.png",
            "assets/walls/16.png",
            "assets/walls/17.png",
            "assets/walls/18.png",
            "assets/walls/19.png",
            "assets/walls/20.png",
            "assets/walls/21.png",
            "assets/walls/22.png",
            "assets/walls/23.png",
            "assets/walls/24.png",
            "assets/walls/25.png",
            "assets/walls/26.png",
            "assets/walls/27.png",
            "assets/walls/28.png",
            "assets/walls/29.png",
            "assets/walls/30.png",
            "assets/walls/31.png"
    };

    for(auto& path : texturePathsBlue) {
        if(path.empty()) texturesBlue.push_back(nullptr);
        else texturesBlue.push_back(textureManager->loadTexture(path));
    }

    // تکسچرهای سفید
    std::vector<std::string> texturePathsWhite = {
            "",
            "assets/walls_white/1.png",
            "assets/walls_white/2.png",
            "assets/walls_white/3.png",
            "assets/walls_white/4.png",
            "assets/walls_white/5.png",
            "assets/walls_white/6.png",
            "assets/walls_white/7.png",
            "assets/walls_white/8.png",
            "assets/walls_white/9.png",
            "assets/walls_white/10.png",
            "assets/walls_white/11.png",
            "assets/walls_white/12.png",
            "assets/walls_white/13.png",
            "assets/walls_white/14.png",
            "assets/walls_white/15.png",
            "assets/walls_white/16.png",
            "assets/walls_white/17.png",
            "assets/walls_white/18.png",
            "assets/walls_white/19.png",
            "assets/walls_white/20.png",
            "assets/walls_white/21.png",
            "assets/walls_white/22.png",
            "assets/walls_white/23.png",
            "assets/walls_white/24.png",
            "assets/walls_white/25.png",
            "assets/walls_white/26.png",
            "assets/walls_white/27.png",
            "assets/walls_white/28.png",
            "assets/walls_white/29.png",
            "assets/walls_white/30.png",
            "assets/walls_white/31.png"
    };

    for(auto& path : texturePathsWhite) {
        if(path.empty()) texturesWhite.push_back(nullptr);
        else texturesWhite.push_back(textureManager->loadTexture(path));
    }

    // شروع با تکسچرهای آبی
    currentTextures = &texturesBlue;
}
void Map::loadLevel1() {
    std::vector<std::vector<int>> level = {
            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
            {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
            {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,0,1},
            {1,0,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,0,1},
            {1,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,1},
            {1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1},
            {2,2,2,2,2,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,2,2,2,2,2},
            {2,2,2,2,2,1,0,1,1,0,0,0,0,0,0,0,0,0,0,1,1,0,1,2,2,2,2,2},
            {2,2,2,2,2,1,0,1,1,0,1,1,1,0,0,1,1,1,0,1,1,0,1,2,2,2,2,2},
            {1,1,1,1,1,1,0,1,1,0,1,0,0,0,0,0,0,1,0,1,1,0,1,1,1,1,1,1},
            {0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
            {1,1,1,1,1,1,0,1,1,0,1,0,0,0,0,0,0,1,0,1,1,0,1,1,1,1,1,1},
            {2,2,2,2,2,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,2,2,2,2,2},
            {2,2,2,2,2,1,0,1,1,0,0,0,0,0,0,0,0,0,0,1,1,0,1,2,2,2,2,2},
            {2,2,2,2,2,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,2,2,2,2,2},
            {1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
            {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
            {1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,1},
            {1,1,1,0,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,0,1,1,1},
            {1,1,1,0,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,0,1,1,1},
            {1,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,1},
            {1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1},
            {1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };

    loadMap(level);
    spawnDots();


}

bool Map::checkCollision(const SDL_Rect& hitbox) const
{
    SDL_Rect hitboxInt;
    hitboxInt.x = (int)hitbox.x;
    hitboxInt.y = (int)hitbox.y;
    hitboxInt.w = (int)hitbox.w;
    hitboxInt.h = (int)hitbox.h;

    for (int row = 0; row < mapGrid.size(); row++) {
        for (int col = 0; col < mapGrid[row].size(); col++) {

            int tile = mapGrid[row][col];
            if (tile != 0) {

                SDL_Rect wallRect;
                wallRect.x = col * tileSize;
                wallRect.y = 3 * tileSize + row * tileSize;
                wallRect.w = tileSize;
                wallRect.h = tileSize;

                if (SDL_HasIntersection(&hitboxInt, &wallRect)) {
                    return true;
                }
            }
        }
    }

    return false;
}


void Map::loadMap(const std::vector<std::vector<int>>& mapData) {
    mapGrid = mapData;
    tileGrid.assign(mapGrid.size(), std::vector<int>(mapGrid[0].size(), 0));

    for(int row=0; row<mapGrid.size(); ++row){
        for(int col=0; col<mapGrid[row].size(); ++col){
            int texIndex = 0;
            if((row < 12 || row > 16) || (col < 10 || col > 17)){
                if(row==0 && col==0) texIndex=14;
                else if(row==0 && col==mapGrid[row].size()-1) texIndex=13;
                else if(row==mapGrid.size()-1 && col==0) texIndex=15;
                else if(row==mapGrid.size()-1 && col==mapGrid[row].size()-1) texIndex=16;
                else if(row==0) texIndex=8;
                else if(row==mapGrid.size()-1) texIndex=6;
                else if(col==0 && mapGrid[row][col+1] == 0 && row != 14) texIndex=5;
                else if(col==mapGrid[row].size()-1 && mapGrid[row][col - 1] == 0 && row != 14) texIndex=7;
                else if(col== 0 && mapGrid[row][col + 1] == 1 && mapGrid[row + 1][col + 1] == 1) texIndex = 25;
                else if(col== 0 && mapGrid[row][col + 1] == 1 && mapGrid[row - 1][col + 1] == 1) texIndex = 28;
                else if(col== mapGrid[row].size() - 1 && mapGrid[row][col - 1] == 1 && mapGrid[row + 1][col - 1] == 1) texIndex = 30;
                else if(col== mapGrid[row].size() - 1 && mapGrid[row][col - 1] == 1 && mapGrid[row - 1][col - 1] == 1) texIndex = 23;
                else if(col != 0 && col != mapGrid[row].size() - 1  && mapGrid[row][col] == 1 && mapGrid[row - 1][col] == 1 && mapGrid[row + 1][col] == 1 && mapGrid[row][col + 1] == 1 && mapGrid[row][col - 1] == 0) texIndex = 3;
                else if(col != 0 && col != mapGrid[row].size() - 1  && mapGrid[row][col] == 1 && mapGrid[row - 1][col] == 1 && mapGrid[row + 1][col] == 1 && mapGrid[row][col + 1] == 0 && mapGrid[row][col - 1] == 1) texIndex = 1;
                else if(col != 0 && col != mapGrid[row].size() - 1 && mapGrid[row][col] == 1 && mapGrid[row][col + 1] == 1 && mapGrid[row][col - 1] == 0 && mapGrid[row - 1][col] == 0 && mapGrid[row + 1][col] == 1) texIndex = 10;
                else if(col != 0 && col != mapGrid[row].size() - 1 && mapGrid[row][col] == 1 && mapGrid[row][col + 1] == 0 && mapGrid[row][col - 1] == 1 && mapGrid[row - 1][col] == 0 && mapGrid[row + 1][col] == 1) texIndex = 9;
                else if(col != 0 && col != mapGrid[row].size() - 1 && mapGrid[row][col] == 1 && mapGrid[row][col + 1] == 1 && mapGrid[row][col - 1] == 0 && mapGrid[row - 1][col] == 1 && mapGrid[row + 1][col] == 0) texIndex = 11;
                else if(col != 0 && col != mapGrid[row].size() - 1 && mapGrid[row][col] == 1 && mapGrid[row][col + 1] == 0 && mapGrid[row][col - 1] == 1 && mapGrid[row - 1][col] == 1 && mapGrid[row + 1][col] == 0) texIndex = 12;
                else if(col != 0 && col != mapGrid[row].size() - 1 && mapGrid[row][col] == 1 && mapGrid[row + 1][col] == 1 && mapGrid[row][col + 1] == 1 && mapGrid[row][col - 1] == 1 && mapGrid[row - 1][col] == 0) texIndex = 2;
                else if(col != 0 && col != mapGrid[row].size() - 1 && mapGrid[row][col] == 1&& mapGrid[row - 1][col] == 1  && mapGrid[row][col + 1] == 1 && mapGrid[row][col - 1] == 1 && mapGrid[row + 1][col] == 0) texIndex = 4;
                else if(col != 0 && col != mapGrid[row].size() - 1 && mapGrid[row][col] == 1  && mapGrid[row][col + 1] == 1 && mapGrid[row - 1][col] == 1 && mapGrid[row - 1][col + 1] == 0) texIndex = 11;
                else if(col != 0 && col != mapGrid[row].size() - 1 && mapGrid[row][col] == 1  && mapGrid[row][col - 1] == 1 && mapGrid[row - 1][col] == 1 && mapGrid[row - 1][col - 1] == 0) texIndex = 12;
                else if(col != 0 && col != mapGrid[row].size() - 1 && mapGrid[row][col] == 1  && mapGrid[row][col + 1] == 1 && mapGrid[row + 1][col] == 1 && mapGrid[row + 1][col + 1] == 0) texIndex = 10;
                else if(col != 0 && col != mapGrid[row].size() - 1 && mapGrid[row][col] == 1  && mapGrid[row][col - 1] == 1 && mapGrid[row + 1][col] == 1 && mapGrid[row + 1][col - 1] == 0) texIndex = 9;
                else if(col != 0 && col != (mapGrid[row].size() - 1) && mapGrid[row][col] == 1  && mapGrid[row][col - 1] == 1 && mapGrid[row + 1][col] == 1 && mapGrid[row + 1][col - 1] == 0) texIndex = 9;
                else if(col != 0 && col != (mapGrid[row].size() - 1) && mapGrid[row][col] == 1 && mapGrid[row][col + 1] == 0 && mapGrid[row][col - 1] == 0 && mapGrid[row + 1][col] == 1 && mapGrid[row - 1][col] == 1 && col < mapGrid[row].size() / 2) texIndex=5;
                else if(col != 0 && col != (mapGrid[row].size() - 1) && mapGrid[row][col] == 1 && mapGrid[row][col + 1] == 0 && mapGrid[row][col - 1] == 0 && mapGrid[row + 1][col] == 1 && mapGrid[row - 1][col] == 1 && col > mapGrid[row].size() / 2) texIndex=7;
                else if(mapGrid[row][col] == 1  && mapGrid[row + 1][col] == 2 && mapGrid[row - 1][col] == 0) texIndex = 6;
                else if(mapGrid[row][col] == 1 && mapGrid[row - 1][col] == 2 && mapGrid[row + 1][col] == 0) texIndex = 8;
                else if(col == 0 && mapGrid[row][col] == 1 && mapGrid[row][col + 1] == 1 && mapGrid[row - 1][col] == 1 && mapGrid[row - 1][col + 1] == 0) texIndex = 15;
                else if(col == 0 && mapGrid[row][col] == 1 && mapGrid[row][col + 1] == 1 && mapGrid[row + 1][col] == 1 && mapGrid[row + 1][col + 1] == 0) texIndex = 14;
                else if(col == mapGrid[row].size() - 1 && mapGrid[row][col] == 1 && mapGrid[row][col - 1] == 1 && mapGrid[row - 1][col] == 1 && mapGrid[row - 1][col - 1] == 0) texIndex = 16;
                else if(col == mapGrid[row].size() - 1 && mapGrid[row][col] == 1 && mapGrid[row][col - 1] == 1 && mapGrid[row + 1][col] == 1 && mapGrid[row + 1][col - 1] == 0) texIndex = 13;
                else if(mapGrid[row][col] == 1 && mapGrid[row][col - 1] == 2 && mapGrid[row][col + 1] == 0 ) texIndex =5;
                else if(mapGrid[row][col] == 1 && mapGrid[row][col - 1] == 0 && mapGrid[row][col + 1] == 2 ) texIndex =7;
                if(row==0 && col>0 && col<mapGrid[row].size()-2
                   && mapGrid[row+1][col]==1 && mapGrid[row+1][col+1]==1){
                    texIndex = 27;
                }else if(row==0 && mapGrid[row + 1][col] == 1 && mapGrid[row + 1][col - 1]==1) {
                    texIndex = 24;
                }
            }else {
                if(row ==12 && col == 10) texIndex = 18;
                if(row == 16 && col == 10) texIndex = 19;
                if(row == 12 && col == 17) texIndex = 17;
                if(row ==16 && col == 17) texIndex = 20;
                if(row > 12 && row < 16 && col == 10) texIndex = 7;
                if(row > 12 && row < 16 && col == 17) texIndex =5;
                if(row == 12 && (col == 11 || col == 12 || col == 15 || col == 16 )) texIndex = 6;
                if(col > 10 && col <17 && row == 16) texIndex = 8;
            }
            if(mapGrid[row][col] == 0 && row == 12 && (col == 13 || col == 14)) texIndex = 31;
            tileGrid[row][col] = texIndex;
        }
    }
}

void Map::render() {
    for(int row=0; row<tileGrid.size(); ++row){
        for(int col=0; col<tileGrid[row].size(); ++col){
            int texIndex = tileGrid[row][col];
            if(texIndex == 0) continue;
            SDL_Texture* tex = (*currentTextures)[texIndex];

            SDL_Rect dst{
                    col * tileSize,
                    3 * tileSize + row * tileSize,
                    tileSize,
                    tileSize
            };
            SDL_RenderCopy(renderer, tex, nullptr, &dst);
        }
    }
}
void Map::setMazeFlash(bool white) {
    currentTextures = white ? &texturesWhite : &texturesBlue;
}


bool Map::isWalkable(int x, int y) const {
    bool walkable = (mapGrid[y][x] == 0);
    return (walkable);
}
bool Map::isInGhostHouse(int x, int y) {
    bool insideGhostHouse = (11 <= x && x <= 16 && 13 <= y && y <= 15);
    bool ghostDoor = ((x == 13 && y == 12) || (x == 14 && y == 12));
    return insideGhostHouse || ghostDoor;
}
