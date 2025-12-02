#include "Clyde.h"

Clyde::Clyde()
        : Ghost(16, 14, 28, 28) {
    scatterCorner = {0,33};
}

void Clyde::update(int level,const Pacman& pacman, const Map& map) {

    switch(state) {
        case WAIT:
            if(readyToExit) state = EXIT;
            wait();
            break;

        case EXIT:
            getOutOfHouse(map);
            break;

        case CHASE: {
            SDL_Point pacTile = pacman.getTile();
            SDL_FPoint clydeTile = getCurrentTile();

            int dx = pacTile.x - clydeTile.x;
            int dy = pacTile.y - clydeTile.y;
            int distanceSquared = dx*dx + dy*dy;

            if(distanceSquared >= 64) {
                setTarget(pacman.getHitbox());
            } else {
                setTargetTile(scatterCorner.x, scatterCorner.y);
            }

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
