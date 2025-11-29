#include "Game.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Blinky.h"
#include "Pinky.h"
#include "Inky.h"
#include "Clyde.h"
#include "SoundManager.h"
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
    // فقط از SoundManager استفاده کن، windowManager رو حذف کن
    SoundManager::get().loadSound("chomp", "assets/Sounds/pacman_chomp.wav");
    SoundManager::get().loadSound("frightened", "assets/Sounds/pacman-ghostfrightened.wav");
    SoundManager::get().loadSound("eaten", "assets/Sounds/pacman_eatghost.wav");
    SoundManager::get().loadSound("death", "assets/Sounds/pacman_death.wav");
    SoundManager::get().loadSound("beginning", "assets/Sounds/pacman_beginning.wav");
    SoundManager::get().loadSound("returningToHouse", "assets/Sounds/pacman-ghostRenturnToHome.wav");
    SoundManager::get().loadSound("siren", "assets/Sounds/pacman-ghostSiren.wav");
}
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

    SoundManager::get().playOnce("beginning");
    textureManager = new TextureManager(windowManager.getRenderer());

    backgroundTexture = textureManager->loadTexture("assets/map/background.png");

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
            "assets/ghost/endFrightening2.png",
            "assets/scores/200.png",
            "assets/scores/400.png",
            "assets/scores/800.png",
            "assets/scores/1600.png"

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
            "assets/ghost/endFrightening2.png",
            "assets/scores/200.png",
            "assets/scores/400.png",
            "assets/scores/800.png",
            "assets/scores/1600.png"
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
            "assets/ghost/endFrightening2.png",
            "assets/scores/200.png",
            "assets/scores/400.png",
            "assets/scores/800.png",
            "assets/scores/1600.png"
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
            "assets/ghost/endFrightening2.png",
            "assets/scores/200.png",
            "assets/scores/400.png",
            "assets/scores/800.png",
            "assets/scores/1600.png"

    };
    clyde->setWindowManager(&windowManager);

    clyde->loadTextures(textureManager, clydeTextures);

    blinky->loadScoreTextures(textureManager,
                              "assets/scores/200.png",
                              "assets/scores/400.png",
                              "assets/scores/800.png",
                              "assets/scores/1600.png");

    pinky->loadScoreTextures(textureManager,
                             "assets/scores/200.png",
                             "assets/scores/400.png",
                             "assets/scores/800.png",
                             "assets/scores/1600.png");

    inky->loadScoreTextures(textureManager,
                            "assets/scores/200.png",
                            "assets/scores/400.png",
                            "assets/scores/800.png",
                            "assets/scores/1600.png");

    clyde->loadScoreTextures(textureManager,
                             "assets/scores/200.png",
                             "assets/scores/400.png",
                             "assets/scores/800.png",
                             "assets/scores/1600.png");


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
    SoundManager::get().stop("siren");
    SoundManager::get().stop("frightened");
    SoundManager::get().stop("returningToHouse");

    SoundManager::get().playOnce("death");

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

        for(auto g : ghosts) {
            if(g->getState() == EATEN) {
                g->setState(SCATTER);
                g->ghostEaten = false;
                g->returningSoundPlaying = false;
            }
            else if(g->getState() == FRIGHTENED) {
                g->setMode(currentMode);
            }
            g->endOfFrightening = false;
        }

        for(auto g : ghosts) {
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
        SoundManager::get().playOnce("beginning");
    } else {
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
            // صدای siren رو متوقف کن
            if(SoundManager::get().isPlaying("siren")) {
                SoundManager::get().stop("siren");
            }

            if(SDL_GetTicks() - readyStartTime >= readyDuration) {
                isReady = true;
                blinky->readyToExit = true;
                modeStartTime = SDL_GetTicks();
                cycleStarted = true;
                SoundManager::get().play("siren", -1);  // loop بی‌نهایت
            }

            // ... بقیه رندر کدها ...
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
    setScore((pacman->getDotsEaten() * GameRules::SMALL_DOTS_SCORE) + (pacman->getBigDotsEaten() * GameRules::BIG_DOTS_SCORE));
    Uint32 now = SDL_GetTicks();

    if (!pacman->isAlive) {
        handlePacmanDeath();
        return;
    }

    if (isReady) {
        pacman->update();
        pacman->move(gameMap, GameRules::PACMAN_SPEED);
    }

    // ========== Big Dot خورده شد ==========
    if (pacman->ateBigDot) {
        pacman->ateBigDot = false;
        ghostsEatenInThisFrightened = 0;

        frightenedUntil = SDL_GetTicks() + frightenedTimes[currentFrightenedIndex];

        // صدای siren رو قطع و frightened رو پخش کن
        SoundManager::get().stop("siren");
        SoundManager::get().play("frightened", -1);

        for (auto g : ghosts) {
            if (g->getState() != WAIT && g->getState() != EXIT && g->getState() != EATEN) {
                g->setState(FRIGHTENED);
                g->endOfFrightening = false;
            }
        }

        if (currentFrightenedIndex < frightenedTimes.size() - 1) {
            currentFrightenedIndex++;
        }
    }

    // ========== برخورد با روح‌ها ==========
    if (isReady) {
        for (auto g : ghosts) {
            if (g->checkCollisionWithPacman(pacman)) {
                if (g->getState() == FRIGHTENED && !g->isShowingScore()) {
                    int scoreValue;
                    switch (ghostsEatenInThisFrightened) {
                        case 0: scoreValue = 200;   break;
                        case 1: scoreValue = 400;  break;
                        case 2: scoreValue = 800; break;
                        case 3: scoreValue = 1600; break;
                        default: scoreValue = 200; break;
                    }
                    g->startShowingScore(scoreValue);
                    ghostsEatenInThisFrightened++;
                    SoundManager::get().playOnce("eaten");
                }
            }
        }
    }

    for (auto g : ghosts) {
        g->updateScoreDisplay();
    }

    // ========== پایان Frightened Mode ==========
    if (frightenedUntil != 0) {
        Uint32 remaining = frightenedUntil - now;

        if (remaining <= 2000) {
            for (auto g : ghosts) {
                g->endOfFrightening = true;
            }
        }

        if (remaining == 0 || now > frightenedUntil) {
            frightenedUntil = 0;
            ghostsEatenInThisFrightened = 0;

            // صدای frightened رو متوقف کن
            SoundManager::get().stop("frightened");

            for (auto g : ghosts) {
                if (g->getState() == FRIGHTENED) {
                    g->setMode(currentMode);
                }
                g->endOfFrightening = false;
            }
        }
    }

    // ========== صدای بازگشت روح‌ها ==========
    bool anyGhostReturning = false;
    for (auto g : ghosts) {
        if (g->canGotoGhostHouse && g->getState() == EATEN) {
            anyGhostReturning = true;
            if (!g->returningSoundPlaying) {
                SoundManager::get().play("returningToHouse", -1);
                g->returningSoundPlaying = true;
            }
            break; // فقط یکبار پخش کن
        }
    }

// اگر هیچ روحی در حال بازگشت نیست، صدا رو قطع کن
    if (!anyGhostReturning) {
        for (auto g : ghosts) {
            if (g->returningSoundPlaying) {
                SoundManager::get().stop("returningToHouse");
                g->returningSoundPlaying = false;
            }
        }
    }


    // ========== صدای Siren ==========
    bool shouldPlaySiren = false;
    if (isReady && frightenedUntil == 0) {
        bool anyGhostEaten = false;
        for (auto g : ghosts) {
            if (g->getState() == EATEN) {
                anyGhostEaten = true;
                break;
            }
        }
        shouldPlaySiren = !anyGhostEaten;
    }

    if (shouldPlaySiren) {
        if (!SoundManager::get().isPlaying("siren")) {
            SoundManager::get().play("siren", -1);
        }
    } else {
        if (SoundManager::get().isPlaying("siren")) {
            SoundManager::get().stop("siren");
        }
    }

    // ========== باقی update ==========
    int pelletsEaten = pacman->getDotsEaten();
    if (pelletsEaten >= 10 && !inky->readyToExit) inky->readyToExit = true;
    if (pelletsEaten >= 20 && !pinky->readyToExit) pinky->readyToExit = true;
    if (pelletsEaten >= 30 && !clyde->readyToExit) clyde->readyToExit = true;

    if (cycleStarted && cycleIndex < cycleTimes.size()) {
        if (now - modeStartTime >= cycleTimes[cycleIndex]) {
            currentMode = (currentMode == SCATTER) ? CHASE : SCATTER;
            modeStartTime = now;
            cycleIndex++;
        }
    }

    for (auto g : ghosts) {
        if (g->getState() == SCATTER || g->getState() == CHASE) {
            g->setMode(currentMode);
        }
    }

    if (isReady) {
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



