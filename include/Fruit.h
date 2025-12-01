#pragma once
#include <SDL2/SDL.h>
#include <string>

class Fruit {
public:
    Fruit(int level);

    void loadTextures(SDL_Renderer* renderer);
    void render(SDL_Renderer* renderer);
    void update();
    bool isVisible() const { return visible; }
    bool checkCollision(const SDL_Rect& pacmanHitbox);

    int getScore() const { return score; }

private:
    SDL_Texture* texture = nullptr;
    SDL_Rect rect;
    bool visible = false;
    Uint32 spawnTime = 0;
    Uint32 duration = 8000; // 8 seconds like arcade
    int score;

    void selectFruitByLevel(int level);
};
