#include "../include/Pacman.h"
#include "../include/TextureManager.h"

Pacman::Pacman(int x, int y, int w, int h)
        : frameIndex(0), frameDir(1), lastTime(SDL_GetTicks()), dotsEaten(0) {
    rect = {x, y, w, h};
    frames[0] = frames[1] = frames[2] = nullptr;
    updateHitbox();
}
bool Pacman::loadTextures(TextureManager* texManager,
                          const std::string& closePath,
                          const std::string& halfOpenPath,
                          const std::string& openPath)
{
    frames[0] = texManager->loadTexture(closePath);
    frames[1] = texManager->loadTexture(halfOpenPath);
    frames[2] = texManager->loadTexture(openPath);
    return frames[0] && frames[1] && frames[2];
}

void Pacman::update() {
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastTime >= 50) {
        frameIndex += frameDir;
        if(frameIndex == 2) frameDir = -1;
        else if(frameIndex == 0) frameDir = 1;
        lastTime = currentTime;
    }
    updateHitbox();
}

double Pacman::getAngle() const {
    switch(currentDir) {
        case UP:    return 270;
        case DOWN:  return 90;
        case LEFT:  return 180;
        case RIGHT: return 0;
        default:    return 0;
    }
}

void Pacman::render(SDL_Renderer* renderer) {
    SDL_Texture* tex = frames[frameIndex];
    double angle = getAngle();

    if(tex) SDL_RenderCopyEx(renderer, tex, nullptr, &rect, angle, nullptr, SDL_FLIP_NONE);
}


void Pacman::cleanup() {
    for(int i=0;i<3;i++) if(frames[i]) {
            SDL_DestroyTexture(frames[i]);
            frames[i] = nullptr;
        }
}

SDL_Rect Pacman::getHitbox() const { return hitbox; }

void Pacman::setPosition(int x, int y) {
    rect.x = x;
    rect.y = y;
    updateHitbox();
}

void Pacman::updateHitbox() {
    int hbSize = 14;
    hitbox.w = hbSize;
    hitbox.h = hbSize;
    hitbox.x = rect.x + (rect.w - hbSize) / 2;
    hitbox.y = rect.y + (rect.h - hbSize) / 2;

}

#include "../include/Map.h"

void Pacman::handleInput(const SDL_Event& event) {
    if(event.type == SDL_KEYDOWN) {
        switch(event.key.keysym.sym) {
            case SDLK_UP:    nextDir = UP; break;
            case SDLK_DOWN:  nextDir = DOWN; break;
            case SDLK_LEFT:  nextDir = LEFT; break;
            case SDLK_RIGHT: nextDir = RIGHT; break;
        }
    }
}
void Pacman::move(Map* map, int speed) {
    // 1. تست جهت بعدی
    SDL_Rect testHitbox = hitbox;
    switch(nextDir) {
        case UP:    testHitbox.y -= speed; break;
        case DOWN:  testHitbox.y += speed; break;
        case LEFT:  testHitbox.x -= speed; break;
        case RIGHT: testHitbox.x += speed; break;
        default: break;
    }
    if(nextDir != STOP && !map->checkCollision(testHitbox)) {
        currentDir = nextDir;
    }

    // 2. تست جهت فعلی قبل از حرکت
    SDL_Rect nextHitbox = hitbox;
    switch(currentDir) {
        case UP:    nextHitbox.y -= speed; break;
        case DOWN:  nextHitbox.y += speed; break;
        case LEFT:  nextHitbox.x -= speed; break;
        case RIGHT: nextHitbox.x += speed; break;
        default: break;
    }

    // 3. اگه مسیر فعلی خالیه، حرکت کن
    if(currentDir != STOP && !map->checkCollision(nextHitbox)) {
        rect.x = nextHitbox.x - (rect.w - hitbox.w)/2;
        rect.y = nextHitbox.y - (rect.h - hitbox.h)/2;
    }
    SDL_Point tile = getTile();
    if (tile.y >=0 && tile.y < map->mapGrid.size() &&
        tile.x >=0 && tile.x < map->mapGrid[0].size())
    {
        int &cell = map->tileGrid[tile.y][tile.x];

        if (cell == 21) { // small dot
            setDotsEaten(dotsEaten + 1);
            cell = 0;
        }
        else if (cell == 22) { // big dot
            cell = 0;
            // اینجا می‌تونی حالت power رو هم فعال کنی
        }


    }

    // 4. wrap افقی
    int mapWidth = 28 * 16;
    if (rect.x + rect.w < 0) {
        rect.x = mapWidth;
    }
    else if (rect.x > mapWidth) {
        rect.x = -rect.w;
    }

    // 5. آپدیت hitbox
    updateHitbox();
}
SDL_Point Pacman::getTile() const {
    SDL_Point tile;
    tile.x = (hitbox.x + hitbox.w / 2) / 16;
    tile.y = (hitbox.y + hitbox.h / 2 - 3*16) / 16;
    return tile;
}
int Pacman::getDotsEaten() const{
    return dotsEaten;
}
void Pacman::setDotsEaten(int x) {
    dotsEaten = x;
}