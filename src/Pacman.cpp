#include "../include/Pacman.h"
#include "../include/WindowManager.h"
#include "../include/TextureManager.h"
#include "../include/Ghost.h"

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
int Pacman::getBigDotsEaten() const{
    return bigDotsEaten;
};
void Pacman::setBigDotsEaten(int x){
    bigDotsEaten = x;
}
void Pacman::updateHitbox() {
    int hbSize = 14;
    hitbox.w = hbSize;
    hitbox.h = hbSize;
    hitbox.x = rect.x + (rect.w - hbSize) / 2;
    hitbox.y = rect.y + (rect.h - hbSize) / 2;

}

void Pacman::handleInput(const SDL_Event& event) {
    if(event.type == SDL_KEYDOWN) {
        switch(event.key.keysym.sym) {
            case SDLK_UP:    nextDir = UP; break;
            case SDLK_DOWN:  nextDir = DOWN; break;
            case SDLK_LEFT:  nextDir = LEFT; break;
            case SDLK_RIGHT: nextDir = RIGHT; break;
        }
    }
}void Pacman::move(Map* map, int speed) {

    SDL_Rect testHitbox = hitbox;
    SDL_Point nextTile = getTile();

    switch(nextDir) {
        case UP:    testHitbox.y -= speed; nextTile.y -= 1; break;
        case DOWN:  testHitbox.y += speed; nextTile.y += 1; break;
        case LEFT:  testHitbox.x -= speed; nextTile.x -= 1; break;
        case RIGHT: testHitbox.x += speed; nextTile.x += 1; break;
        default: break;
    }

    if(nextDir != STOP && !map->checkCollision(testHitbox) && !Map::isInGhostHouse(nextTile.x,nextTile.y)) {
        currentDir = nextDir;
    }

    SDL_Rect nextHitbox = hitbox;
    SDL_Point currNextTile = getTile();

    switch(currentDir) {
        case UP:    nextHitbox.y -= speed; currNextTile.y -= 1; break;
        case DOWN:  nextHitbox.y += speed; currNextTile.y += 1; break;
        case LEFT:  nextHitbox.x -= speed; currNextTile.x -= 1; break;
        case RIGHT: nextHitbox.x += speed; currNextTile.x += 1; break;
        default: break;
    }



    if(currentDir != STOP && !map->checkCollision(nextHitbox) &&
       !Map::isInGhostHouse(currNextTile.x, currNextTile.y))
    {
        rect.x = nextHitbox.x - (rect.w - hitbox.w)/2;
        rect.y = nextHitbox.y - (rect.h - hitbox.h)/2;
    }

    SDL_Point tile = getTile();
    if(tile.y >= 0 && tile.y < map->mapGrid.size() &&
       tile.x >= 0 && tile.x < map->mapGrid[0].size())
    {
        int &cell = map->tileGrid[tile.y][tile.x];
        if(cell == 21) {
            setDotsEaten(dotsEaten + 1);
            cell = 0;
            if(dotsEaten % 5 == 0)   windowManager->playSound("chomp");
        } else if(cell == 22) {
            setBigDotsEaten(dotsEaten + 1);
            cell = 0;
            ateBigDot = true;
        }
    }
//    for (Ghost* g : ghosts) {
//        if(SDL_HasIntersection(&hitbox, g->getHitBox()) && g->getState() == FRIGHTENED) {
//            g->ghostEaten = true;
//            g->setState(EATEN);
//        }
//    }



    // wrap افقی
    int mapWidth = 28 * 16;
    if(rect.x + rect.w < 0) rect.x = mapWidth;
    else if(rect.x > mapWidth) rect.x = -rect.w;

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