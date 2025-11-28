#pragma once
#include <SDL2/SDL.h>
#include <string>
#include <functional>
#include "Direction.h"
#include "Map.h"
#include "WindowManager.h"

class TextureManager;

class Pacman {
public:
    Pacman(int x, int y, int w, int h);
    bool loadTextures(TextureManager* texManager,
                      const std::string& closePath,
                      const std::string& halfOpenPath,
                      const std::string& openPath);

    void update();
    void render(SDL_Renderer* renderer);
    void cleanup();
    double getAngle() const;
    SDL_Rect getHitbox() const;
    int getDotsEaten() const;
    void setBigDotsEaten(int x);
    int getBigDotsEaten() const;
    void setPosition(int x, int y);
    void setDirection(Direction dir) { currentDir = dir; }
    void setWindowManager(WindowManager* wm) {
        windowManager = wm;
    }
    void die();
    void setDotsEaten(int x);
    Direction getDirection() const { return currentDir; }
    void handleInput(const SDL_Event& event);
    void move(Map* map, int speed);
    Direction getNextDirection() const { return nextDir; }
    void setNextDirection(Direction dir) { nextDir = dir; }
    SDL_Point getTile() const;
    bool ateBigDot = false;
    std::function<void()> onDeathAnimationFinished;
    SDL_Rect rect;
    bool isAlive = true;

private:
    Direction currentDir = STOP;
    Direction nextDir = STOP;
    SDL_Rect hitbox;
    SDL_Texture* frames[3];
    WindowManager* windowManager = nullptr;
    int frameIndex;
    int frameDir;
    Uint32 lastTime;
    int dotsEaten;
    int bigDotsEaten = 0;
    void updateHitbox();
};
