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
SDL_Texture* backgroundTexture = nullptr; // در Game.h تعریف کن

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
    if(!windowManager.init(title.c_str(), w, h)) return false;
    textureManager = new TextureManager(windowManager.getRenderer());

    backgroundTexture = textureManager->loadTexture("assets/map/background.png");

    gameMap = new Map(textureManager, windowManager.getRenderer());
    gameMap->loadLevel1();

    pacman = new Pacman(0, 16 * 16 + 11, 26, 26);
    pacman->loadTextures(textureManager, "assets/Pacman/2.png","assets/Pacman/1.png", "assets/Pacman/0.png");

    blinky = new Blinky(10, 10, 28, 28);
    blinky = new Blinky(10, 10, 28, 28);
    blinky->loadTextures(textureManager,
                         "assets/ghost/blinky_up.png",
                         "assets/ghost/blinky_down.png",
                         "assets/ghost/blinky_left.png",
                         "assets/ghost/blinky_right.png");
//    blinky->loadTargetTexture(windowManager.getRenderer(), "assets/blinky_target.png");

    pinky = new Pinky(14, 14, 28, 28);
    pinky->loadTextures(textureManager,
                        "assets/ghost/pinky_up.png",
                        "assets/ghost/pinky_down.png",
                        "assets/ghost/pinky_left.png",
                        "assets/ghost/pinky_right.png");
    pinky->loadTargetTexture(windowManager.getRenderer(), "assets/pinky_target.png");

    inky = new Inky(12,14,28,28);
    inky->loadTextures(textureManager,
                       "assets/ghost/inky_up.png",
                       "assets/ghost/inky_down.png",
                       "assets/ghost/inky_left.png",
                       "assets/ghost/inky_right.png");
//    inky->loadTargetTexture(windowManager.getRenderer(), "assets/inky_target.png");
    clyde = new Clyde(16,14,28,28);
    clyde->loadTextures(textureManager,
                       "assets/ghost/clyde_up.png",
                       "assets/ghost/clyde_down.png",
                       "assets/ghost/clyde_left.png",
                       "assets/ghost/clyde_right.png");
//    clyde->loadTargetTexture(windowManager.getRenderer(), "assets/clyde_target.png");

    isRunning = true;
    return true;
}
void Game::render() {
    windowManager.clear();

    // رندر بک‌گراند
    if (backgroundTexture) {
        SDL_Rect dst = {0, 0, 448, 576}; // سایز پنجره‌ات
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
    Uint32 now = SDL_GetTicks();
    Uint32 elapsed = (now - gameStartTime) / 1000; // به ثانیه

    // برای هر روح فقط در شرایط WAIT و SCATTER وضعیت رو تغییر بده، EXIT رو روح خودش کنترل می‌کنه

    // حالت WAIT فقط اگر کمتر از 5 ثانیه گذشته باشه
    if (elapsed < 5) {
        if (blinky->state != WAIT) blinky->state = WAIT;
        if (pinky->state != WAIT) pinky->state = WAIT;
        if (inky->state != WAIT) inky->state = WAIT;
        if (clyde->state != WAIT) clyde->state = WAIT;
    }
        // حالت SCATTER اگر بین 10 تا 15 ثانیه باشیم
    else if (elapsed >= 10 && elapsed < 15) {
        if (blinky->state == WAIT || blinky->state == EXIT) blinky->state = SCATTER;
        if (pinky->state == WAIT || pinky->state == EXIT) pinky->state = SCATTER;
        if (inky->state == WAIT || inky->state == EXIT) inky->state = SCATTER;
        if (clyde->state == WAIT || clyde->state == EXIT) clyde->state = SCATTER;
    }
        // بقیه زمان‌ها حالت CHASE می‌باشد
    else if (elapsed >= 15) {
        if (blinky->state != CHASE) blinky->state = CHASE;
        if (pinky->state != CHASE) pinky->state = CHASE;
        if (inky->state != CHASE) inky->state = CHASE;
        if (clyde->state != CHASE) clyde->state = CHASE;
    }



    if (elapsed >= 5 && elapsed < 10) {
        if (blinky->state == WAIT) blinky->state = EXIT;
        if (pinky->state == WAIT) pinky->state = EXIT;
        if (inky->state == WAIT) inky->state = EXIT;
        if (clyde->state == WAIT) clyde->state = EXIT;
    }

    // آپدیت پک‌من
    pacman->update();
    pacman->move(gameMap, speed);

    // آپدیت روح‌ها
    blinky->update(*pacman, *gameMap);
    pinky->update(*pacman, *gameMap);
    inky->update(*pacman, *blinky, *gameMap);
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





