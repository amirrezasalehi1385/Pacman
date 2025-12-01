#include "Pinky.h"

Pinky::Pinky()
        : Ghost(14, 14, 28, 28) {
    scatterCorner = {0,0};
}

void Pinky::update(int level, const Pacman& pacman, const Map& map) {

    switch(state) {
        case WAIT:
            wait();
            if(readyToExit) state = EXIT;
            break;

        case EXIT:
            getOutOfHouse(map);
            break;

        case CHASE: {
            SDL_Rect pacRect = pacman.getHitbox();
            SDL_Rect targetRect = pacRect;

            int offset = 4 * 16;

            switch (pacman.getDirection()) {
                case UP:
                    targetRect.y -= offset;
                    targetRect.x -= offset;
                    break;
                case DOWN:
                    targetRect.y += offset;
                    break;
                case LEFT:
                    targetRect.x -= offset;
                    break;
                case RIGHT:
                    targetRect.x += offset;
                    break;
                default: break;
            }

            setTarget(targetRect);
            Ghost::update(level,map);
            break;
        }

        case SCATTER:
            setTargetTile(scatterCorner.x, scatterCorner.y);
            Ghost::update(level,map);
            break;
        case FRIGHTENED :
            Ghost::update(level,map);
            break;
        case EATEN:
            setTargetTile(13.5, 14);
            Ghost::update(level,map);
            break;
    }
}
