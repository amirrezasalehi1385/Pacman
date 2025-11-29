#include "../include/Ghost.h"
#include "../include/TextureManager.h"
#include <cmath>
#include "../include/Map.h"
#include <queue>
#include <vector>
#include "../include/Direction.h"
#include "../include/GhostManager.h"
#include <SDL2/SDL.h>


Ghost::Ghost(float initX, float initY, int w, int h)
        : initialTileX(initX), initialTileY(initY), w(w), h(h), pixelsMoved(0), speed(1)
{
    endOfFrightening = false;
    canGotoGhostHouse = true;
    readyToExit = false;
    ghostEaten = false;
    state = WAIT;
    int pixelX = initX * 16 + 8 - w/2;
    int pixelY = initY * 16 + 3 * 16 + 8 - h/2;
    rect = {pixelX -8 , pixelY + 8, w, h};
    updateHitbox();
    targetTexture = nullptr;
    currentTile = {(int)initX, (int)initY};
    targetTile = currentTile;
    currentDirection = STOP;
//    scoreTexture200 = nullptr;
//    scoreTexture400 = nullptr;
//    scoreTexture800 = nullptr;
//    scoreTexture1600 = nullptr;
//    showingScore = false;
//    scoreDisplayDuration = 1000; // 1 ثانیه
//    currentScoreValue = 0;

}
//void Ghost::loadScoreTextures(TextureManager* textureManager,
//                              const std::string& score200Path,
//                              const std::string& score400Path,
//                              const std::string& score800Path,
//                              const std::string& score1600Path) {
//    scoreTexture200 = textureManager->loadTexture(score200Path);
//    scoreTexture400 = textureManager->loadTexture(score400Path);
//    scoreTexture800 = textureManager->loadTexture(score800Path);
//    scoreTexture1600 = textureManager->loadTexture(score1600Path);
//}
//
//void Ghost::startShowingScore(int score) {
//    showingScore = true;
//    scoreDisplayStartTime = SDL_GetTicks();
//    currentScoreValue = score;
//}
//
//void Ghost::updateScoreDisplay() {
//    if(showingScore) {
//        Uint32 now = SDL_GetTicks();
//        if(now - scoreDisplayStartTime >= scoreDisplayDuration) {
//            showingScore = false;
//            // بعد از نمایش امتیاز، وضعیت به EATEN تغییر می‌کند
//            setState(EATEN);
//        }
//    }
//}
//
//void Ghost::renderScore(SDL_Renderer* renderer) {
//    if(!showingScore) return;
//
//    SDL_Texture* scoreTex = nullptr;
//    switch(currentScoreValue) {
//        case 200:  scoreTex = scoreTexture200;  break;
//        case 400:  scoreTex = scoreTexture400;  break;
//        case 800:  scoreTex = scoreTexture800;  break;
//        case 1600: scoreTex = scoreTexture1600; break;
//    }
//
//    if(scoreTex) {
//        SDL_Rect dstRect = rect;
//        SDL_RenderCopy(renderer, scoreTex, nullptr, &dstRect);
//    }
//}

bool Ghost::ghostInGhostHouse() {
    const SDL_Rect ghostHouseTiles = {11, 16, 8, 5};
    const int tileSize = 16;

    SDL_Rect ghostHouse = {
            ghostHouseTiles.x * tileSize,
            ghostHouseTiles.y * tileSize,
            ghostHouseTiles.w * tileSize,
            ghostHouseTiles.h * tileSize
    };

    SDL_Rect ghostRect = rect;

    bool inside =
            ghostRect.x + ghostRect.w > ghostHouse.x &&
            ghostRect.x < ghostHouse.x + ghostHouse.w &&
            ghostRect.y + ghostRect.h > ghostHouse.y &&
            ghostRect.y < ghostHouse.y + ghostHouse.h;

    return inside;
}

bool Ghost::checkCollisionWithPacman(Pacman* pacman) {
    SDL_Rect pacHitbox = pacman->getHitbox();
    if(SDL_HasIntersection(&hitbox, &pacHitbox)) {
        if(state == FRIGHTENED) {
            ghostEaten = true;
        }else if(state == CHASE || state == SCATTER){
            pacman->isAlive = false;
        }
        return true;
    }
    return false;

}

void Ghost::reset() {
    endOfFrightening = false;
    canGotoGhostHouse = true;
    readyToExit = false;
    ghostEaten = false;
    state = WAIT;
    pixelsMoved = 0;
    currentDirection = STOP;
    speed = 1;
    int pixelX = initialTileX * 16 + 8 - w/2;
    int pixelY = initialTileY * 16 + 3 * 16 + 8 - h/2;
    rect = {pixelX - 8, pixelY + 8, w, h};

    updateHitbox();
    currentTile = {(int)initialTileX, (int)initialTileY};
    targetTile = currentTile;
}

void Ghost::wait() {
    const int tileSize = 16;

    if(currentDirection == STOP) {
        currentDirection = UP;
    }

    switch (currentDirection) {
        case UP:    rect.y -= 1; currentEye = eyeUp; break;
        case DOWN:  rect.y += 1; currentEye = eyeDown; break;
        default: break;
    }

    pixelsMoved += 1;

    if(pixelsMoved >= tileSize) {
        pixelsMoved = 0;
        if(currentDirection == UP) currentDirection = DOWN;
        else if(currentDirection == DOWN) currentDirection = UP;
    }

    updateHitbox();
}

void Ghost::updateBodyAnimation() {
    frameCounter++;
    if(frameCounter >= frameSpeed) {
        bodyFrame = 1 - bodyFrame;
        frameCounter = 0;
    }
}


void Ghost::update(const Map& map) {
    if(frozen) return;
    switch(state) {
        case WAIT:
            wait();
            if (readyToExit) state = EXIT;
            break;
        case EXIT:
            getOutOfHouse(map);
            canGotoGhostHouse = true;
            break;
        case CHASE:
        case SCATTER:
            canGotoGhostHouse = false;
            updateChaseScatter(map);
            break;
        case FRIGHTENED:
            speed = 1;
            canGotoGhostHouse = false;
            updateFrightened(map);
            break;
        case EATEN:
            speed = 3;
            canGotoGhostHouse = true;
            updateChaseScatter(map);
            if (currentTile.x == 13 && currentTile.y == 14) {
                speed = 1;
                setMode(EXIT);
            }
    }
    updateBodyAnimation();
}



void Ghost::getOutOfHouse(const Map& map) {
    const int tileSize = 16;
    const int mapWidthTiles = 28;
    const int mapHeightTiles = 31;
    const int exitTileX = 14;
    const int exitTileY = 11;
    const int offsetX = -8;

    if (pixelsMoved == 0) {
        int rawTileX = (rect.x + 8 - offsetX) / tileSize;
        currentTile.x = ((rawTileX % mapWidthTiles) + mapWidthTiles) % mapWidthTiles;
        currentTile.y = (rect.y + 8 - 3 * tileSize) / tileSize;

        Direction nextDir = STOP;

        if (currentTile.x < exitTileX) nextDir = RIGHT;
        else if (currentTile.x > exitTileX) nextDir = LEFT;
        else if (currentTile.y > exitTileY) nextDir = UP;
        else if (currentTile.y == exitTileY) {
            state = SCATTER;
            pixelsMoved = 0;
            currentDirection = LEFT;
            currentEye = eyeLeft;
            return;
        }

        if (nextDir != STOP) {
            const SDL_Point dirs[4] = { {0,-1}, {-1,0}, {0,1}, {1,0} };
            Direction dirMap[4] = { UP, LEFT, DOWN, RIGHT };

            for (int i = 0; i < 4; i++) {
                if (dirMap[i] == nextDir) {
                    int nx = currentTile.x + dirs[i].x;
                    int ny = currentTile.y + dirs[i].y;

                    if (nx >= 0 && nx < mapWidthTiles && ny >= 0 && ny < mapHeightTiles) {
                        if (map.isWalkable(nx, ny) || Map::isInGhostHouse(nx, ny)) {
                            currentDirection = nextDir;
                            break;
                        }
                    }
                }
            }
        }
    }

    if (currentDirection != STOP) {
        switch (currentDirection) {
            case UP:
                rect.y -= speed;
                currentEye = eyeUp;
                break;
            case DOWN:
                rect.y += speed;
                currentEye = eyeDown;
                break;
            case LEFT:
                rect.x -= speed;
                currentEye = eyeLeft;
                break;
            case RIGHT:
                rect.x += speed;
                currentEye = eyeRight;
                break;
            default:
                break;
        }
        pixelsMoved += speed;
    }

    if (pixelsMoved >= tileSize) {
        pixelsMoved = 0;

        int rawNewTileX = (rect.x + 8 - offsetX) / tileSize; // برای collision
        int newTileX = ((rawNewTileX % mapWidthTiles) + mapWidthTiles) % mapWidthTiles;
        int newTileY = (rect.y + 8 - 3 * tileSize) / tileSize;

        rect.x = newTileX * tileSize + 8 - rect.w / 2 + offsetX;
        rect.y = newTileY * tileSize + 3 * tileSize + 8 - rect.h / 2;

        currentTile.x = newTileX;
        currentTile.y = newTileY;

    }

    updateHitbox();
}




bool Ghost::loadTextures(TextureManager* texManager, const std::vector<std::string>& paths) {
    if(paths.size() < 10) return false; // برگشت به 10 تکسچر

    eyeUp    = texManager->loadTexture(paths[0]);
    eyeDown  = texManager->loadTexture(paths[1]);
    eyeLeft  = texManager->loadTexture(paths[2]);
    eyeRight = texManager->loadTexture(paths[3]);
    bodyTex1 = texManager->loadTexture(paths[4]);
    bodyTex2 = texManager->loadTexture(paths[5]);
    frightenedTex = texManager->loadTexture(paths[6]);
    frightenedTex2 = texManager->loadTexture(paths[7]);
    endFrightened = texManager->loadTexture(paths[8]);
    endFrightened2 = texManager->loadTexture(paths[9]);

    currentEye = eyeDown;

    return eyeUp && eyeDown && eyeLeft && eyeRight && bodyTex1 && bodyTex2;
}



void Ghost::setTarget(const SDL_Rect& targetRect) {
    targetTile.x = (targetRect.x + targetRect.w/2) / 16;
    targetTile.y = (targetRect.y + targetRect.h/2 - 3*16) / 16;
    printf("Ghost target set to tile (%d,%d)\n", targetTile.x, targetTile.y);
}

void Ghost::setTargetTile(int tileX, int tileY) {
    targetTile.x = tileX;
    targetTile.y = tileY;

    printf("Ghost target set to tile (%d,%d)\n", targetTile.x, targetTile.y);
}
void Ghost::updateChaseScatter(const Map& map) {
    const int mapWidthTiles = 28;
    const int mapHeightTiles = 31;
    const int tileSize = 16;
    const int tunnelRow = 14;

    if (pixelsMoved == 0) {
        int rawTileX = (rect.x + 8) / tileSize;
        currentTile.x = ((rawTileX % mapWidthTiles) + mapWidthTiles) % mapWidthTiles;
        currentTile.y = (rect.y + 8 - 3 * tileSize) / tileSize;

        int adjustedTargetX = targetTile.x;
        int adjustedTargetY = targetTile.y;

        bool targetInTunnel = (targetTile.y == tunnelRow) &&
                              (targetTile.x == 0 || targetTile.x == mapWidthTiles - 1);
        if (targetInTunnel) {
            adjustedTargetX = (targetTile.x == 0) ? mapWidthTiles - 1 : 0;
        }

        // جهت معکوس
        Direction reverseDir;
        switch (currentDirection) {
            case UP:    reverseDir = DOWN; break;
            case DOWN:  reverseDir = UP; break;
            case LEFT:  reverseDir = RIGHT; break;
            case RIGHT: reverseDir = LEFT; break;
            default:    reverseDir = STOP; break;
        }

        const SDL_Point dirs[4] = { {0,-1}, {-1,0}, {0,1}, {1,0} };
        Direction dirMap[4] = { UP, LEFT, DOWN, RIGHT };

        Direction bestDir = STOP;
        int bestDist = INT_MAX;

        for (int i = 0; i < 4; i++) {
            int nx = currentTile.x + dirs[i].x;
            int ny = currentTile.y + dirs[i].y;
            Direction dir = dirMap[i];

            bool inNoUpZoneNextTile = ((ny == 1 || ny == 22) && (nx >= 12 && nx <= 17));
            if (dir == UP && inNoUpZoneNextTile) continue;
            if (dir == reverseDir) continue;

            if (!canGotoGhostHouse && Map::isInGhostHouse(nx, ny)) continue;

            bool isValidTile = false;
            int actualNx = nx;

            if (ny == tunnelRow) {
                if (nx < 0) actualNx = mapWidthTiles - 1;
                else if (nx >= mapWidthTiles) actualNx = 0;

                if (ny >= 0 && ny < mapHeightTiles) isValidTile = map.isWalkable(actualNx, ny);
            } else {
                if (nx >= 0 && nx < mapWidthTiles && ny >= 0 && ny < mapHeightTiles) {
                    actualNx = nx;
                    isValidTile = map.isWalkable(actualNx, ny);
                }
            }

            if (!isValidTile) continue;

            int dx = actualNx - adjustedTargetX;
            int dy = ny - adjustedTargetY;

            if (ny == tunnelRow) {
                int normalDist = abs(dx);
                int wrapDist = mapWidthTiles - normalDist;
                if (wrapDist < normalDist) dx = (dx > 0) ? -wrapDist : wrapDist;
            }

            int dist = dx * dx + dy * dy;

            if (dist < bestDist) {
                bestDist = dist;
                bestDir = dir;
            }
        }

        if (bestDir == STOP && reverseDir != STOP) bestDir = reverseDir;
        currentDirection = bestDir;
    }

    if (currentDirection != STOP) {
        switch (currentDirection) {
            case UP:    rect.y -= speed; currentEye = eyeUp; break;
            case DOWN:  rect.y += speed; currentEye = eyeDown; break;
            case LEFT:  rect.x -= speed; currentEye = eyeLeft; break;
            case RIGHT: rect.x += speed; currentEye = eyeRight; break;
            default: break;
        }
        pixelsMoved += speed;
    }

    if (pixelsMoved >= tileSize) {
        pixelsMoved = 0;

        int rawNewTileX = (rect.x + 8) / tileSize;
        int newTileX = ((rawNewTileX % mapWidthTiles) + mapWidthTiles) % mapWidthTiles;
        int newTileY = (rect.y + 8 - 3 * tileSize) / tileSize;

        rect.x = newTileX * tileSize + 8 - rect.w / 2;
        rect.y = newTileY * tileSize + 3 * tileSize + 8 - rect.h / 2;

        currentTile.x = newTileX;
        currentTile.y = newTileY;
    }

    int mapWidth = mapWidthTiles * tileSize;
    if (rect.x + rect.w/2 < 0) rect.x = mapWidth - rect.w/2;
    else if (rect.x + rect.w/2 >= mapWidth) rect.x = -rect.w/2;

    updateHitbox();
}

void Ghost::updateFrightened(const Map& map) {
    const int mapWidthTiles = 28;
    const int mapHeightTiles = 31;
    const int tileSize = 16;
    const int tunnelRow = 14;

    if (pixelsMoved == 0) {
        int rawTileX = (rect.x + 8) / tileSize;
        currentTile.x = ((rawTileX % mapWidthTiles) + mapWidthTiles) % mapWidthTiles;
        currentTile.y = (rect.y + 8 - 3 * tileSize) / tileSize;

        Direction reverseDir;
        switch (currentDirection) {
            case UP:    reverseDir = DOWN; break;
            case DOWN:  reverseDir = UP; break;
            case LEFT:  reverseDir = RIGHT; break;
            case RIGHT: reverseDir = LEFT; break;
            default:    reverseDir = STOP; break;
        }

        std::vector<Direction> possibleDirs;
        const SDL_Point dirs[4] = { {0,-1}, {-1,0}, {0,1}, {1,0} };
        Direction dirMap[4] = { UP, LEFT, DOWN, RIGHT };

        for (int i = 0; i < 4; i++) {
            Direction dir = dirMap[i];
            int nx = currentTile.x + dirs[i].x;
            int ny = currentTile.y + dirs[i].y;

            if (dir == reverseDir) continue;

            if (!canGotoGhostHouse && Map::isInGhostHouse(nx, ny)) continue;

            bool isValidTile = false;
            int actualNx = nx;

            if (ny == tunnelRow) {
                if (nx < 0) actualNx = mapWidthTiles - 1;
                else if (nx >= mapWidthTiles) actualNx = 0;

                if (ny >= 0 && ny < mapHeightTiles) {
                    isValidTile = map.isWalkable(actualNx, ny);
                }
            } else {
                if (nx >= 0 && nx < mapWidthTiles && ny >= 0 && ny < mapHeightTiles) {
                    actualNx = nx;
                    isValidTile = map.isWalkable(actualNx, ny);
                }
            }

            if (isValidTile) {
                possibleDirs.push_back(dir);
            }
        }

        if (!possibleDirs.empty()) {
            int idx = rand() % possibleDirs.size();
            currentDirection = possibleDirs[idx];
        } else if (reverseDir != STOP) {
            currentDirection = reverseDir;
        }
    }

    if (currentDirection != STOP) {
        switch (currentDirection) {
            case UP:    rect.y -= speed; currentEye = eyeUp; break;
            case DOWN:  rect.y += speed; currentEye = eyeDown; break;
            case LEFT:  rect.x -= speed; currentEye = eyeLeft; break;
            case RIGHT: rect.x += speed; currentEye = eyeRight; break;
            default: break;
        }
        pixelsMoved += speed;
    }

    if (pixelsMoved >= tileSize) {
        pixelsMoved = 0;

        int rawNewTileX = (rect.x + 8) / tileSize;
        int newTileX = ((rawNewTileX % mapWidthTiles) + mapWidthTiles) % mapWidthTiles;
        int newTileY = (rect.y + 8 - 3 * tileSize) / tileSize;

        // تنظیم دقیق موقعیت
        rect.x = newTileX * tileSize + 8 - rect.w / 2;
        rect.y = newTileY * tileSize + 3 * tileSize + 8 - rect.h / 2;

        currentTile.x = newTileX;
        currentTile.y = newTileY;
    }

    int mapWidth = mapWidthTiles * tileSize;
    if (rect.x + rect.w/2 < 0) {
        rect.x = mapWidth - rect.w/2;
    } else if (rect.x + rect.w/2 >= mapWidth) {
        rect.x = -rect.w/2;
    }

    updateHitbox();
}

SDL_Rect* Ghost::getHitBox() {
    return &hitbox;
}


void Ghost::render(SDL_Renderer* renderer) {
//    if(showingScore) {
//        renderScore(renderer);
//        return;
//    }
    SDL_Texture* texToRender = (bodyFrame == 0) ? bodyTex1 : bodyTex2;
    SDL_Texture* frightenedToRender = (bodyFrame == 0) ? frightenedTex : frightenedTex2;
    SDL_Texture* endFrightenedToRender = (bodyFrame == 0) ? endFrightened : endFrightened2;
    SDL_Texture* endFrightToRender = (bodyFrame == 0) ? frightenedToRender : endFrightenedToRender;
    if(getState() != FRIGHTENED && getState() != EATEN) {
        if(texToRender) SDL_RenderCopy(renderer, texToRender, nullptr, &rect);
        if(currentEye) SDL_RenderCopy(renderer, currentEye, nullptr, &rect);
    }
    if(getState() == FRIGHTENED) {
        if(frightenedToRender) SDL_RenderCopy(renderer,frightenedToRender, nullptr, &rect);
        if(endOfFrightening) SDL_RenderCopy(renderer,endFrightToRender, nullptr, &rect);
    }
    if(getState() == EATEN) {
        if(currentEye) SDL_RenderCopy(renderer, currentEye, nullptr, &rect);
    }
    renderTarget(renderer);
//    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
//    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 100);
//    SDL_RenderFillRect(renderer, &hitbox);
}

void Ghost::updateHitbox(){
    int hbSize = 16;
    hitbox.w = hbSize;
    hitbox.h = hbSize;
    hitbox.x = rect.x + (rect.w - hbSize) / 2;
    hitbox.y = rect.y + (rect.h - hbSize) / 2;
}


bool Ghost::loadTargetTexture(SDL_Renderer* renderer, const std::string& path) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        printf("Failed to load target image: %s - %s\n", path.c_str(), IMG_GetError());
        return false;
    }

    targetTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!targetTexture) {
        printf("Failed to create target texture: %s\n", SDL_GetError());
        return false;
    }

    printf("Target texture loaded successfully: %s\n", path.c_str());
    return true;
}

void Ghost::renderTarget(SDL_Renderer* renderer) {
    if (!targetTexture) return;

    int targetPixelX = targetTile.x * 16;
    int targetPixelY = targetTile.y * 16 + 3*16;

    int targetSize = 16;

    SDL_Rect targetRect = {
            targetPixelX,
            targetPixelY,
            targetSize,
            targetSize
    };

    SDL_RenderCopy(renderer, targetTexture, NULL, &targetRect);
}

void Ghost::clearTargetTexture() {
    if (targetTexture) {
        SDL_DestroyTexture(targetTexture);
        targetTexture = nullptr;
    }
}

SDL_Point Ghost::getCurrentTile() const {
    return currentTile;
}

void Ghost::setState(GhostState mode) {
    state = mode;
};
void Ghost::setMode(GhostState mode) {
    state = mode;
};