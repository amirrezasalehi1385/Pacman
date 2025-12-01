#ifndef FRUITMANAGER_H
#define FRUITMANAGER_H

#include <SDL2/SDL.h>
#include <string>
#include <vector>

class TextureManager;

class FruitManager {
private:
    std::vector<SDL_Texture*> fruitTextures;
    std::vector<int> fruitScores;
    SDL_Rect rect;

    bool visible;
    bool spawned1;
    bool spawned2;

    int scoreValue;
    Uint32 spawnTime;

    TextureManager* texManager;
    SDL_Renderer* renderer;

    const int spawn1 = 70;
    const int spawn2 = 170;
    const Uint32 duration = 9500;
    int currentFruitIndex = -1;

public:
    std::vector<int> eatenFruits; // حداکثر طول 7
    FruitManager();
    ~FruitManager();
    void renderHUD(SDL_Renderer* renderer);
    void init(TextureManager* texManager, SDL_Renderer* renderer);
    void reset(int level);
    void update(int dotsEaten, const SDL_Rect& pacHitbox, int& score);
    void render(SDL_Renderer* renderer);
    bool loadTextures(TextureManager* texManager);
    int getScore();
    bool isVisible() const { return visible; }
    void selectFruitByLevel(int level); // ✅ با آرایه کار می‌کنه
private:
};

#endif
