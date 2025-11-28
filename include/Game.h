#pragma once
#include "WindowManager.h"
#include "TextureManager.h"
#include "Direction.h"
#include "GhostManager.h"
#include "Ghost.h"
#include "Map.h"
#include "Pacman.h"
#include <string>
#include <SDL2/SDL_ttf.h>

class Game {
public:
    Game();
    ~Game();
    bool init(const std::string& title, int w, int h);
    bool isReady = false;
    void showReadyScreen();
    void run();
    void quit();
    void handlePacmanDeath();
    void resetGhostsPostion();
    Uint32 readyStartTime = 0;
    const Uint32 readyDuration = 4267;
    bool pacmanDied = false;
    bool showGameOver = false;
    Uint32 gameOverStartTime = 0;
    TTF_Font* font = nullptr;
    int lives = 3;
    std::vector<SDL_Texture*> livesTextures;
    std::vector<SDL_Texture*> pacmanDeathTextures;
    void resetPacmanPosition();
    void renderLives();
    void renderScore();
    void loadSounds();
    int getScore(){
        return score;
    };
    void setScore(int s){
        score = s;
    }
private:
    int score;
    Direction currentDir = STOP;
    std::vector<Uint32> cycleTimes = {7000, 20000,7000, 20000, 5000, 20000, 5000};
    std::vector<Uint32> frightenedTimes = {6000, 5000,4000, 3000};
    std::vector<Ghost*> ghosts;
    GhostState currentMode = SCATTER;
    int cycleIndex = 0;
    Uint32 modeStartTime = 0;
    bool cycleStarted = false;
    Uint32 frightenedUntil = 0;

    int currentFrightenedIndex = 0;
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
    Direction nextDir = STOP;
};
