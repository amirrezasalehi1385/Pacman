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
    void generateTilesFromWalls();  // این مرحله اتومات انتخاب تکسچر
    std::vector<std::vector<int>> mapGrid;
    void loadMap(const std::vector<std::vector<int>>& mapData);
    void render();
    bool checkCollision(const SDL_Rect& hitbox) const;
    bool isWalkable(int x, int y) const;
    static bool isInGhostHouse(int x,int y) ;
    std::vector<std::vector<int>> tileGrid;
private:
private:
    std::vector<std::string> texturePaths;
    std::vector<SDL_Texture*> textures;
    TextureManager* textureManager;
    SDL_Renderer* renderer;
    int tileSize = 16;
};
