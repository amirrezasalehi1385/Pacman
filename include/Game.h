#pragma once
#include "WindowManager.h"
#include "TextureManager.h"
#include "Map.h"
#include "Pacman.h"
#include <string>

class Game {
public:
    Game();
    ~Game();

    bool init(const std::string& title, int w, int h);
    void run();
    void quit();

private:
    // در Game.h
    Uint32 gameStartTime; // زمان شروع بازی
    void handleEvents();
    void update();
    void render();

    bool isRunning = false;
    int speed = 4;

    WindowManager windowManager;
    TextureManager* textureManager;
    Map* gameMap;
    Pacman* pacman;

    enum Direction { STOP, UP, DOWN, LEFT, RIGHT };
    Direction currentDir = STOP;
    Direction nextDir = STOP;
};
