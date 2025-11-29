#include "../include/Game.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "../include/Blinky.h"
#include "../include/Pinky.h"
#include "../include/Inky.h"
#include "../include/Clyde.h"
#include <SDL2/SDL_ttf.h>


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
    if(font) {
        TTF_CloseFont(font);
        font = nullptr;
    }

    if(pacman) { pacman->cleanup(); delete pacman; }
    if(textureManager) delete textureManager;
    if (backgroundTexture) {
        SDL_DestroyTexture(backgroundTexture);
        backgroundTexture = nullptr;
    }

    for(auto tex : livesTextures) {
        if(tex) SDL_DestroyTexture(tex);
    }
    livesTextures.clear();
}


void Game::loadSounds() {
        windowManager.loadSound("chomp", "assets/Sounds/pacman_chomp.wav");
        windowManager.loadSound("frightened", "assets/Sounds/pacman-ghostfrightened.wav");
        windowManager.loadSound("eaten", "assets/Sounds/pacman_eatghost.wav");
        windowManager.loadSound("death", "assets/Sounds/pacman_death.wav");
        windowManager.loadSound("beginning", "assets/Sounds/pacman_beginning.wav");
        windowManager.loadSound("returningToHouse" , "assets/Sounds/pacman-ghostRenturnToHome.wav");
        windowManager.loadSound("siren", "assets/Sounds/pacman-ghostSiren.wav");
};
bool Game::init(const std::string& title, int w, int h) {
    if(!windowManager.init(title.c_str(), w, h))
        return false;
    loadSounds();
    gameStartTime = SDL_GetTicks();
    modeStartTime = gameStartTime;
    cycleStarted = true;
    currentMode = SCATTER;
    cycleIndex = 0;
    isReady = false;
    readyStartTime = SDL_GetTicks();

    if (TTF_Init() != 0) {
        SDL_Log("TTF_Init Error: %s", TTF_GetError());
        return false;
    }

    font = TTF_OpenFont("assets/fonts/Emulogic-zrEw.ttf", 24);
    if(!font) {
        SDL_Log("Failed to load font: %s", TTF_GetError());
        return false;
    }


    windowManager.playSound("beginning");
    textureManager = new TextureManager(windowManager.getRenderer());

//    backgroundTexture = textureManager->loadTexture("assets/map/background.png");

    gameMap = new Map(textureManager, windowManager.getRenderer());
    gameMap->loadLevel1();

    pacman = new Pacman(13 * 16 + 3, 25 * 16 + 11, 26, 26);
    pacman->setWindowManager(&windowManager);
    pacman->loadTextures(textureManager, "assets/Pacman/0.png","assets/Pacman/1.png", "assets/Pacman/2.png");

    blinky = new Blinky();
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


    pinky = new Pinky();
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

    inky = new Inky();
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
    clyde = new Clyde();
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

//    loadTargetTexture();
    ghosts.push_back(blinky);
    ghosts.push_back(pinky);
    ghosts.push_back(inky);
    ghosts.push_back(clyde);

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
    for(int i = 0; i < 3; i++) {
        SDL_Texture* lifeTex = textureManager->loadTexture("assets/Pacman/1.png");
        if(lifeTex) livesTextures.push_back(lifeTex);
    }

    lives = 3;

    isRunning = true;
    return true;

}

void Game::loadTargetTexture(){
    clyde->loadTargetTexture(windowManager.getRenderer(), "assets/clyde_target.png");
    pinky->loadTargetTexture(windowManager.getRenderer(), "assets/pinky_target.png");
    blinky->loadTargetTexture(windowManager.getRenderer(), "assets/blinky_target.png");
    inky->loadTargetTexture(windowManager.getRenderer(), "assets/inky_target.png");

}
void Game::resetPacmanPosition() {
    pacman->setPosition(13 * 16 + 3, 25 * 16 + 11);
    pacman->isAlive = true;
    currentDir = STOP;
    nextDir = STOP;
    pacman->setDirection(STOP);
    pacman->setNextDirection(STOP);
}

void Game::handlePacmanDeath() {
    windowManager.stopSound("siren"); // قطع صدای سایرن
    windowManager.playSound("death");
    isReady = false;
    readyStartTime = SDL_GetTicks();

    blinky->readyToExit = false;
    pinky->readyToExit = false;
    inky->readyToExit = false;
    clyde->readyToExit = false;

    modeStartTime = SDL_GetTicks();
    cycleStarted = true;
    cycleIndex = 0;
    currentMode = SCATTER;

    const int frameCount = pacmanDeathTextures.size();
    const int frameDelay = 150;

    for(int i = 0; i < frameCount; i++) {
        SDL_SetRenderDrawColor(windowManager.getRenderer(), 0, 0, 0, 255);
        SDL_RenderClear(windowManager.getRenderer());

        SDL_Rect dst = pacman->rect;
        SDL_RenderCopy(windowManager.getRenderer(), pacmanDeathTextures[i], nullptr, &dst);

        windowManager.present();
        SDL_Delay(frameDelay);
    }

    lives--;

    if(lives > 0) {

        SDL_Delay(500);

        frightenedUntil = 0;
        if(blinky->getState() == FRIGHTENED) blinky->setMode(currentMode);
        if(pinky->getState() == FRIGHTENED) pinky->setMode(currentMode);
        if(inky->getState() == FRIGHTENED) inky->setMode(currentMode);
        if(clyde->getState() == FRIGHTENED) clyde->setMode(currentMode);

        blinky->endOfFrightening = false;
        pinky->endOfFrightening = false;
        inky->endOfFrightening = false;
        clyde->endOfFrightening = false;

        for(auto g : ghosts){
            g->reset();
        }

        isReady = false;
        readyStartTime = SDL_GetTicks();
        modeStartTime = SDL_GetTicks();
        cycleStarted = true;
        cycleIndex = 0;
        currentMode = SCATTER;

        blinky->readyToExit = false;
        pinky->readyToExit = false;
        inky->readyToExit = false;
        clyde->readyToExit = false;

        resetPacmanPosition();
    }else {
        quit();
    }
}


void Game::renderLives() {
    int startX = 2;
    int startY = 576 - 29;
    int spacing = 32;

    for(int i = 0; i < lives; i++) {
        if(i < livesTextures.size()) {
            SDL_Rect lifeRect = {
                    startX + i * spacing,
                    startY,
                    26,
                    26
            };
            SDL_RenderCopy(windowManager.getRenderer(), livesTextures[i], nullptr, &lifeRect);
        }
    }
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
    renderLives();
    renderScore();
    windowManager.present();
}
void Game::renderScore() {
    if (!font) return;
    SDL_Color color = {255, 255, 255, 255};
    std::string scoreText = "Score: " + std::to_string(getScore());
    SDL_Surface* surface = TTF_RenderText_Solid(font, scoreText.c_str(), color);
    if(!surface) return;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(windowManager.getRenderer(), surface);
    SDL_FreeSurface(surface);
    SDL_Rect dstRect = {8, 8, 100, 20};
    SDL_RenderCopy(windowManager.getRenderer(), texture, nullptr, &dstRect);
    SDL_DestroyTexture(texture);
}


void Game::run() {
    const int FPS = 60;
    const int frameDelay = 1000 / FPS;
    const float fixedDelta = 1.0f / 60.0f;
    float accumulator = 0;
    Uint32 lastTime = SDL_GetTicks();

    while(isRunning) {
        Uint32 now = SDL_GetTicks();
        float delta = (now - lastTime) / 1000.0f;
        lastTime = now;
        accumulator += delta;

        handleEvents();

        while (accumulator >= fixedDelta) {
            update();
            accumulator -= fixedDelta;
        }

        if(!isReady) {
            if(windowManager.isSoundPlaying("siren")) {
                windowManager.stopSound("siren");
            }

            if(SDL_GetTicks() - readyStartTime >= readyDuration) {
                isReady = true;
                blinky->readyToExit = true;
                modeStartTime = SDL_GetTicks();
                cycleStarted = true;
                windowManager.playSound("siren", true);
            }
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
            renderLives();
            renderScore();

            // متن READY
            SDL_Color color = {255, 255, 0, 255};
            std::string text = "READY!";
            SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
            if(surface) {
                SDL_Texture* texture = SDL_CreateTextureFromSurface(windowManager.getRenderer(), surface);
                SDL_FreeSurface(surface);

                int texW, texH;
                SDL_QueryTexture(texture, nullptr, nullptr, &texW, &texH);
                SDL_Rect dstRect = { (448 - texW)/2, (576 - texH)/2 + 36, texW, texH };
                SDL_RenderCopy(windowManager.getRenderer(), texture, nullptr, &dstRect);
                SDL_DestroyTexture(texture);
            }

            windowManager.present();
        }
        else {
            render();
        }
    }
}


void Game::update() {
    setScore((pacman->getDotsEaten() * 10) + (pacman->getBigDotsEaten() * 50));
    Uint32 now = SDL_GetTicks();

    if (!pacman->isAlive) {
        handlePacmanDeath();
        return;
    }

    if(isReady) {
        pacman->update();
        pacman->move(gameMap, speed);
    }

    if(isReady) {
        blinky->checkCollisionWithPacman(pacman);
        pinky->checkCollisionWithPacman(pacman);
        inky->checkCollisionWithPacman(pacman);
        clyde->checkCollisionWithPacman(pacman);
    }

    if(pacman->ateBigDot) {
        pacman->ateBigDot = false;
        frightenedUntil = SDL_GetTicks() + frightenedTimes[currentFrightenedIndex];
        if(SDL_GetTicks() < frightenedUntil) {
            windowManager.playSound("frightened");
        }
        for(auto g : ghosts) {
            if(g->getState() != WAIT && g->getState() != EXIT && g->getState() != EATEN) {
                g->setState(FRIGHTENED);
                g->endOfFrightening = false;
            }
        }
        if(currentFrightenedIndex < frightenedTimes.size() - 1) {
            currentFrightenedIndex++;
        }
    }

    for(auto g : ghosts) {
        if(g->ghostEaten && g->getState() == FRIGHTENED){
            g->setState(EATEN);
            windowManager.playSound("eaten");
            g->ghostEaten = false;
        }
    }

    for(auto g : ghosts) {
        if(!g->ghostInGhostHouse() && g->getState() == EATEN) {
            if(!g->returningSoundPlaying) {
                windowManager.playSound("returningToHouse");
                g->returningSoundPlaying = true;
            }
        }
        else if(g->ghostInGhostHouse() && g->getState() == EATEN) {
            if(g->returningSoundPlaying) {
                windowManager.stopSound("returningToHouse");
                g->returningSoundPlaying = false;
            }
        }
    }

    if (frightenedUntil != 0) {
        if(windowManager.isSoundPlaying("siren")) {
            windowManager.stopSound("siren");
        }

        Uint32 remaining = frightenedUntil - now;

        if (remaining <= 2000) {
            for(auto g : ghosts) {
                g->endOfFrightening = true;
            }
        }

        if (remaining == 0 || now > frightenedUntil) {
            frightenedUntil = 0;
            for(auto g : ghosts) {
                if(g->getState() == FRIGHTENED) g->setMode(currentMode);
                g->endOfFrightening = false;
            }
        }
    }

    bool shouldPlaySiren = false;
    if(isReady && frightenedUntil == 0) {
        bool anyGhostEaten = false;
        for(auto g : ghosts) {
            if(g->getState() == EATEN) {
                anyGhostEaten = true;
                break;
            }
        }
        shouldPlaySiren = !anyGhostEaten;
    }

    if(shouldPlaySiren) {
        if(!windowManager.isSoundPlaying("siren")) {
            windowManager.playSound("siren", true);
        }
    } else {
        if(windowManager.isSoundPlaying("siren")) {
            windowManager.stopSound("siren");
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

    for(auto g : ghosts) {
        if(g->getState() == SCATTER || g->getState() == CHASE){
            g->setMode(currentMode);
        }
    }

    if(isReady) {
        blinky->update(*pacman, *gameMap);
        inky->update(*pacman, *blinky, *gameMap);
        pinky->update(*pacman, *gameMap);
        clyde->update(*pacman, *gameMap);
    }
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



