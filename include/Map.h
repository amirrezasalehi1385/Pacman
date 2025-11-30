// Map.h
#pragma once
#include <vector>
#include <string>
#include <SDL2/SDL.h>

class TextureManager;

class Map {
public:
    Map(TextureManager* texManager, SDL_Renderer* rend);
    void spawnDots();
    void loadLevel1();
    std::vector<std::vector<int>> mapGrid;
    void loadMap(const std::vector<std::vector<int>>& mapData);
    void render();
    bool checkCollision(const SDL_Rect& hitbox) const;
    bool isWalkable(int x, int y) const;
    static bool isInGhostHouse(int x,int y) ;
    std::vector<std::vector<int>> tileGrid;
    void setMazeColor(Uint8 r, Uint8 g, Uint8 b) {
        mazeR = r; mazeG = g; mazeB = b;
    }
    void setMazeFlash(bool white);
    int totalDots = 0;
    void resetDots();
private:
    std::vector<SDL_Texture*> texturesBlue;   // تکسچرهای آبی
    std::vector<SDL_Texture*> texturesWhite;  // تکسچرهای سفید
    std::vector<SDL_Texture*>* currentTextures; // اشاره‌گر به ست فعلی

    Uint8 mazeR = 33, mazeG = 33, mazeB = 255;    std::vector<std::string> texturePaths;
    std::vector<SDL_Texture*> textures;
    TextureManager* textureManager;
    SDL_Renderer* renderer;
    int tileSize = 16;
};
