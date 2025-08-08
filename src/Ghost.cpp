#include "../include/Ghost.h"
#include "../include/TextureManager.h"
#include <cmath>
#include "../include/Map.h"
#include <queue>
#include <vector>
#include "../include/Direction.h"
#include <SDL2/SDL.h>

Ghost::Ghost(int tileX, int tileY, int w, int h) : speed(2), pixelsMoved(0)
{
    state = WAIT;
    int pixelX = tileX * 16 + 8 - w / 2;
    int pixelY = tileY * 16 + 3*16 + 8 - h / 2;
    rect = {pixelX -8 , pixelY + 8, w, h};
    updateHitbox();
    targetTexture = nullptr;

    currentTile = {tileX, tileY};
    targetTile = currentTile;
    currentDirection = STOP;

    printf("Ghost created at tile (%d,%d), pixel (%d,%d)\n",
           tileX, tileY, pixelX, pixelY);
}

void Ghost::wait() {
    const int tileSize = 16;

    if(currentDirection == STOP) {
        currentDirection = UP;
    }

    // حرکت طبق جهت فعلی
    switch (currentDirection) {
        case UP:    rect.y -= speed / 2; currentTex = texUp; break;
        case DOWN:  rect.y += speed / 2; currentTex = texDown; break;
        default: break;
    }

    pixelsMoved += speed / 2;

    if(pixelsMoved >= tileSize) {
        pixelsMoved = 0;
        if(currentDirection == UP) currentDirection = DOWN;
        else if(currentDirection == DOWN) currentDirection = UP;
    }

    updateHitbox();
}
void Ghost::update(const Map& map) {
    switch (state) {
        case WAIT:
            wait();
            break;
        case EXIT:
            getOutOfHouse(map);
            break;
        case CHASE:
        case SCATTER:
            updateChaseScatter(map);
            break;
    }
}

void Ghost::getOutOfHouse(const Map& map) {
    const int tileSize = 16;
    const int exitTileX = 14;  // تایل خروج
    const int exitTileY = 11;  // بالای ghost house

    int currentTileX = (rect.x + rect.w / 2) / tileSize;
    int currentTileY = (rect.y + rect.h / 2 - 3 * tileSize) / tileSize;

    // هدف نهایی
    SDL_Point exitTile = {exitTileX, exitTileY};

    // جهت حرکت به سمت exit (اول عمودی، بعد افقی)
    if(currentTileY > exitTileY) {
        // قصد حرکت به بالا
        int nextTileY = currentTileY - 1;
        if(map.isWalkable(currentTileX, nextTileY)) {
            rect.y -= speed;
            currentDirection = UP;
            currentTex = texUp;
        } else {
            // اگر بالایی راه نیست، سعی کن افقی حرکت کنی
            if(currentTileX < exitTileX) {
                int nextTileX = currentTileX + 1;
                if(map.isWalkable(nextTileX, currentTileY)) {
                    rect.x += speed;
                    currentDirection = RIGHT;
                    currentTex = texRight;
                }
            } else if(currentTileX > exitTileX) {
                int nextTileX = currentTileX - 1;
                if(map.isWalkable(nextTileX, currentTileY)) {
                    rect.x -= speed;
                    currentDirection = LEFT;
                    currentTex = texLeft;
                }
            }
        }
    } else if(currentTileX < exitTileX) {
        // حرکت به راست
        int nextTileX = currentTileX + 1;
        if(map.isWalkable(nextTileX, currentTileY)) {
            rect.x += speed;
            currentDirection = RIGHT;
            currentTex = texRight;
        }
    } else if(currentTileX > exitTileX) {
        // حرکت به چپ
        int nextTileX = currentTileX - 1;
        if(map.isWalkable(nextTileX, currentTileY)) {
            rect.x -= speed;
            currentDirection = LEFT;
            currentTex = texLeft;
        }
    }

    // اگر به تایل خروج رسید
    if(currentTileX == exitTileX && currentTileY == exitTileY) {
        state = CHASE;
        printf("Ghost exited house -> CHASE\n");
    }

    updateHitbox();
}




bool Ghost::loadTextures(TextureManager* texManager,
                         const std::string& upPath,
                         const std::string& downPath,
                         const std::string& leftPath,
                         const std::string& rightPath)
{
    texUp    = texManager->loadTexture(upPath);
    texDown  = texManager->loadTexture(downPath);
    texLeft  = texManager->loadTexture(leftPath);
    texRight = texManager->loadTexture(rightPath);

    currentTex = texDown;
    return texUp && texDown && texLeft && texRight;
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
    if (pixelsMoved == 0) {
        int rawTileX = (rect.x + 8) / tileSize;
        currentTile.x = ((rawTileX % mapWidthTiles) + mapWidthTiles) % mapWidthTiles;
        currentTile.y = (rect.y + 8 - 3*tileSize) / tileSize;

        int adjustedTargetX = targetTile.x;
        int adjustedTargetY = targetTile.y;
        const int tunnelRow = 14;

        bool targetInTunnel = (targetTile.y == tunnelRow) &&
                              (targetTile.x == 0 || targetTile.x == mapWidthTiles - 1);

        if (targetInTunnel) {
            if (targetTile.x == 0) {
                adjustedTargetX = mapWidthTiles - 1;
            }
            else if (targetTile.x == mapWidthTiles - 1) {
                adjustedTargetX = 0;
            }
        }
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

            bool isValidTile = false;
            int actualNx = nx;
            if (ny == tunnelRow) {
                if (nx < 0) {
                    actualNx = mapWidthTiles - 1;
                } else if (nx >= mapWidthTiles) {
                    actualNx = 0;
                } else {
                    actualNx = nx;
                }
                if (ny >= 0 && ny < mapHeightTiles) {
                    isValidTile = map.isWalkable(actualNx, ny);
                }
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
                if (wrapDist < normalDist) {
                    dx = (dx > 0) ? -wrapDist : wrapDist;
                }
            }

            int dist = dx * dx + dy * dy;

            if (dist < bestDist) {
                bestDist = dist;
                bestDir = dir;
            }
        }
        if (bestDir == STOP && reverseDir != STOP) {
            bestDir = reverseDir;
        }

        currentDirection = bestDir;
    }

    if (currentDirection != STOP) {
        switch (currentDirection) {
            case UP:    rect.y -= speed; currentTex = texUp; break;
            case DOWN:  rect.y += speed; currentTex = texDown; break;
            case LEFT:  rect.x -= speed; currentTex = texLeft; break;
            case RIGHT: rect.x += speed; currentTex = texRight; break;
            default: break;
        }
        pixelsMoved += speed;
    }

    if (pixelsMoved >= tileSize) {
        pixelsMoved = 0;

        int rawNewTileX = (rect.x + 8) / tileSize;
        int newTileX = ((rawNewTileX % mapWidthTiles) + mapWidthTiles) % mapWidthTiles;
        int newTileY = (rect.y + 8 - 3*tileSize) / tileSize;

        rect.x = newTileX * tileSize + 8 - rect.w / 2;
        rect.y = newTileY * tileSize + 3*tileSize + 8 - rect.h / 2;

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


void Ghost::render(SDL_Renderer* renderer) {
    if(currentTex) SDL_RenderCopy(renderer, currentTex, nullptr, &rect);
    renderTarget(renderer);
//    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
//    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 100);
//    SDL_RenderFillRect(renderer, &hitbox);
}

void Ghost::updateHitbox()
{
    int hbSize = 16;
    hitbox.w = hbSize;
    hitbox.h = hbSize;
    hitbox.x = rect.x + (rect.w - hbSize) / 2;
    hitbox.y = rect.y + (rect.h - hbSize) / 2;
}

void Ghost::setPosition(int x, int y)
{
    rect.x = x;
    rect.y = y;
    updateHitbox();
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
