#include "../include/Pinky.h"
#include "../include/Map.h"

Pinky::Pinky(int x, int y, int w, int h)
        : Ghost(x, y, w, h) {
    scatterCorner = {0,0};
}

void Pinky::update(const Pacman& pacman, const Map& map) {

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
            Ghost::update(map);
            break;
        }

        case SCATTER:
            setTargetTile(scatterCorner.x, scatterCorner.y);
            Ghost::update(map);
            break;
        case FRIGHTENED :
            Ghost::update(map);
    }
}
