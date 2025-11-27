#pragma once
#include "WindowManager.h"
#include "TextureManager.h"
#include "GhostManager.h"
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
    void handlePacmanDeath();
    bool pacmanDied = false;
    bool showGameOver = false;
    Uint32 gameOverStartTime = 0;

    std::vector<SDL_Texture*> pacmanDeathTextures;
private:

        std::vector<Uint32> cycleTimes = {15000, 20000,15000, 20000, 15000, 20000, 15000};
    GhostState currentMode = SCATTER;
    int cycleIndex = 0;
    Uint32 modeStartTime = 0;
    bool cycleStarted = false;
    Uint32 frightenedUntil = 0;
    Uint32 gameStartTime;
    void handleEvents();
    void update();
    void render();
    bool isRunning = false;
    int speed = 2;

    WindowManager windowManager;
    TextureManager* textureManager;
    Map* gameMap;
    Pacman* pacman;

    enum Direction { STOP, UP, DOWN, LEFT, RIGHT };
    Direction currentDir = STOP;
    Direction nextDir = STOP;
};
