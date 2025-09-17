#include "../include/Clyde.h"
#include "../include/Map.h"

Clyde::Clyde(int x, int y, int w, int h)
        : Ghost(x, y, w, h) {
    scatterCorner = {0,30};
}

void Clyde::update(const Pacman& pacman, const Map& map) {

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
            SDL_Point clydeTile = getCurrentTile();

            int dx = pacTile.x - clydeTile.x;
            int dy = pacTile.y - clydeTile.y;
            int distanceSquared = dx*dx + dy*dy;

            if(distanceSquared >= 64) {
                setTarget(pacman.getHitbox());
            } else {
                setTargetTile(scatterCorner.x, scatterCorner.y);
            }

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
