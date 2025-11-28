#include "../include/Inky.h"
#include "../include/Map.h"

Inky::Inky()
        : Ghost(12, 14, 28, 28) {
    scatterCorner = {27,30};
}

void Inky::update(const Pacman& pacman, const Ghost& blinky, const Map& map) {
    switch(state) {
        case WAIT:
            wait();
            if(readyToExit) state = EXIT;
            break;

        case EXIT:
            getOutOfHouse(map);
            break;

        case CHASE: {
            int blinkyX = blinky.getCurrentTile().x;
            int blinkyY = blinky.getCurrentTile().y;
            int centerX = 0, centerY = 0;

            switch (pacman.getDirection()) {
                case UP:
                    centerX = (pacman.getHitbox().x + pacman.getHitbox().w / 2) / 16 - 2;
                    centerY = (pacman.getHitbox().y + pacman.getHitbox().h / 2 - 3*16) / 16 - 2;
                    break;
                case DOWN:
                    centerX = (pacman.getHitbox().x + pacman.getHitbox().w / 2) / 16;
                    centerY = (pacman.getHitbox().y + pacman.getHitbox().h / 2 - 3*16) / 16 + 2;
                    break;
                case LEFT:
                    centerX = (pacman.getHitbox().x + pacman.getHitbox().w / 2) / 16 - 2;
                    centerY = (pacman.getHitbox().y + pacman.getHitbox().h / 2 - 3*16) / 16;
                    break;
                case RIGHT:
                    centerX = (pacman.getHitbox().x + pacman.getHitbox().w / 2) / 16 + 2;
                    centerY = (pacman.getHitbox().y + pacman.getHitbox().h / 2 - 3*16) / 16;
                    break;
                default: break;
            }

            int vecX = centerX - blinkyX;
            int vecY = centerY - blinkyY;

            int targetX = centerX + vecX;
            int targetY = centerY + vecY;
            setTargetTile(targetX, targetY);

            Ghost::update(map);
            break;
        }

        case SCATTER:
            setTargetTile(scatterCorner.x, scatterCorner.y);
            Ghost::update(map);
            break;
        case FRIGHTENED :
            Ghost::update(map);
            break;
        case EATEN:
            setTargetTile(13, 14);

            Ghost::update(map);
            break;

    }
}
