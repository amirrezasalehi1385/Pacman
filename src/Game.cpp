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

bool pacmanDied;

bool Game::init(const std::string& title, int w, int h) {
    if(!windowManager.init(title.c_str(), w, h))
        return false;
    windowManager.loadSound("chomp", "assets/Sounds/pacman_chomp.wav");
    windowManager.loadSound("frightened", "assets/Sounds/pacman-ghostfrightened.wav");
    windowManager.loadSound("eaten", "assets/Sounds/pacman_eatghost.wav");
    windowManager.loadSound("death", "assets/Sounds/pacman_death.wav");
    windowManager.loadSound("beginning", "assets/Sounds/pacman_beginning.wav");

    gameStartTime = SDL_GetTicks();
    gameStartTime = SDL_GetTicks();
    modeStartTime = gameStartTime;
    cycleStarted = true;
    currentMode = SCATTER;
    cycleIndex = 0;




    windowManager.playSound("beginning");
    textureManager = new TextureManager(windowManager.getRenderer());

    backgroundTexture = textureManager->loadTexture("assets/map/background.png");

    gameMap = new Map(textureManager, windowManager.getRenderer());
    gameMap->loadLevel1();

    pacman = new Pacman(13 * 16 + 3, 25 * 16 + 11, 26, 26);
    pacman->setWindowManager(&windowManager);
    pacman->loadTextures(textureManager, "assets/Pacman/2.png","assets/Pacman/1.png", "assets/Pacman/0.png");

    blinky = new Blinky(14, 12, 28, 28);
    std::vector<std::string> blinkyTextures = {
            "assets/ghost/eyeUp.png",
            "assets/ghost/eyeDown.png",
            "assets/ghost/eyeLeft.png",
            "assets/ghost/eyeRight.png",
            "assets/ghost/Blinky.png",
            "assets/ghost/Blinky2.png",
            "assets/ghost/frightened.png",
            "assets/ghost/frightened2.png",
            "assets/ghost/endFrightening.png",
            "assets/ghost/endFrightening2.png"
    };
    blinky->setWindowManager(&windowManager);
    blinky->loadTextures(textureManager, blinkyTextures);

    blinky->loadTargetTexture(windowManager.getRenderer(), "assets/blinky_target.png");

    pinky = new Pinky(14, 14, 28, 28);
    std::vector<std::string> pinkyTextures = {
            "assets/ghost/eyeUp.png",
            "assets/ghost/eyeDown.png",
            "assets/ghost/eyeLeft.png",
            "assets/ghost/eyeRight.png",
            "assets/ghost/Pinky.png",
            "assets/ghost/Pinky2.png",
            "assets/ghost/frightened.png",
            "assets/ghost/frightened2.png",
            "assets/ghost/endFrightening.png",
            "assets/ghost/endFrightening2.png"
    };
    pinky->setWindowManager(&windowManager);

    pinky->loadTextures(textureManager, pinkyTextures);
    pinky->loadTargetTexture(windowManager.getRenderer(), "assets/pinky_target.png");

    inky = new Inky(12,14,28,28);
    std::vector<std::string> InkyTextures = {
            "assets/ghost/eyeUp.png",
            "assets/ghost/eyeDown.png",
            "assets/ghost/eyeLeft.png",
            "assets/ghost/eyeRight.png",
            "assets/ghost/Inky.png",
            "assets/ghost/Inky2.png",
            "assets/ghost/frightened.png",
            "assets/ghost/frightened2.png",
            "assets/ghost/endFrightening.png",
            "assets/ghost/endFrightening2.png"
    };
    inky->setWindowManager(&windowManager);

    inky->loadTextures(textureManager, InkyTextures);
    inky->loadTargetTexture(windowManager.getRenderer(), "assets/inky_target.png");
    clyde = new Clyde(16,14,28,28);
    std::vector<std::string> clydeTextures = {
            "assets/ghost/eyeUp.png",
            "assets/ghost/eyeDown.png",
            "assets/ghost/eyeLeft.png",
            "assets/ghost/eyeRight.png",
            "assets/ghost/Clyde.png",
            "assets/ghost/Clyde2.png",
            "assets/ghost/frightened.png",
            "assets/ghost/frightened2.png",
            "assets/ghost/endFrightening.png",
            "assets/ghost/endFrightening2.png"

    };
    clyde->setWindowManager(&windowManager);

    clyde->loadTextures(textureManager, clydeTextures);
    clyde->loadTargetTexture(windowManager.getRenderer(), "assets/clyde_target.png");

    for(int i = 1; i < 12; i++) {
        std::string path = "assets/Pacman/pacman_death_" + std::to_string(i) + ".png";
        SDL_Surface* surface = IMG_Load(path.c_str());
        if(!surface) {
            printf("Failed to load %s: %s\n", path.c_str(), IMG_GetError());
            continue;
        }
        SDL_Texture* tex = SDL_CreateTextureFromSurface(windowManager.getRenderer(), surface);
        SDL_FreeSurface(surface);
        if(tex) pacmanDeathTextures.push_back(tex);
    }

    isRunning = true;
    return true;
}

void Game::handlePacmanDeath() {
    pacmanDied = true;

    // پخش صدای مرگ
    windowManager.playSound("death");

    SDL_Delay(500); // صبر کوتاه برای شروع صدا

    const int frameCount = pacmanDeathTextures.size();
    const int frameDelay = 150; // ms

    for(int i = 0; i < frameCount; i++) {
        // صفحه سیاه
        SDL_SetRenderDrawColor(windowManager.getRenderer(), 0, 0, 0, 255);
        SDL_RenderClear(windowManager.getRenderer());

        // انیمیشن مرگ Pacman
        SDL_Rect dst = pacman->rect;
        SDL_RenderCopy(windowManager.getRenderer(), pacmanDeathTextures[i], nullptr, &dst);

        windowManager.present();
        SDL_Delay(frameDelay);
    }

    // بعد از آخرین فریم، مستقیم بازی رو ببند
    quit();
}





void Game::render() {
    windowManager.clear();

    if (backgroundTexture) {
        SDL_Rect dst = {0, 0, 448, 576};
        SDL_RenderCopy(windowManager.getRenderer(), backgroundTexture, nullptr, &dst);
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
    const int FPS = 144;            // اینو هر چقدر میخوای بالا ببر
    const int frameDelay = 1000 / FPS;

    const float fixedDelta = 1.0f / 60.0f;   // آپدیت با 60 بار در ثانیه
    float accumulator = 0;

    Uint32 lastTime = SDL_GetTicks();

    while(isRunning) {
        Uint32 now = SDL_GetTicks();
        float delta = (now - lastTime) / 1000.0f;
        lastTime = now;

        accumulator += delta;

        handleEvents();

        // update با نرخ ثابت (60 بار در ثانیه)
        while (accumulator >= fixedDelta) {
            update();               // سرعت ثابت
            accumulator -= fixedDelta;
        }

        render();

        int frameTime = SDL_GetTicks() - now;
        if(frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }
}

void Game::update() {
    Uint32 now = SDL_GetTicks();
    if (!pacman->isAlive) {
        handlePacmanDeath();
        return;
    }
    pacman->update();
    pacman->move(gameMap, speed);
    blinky->checkCollisionWithPacman(pacman);
    pinky->checkCollisionWithPacman(pacman);
    inky->checkCollisionWithPacman(pacman);
    clyde->checkCollisionWithPacman(pacman);
    if (pacman->ateBigDot) {
        pacman->ateBigDot = false;
        frightenedUntil = now + 6000;
        if (now < frightenedUntil) {
            windowManager.playSound("frightened");
        }

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
        windowManager.playSound("eaten");
        blinky->ghostEaten = false;
    }
    if(pinky->ghostEaten && pinky->getState() == FRIGHTENED) {
        windowManager.playSound("eaten");
        pinky->setState(EATEN);
        pinky->ghostEaten = false;
    }
    if(inky->ghostEaten && inky->getState() == FRIGHTENED) {
        windowManager.playSound("eaten");

        inky->setState(EATEN);
        inky->ghostEaten = false;
    }
    if(clyde->ghostEaten && clyde->getState() == FRIGHTENED) {
        windowManager.playSound("eaten");

        clyde->setState(EATEN);
        clyde->ghostEaten = false;
    }


    if (frightenedUntil != 0) {
        Uint32 remaining = frightenedUntil - now;

        if (remaining <= 2000) {
            blinky->endOfFrightening = true;
            pinky->endOfFrightening = true;
            inky->endOfFrightening = true;
            clyde->endOfFrightening = true;
        }

        if (remaining == 0 || now > frightenedUntil) {
            frightenedUntil = 0;

            if(blinky->getState() == FRIGHTENED) blinky->setMode(currentMode);
            if(pinky->getState() == FRIGHTENED) pinky->setMode(currentMode);
            if(inky->getState() == FRIGHTENED) inky->setMode(currentMode);
            if(clyde->getState() == FRIGHTENED) clyde->setMode(currentMode);

            blinky->endOfFrightening = false;
            pinky->endOfFrightening = false;
            inky->endOfFrightening = false;
            clyde->endOfFrightening = false;
        }
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

    if(blinky->getState() == SCATTER || blinky->getState() == CHASE){
        blinky->setMode(currentMode);
    }
    if(pinky->getState() == SCATTER || pinky->getState() == CHASE){
        pinky->setMode(currentMode);
    }
    if(inky->getState() == SCATTER || inky->getState() == CHASE){
        inky->setMode(currentMode);
    }
    if(clyde->getState() == SCATTER || clyde->getState() == CHASE){
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





