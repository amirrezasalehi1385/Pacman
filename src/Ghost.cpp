#include "Ghost.h"
#include "GameRules.h"
#include <SDL2/SDL.h>
#include <cmath>
#include <vector>

Ghost::Ghost(float initX, float initY, int w, int h)
        : initialTileX(initX), initialTileY(initY), w(w), h(h), pixelsMoved(0.0f),
          posX(initX * 16 + 8 - w / 2), posY(initY * 16 + 3*16 + 8 - h/2),
          speed(GameRules::GHOST_SPEED_NORMAL)
{
    endOfFrightening = false;
    canGotoGhostHouse = true;
    readyToExit = false;
    ghostEaten = false;
    state = WAIT;
    rect = {static_cast<int>(posX) - 8, static_cast<int>(posY) + 8, w, h};
    updateHitbox();
    targetTexture = nullptr;
    currentTile = {(int)initX, (int)initY};
    targetTile = currentTile;
    currentDirection = STOP;
    scoreTexture200 = nullptr;
    scoreTexture400 = nullptr;
    scoreTexture800 = nullptr;
    scoreTexture1600 = nullptr;
    showingScore = false;
    scoreDisplayDuration = 1000;
    currentScoreValue = 0;
}

//---------------- Score Handling ----------------

void Ghost::loadScoreTextures(TextureManager* textureManager,
                              const std::string& score200Path,
                              const std::string& score400Path,
                              const std::string& score800Path,
                              const std::string& score1600Path) {
    scoreTexture200 = textureManager->loadTexture(score200Path);
    scoreTexture400 = textureManager->loadTexture(score400Path);
    scoreTexture800 = textureManager->loadTexture(score800Path);
    scoreTexture1600 = textureManager->loadTexture(score1600Path);
}

void Ghost::startShowingScore(int score) {
    showingScore = true;
    scoreBeingDisplayed = true;
    scoreDisplayStartTime = SDL_GetTicks();
    currentScoreValue = score;
}

void Ghost::updateScoreDisplay() {
    if(showingScore) {
        Uint32 now = SDL_GetTicks();
        if(now - scoreDisplayStartTime >= scoreDisplayDuration) {
            showingScore = false;
            scoreBeingDisplayed = false;
            setState(EATEN);
        }
    }
}

void Ghost::renderScore(SDL_Renderer* renderer) {
    if(!showingScore) return;
    SDL_Texture* scoreTex = nullptr;
    switch(currentScoreValue) {
        case 200: scoreTex = scoreTexture200; break;
        case 400: scoreTex = scoreTexture400; break;
        case 800: scoreTex = scoreTexture800; break;
        case 1600: scoreTex = scoreTexture1600; break;
    }
    if(scoreTex) {
        SDL_Rect dstRect = rect;
        SDL_RenderCopy(renderer, scoreTex, nullptr, &dstRect);
    }
}

//---------------- State / Reset ----------------

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
    return (ghostRect.x + ghostRect.w > ghostHouse.x &&
            ghostRect.x < ghostHouse.x + ghostHouse.w &&
            ghostRect.y + ghostRect.h > ghostHouse.y &&
            ghostRect.y < ghostHouse.y + ghostHouse.h);
}

bool Ghost::checkCollisionWithPacman(Pacman* pacman) {
    SDL_Rect pacHitbox = pacman->getHitbox();
    if(SDL_HasIntersection(&hitbox, &pacHitbox)) {
        if(state == FRIGHTENED) {
            ghostEaten = true;
            return true;
        } else if(state == CHASE || state == SCATTER) {
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
    pixelsMoved = 0.0f;
    currentDirection = STOP;
    speed = GameRules::GHOST_SPEED_NORMAL;
    posX = initialTileX * 16 + 8 - w/2;
    posY = initialTileY * 16 + 3*16 + 8 - h/2;
    rect = {static_cast<int>(posX) - 8, static_cast<int>(posY) + 8, w, h};
    updateHitbox();
    currentTile = {(int)initialTileX, (int)initialTileY};
    targetTile = currentTile;
}

//---------------- Wait State ----------------

void Ghost::wait() {
    const float tileSize = 16.0f;
    const int offset = 8; // وسط تایل

    // تعیین جهت اولیه
    if(currentDirection == STOP) currentDirection = UP;

    // حرکت واقعی
    switch(currentDirection) {
        case UP:
            posY -= speed;
            currentEye = eyeUp;
            break;
        case DOWN:
            posY += speed;
            currentEye = eyeDown;
            break;
        default:
            break;
    }

    pixelsMoved += speed;

    if(pixelsMoved >= tileSize) {
        pixelsMoved = 0.0f;
        currentDirection = (currentDirection == UP) ? DOWN : UP;

        // تراز کردن در وسط تایل
        currentTile.x = static_cast<int>((posX + offset) / tileSize);
        currentTile.y = static_cast<int>((posY + offset) / tileSize);

        posX = currentTile.x * tileSize + offset - w / 2;
        posY = currentTile.y * tileSize + offset - h / 2;
    }

    rect.x = static_cast<int>(posX);
    rect.y = static_cast<int>(posY);
    updateHitbox();
}




//---------------- Update ----------------

void Ghost::update(const Map& map) {
    if(frozen) return;

    switch(state) {
        case WAIT:
            wait();
            if(readyToExit) state = EXIT;
            break;
        case EXIT:
            speed = GameRules::GHOST_SPEED_NORMAL;
            getOutOfHouse(map);
            canGotoGhostHouse = true;
            break;
        case CHASE:
        case SCATTER:
            speed = GameRules::GHOST_SPEED_NORMAL;
            canGotoGhostHouse = false;
            updateChaseScatter(map);
            break;
        case FRIGHTENED:
            speed = GameRules::GHOST_SPEED_FRIGHTENED;
            canGotoGhostHouse = false;
            updateFrightened(map);
            break;
        case EATEN:
            speed = GameRules::GHOST_SPEED_EATEN;
            canGotoGhostHouse = true;
            updateChaseScatter(map);
            if(currentTile.x == 13 && currentTile.y == 14) {
                speed = 1.0f;
                setMode(EXIT);
            }
            break;
    }
    updateBodyAnimation();
}

//---------------- Movement ----------------

void Ghost::getOutOfHouse(const Map& map) {
    const float tileSize = 16.0f;
    const int mapWidthTiles = 28;
    const int mapHeightTiles = 31;
    const int exitTileX = 14;
    const int exitTileY = 11;
    const int offsetX = -8;

    if(pixelsMoved == 0.0f) {
        int rawTileX = static_cast<int>((posX + 8 - offsetX) / tileSize);
        currentTile.x = ((rawTileX % mapWidthTiles) + mapWidthTiles) % mapWidthTiles;
        currentTile.y = static_cast<int>((posY + 8 - 3 * tileSize) / tileSize);

        Direction nextDir = STOP;
        if(currentTile.x < exitTileX) nextDir = RIGHT;
        else if(currentTile.x > exitTileX) nextDir = LEFT;
        else if(currentTile.y > exitTileY) nextDir = UP;
        else if(currentTile.y == exitTileY) {
            state = SCATTER;
            pixelsMoved = 0.0f;
            currentDirection = LEFT;
            currentEye = eyeLeft;
            return;
        }
        currentDirection = nextDir;
    }

    if(currentDirection != STOP) {
        switch(currentDirection) {
            case UP: posY -= speed; currentEye = eyeUp; break;
            case DOWN: posY += speed; currentEye = eyeDown; break;
            case LEFT: posX -= speed; currentEye = eyeLeft; break;
            case RIGHT: posX += speed; currentEye = eyeRight; break;
            default: break;
        }
        pixelsMoved += speed;
    }

    if(pixelsMoved >= tileSize) {
        pixelsMoved = 0.0f;
        currentTile.x = static_cast<int>((posX + 8 - offsetX) / tileSize);
        currentTile.y = static_cast<int>((posY + 8 - 3*tileSize) / tileSize);
        posX = currentTile.x * tileSize + 8 - w/2 + offsetX;
        posY = currentTile.y * tileSize + 3*tileSize + 8 - h/2;
    }

    rect.x = static_cast<int>(posX);
    rect.y = static_cast<int>(posY);
    updateHitbox();
}

void Ghost::updateChaseScatter(const Map& map) {
    const int mapWidthTiles = 28;
    const int mapHeightTiles = 31;
    const float tileSize = 16.0f;
    const int tunnelRow = 14;

    if(pixelsMoved == 0.0f) {
        currentTile.x = static_cast<int>((posX + 8) / tileSize) % mapWidthTiles;
        currentTile.y = static_cast<int>((posY + 8 - 3*tileSize) / tileSize);

        int adjustedTargetX = targetTile.x;
        int adjustedTargetY = targetTile.y;

        bool targetInTunnel = (targetTile.y == tunnelRow) &&
                              (targetTile.x == 0 || targetTile.x == mapWidthTiles - 1);
        if(targetInTunnel) adjustedTargetX = (targetTile.x == 0) ? mapWidthTiles - 1 : 0;

        Direction reverseDir;
        switch(currentDirection) {
            case UP: reverseDir = DOWN; break;
            case DOWN: reverseDir = UP; break;
            case LEFT: reverseDir = RIGHT; break;
            case RIGHT: reverseDir = LEFT; break;
            default: reverseDir = STOP; break;
        }

        const SDL_Point dirs[4] = { {0,-1}, {-1,0}, {0,1}, {1,0} };
        Direction dirMap[4] = { UP, LEFT, DOWN, RIGHT };
        Direction bestDir = STOP;
        int bestDist = INT_MAX;

        for(int i=0;i<4;i++) {
            Direction dir = dirMap[i];
            if(dir == reverseDir) continue;

            int nx = currentTile.x + dirs[i].x;
            int ny = currentTile.y + dirs[i].y;

            if(!canGotoGhostHouse && Map::isInGhostHouse(nx, ny)) continue;

            bool isValidTile = false;
            int actualNx = nx;
            if(ny == tunnelRow) {
                if(nx < 0) actualNx = mapWidthTiles - 1;
                else if(nx >= mapWidthTiles) actualNx = 0;
                isValidTile = map.isWalkable(actualNx, ny);
            } else {
                if(nx>=0 && nx<mapWidthTiles && ny>=0 && ny<mapHeightTiles) {
                    actualNx = nx;
                    isValidTile = map.isWalkable(actualNx, ny);
                }
            }
            if(!isValidTile) continue;

            int dx = actualNx - adjustedTargetX;
            int dy = ny - adjustedTargetY;

            if(ny == tunnelRow) {
                int normalDist = abs(dx);
                int wrapDist = mapWidthTiles - normalDist;
                if(wrapDist < normalDist) dx = (dx>0)? -wrapDist : wrapDist;
            }

            int dist = dx*dx + dy*dy;
            if(dist < bestDist) {
                bestDist = dist;
                bestDir = dir;
            }
        }
        if(bestDir == STOP && reverseDir != STOP) bestDir = reverseDir;
        currentDirection = bestDir;
    }

    if(currentDirection != STOP) {
        switch(currentDirection) {
            case UP: posY -= speed; currentEye = eyeUp; break;
            case DOWN: posY += speed; currentEye = eyeDown; break;
            case LEFT: posX -= speed; currentEye = eyeLeft; break;
            case RIGHT: posX += speed; currentEye = eyeRight; break;
        }
        pixelsMoved += speed;
    }

    if(pixelsMoved >= tileSize) {
        pixelsMoved = 0.0f;
        currentTile.x = static_cast<int>((posX + 8) / tileSize);
        currentTile.y = static_cast<int>((posY + 8 - 3*tileSize) / tileSize);
        posX = currentTile.x * tileSize + 8 - w/2;
        posY = currentTile.y * tileSize + 3*tileSize + 8 - h/2;
    }

    int mapWidth = mapWidthTiles * static_cast<int>(tileSize);
    if(posX + w/2 < 0) posX = mapWidth - w/2;
    else if(posX + w/2 >= mapWidth) posX = -w/2;

    rect.x = static_cast<int>(posX);
    rect.y = static_cast<int>(posY);
    updateHitbox();
}

//---------------- Update Frightened ----------------
void Ghost::updateFrightened(const Map& map) {
    const int mapWidthTiles = 28;
    const int mapHeightTiles = 31;
    const float tileSize = 16.0f;
    const int tunnelRow = 14;

    if(pixelsMoved == 0.0f) {
        currentTile.x = static_cast<int>((posX + 8) / tileSize);
        currentTile.y = static_cast<int>((posY + 8 - 3*tileSize) / tileSize);

        Direction reverseDir;
        switch(currentDirection) {
            case UP: reverseDir = DOWN; break;
            case DOWN: reverseDir = UP; break;
            case LEFT: reverseDir = RIGHT; break;
            case RIGHT: reverseDir = LEFT; break;
            default: reverseDir = STOP; break;
        }

        std::vector<Direction> possibleDirs;
        const SDL_Point dirs[4] = { {0,-1}, {-1,0}, {0,1}, {1,0} };
        Direction dirMap[4] = { UP, LEFT, DOWN, RIGHT };

        for(int i=0;i<4;i++) {
            Direction dir = dirMap[i];
            int nx = currentTile.x + dirs[i].x;
            int ny = currentTile.y + dirs[i].y;
            if(dir == reverseDir) continue;
            if(!canGotoGhostHouse && Map::isInGhostHouse(nx, ny)) continue;

            bool isValidTile = false;
            int actualNx = nx;
            if(ny == tunnelRow) {
                if(nx<0) actualNx = mapWidthTiles-1;
                else if(nx>=mapWidthTiles) actualNx = 0;
                isValidTile = map.isWalkable(actualNx, ny);
            } else {
                if(nx>=0 && nx<mapWidthTiles && ny>=0 && ny<mapHeightTiles) {
                    actualNx = nx;
                    isValidTile = map.isWalkable(actualNx, ny);
                }
            }
            if(isValidTile) possibleDirs.push_back(dir);
        }

        if(!possibleDirs.empty()) {
            int idx = rand() % possibleDirs.size();
            currentDirection = possibleDirs[idx];
        } else if(reverseDir != STOP) currentDirection = reverseDir;
    }

    if(currentDirection != STOP) {
        switch(currentDirection) {
            case UP: posY -= speed; currentEye = eyeUp; break;
            case DOWN: posY += speed; currentEye = eyeDown; break;
            case LEFT: posX -= speed; currentEye = eyeLeft; break;
            case RIGHT: posX += speed; currentEye = eyeRight; break;
        }
        pixelsMoved += speed;
    }

    if(pixelsMoved >= tileSize) {
        pixelsMoved = 0.0f;
        currentTile.x = static_cast<int>((posX + 8) / tileSize);
        currentTile.y = static_cast<int>((posY + 8 - 3*tileSize) / tileSize);
        posX = currentTile.x*tileSize + 8 - w/2;
        posY = currentTile.y*tileSize + 3*tileSize + 8 - h/2;
    }

    int mapWidth = mapWidthTiles * static_cast<int>(tileSize);
    if(posX + w/2 < 0) posX = mapWidth - w/2;
    else if(posX + w/2 >= mapWidth) posX = -w/2;

    rect.x = static_cast<int>(posX);
    rect.y = static_cast<int>(posY);
    updateHitbox();
}

//---------------- Render ----------------
void Ghost::render(SDL_Renderer* renderer) {
    if(showingScore) { renderScore(renderer); return; }

    SDL_Texture* texToRender = (bodyFrame==0)? bodyTex1: bodyTex2;
    SDL_Texture* frightenedToRender = (bodyFrame==0)? frightenedTex: frightenedTex2;
    SDL_Texture* endFrightenedToRender = (bodyFrame==0)? endFrightened: endFrightened2;
    SDL_Texture* endFrightToRender = (bodyFrame==0)? frightenedToRender:endFrightenedToRender;

    if(getState() != FRIGHTENED && getState() != EATEN) {
        if(texToRender) SDL_RenderCopy(renderer, texToRender, nullptr, &rect);
        if(currentEye) SDL_RenderCopy(renderer, currentEye, nullptr, &rect);
    }
    if(getState() == FRIGHTENED) {
        if(frightenedToRender) SDL_RenderCopy(renderer, frightenedToRender, nullptr, &rect);
        if(endOfFrightening) SDL_RenderCopy(renderer, endFrightToRender, nullptr, &rect);
    }
    if(getState() == EATEN) {
        if(currentEye) SDL_RenderCopy(renderer, currentEye, nullptr, &rect);
    }
    renderTarget(renderer);
}

//---------------- Load Textures ----------------
bool Ghost::loadTextures(TextureManager* texManager, const std::vector<std::string>& paths) {
    if(paths.size() < 10) return false;

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

    if(paths.size()>=14){
        scoreTexture200 = texManager->loadTexture(paths[10]);
        scoreTexture400 = texManager->loadTexture(paths[11]);
        scoreTexture800 = texManager->loadTexture(paths[12]);
        scoreTexture1600 = texManager->loadTexture(paths[13]);
    }

    bool mainLoaded = eyeUp && eyeDown && eyeLeft && eyeRight &&
                      bodyTex1 && bodyTex2 &&
                      frightenedTex && frightenedTex2 &&
                      endFrightened && endFrightened2;

    if(paths.size()>=14){
        return mainLoaded && scoreTexture200 && scoreTexture400 &&
               scoreTexture800 && scoreTexture1600;
    }
    return mainLoaded;
}
// -------------------- Target Handling --------------------
bool Ghost::loadTargetTexture(SDL_Renderer* renderer, const std::string& path) {
    if(targetTexture) SDL_DestroyTexture(targetTexture);
    targetTexture = SDL_CreateTextureFromSurface(renderer, SDL_LoadBMP(path.c_str()));
    return targetTexture != nullptr;
}

void Ghost::renderTarget(SDL_Renderer* renderer) {
    if(targetTexture) SDL_RenderCopy(renderer, targetTexture, nullptr, &rect);
}

void Ghost::clearTargetTexture() {
    if(targetTexture) {
        SDL_DestroyTexture(targetTexture);
        targetTexture = nullptr;
    }
}

// -------------------- State & Mode --------------------
void Ghost::setState(GhostState ghostState) {
    state = ghostState;
    updateHitbox();
}

void Ghost::setMode(GhostState mode) {
    state = mode;
}

// -------------------- Hitbox --------------------
void Ghost::updateHitbox() {
    hitbox.x = static_cast<int>(posX);
    hitbox.y = static_cast<int>(posY);
    hitbox.w = w;
    hitbox.h = h;
}

SDL_Rect* Ghost::getHitBox() { return &hitbox; }

// -------------------- Position / Target --------------------
void Ghost::setPosition(int x, int y) {
    posX = static_cast<float>(x);
    posY = static_cast<float>(y);
    rect.x = x;
    rect.y = y;
    updateHitbox();
}

SDL_Point Ghost::getCurrentTile() const { return currentTile; }

void Ghost::setTarget(const SDL_Rect& targetRect) {
    targetTile.x = targetRect.x / 16;
    targetTile.y = targetRect.y / 16;
}

void Ghost::setTargetTile(int tileX, int tileY) {
    targetTile.x = tileX;
    targetTile.y = tileY;
}

void Ghost::updateBodyAnimation() {
    frameCounter++;
    if(frameCounter >= frameSpeed) {
        frameCounter = 0;
        bodyFrame = (bodyFrame + 1) % 2;
    }
    bodyTex = (bodyFrame == 0) ? bodyTex1 : bodyTex2;
}
