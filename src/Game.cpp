#include "Game.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Blinky.h"
#include "Pinky.h"
#include "Inky.h"
#include "Clyde.h"
#include "SoundManager.h"
#include <SDL2/SDL_ttf.h>
#include "GhostType.h"
#include <fstream>
#include <string>



Ghost* ghost;

SDL_Texture* backgroundTexture = nullptr;

Direction currentDir = STOP;
Direction nextDir = STOP;

Map* gameMap;
Pacman* pacman;

Blinky* blinky;
Pinky* pinky;
Inky* inky;
Clyde* clyde;

// 1. Constructor & Destructor
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
Game::Game() {
    textureManager = nullptr;
}

// 2. Initialization & Setup
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
    loadHighScore();
    if (TTF_Init() != 0) {
        SDL_Log("TTF_Init Error: %s", TTF_GetError());
        return false;
    }
    font = TTF_OpenFont("assets/fonts/Emulogic-zrEw.ttf", 24);
    if(!font) {
        SDL_Log("Failed to load font: %s", TTF_GetError());
        return false;
    }

    textureManager = new TextureManager(windowManager.getRenderer());


    gameMap = new Map(textureManager, windowManager.getRenderer());
    gameMap->loadLevel1();
    totalDots = gameMap->totalDots;
    pacman = new Pacman(13 * 16 + 3, 25 * 16 + 11, 26, 26);
    pacman->setWindowManager(&windowManager);
    pacman->loadTextures(textureManager, "assets/Pacman/0.png","assets/Pacman/1.png", "assets/Pacman/2.png");

    fruitManager.init(textureManager, windowManager.getRenderer());
    fruitManager.loadTextures(textureManager);
    fruitManager.reset(currentLevel);

    blinky = static_cast<Blinky *>(createGhost(BLINKY_TYPE, getGhostTextures("Blinky")));
    pinky = static_cast<Pinky *>(createGhost(PINKY_TYPE, getGhostTextures("Pinky")));
    inky = static_cast<Inky *>(createGhost(INKY_TYPE, getGhostTextures("Inky")));
    clyde = static_cast<Clyde *>(createGhost(CLYDE_TYPE, getGhostTextures("Clyde")));

    ghosts.push_back(blinky);
    ghosts.push_back(pinky);
    ghosts.push_back(inky);
    ghosts.push_back(clyde);


    pauseButtonTexture = textureManager->loadTexture("assets/ui/pause_button.png");
    pauseButtonHoverTexture = textureManager->loadTexture("assets/ui/pause_button_hover.png");


    if (!pauseButtonHoverTexture) pauseButtonHoverTexture = pauseButtonTexture;
    loadTargetTexture();
    fruitManager.loadTextures(textureManager);
    for(int i = 1; i < 12; i++) {
        std::string path = "assets/Pacman/pacman_death_" + std::to_string(i) + ".png";
        SDL_Surface* surface = IMG_Load(path.c_str());
        if(!surface) {
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
void Game::loadSounds() {
    SoundManager::get().loadSound("chomp", "assets/Sounds/pacman_chomp.wav");
    SoundManager::get().loadSound("frightened", "assets/Sounds/pacman-ghostfrightened.wav");
    SoundManager::get().loadSound("eaten", "assets/Sounds/pacman_eatghost.wav");
    SoundManager::get().loadSound("death", "assets/Sounds/pacman_death.wav");
    SoundManager::get().loadSound("beginning", "assets/Sounds/pacman_beginning.wav");
    SoundManager::get().loadSound("returningToHouse", "assets/Sounds/pacman-ghostRenturnToHome.wav");
    SoundManager::get().loadSound("siren", "assets/Sounds/pacman-ghostSiren.wav");
    SoundManager::get().loadSound("fruit", "assets/Sounds/pacman_eatfruit.wav");
    SoundManager::get().loadSound("siren1", "assets/Sounds/pacman-ghostSiren1.wav");
    SoundManager::get().loadSound("siren2", "assets/Sounds/pacman-ghostSiren2.wav");

}
void Game::loadTargetTexture(){
    clyde->loadTargetTexture(windowManager.getRenderer(), "assets/clyde_target.png");
    pinky->loadTargetTexture(windowManager.getRenderer(), "assets/pinky_target.png");
    blinky->loadTargetTexture(windowManager.getRenderer(), "assets/blinky_target.png");
    inky->loadTargetTexture(windowManager.getRenderer(), "assets/inky_target.png");

}

// 3. Game Loop & Main Flow
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

        if (currentState == GameState::PLAYING && isReady) {
            accumulator += delta;
        }

        handleEvents();

        if(currentState == GameState::MENU) {
            renderMenu();
            SDL_Delay(frameDelay);
            continue;
        }

        if(currentState == GameState::PAUSED) {
            renderGame();

            SDL_Color color = {255, 255, 0, 255};
            renderPauseMenu();
            windowManager.present();
            SDL_Delay(frameDelay);
            continue;
        }

        if(currentState == GameState::GAME_OVER) {
            renderGame();

            SDL_Color color = {255, 0, 0, 255};
            std::string text = "GAME OVER";
            SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
            if(surface) {
                SDL_Texture* texture = SDL_CreateTextureFromSurface(windowManager.getRenderer(), surface);
                SDL_FreeSurface(surface);
                int texW, texH;
                SDL_QueryTexture(texture, nullptr, nullptr, &texW, &texH);
                int screenWidth = GameRules::MAP_WIDTH_TILES * GameRules::TILE_SIZE;
                SDL_Rect dstRect = {(screenWidth - texW) / 2, 280, texW, texH};
                SDL_RenderCopy(windowManager.getRenderer(), texture, nullptr, &dstRect);
                SDL_DestroyTexture(texture);
            }

            windowManager.present();
            SDL_Delay(frameDelay);
            continue;
        }

        if (currentState == GameState::PLAYING && isReady) {
            while (accumulator >= fixedDelta) {
                update();
                accumulator -= fixedDelta;
            }
        }
        if(!isReady) {
            if(SDL_GetTicks() - readyStartTime >= readyDuration) {
                isReady = true;
                blinky->readyToExit = true;
                modeStartTime = SDL_GetTicks();
                cycleStarted = true;
                SoundManager::get().play("siren", -1);
            }

            renderGame();

            SDL_Color color = {255, 255, 0, 255};
            std::string text = "READY!";
            SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
            if(surface) {
                SDL_Texture* texture = SDL_CreateTextureFromSurface(windowManager.getRenderer(), surface);
                SDL_FreeSurface(surface);
                int texW, texH;
                SDL_QueryTexture(texture, nullptr, nullptr, &texW, &texH);
                texW = 7 * 16;
                int screenWidth = GameRules::MAP_WIDTH_TILES * GameRules::TILE_SIZE;
                SDL_Rect dstRect = { (screenWidth - texW) / 2, 20 * GameRules::TILE_SIZE - 5, texW, 26 };
                SDL_RenderCopy(windowManager.getRenderer(), texture, nullptr, &dstRect);
                SDL_DestroyTexture(texture);
            }

            windowManager.present();
        }
        else {
            renderGame();
            windowManager.present();
        }
    }
}
void Game::update() {
    if (isPlayingDeathAnimation) {
        return;
    }

    fruitManager.update(pacman->getDotsEaten() % 240, pacman->getHitbox(), fruitsScore);

    if(pacman->getDotsEaten() >= 240 * currentLevel && !levelComplete) {
        for(auto g : ghosts) {
            g->visible = false;
        }
        pacman->resetAnimation();
        levelComplete = true;
        handleLevelComplete();
        return;
    }


    updateScore();
    updatePacman();
    updateGhosts();
    updateFrightened();
}
void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            quit();
            return;
        }

        handleMouseEvents(event);
        handleKeyboardEvents(event);
    }
}


// 4. Update Sub-systems
void Game::updatePacman(){
    if (!pacman->isAlive) {
        handlePacmanDeath();
        return;
    }

    if (isReady && !isGhostScoreShowing) {
        pacman->update();
        pacman->move(gameMap, GameRules::PACMAN_SPEED);
    }

    if (pacman->ateBigDot) {
        pacman->ateBigDot = false;
        ghostsEatenInThisFrightened = 0;

        frightenedUntil = SDL_GetTicks() + frightenedTime;

        SoundManager::get().stop("siren");
        SoundManager::get().stop("siren1");
        SoundManager::get().stop("siren2");
        SoundManager::get().play("frightened", -1);

        for (auto g : ghosts) {
            if (g->getState() != WAIT && g->getState() != EXIT && g->getState() != EATEN) {
                g->setState(FRIGHTENED);
                g->endOfFrightening = false;
            }
        }
    }
}
void Game::updateGhosts(){
    if (!isReady) return;

    Uint32 now = SDL_GetTicks();
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
        }else if(SoundManager::get().isPlaying("siren1")) {
            SoundManager::get().stop("siren1");
        }else if(SoundManager::get().isPlaying("siren2")){
            SoundManager::get().stop("siren2");
        }
    }
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
        updateSirenSound();
        updateCruiseElroy();
        blinky->update(currentLevel,*pacman, *gameMap);
        inky->update(currentLevel,*pacman, *blinky, *gameMap);
        pinky->update(currentLevel,*pacman, *gameMap);
        clyde->update(currentLevel,*pacman, *gameMap);
    }

}
void Game::updateFrightened(){
    Uint32 now = SDL_GetTicks();

    if(isGhostScoreShowing) {
        for (auto g : ghosts) {
            if(g->isShowingScore()) {
                g->updateScoreDisplay();
            }
        }

        if(now - ghostScoreFreezeStart >= GHOST_SCORE_FREEZE_DURATION) {
            isGhostScoreShowing = false;
            pacman->isFrozenForGhostScore = false;
            for(auto g : ghosts) {
                g->isFrozenForScore = false;
            }
        }
        return;
    }

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
                    ghostScore += scoreValue;
                    SoundManager::get().playOnce("eaten");

                    isGhostScoreShowing = true;
                    ghostScoreFreezeStart = now;

                    pacman->isFrozenForGhostScore = true;
                    for(auto ghost : ghosts) {
                        ghost->isFrozenForScore = true;
                    }
                }
            }
        }

        if(!isGhostScoreShowing) {
            for (auto g : ghosts) {
                g->updateScoreDisplay();
            }
        }

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
                SoundManager::get().stop("frightened");

                for (auto g : ghosts) {
                    if (g->getState() == FRIGHTENED) {
                        g->setMode(currentMode);
                    }
                    g->endOfFrightening = false;
                }
            }
        }

        bool anyGhostReturning = false;
        for (auto g : ghosts) {
            if (g->canGotoGhostHouse && g->getState() == EATEN) {
                anyGhostReturning = true;
                if (!g->returningSoundPlaying) {
                    SoundManager::get().play("returningToHouse", -1);
                    g->returningSoundPlaying = true;
                }
                break;
            }
        }

        if (!anyGhostReturning) {
            for (auto g : ghosts) {
                if (g->returningSoundPlaying) {
                    SoundManager::get().stop("returningToHouse");
                    g->returningSoundPlaying = false;
                }
            }
        }
    }
}
void Game::updateScore(){
    int dotsScore = (pacman->getDotsEaten() * GameRules::SMALL_DOTS_SCORE) +
                    (pacman->getBigDotsEaten() * GameRules::BIG_DOTS_SCORE);
    int newScore = dotsScore + ghostScore + fruitsScore;

    static int lastExtraLifeScore = 0;
    if(newScore / 10000 > lastExtraLifeScore / 10000) {
        lives++;
        lastExtraLifeScore = newScore;
        SoundManager::get().playOnce("extra_life");
    }
    if (newScore > highScore) {
        highScore = newScore;
        saveHighScore();
    }
    setScore(newScore);
}
void Game::updateSirenSound() {
    std::string targetSiren;

    switch(eloryIndex) {
        case 0: targetSiren = "siren"; break;
        case 1: targetSiren = "siren1"; break;
        case 2: targetSiren = "siren2"; break;
        default: targetSiren = "siren"; break;
    }
    if(currentSiren != targetSiren) {
        if(!currentSiren.empty() && SoundManager::get().isPlaying(currentSiren)) {
            SoundManager::get().stop(currentSiren);
        }
        SoundManager::get().play(targetSiren, -1);
        currentSiren = targetSiren;
    }
}
void Game::updateCruiseElroy() {
    int dotsLeft = gameMap->totalDots - pacman->getDotsEaten();

    if(dotsLeft <= 20 && dotsLeft > 10) {
        eloryIndex = 1;
        blinky->setNormalSpeed(GameRules::GHOST_SPEED_CRUISE_ELROY_1);
    } else if(dotsLeft <= 10) {
        eloryIndex = 2;
        blinky->setNormalSpeed(GameRules::GHOST_SPEED_CRUISE_ELROY_2);
    } else {
        eloryIndex = 0;
        blinky->setNormalSpeed(GameRules::GHOST_SPEED_NORMAL);
    }
}

// 5. Ghost AI & Behavior
void Game::updateGhostSpeedForLevel() {
    float newSpeed = GameRules::getGhostSpeed(currentLevel);

    for(auto g : ghosts) {
        g->setNormalSpeed(newSpeed);
    }

    printf("Level %d - Ghost speed: %.1f\n", currentLevel, newSpeed);
}
void Game::updateFrightenedTimeForLevel() {
    Uint32 newTime = GameRules::getFrightenedTime(currentLevel);

    frightenedTime = newTime;
    printf("Level %d - Frightened time: %ums\n", currentLevel, newTime);
}

// 6. Rendering
void Game::renderGameStatic() {
    windowManager.clear();

    if(backgroundTexture) {
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
void Game::renderDebug(){
    backgroundTexture = textureManager->loadTexture("assets/map/background.png");
    for(auto g : ghosts){
        g->renderTarget(windowManager.getRenderer());
        g->renderHitbox(windowManager.getRenderer());

    }
    pacman->renderHitbox(windowManager.getRenderer());
}
void Game::renderScore() {
    if (!font) return;

    SDL_Color white = {255, 255, 255, 255};
    int tileSize = GameRules::TILE_SIZE;
    int screenWidth = GameRules::MAP_WIDTH_TILES * tileSize;

    TTF_Font* bigFont = TTF_OpenFont("assets/fonts/Emulogic-zrEw.ttf", 16); // فونت بزرگتر
    if(bigFont) {
        std::string highScoreLabel = "HIGH SCORE";
        SDL_Surface* labelSurface = TTF_RenderText_Solid(bigFont, highScoreLabel.c_str(), white);
        if(labelSurface) {
            SDL_Texture* labelTexture = SDL_CreateTextureFromSurface(windowManager.getRenderer(), labelSurface);
            int labelW, labelH;
            SDL_QueryTexture(labelTexture, nullptr, nullptr, &labelW, &labelH);

            SDL_Rect labelRect = {
                    (screenWidth - labelW) / 2,
                    0,
                    labelW,
                    16
            };
            SDL_RenderCopy(windowManager.getRenderer(), labelTexture, nullptr, &labelRect);
            SDL_DestroyTexture(labelTexture);
            SDL_FreeSurface(labelSurface);
        }
        TTF_CloseFont(bigFont);
    }

    std::string highScoreText = std::to_string(highScore);
    int digitHeight = 16;
    int digitSpacing = 4;

    int totalWidth = 0;
    for(char digit : highScoreText) {
        std::string digitStr(1, digit);
        int digitW, digitH;
        TTF_SizeText(font, digitStr.c_str(), &digitW, &digitH);
        float scale = static_cast<float>(digitHeight) / digitH;
        totalWidth += static_cast<int>(digitW * scale) + digitSpacing;
    }
    totalWidth -= digitSpacing;

    int startX = (screenWidth - totalWidth) / 2 - 8;
    int currentX = startX;
    int yPos = 16;

    for(char digit : highScoreText) {
        std::string digitStr(1, digit);
        int digitW, digitH;
        TTF_SizeText(font, digitStr.c_str(), &digitW, &digitH);

        SDL_Surface* digitSurface = TTF_RenderText_Solid(font, digitStr.c_str(), white);
        if(digitSurface) {
            SDL_Texture* digitTexture = SDL_CreateTextureFromSurface(windowManager.getRenderer(), digitSurface);
            SDL_FreeSurface(digitSurface);

            float scale = static_cast<float>(digitHeight) / digitH;
            int scaledWidth = static_cast<int>(digitW * scale);

            SDL_Rect digitRect = {currentX, yPos, scaledWidth, digitHeight};
            SDL_RenderCopy(windowManager.getRenderer(), digitTexture, nullptr, &digitRect);
            SDL_DestroyTexture(digitTexture);

            currentX += scaledWidth + digitSpacing;
        }
    }

    std::string scoreText = std::to_string(getScore());
    int fixedHeight = 16;
    int tileIndex = 3;

    for (char digit : scoreText) {
        std::string digitStr(1, digit);
        int digitW, digitH;
        TTF_SizeText(font, digitStr.c_str(), &digitW, &digitH);

        SDL_Surface* digitSurface = TTF_RenderText_Solid(font, digitStr.c_str(), white);
        if(!digitSurface) continue;

        SDL_Texture* digitTexture = SDL_CreateTextureFromSurface(windowManager.getRenderer(), digitSurface);
        SDL_FreeSurface(digitSurface);

        float scale = static_cast<float>(fixedHeight) / digitH;
        int scaledWidth = static_cast<int>(digitW * scale);
        int tileX = tileIndex * tileSize;
        int centeredX = tileX + (tileSize - scaledWidth) / 2;

        SDL_Rect digitRect = {centeredX, 16, scaledWidth, fixedHeight};
        SDL_RenderCopy(windowManager.getRenderer(), digitTexture, nullptr, &digitRect);
        SDL_DestroyTexture(digitTexture);
        tileIndex++;
    }
}
void Game::renderLives() {
    int startX = 2;
    int startY = 576 - 29;
    int spacing = 32;

    SDL_Texture* lifeTex = livesTextures.empty() ? nullptr : livesTextures[0];

    for(int i = 0; i < lives; i++) {
        if(lifeTex) {
            SDL_Rect lifeRect = { startX + i * spacing, startY, 26, 26 };
            SDL_RenderCopy(windowManager.getRenderer(), lifeTex, nullptr, &lifeRect);
        }
    }
}
void Game::renderGame() {
    windowManager.clear();

    if (backgroundTexture) {
        SDL_Rect dst = {0, 0, 448, 576};
        SDL_RenderCopy(windowManager.getRenderer(), backgroundTexture, nullptr, &dst);
    }

    gameMap->render();
    pacman->render(windowManager.getRenderer());

    if(GameRules::SHOW_DEBUG){
        renderDebug();
    }

    blinky->render(windowManager.getRenderer());
    pinky->render(windowManager.getRenderer());
    inky->render(windowManager.getRenderer());
    clyde->render(windowManager.getRenderer());
    fruitManager.render(windowManager.getRenderer());
    fruitManager.renderHUD(windowManager.getRenderer());
    renderLives();
    renderScore();

    if(currentState == GameState::PLAYING && isReady) {
        renderPauseButton();
    }
}
void Game::renderMenu() {
    windowManager.clear();

    if(backgroundTexture) {
        SDL_Rect dst = {0, 0, 448, 576};
        SDL_RenderCopy(windowManager.getRenderer(), backgroundTexture, nullptr, &dst);
    }

    SDL_Color titleColor = {255, 255, 0, 255};
    SDL_Color normalColor = {255, 255, 255, 255};
    SDL_Color selectedColor = {255, 255, 0, 255};

    std::string titleText = "PAC-MAN";
    SDL_Surface* titleSurface = TTF_RenderText_Solid(font, titleText.c_str(), titleColor);
    if(titleSurface) {
        SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(windowManager.getRenderer(), titleSurface);
        int texW, texH;
        SDL_QueryTexture(titleTexture, nullptr, nullptr, &texW, &texH);
        int screenWidth = GameRules::MAP_WIDTH_TILES * GameRules::TILE_SIZE;
        SDL_Rect titleRect = {(screenWidth - texW) / 2, 150, texW, texH};
        SDL_RenderCopy(windowManager.getRenderer(), titleTexture, nullptr, &titleRect);
        SDL_FreeSurface(titleSurface);
        SDL_DestroyTexture(titleTexture);
    }

    SDL_Color startColor = (selectedMenuItem == 0) ? selectedColor : normalColor;
    std::string startText = "START GAME";
    SDL_Surface* startSurface = TTF_RenderText_Solid(font, startText.c_str(), startColor);
    if(startSurface) {
        SDL_Texture* startTexture = SDL_CreateTextureFromSurface(windowManager.getRenderer(), startSurface);
        int texW, texH;
        SDL_QueryTexture(startTexture, nullptr, nullptr, &texW, &texH);
        int screenWidth = GameRules::MAP_WIDTH_TILES * GameRules::TILE_SIZE;
        SDL_Rect startRect = {(screenWidth - texW) / 2, 300, texW, texH};
        SDL_RenderCopy(windowManager.getRenderer(), startTexture, nullptr, &startRect);
        SDL_FreeSurface(startSurface);
        SDL_DestroyTexture(startTexture);
    }

    SDL_Color exitColor = (selectedMenuItem == 1) ? selectedColor : normalColor;
    std::string exitText = "EXIT";
    SDL_Surface* exitSurface = TTF_RenderText_Solid(font, exitText.c_str(), exitColor);
    if(exitSurface) {
        SDL_Texture* exitTexture = SDL_CreateTextureFromSurface(windowManager.getRenderer(), exitSurface);
        int texW, texH;
        SDL_QueryTexture(exitTexture, nullptr, nullptr, &texW, &texH);
        int screenWidth = GameRules::MAP_WIDTH_TILES * GameRules::TILE_SIZE;
        SDL_Rect exitRect = {(screenWidth - texW) / 2, 360, texW, texH};
        SDL_RenderCopy(windowManager.getRenderer(), exitTexture, nullptr, &exitRect);
        SDL_FreeSurface(exitSurface);
        SDL_DestroyTexture(exitTexture);
    }

    windowManager.present();
}
void Game::renderPauseMenu() {
    int screenWidth = GameRules::MAP_WIDTH_TILES * GameRules::TILE_SIZE;

    SDL_SetRenderDrawBlendMode(windowManager.getRenderer(), SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(windowManager.getRenderer(), 0, 0, 0, 160);
    SDL_Rect overlay = {0, 0, 448, 576};
    SDL_RenderFillRect(windowManager.getRenderer(), &overlay);
    SDL_SetRenderDrawBlendMode(windowManager.getRenderer(), SDL_BLENDMODE_NONE);

    if(font) {
        SDL_Color yellow = {255, 255, 0, 255};
        SDL_Surface* surf = TTF_RenderText_Solid(font, "PAUSED", yellow);
        if(surf) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(windowManager.getRenderer(), surf);
            int w, h; SDL_QueryTexture(tex, nullptr, nullptr, &w, &h);
            SDL_Rect r = {(screenWidth - w)/2, 180, w, h};
            SDL_RenderCopy(windowManager.getRenderer(), tex, nullptr, &r);
            SDL_DestroyTexture(tex); SDL_FreeSurface(surf);
        }
    }

    SDL_Color normal = {255, 255, 255, 255};
    SDL_Color selected = {255, 255, 0, 255};

    SDL_Color resumeColor = (selectedPauseItem == 0) ? selected : normal;
    if(font) {
        SDL_Surface* surf = TTF_RenderText_Solid(font, "RESUME", resumeColor);
        if(surf) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(windowManager.getRenderer(), surf);
            int w, h; SDL_QueryTexture(tex, nullptr, nullptr, &w, &h);
            SDL_Rect r = {(screenWidth - w)/2, 280, w, h};
            SDL_RenderCopy(windowManager.getRenderer(), tex, nullptr, &r);
            SDL_DestroyTexture(tex); SDL_FreeSurface(surf);
        }
    }

    // MAIN MENU
    SDL_Color menuColor = (selectedPauseItem == 1) ? selected : normal;
    if(font) {
        SDL_Surface* surf = TTF_RenderText_Solid(font, "MAIN MENU", menuColor);
        if(surf) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(windowManager.getRenderer(), surf);
            int w, h; SDL_QueryTexture(tex, nullptr, nullptr, &w, &h);
            SDL_Rect r = {(screenWidth - w)/2, 340, w, h};
            SDL_RenderCopy(windowManager.getRenderer(), tex, nullptr, &r);
            SDL_DestroyTexture(tex); SDL_FreeSurface(surf);
        }
    }
}
void Game::renderPauseButton() {
    if (!pauseButtonTexture) return;

    pauseButtonRect = {448 - 64, 8, 32, 32};

    SDL_Texture* texToRender = isPauseButtonHovered ? pauseButtonHoverTexture : pauseButtonTexture;

    SDL_RenderCopy(windowManager.getRenderer(), texToRender, nullptr, &pauseButtonRect);

    if (isPauseButtonHovered) {
        SDL_SetRenderDrawColor(windowManager.getRenderer(), 255, 255, 0, 255);
        SDL_RenderDrawRect(windowManager.getRenderer(), &pauseButtonRect);
        SDL_Rect thick = {pauseButtonRect.x - 1, pauseButtonRect.y - 1,
                          pauseButtonRect.w + 2, pauseButtonRect.h + 2};
        SDL_RenderDrawRect(windowManager.getRenderer(), &thick);
    }
}

// 7. Game State Management
void Game::startGame() {
    currentState = GameState::PLAYING;
    fruitManager.eatenFruits.clear();
    isReady = false;
    readyStartTime = SDL_GetTicks();
    modeStartTime = SDL_GetTicks();
    SoundManager::get().playOnce("beginning");
}
void Game::pauseGame() {
    if(currentState == GameState::PLAYING) {
        currentState = GameState::PAUSED;
        SoundManager::get().stopAll();
    }
}
void Game::resumeGame() {
    if(currentState == GameState::PAUSED) {
        currentState = GameState::PLAYING;
    }
}
void Game::gameOver() {
    currentState = GameState::GAME_OVER;
    SoundManager::get().stopAll();
    saveHighScore();
}
void Game::quit() {
    if (lives <= 0 && currentState == GameState::PLAYING) {
        gameOver();
    }
    isRunning = false;
}


// 8. Level & Reset

void Game::handleLevelComplete() {
    pacman->setDirection(STOP);
    pacman->setNextDirection(STOP);

    for(auto g : ghosts) {
        g->setFrozen(true);
    }

    SoundManager::get().stopAll();

    playMazeFlashAnimation();

    SDL_Delay(1000);

    startNextLevel();

}
void Game::startNextLevel() {
    currentLevel++;
    lives = 3;
    if(currentLevel > 255) {
        quit();
        return;
    }

    gameMap->resetDots();
    totalDots = gameMap->totalDots;
    levelComplete = false;

    resetPacmanPosition();

    for(auto g : ghosts) {
        g->reset();
        g->setFrozen(false);
    }

    fruitManager.reset(currentLevel);


    updateGhostSpeedForLevel();

    updateFrightenedTimeForLevel();

    frightenedUntil = 0;
    ghostsEatenInThisFrightened = 0;
    currentFrightenedIndex = 0;

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

    SoundManager::get().playOnce("beginning");
}
void Game::playMazeFlashAnimation() {
    const int flashCount = 4;
    const int flashDuration = 200;

    for(int i = 0; i < flashCount; i++) {
        gameMap->setMazeFlash(true);
        renderGameStatic();
        SDL_Delay(flashDuration);

        gameMap->setMazeFlash(false);
        renderGameStatic();
        SDL_Delay(flashDuration);
    }
}
void Game::resetPacmanPosition() {
    pacman->visible = true;
    pacman->setPosition(13 * 16 + 3, 25 * 16 + 11);
    pacman->isAlive = true;
    pacman->isFrozenForGhostScore = false;
    currentDir = STOP;
    nextDir = STOP;
    pacman->setDirection(STOP);
    pacman->setNextDirection(STOP);
    pacman->resetAnimation();
}
void Game::resetGame() {
    currentLevel = 1;
    lives = 3;
    score = 0;
    ghostScore = 0;
    fruitsScore = 0;
    eloryIndex = 0;
    currentSiren = "";
    frightenedUntil = 0;
    ghostsEatenInThisFrightened = 0;
    levelComplete = false;
    isGhostScoreShowing = false;
    ghostScoreFreezeStart = 0;

    resetPacmanPosition();
    gameMap->resetDots();
    totalDots = gameMap->totalDots;
    pacman->setDotsEaten(0);
    pacman->setBigDotsEaten(0);

    for (auto g : ghosts) {
        g->reset();
    }

    fruitManager.reset(currentLevel);

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

//    SoundManager::get().playOnce("beginning");
}

// 9. Event Handlers
void Game::handlePacmanDeath() {
    isPlayingDeathAnimation = true;

    SoundManager::get().stopAll();

    SoundManager::get().playOnce("death");

    isReady = false;
    readyStartTime = SDL_GetTicks();
    pacman->visible = false;
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
    SDL_Renderer* renderer = windowManager.getRenderer();
    SDL_Rect dstRect = pacman->rect;

    for(int i = 0; i < frameCount; i++) {
        windowManager.clear();

        if(backgroundTexture) {
            SDL_Rect bgRect = {0, 0, 448, 576};
            SDL_RenderCopy(renderer, backgroundTexture, nullptr, &bgRect);
        }

        gameMap->render();

        blinky->render(renderer);
        pinky->render(renderer);
        inky->render(renderer);
        clyde->render(renderer);

        renderLives();
        renderScore();

        SDL_RenderCopy(renderer, pacmanDeathTextures[i], nullptr, &dstRect);
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

        fruitManager.pauseFruit();

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
        isPlayingDeathAnimation = false;

    }else {
        gameOver();
        SDL_Delay(2000);
        currentState = GameState::MENU;
        selectedMenuItem = 0;
        resetGame();
        isPlayingDeathAnimation = false;
    }
}
void Game::handleMenuInput(SDL_Event& event) {
    if(event.type == SDL_KEYDOWN) {
        switch(event.key.keysym.sym) {
            case SDLK_UP:
            case SDLK_w:
                selectedMenuItem = 0;
                break;
            case SDLK_DOWN:
            case SDLK_s:
                selectedMenuItem = 1;
                break;
            case SDLK_RETURN:
            case SDLK_SPACE:
                if(selectedMenuItem == 0) {
                    startGame();
                } else {
                    quit();
                }
                break;
        }
    }
}


// 10. Utility & Helpers
bool Game::isMouseOverPauseButton(int x, int y) {
    return (x >= pauseButtonRect.x && x <= pauseButtonRect.x + pauseButtonRect.w &&
            y >= pauseButtonRect.y && y <= pauseButtonRect.y + pauseButtonRect.h);
}
void Game::handleMouseEvents(SDL_Event& event) {
    if (event.type == SDL_MOUSEMOTION && currentState == GameState::PLAYING) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        isPauseButtonHovered = isMouseOverPauseButton(mouseX, mouseY);
    }

    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        if (currentState == GameState::PLAYING && isReady) {
            if (isMouseOverPauseButton(event.button.x, event.button.y)) {
                pauseGame();
            }
        }
    }
}
void Game::handleKeyboardEvents(SDL_Event& event) {
    if (event.type != SDL_KEYDOWN || event.key.repeat) return;

    switch(currentState) {
        case GameState::PLAYING:
            handlePlayingStateKeys(event);
            break;
        case GameState::PAUSED:
            handlePausedStateKeys(event);
            break;
        case GameState::MENU:
            handleMenuInput(event);
            break;
        case GameState::GAME_OVER:
            currentState = GameState::MENU;
            selectedMenuItem = 0;
            break;
    }
}
void Game::handlePlayingStateKeys(SDL_Event& event) {
    if (event.key.keysym.sym == SDLK_SPACE ||
        event.key.keysym.sym == SDLK_p ||
        event.key.keysym.sym == SDLK_ESCAPE) {
        if (isReady) pauseGame();
    } else {
        pacman->handleInput(event);
    }
}
void Game::handlePausedStateKeys(SDL_Event& event) {
    switch (event.key.keysym.sym) {
        case SDLK_SPACE:
        case SDLK_p:
        case SDLK_ESCAPE:
            resumeGame();
            break;
        case SDLK_UP:
        case SDLK_w:
            selectedPauseItem = 0;
            break;
        case SDLK_DOWN:
        case SDLK_s:
            selectedPauseItem = 1;
            break;
        case SDLK_RETURN:
            if (selectedPauseItem == 0) {
                resumeGame();
            } else {
                currentState = GameState::MENU;
                selectedMenuItem = 0;
                resetGame();
            }
            break;
    }
}

Ghost* Game::createGhost(GhostType type, const std::vector<std::string>& texturePaths) {
    Ghost* ghost = nullptr;

    switch(type) {
        case BLINKY_TYPE: ghost = new Blinky(); break;
        case PINKY_TYPE: ghost = new Pinky(); break;
        case INKY_TYPE: ghost = new Inky(); break;
        case CLYDE_TYPE: ghost = new Clyde(); break;
    }

    if (ghost) {
        ghost->setWindowManager(&windowManager);
        ghost->loadTextures(textureManager, texturePaths);
        ghost->loadScoreTextures(textureManager,
                                 "assets/scores/200.png",
                                 "assets/scores/400.png",
                                 "assets/scores/800.png",
                                 "assets/scores/1600.png");
    }

    return ghost;
}
std::vector<std::string> Game::getGhostTextures(const std::string& ghostName) {
    return {
            "assets/ghost/eyeUp.png",
            "assets/ghost/eyeDown.png",
            "assets/ghost/eyeLeft.png",
            "assets/ghost/eyeRight.png",
            "assets/ghost/" + ghostName + ".png",
            "assets/ghost/" + ghostName + "2.png",
            "assets/ghost/frightened.png",
            "assets/ghost/frightened2.png",
            "assets/ghost/endFrightening.png",
            "assets/ghost/endFrightening2.png",
            "assets/scores/200.png",
            "assets/scores/400.png",
            "assets/scores/800.png",
            "assets/scores/1600.png"
    };
}

void Game::saveHighScore() {
    std::ofstream file("assets/highscore.txt");
    if(file.is_open()) {
        file << highScore;
        file.close();
    }
}

void Game::loadHighScore() {
    printf("DEBUG: Trying to load highscore from assets/highscore.txt\n");
    std::ifstream file("assets/highscore.txt");
    if(file.is_open() && file >> highScore) {
        file.close();
    }
}