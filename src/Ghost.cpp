#include "Ghost.h"
#include "GameRules.h"
#include <SDL2/SDL.h>
#include <cmath>
#include <vector>

#define TILE_SIZE GameRules::TILE_SIZE
#define MAP_WIDTH_TILE GameRules::MAP_WIDTH_TILES
#define MAP_HEIGHT_TILE GameRules::MAP_HEIGHT_TILES

// ==================== 1. CONSTRUCTOR ====================
Ghost::Ghost(float initX, float initY, int w, int h)
        : initialTileX(initX), initialTileY(initY), w(w), h(h), pixelsMoved(0.0f),
          posX(initX * 16 + 8 - w / 2), posY(initY * 16 + 3*16  - h/2),
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
    currentTile = {initX, initY};
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

// ==================== 2. INITIALIZATION & TEXTURE LOADING ====================
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

bool Ghost::loadTargetTexture(SDL_Renderer* renderer, const std::string& path) {
    if(targetTexture) {
        SDL_DestroyTexture(targetTexture);
        targetTexture = nullptr;
    }

    SDL_Surface* surf = IMG_Load(path.c_str());
    if(!surf) {
        SDL_Log("Failed to load target image %s: %s", path.c_str(), IMG_GetError());
        return false;
    }

    targetTexture = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);

    if(!targetTexture) {
        SDL_Log("Failed to create texture from %s: %s", path.c_str(), SDL_GetError());
        return false;
    }

    return true;
}
void Ghost::clearTargetTexture() {
    if(targetTexture) {
        SDL_DestroyTexture(targetTexture);
        targetTexture = nullptr;
    }
}

// ==================== 3. STATE MANAGEMENT ====================
void Ghost::setState(GhostState ghostState) {
    state = ghostState;
    updateHitbox();
}
void Ghost::setMode(GhostState mode) {
    state = mode;
}
void Ghost::reset() {
    visible = true;
    endOfFrightening = false;
    canGotoGhostHouse = true;
    readyToExit = false;
    ghostEaten = false;
    state = WAIT;
    pixelsMoved = 0.0f;
    currentDirection = STOP;
    speed = normalSpeed;
    posX = initialTileX * 16 + 8 - w/2;
    posY = initialTileY * 16 + 3*16  - h/2;
    rect = {static_cast<int>(posX) - 8, static_cast<int>(posY) + 8, w, h};
    updateHitbox();
    currentTile = {initialTileX, initialTileY};
    targetTile = currentTile;
}

// ==================== 4. MAIN UPDATE ====================
void Ghost::update(int level, const Map& map) {
    if(frozen || isFrozenForScore) return;

    switch(state) {
        case WAIT:
            wait();
            if(readyToExit) state = EXIT;
            break;

        case EXIT:
            speed = normalSpeed;
            getOutOfHouse(map);
            canGotoGhostHouse = true;
            break;

        case CHASE:
        case SCATTER:
            speed = GameRules::getGhostSpeed(level);
            canGotoGhostHouse = false;
            updateChaseScatter(map);
            break;

        case FRIGHTENED:
            speed = GameRules::getFrightenedSpeed(level);
            canGotoGhostHouse = false;
            updateFrightened(map);
            break;

        case EATEN:
            speed = GameRules::GHOST_SPEED_EATEN;
            canGotoGhostHouse = true;
            updateChaseScatter(map);
            if(currentTile.x == 13 && currentTile.y == 14) {
                speed = GameRules::getGhostSpeed(level);
                setMode(EXIT);
            }
            break;
    }
    updateBodyAnimation();
}

// ==================== 5. STATE-SPECIFIC UPDATES ====================
bool flag = true;
void Ghost::wait() {
    if(currentDirection == STOP) { currentDirection = UP; }
    switch (currentDirection) {
        case UP: rect.y -= 1; currentEye = eyeUp; break;
        case DOWN: rect.y += 1; currentEye = eyeDown; break;
        default: break;
    }
    pixelsMoved += 1;
    if(pixelsMoved >= TILE_SIZE) {
        pixelsMoved = 0;
        if(currentDirection == UP)
            currentDirection = DOWN;
        else if(currentDirection == DOWN)
            currentDirection = UP;
    }
    updateHitbox();
}
void Ghost::getOutOfHouse(const Map& map) {
    const int exitTileX = 14;
    const int exitTileY = 11;
    const int offsetX = -8;

    if(flag) {
        posX = currentTile.x * TILE_SIZE + 8 - w/2 + offsetX;
        posY = currentTile.y * TILE_SIZE + 3*TILE_SIZE + 8 - h/2;
        flag = false;
    }

    if(pixelsMoved == 0.0f) {
        int rawTileX = static_cast<int>((posX + 8 - offsetX) / TILE_SIZE);
        currentTile.x = ((rawTileX % MAP_WIDTH_TILE) + MAP_WIDTH_TILE) % MAP_WIDTH_TILE;
        currentTile.y = static_cast<int>((posY + 8 - 3 * TILE_SIZE) / TILE_SIZE);

        Direction nextDir = STOP;
        if(currentTile.x < exitTileX) nextDir = RIGHT;
        else if(currentTile.x > exitTileX) nextDir = LEFT;
        else if(currentTile.y > exitTileY) nextDir = UP;
        else if(currentTile.y == exitTileY) {
            state = SCATTER;
            flag = true;
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

    if(pixelsMoved >= TILE_SIZE) {
        pixelsMoved = 0.0f;
        currentTile.x = static_cast<int>((posX + 8 - offsetX) / TILE_SIZE);
        currentTile.y = static_cast<int>((posY + 8 - 3*TILE_SIZE) / TILE_SIZE);
        posX = currentTile.x * TILE_SIZE + 8 - w/2 + offsetX;
        posY = currentTile.y * TILE_SIZE + 3*TILE_SIZE + 8 - h/2;
    }

    rect.x = static_cast<int>(posX);
    rect.y = static_cast<int>(posY);
    updateHitbox();
}
void Ghost::updateChaseScatter(const Map& map) {
    if(pixelsMoved == 0.0f) {
        updateCurrentTile();
        currentDirection = findBestDirection(map);
    }

    if(currentDirection != STOP) {
        moveInDirection(currentDirection);
    }

    updatePositionAfterMove();
}
void Ghost::updateFrightened(const Map& map) {
    if(pixelsMoved == 0.0f) {
        updateCurrentTile();
        currentDirection = getRandomValidDirection(map);
    }

    if(currentDirection != STOP) {
        moveInDirection(currentDirection);
    }

    updatePositionAfterMove();
}
void Ghost::updateBodyAnimation() {
    frameCounter++;
    if(frameCounter >= frameSpeed) {
        frameCounter = 0;
        bodyFrame = (bodyFrame + 1) % 2;
    }
    bodyTex = (bodyFrame == 0) ? bodyTex1 : bodyTex2;
}

// ==================== 6. MOVEMENT HELPERS ====================
void Ghost::moveInDirection(Direction dir) {
    switch(dir) {
        case UP:    posY -= speed; currentEye = eyeUp; break;
        case DOWN:  posY += speed; currentEye = eyeDown; break;
        case LEFT:  posX -= speed; currentEye = eyeLeft; break;
        case RIGHT: posX += speed; currentEye = eyeRight; break;
        default: break;
    }
    pixelsMoved += speed;
}
void Ghost::updatePositionAfterMove() {
    if(pixelsMoved >= TILE_SIZE) {
        pixelsMoved = 0.0f;
        currentTile.x = static_cast<int>((posX + 8) / TILE_SIZE);
        currentTile.y = static_cast<int>((posY + 8 - 3*TILE_SIZE) / TILE_SIZE);
        posX = currentTile.x * TILE_SIZE + 8 - w/2;
        posY = currentTile.y * TILE_SIZE + 3*TILE_SIZE + 8 - h/2;
    }

    // Handle tunnel wrapping
    int mapWidth = MAP_WIDTH_TILE * static_cast<int>(TILE_SIZE);
    if(posX + w/2 < 0) posX = mapWidth - w/2;
    else if(posX + w/2 >= mapWidth) posX = -w/2;

    rect.x = static_cast<int>(posX);
    rect.y = static_cast<int>(posY);
    updateHitbox();
}
void Ghost::updateCurrentTile() {
    currentTile.x = static_cast<int>((posX + 8) / TILE_SIZE);
    currentTile.y = static_cast<int>((posY + 8 - 3*TILE_SIZE) / TILE_SIZE);
}
Direction Ghost::getReverseDirection() const {
    switch(currentDirection) {
        case UP: return DOWN;
        case DOWN: return UP;
        case LEFT: return RIGHT;
        case RIGHT: return LEFT;
        default: return STOP;
    }
}
bool Ghost::isValidMove(int nx, int ny, const Map& map) const {
    if(!canGotoGhostHouse && Map::isInGhostHouse(nx, ny))
        return false;

    int actualNx = nx;
    if(ny == GameRules::TUNNEL_ROW) {
        if(nx < 0) actualNx = MAP_WIDTH_TILE - 1;
        else if(nx >= MAP_WIDTH_TILE) actualNx = 0;
        return map.isWalkable(actualNx, ny);
    } else {
        if(nx >= 0 && nx < MAP_WIDTH_TILE && ny >= 0 && ny < MAP_HEIGHT_TILE) {
            return map.isWalkable(nx, ny);
        }
    }
    return false;
}
int Ghost::calculateDistance(int fromX, int fromY, int toX, int toY) const {
    int dx = fromX - toX;
    int dy = fromY - toY;

    // Handle tunnel wrapping
    if(fromY == GameRules::TUNNEL_ROW) {
        int normalDist = abs(dx);
        int wrapDist = MAP_WIDTH_TILE - normalDist;
        if(wrapDist < normalDist) {
            dx = (dx > 0) ? -wrapDist : wrapDist;
        }
    }

    return dx * dx + dy * dy;
}
Direction Ghost::findBestDirection(const Map& map) {
    int adjustedTargetX = targetTile.x;
    int adjustedTargetY = targetTile.y;

    // Handle tunnel target adjustment
    bool targetInTunnel = (targetTile.y == GameRules::TUNNEL_ROW) &&
                          (targetTile.x == 0 || targetTile.x == MAP_WIDTH_TILE - 1);
    if(targetInTunnel) {
        adjustedTargetX = (targetTile.x == 0) ? MAP_WIDTH_TILE - 1 : 0;
    }

    Direction reverseDir = getReverseDirection();

    const SDL_Point dirs[4] = { {0,-1}, {-1,0}, {0,1}, {1,0} };
    Direction dirMap[4] = { UP, LEFT, DOWN, RIGHT };
    Direction bestDir = STOP;
    int bestDist = INT_MAX;

    for(int i = 0; i < 4; i++) {
        Direction dir = dirMap[i];
        if(dir == reverseDir) continue;

        int nx = currentTile.x + dirs[i].x;
        int ny = currentTile.y + dirs[i].y;

        if(!isValidMove(nx, ny, map)) continue;

        int actualNx = (ny == GameRules::TUNNEL_ROW && nx < 0) ? MAP_WIDTH_TILE - 1 :
                       (ny == GameRules::TUNNEL_ROW && nx >= MAP_WIDTH_TILE) ? 0 : nx;

        int dist = calculateDistance(actualNx, ny, adjustedTargetX, adjustedTargetY);

        if(dist < bestDist) {
            bestDist = dist;
            bestDir = dir;
        }
    }

    return (bestDir == STOP && reverseDir != STOP) ? reverseDir : bestDir;
}
Direction Ghost::getRandomValidDirection(const Map& map) {
    Direction reverseDir = getReverseDirection();
    std::vector<Direction> possibleDirs;

    const SDL_Point dirs[4] = { {0,-1}, {-1,0}, {0,1}, {1,0} };
    Direction dirMap[4] = { UP, LEFT, DOWN, RIGHT };

    for(int i = 0; i < 4; i++) {
        Direction dir = dirMap[i];
        if(dir == reverseDir) continue;

        int nx = currentTile.x + dirs[i].x;
        int ny = currentTile.y + dirs[i].y;

        if(isValidMove(nx, ny, map)) {
            possibleDirs.push_back(dir);
        }
    }

    if(!possibleDirs.empty()) {
        int idx = rand() % possibleDirs.size();
        return possibleDirs[idx];
    }

    return (reverseDir != STOP) ? reverseDir : STOP;
}

// ==================== 7. COLLISION & INTERACTION ====================
bool Ghost::ghostInGhostHouse() {
    const SDL_Rect ghostHouseTiles = {11, 16, 8, 5};
    SDL_Rect ghostHouse = {
            ghostHouseTiles.x * TILE_SIZE,
            ghostHouseTiles.y * TILE_SIZE,
            ghostHouseTiles.w * TILE_SIZE,
            ghostHouseTiles.h * TILE_SIZE
    };
    SDL_Rect ghostRect = rect;
    return (ghostRect.x + ghostRect.w > ghostHouse.x &&
            ghostRect.x < ghostHouse.x + ghostHouse.w &&
            ghostRect.y + ghostRect.h > ghostHouse.y &&
            ghostRect.y < ghostHouse.y + ghostHouse.h);
}
bool Ghost::checkCollisionWithPacman(Pacman* pacman) {
    if(showingScore) return false;

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

// ==================== 8. SCORE SYSTEM ====================
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
        case 200:  scoreTex = scoreTexture200;  break;
        case 400:  scoreTex = scoreTexture400;  break;
        case 800:  scoreTex = scoreTexture800;  break;
        case 1600: scoreTex = scoreTexture1600; break;
    }

    if(scoreTex) {
        SDL_Rect dstRect = rect;
        SDL_RenderCopy(renderer, scoreTex, nullptr, &dstRect);
    }
}

// ==================== 9. RENDERING ====================
void Ghost::render(SDL_Renderer* renderer) {
    if(!visible) return;

    if(showingScore) {
        if(!Map::isInGhostHouse(currentTile.x, currentTile.y)) {
            renderScore(renderer);
        }
        return;
    }

    SDL_Texture* texToRender = (bodyFrame == 0) ? bodyTex1 : bodyTex2;
    SDL_Texture* frightenedToRender = (bodyFrame == 0) ? frightenedTex : frightenedTex2;
    SDL_Texture* endFrightenedToRender = (bodyFrame == 0) ? endFrightened : endFrightened2;
    SDL_Texture* endFrightToRender = (bodyFrame == 0) ? frightenedToRender : endFrightenedToRender;

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
}
void Ghost::renderTarget(SDL_Renderer* renderer) {
    if (!targetTexture) return;

    SDL_Rect targetRect;
    targetRect.x = static_cast<int>(targetTile.x * TILE_SIZE) + TILE_SIZE / 4;
    targetRect.y = static_cast<int>(targetTile.y * TILE_SIZE) + TILE_SIZE / 4;
    targetRect.w = TILE_SIZE / 2;
    targetRect.h = TILE_SIZE / 2;

    int ghostCenterX = rect.x + rect.w / 2;
    int ghostCenterY = rect.y + rect.h / 2;
    int targetCenterX = targetRect.x + targetRect.w / 2;
    int targetCenterY = targetRect.y + targetRect.h / 2;

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(renderer, ghostCenterX, ghostCenterY, targetCenterX, targetCenterY);
    SDL_RenderDrawLine(renderer, ghostCenterX, ghostCenterY+1, targetCenterX, targetCenterY+1);

    SDL_RenderCopy(renderer, targetTexture, nullptr, &targetRect);
}
void Ghost::renderHitbox(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &hitbox);
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderDrawRect(renderer, &rect);
}

// ==================== 10. SETTERS & GETTERS ====================
void Ghost::setPosition(int x, int y) {
    posX = static_cast<float>(x);
    posY = static_cast<float>(y);
    rect.x = x;
    rect.y = y;
    updateHitbox();
}
void Ghost::setTarget(const SDL_Rect& targetRect) {
    targetTile.x = targetRect.x / 16;
    targetTile.y = targetRect.y / 16;
}
void Ghost::setTargetTile(float tileX, float tileY) {
    targetTile.x = tileX;
    targetTile.y = tileY;
}
SDL_FPoint Ghost::getCurrentTile() const {
    return currentTile;
}
SDL_Rect* Ghost::getHitBox() {
    return &hitbox;
}
void Ghost::updateHitbox() {
    hitbox = rect;
}