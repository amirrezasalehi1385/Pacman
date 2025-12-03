#pragma once
#include "WindowManager.h"
#include "TextureManager.h"
#include "Direction.h"
#include "Ghost.h"
#include "Map.h"
#include "Pacman.h"
#include <string>
#include <SDL2/SDL_ttf.h>
#include "GameRules.h"
#include "GameState.h"
#include "FruitManager.h"
#include "GhostType.h"
#include <cstdio>

class Game {
public:
    Game();
    ~Game();

    bool init(const std::string& title, int w, int h);
    bool isReady = false;
    void run();
    void quit();

    void handlePacmanDeath();
    void loadTargetTexture();

    Uint32 readyStartTime = 0;
    const Uint32 readyDuration = 4267;

    TTF_Font* font = nullptr;
    std::vector<SDL_Texture*> livesTextures;
    std::vector<SDL_Texture*> pacmanDeathTextures;

    void resetPacmanPosition();
    void renderLives();
    void renderScore();
    void loadSounds();
    int getScore() { return score; }
    void setScore(int s) { score = s; }
    int ghostsEatenInThisFrightened = 0;

    void renderGame();
    void playPacmanDeathAnimation();
    void updateScore();
    void updatePacman();
    void updateGhosts();
    void renderDebug();
    void updateFrightened();
    void handleLevelComplete();
    void playMazeFlashAnimation();
    void renderGameStatic();
    void startNextLevel();
    void updateGhostSpeedForLevel();
    void updateFrightenedTimeForLevel();
    bool isPlayingDeathAnimation = false;
    void handleMenuInput();
    int selectedIndex = 0;
    void updateCruiseElroy();
    std::vector<std::string> options = { "Start", "Exit" };
    void handleMenuEvent(SDL_Event& event);
    void handleEvent(SDL_Event& event);
    void updateSirenSound();
    void handlePlayingStateKeys(SDL_Event& event);
    void handlePausedStateKeys(SDL_Event& event);
    void handleKeyboardEvents(SDL_Event& event);
    void handleMouseEvents(SDL_Event& event);
private:
    SDL_Texture* pauseButtonTexture = nullptr;
    SDL_Texture* pauseButtonHoverTexture = nullptr;

    int selectedPauseItem = 0;
    SDL_Rect resumeButtonRect{0, 0, 0, 0};
    SDL_Rect mainMenuButtonRect{0, 0, 0, 0};
    bool isResumeHovered = false;
    bool isMainMenuHovered = false;

    GameState currentState = GameState::MENU;
    int selectedMenuItem = 0;

    void renderMenu();
    void handleMenuInput(SDL_Event& event);
    void startGame();
    void pauseGame();
    void resumeGame();
    void gameOver();


    SDL_Rect pauseButtonRect;
    bool isPauseButtonHovered = false;

    void renderPauseButton();
    bool isMouseOverPauseButton(int x, int y);


    int score = 0;
    int highScore = 0;
    int ghostScore = 0;
    int fruitsScore = 0;
    int lives = 3;
    int currentLevel = 1;

    FruitManager fruitManager;
    int eloryIndex = 0;
    std::string currentSiren = "";
    bool bigDotVisible = true;
    Uint32 lastBlinkTime = 0;
    const Uint32 BLINK_INTERVAL = 500;
    bool levelComplete = false;
    int totalDots = 0;
    Direction currentDir = STOP;
    std::array<Uint32, 7> cycleTimes = GameRules::GHOST_CYCLES;
    Uint32 frightenedTime = GameRules::getFrightenedTime(currentLevel);
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

    WindowManager windowManager;
    TextureManager* textureManager;
    Map* gameMap;
    Pacman* pacman;
    Direction nextDir = STOP;

    void renderPauseMenu();

    void resetGame();

    bool isGhostScoreShowing = false;
    Uint32 ghostScoreFreezeStart = 0;
    const Uint32 GHOST_SCORE_FREEZE_DURATION = 500;

    SDL_Texture *renderText(const std::string &text, SDL_Color color, TTF_Font *customFont);

    void renderTextCentered(const std::string &text, int y, SDL_Color color, TTF_Font *customFont);

    Ghost *createGhost(GhostType type, const std::vector<std::string> &texturePaths);

    std::vector<std::string> getGhostTextures(const std::string &ghostName);

    void saveHighScore();

    void loadHighScore();
};
