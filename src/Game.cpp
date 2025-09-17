#include "../include/Game.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "../include/Map.h"
#include "../include/Pacman.h"
#include "../include/Blinky.h"
#include "../include/Direction.h"
#include "../include/Blinky.h"
#include "../include/Pathfinder.h"
#include "../include/Pinky.h"
#include "../include/Inky.h"
#include "../include/Clyde.h"


Ghost* ghost;
Game::Game() {
    textureManager = nullptr;
}
SDL_Texture* backgroundTexture = nullptr;

Direction currentDir = STOP;
Direction nextDir = STOP;


Map* gameMap;
Pacman* pacman;

Blinky* blinky;
Pinky* pinky;
Inky* inky;
Clyde* clyde;


Game::~Game() {
    if(pacman) { pacman->cleanup(); delete pacman; }
    if(textureManager) delete textureManager;
    if (backgroundTexture) {
        SDL_DestroyTexture(backgroundTexture);
        backgroundTexture = nullptr;
    }

}

bool Game::init(const std::string& title, int w, int h) {
    gameStartTime = SDL_GetTicks();
    modeStartTime = gameStartTime;
    cycleStarted = true;
    currentMode = SCATTER;
    cycleIndex = 0;    if(!windowManager.init(title.c_str(), w, h)) return false;
    textureManager = new TextureManager(windowManager.getRenderer());

    backgroundTexture = textureManager->loadTexture("assets/map/background.png");

    gameMap = new Map(textureManager, windowManager.getRenderer());
    gameMap->loadLevel1();

    pacman = new Pacman(13 * 16 + 3, 25 * 16 + 11, 26, 26);
    pacman->loadTextures(textureManager, "assets/Pacman/2.png","assets/Pacman/1.png", "assets/Pacman/0.png");

    blinky = new Blinky(14, 12, 28, 28);
    std::vector<std::string> blinkyTextures = {
            "assets/ghost/eyeUp.png",
            "assets/ghost/eyeDown.png",
            "assets/ghost/eyeLeft.png",
            "assets/ghost/eyeRight.png",
            "assets/ghost/Blinky.png",
            "assets/ghost/Blinky2.png",
            "assets/ghost/frightened.png"
    };

    blinky->loadTextures(textureManager, blinkyTextures);

//    blinky->loadTargetTexture(windowManager.getRenderer(), "assets/blinky_target.png");

    pinky = new Pinky(14, 14, 28, 28);
    std::vector<std::string> pinkyTextures = {
            "assets/ghost/eyeUp.png",
            "assets/ghost/eyeDown.png",
            "assets/ghost/eyeLeft.png",
            "assets/ghost/eyeRight.png",
            "assets/ghost/Pinky.png",
            "assets/ghost/Pinky2.png",
            "assets/ghost/frightened.png"
    };

    pinky->loadTextures(textureManager, pinkyTextures);
//    pinky->loadTargetTexture(windowManager.getRenderer(), "assets/pinky_target.png");

    inky = new Inky(12,14,28,28);
    std::vector<std::string> InkyTextures = {
            "assets/ghost/eyeUp.png",
            "assets/ghost/eyeDown.png",
            "assets/ghost/eyeLeft.png",
            "assets/ghost/eyeRight.png",
            "assets/ghost/Inky.png",
            "assets/ghost/Inky2.png",
            "assets/ghost/frightened.png"
    };

    inky->loadTextures(textureManager, InkyTextures);
//    inky->loadTargetTexture(windowManager.getRenderer(), "assets/inky_target.png");
    clyde = new Clyde(16,14,28,28);
    std::vector<std::string> clydeTextures = {
            "assets/ghost/eyeUp.png",
            "assets/ghost/eyeDown.png",
            "assets/ghost/eyeLeft.png",
            "assets/ghost/eyeRight.png",
            "assets/ghost/Clyde.png",
            "assets/ghost/Clyde2.png",
            "assets/ghost/frightened.png"
    };

    clyde->loadTextures(textureManager, clydeTextures);
//    clyde->loadTargetTexture(windowManager.getRenderer(), "assets/clyde_target.png");
    isRunning = true;
    return true;
}




void Game::render() {
    windowManager.clear();

    if (backgroundTexture) {
        SDL_Rect dst = {0, 0, 448, 576};
//        SDL_RenderCopy(windowManager.getRenderer(), backgroundTexture, nullptr, &dst);
    }

    gameMap->render();

    pacman->render(windowManager.getRenderer());
    blinky->render(windowManager.getRenderer());
    pinky->render(windowManager.getRenderer());
    inky->render(windowManager.getRenderer());
    clyde->render(windowManager.getRenderer());

    windowManager.present();
}


void Game::run() {
    const int FPS = 60;
    const int frameDelay = 1000 / FPS;
    Uint32 frameStart;
    int frameTime;

    while(isRunning) {
        frameStart = SDL_GetTicks();

        handleEvents();
        update();
        render();

        frameTime = SDL_GetTicks() - frameStart;
        if(frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }
}

void Game::update() {
    pacman->update();
    pacman->move(gameMap, speed);
    blinky->checkCollisionWithPacman(pacman);
    pinky->checkCollisionWithPacman(pacman);
    inky->checkCollisionWithPacman(pacman);
    clyde->checkCollisionWithPacman(pacman);
    Uint32 now = SDL_GetTicks();

    if (pacman->ateBigDot) {
        pacman->ateBigDot = false;
        frightenedUntil = now + 6000;

        if(blinky->getState() != WAIT && blinky->getState() != EXIT)
            blinky->setState(FRIGHTENED);
        if(pinky->getState() != WAIT && pinky->getState() != EXIT)
            pinky->setState(FRIGHTENED);
        if(inky->getState() != WAIT && inky->getState() != EXIT)
            inky->setState(FRIGHTENED);
        if(clyde->getState() != WAIT && clyde->getState() != EXIT)
            clyde->setState(FRIGHTENED);
    }
    if(blinky->ghostEaten && blinky->getState() == FRIGHTENED) {
        blinky->setState(EATEN);
        blinky->ghostEaten = false;
        blinky->eatenUntil = now + 7000; // 7 ثانیه
    }
    if(pinky->ghostEaten && pinky->getState() == FRIGHTENED) {
        pinky->setState(EATEN);
        pinky->ghostEaten = false;
        pinky->eatenUntil = now + 7000;
    }
    if(inky->ghostEaten && inky->getState() == FRIGHTENED) {
        inky->setState(EATEN);
        inky->ghostEaten = false;
        inky->eatenUntil = now + 7000;
    }
    if(clyde->ghostEaten && clyde->getState() == FRIGHTENED) {
        clyde->setState(EATEN);
        clyde->ghostEaten = false;
        clyde->eatenUntil = now + 7000;
    }

// وقتی زمان EATEN تموم شد، ghost برگرده به حالت عادی (mode)
//    if(blinky->getState() == EATEN && now > blinky->eatenUntil) {
//        blinky->setMode(currentMode);
//    }
//    if(pinky->getState() == EATEN && now > pinky->eatenUntil) {
//        pinky->setMode(currentMode);
//    }
//    if(inky->getState() == EATEN && now > inky->eatenUntil) {
//        inky->setMode(currentMode);
//    }
//    if(clyde->getState() == EATEN && now > clyde->eatenUntil) {
//        clyde->setMode(currentMode);
//    }


    // وقتی زمان frightened تموم شد، حالت ghost ها برمی‌گرده به currentMode
    if (frightenedUntil != 0 && now > frightenedUntil) {
        frightenedUntil = 0;

        if(blinky->getState() == FRIGHTENED) blinky->setMode(currentMode);
        if(pinky->getState() == FRIGHTENED) pinky->setMode(currentMode);
        if(inky->getState() == FRIGHTENED) inky->setMode(currentMode);
        if(clyde->getState() == FRIGHTENED) clyde->setMode(currentMode);
    }

    int pelletsEaten = pacman->getDotsEaten();
    if(pelletsEaten >= 10 && !inky->readyToExit) inky->readyToExit = true;
    if(pelletsEaten >= 20 && !pinky->readyToExit) pinky->readyToExit = true;
    if(pelletsEaten >= 30 && !clyde->readyToExit) clyde->readyToExit = true;

    if(cycleStarted && cycleIndex < cycleTimes.size()) {
        if(now - modeStartTime >= cycleTimes[cycleIndex]) {
            currentMode = (currentMode == SCATTER) ? CHASE : SCATTER;
            modeStartTime = now;
            cycleIndex++;
        }
    }

    // فقط وقتی حالت FRIGHTENED نیستن، mode بگیرن
    if(blinky->getState() != EXIT && blinky->getState() != WAIT && blinky->getState() != FRIGHTENED && blinky->getState() != EATEN){
        blinky->setMode(currentMode);
    }
    if(pinky->getState() != EXIT && pinky->getState() != WAIT && pinky->getState() != FRIGHTENED && blinky->getState() != EATEN){
        pinky->setMode(currentMode);
    }
    if(inky->getState() != EXIT && inky->getState() != WAIT && inky->getState() != FRIGHTENED && blinky->getState() != EATEN){
        inky->setMode(currentMode);
    }
    if(clyde->getState() != EXIT && clyde->getState() != WAIT && clyde->getState() != FRIGHTENED && blinky->getState() != EATEN){
        clyde->setMode(currentMode);
    }

    blinky->update(*pacman, *gameMap);
    inky->update(*pacman, *blinky, *gameMap);
    pinky->update(*pacman, *gameMap);
    clyde->update(*pacman, *gameMap);
}



void Game::quit() {
    isRunning = false;
}

void Game::handleEvents() {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT) {
            quit();
        }
        pacman->handleInput(event);
    }
}





