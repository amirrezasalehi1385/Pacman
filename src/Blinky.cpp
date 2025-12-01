
#include "Blinky.h"

Blinky::Blinky()
        : Ghost(14, 11, 28, 28) {
    scatterCorner = {27,0};
    state = SCATTER;
}

void Blinky::update(int level,const Pacman& pacman, const Map& map) {
    switch(state) {
        case WAIT:
            state = CHASE;
            break;

        case EXIT:
            getOutOfHouse(map);
            break;

        case CHASE:
            setTarget(pacman.getHitbox());
            Ghost::update(level,map);
            break;

        case SCATTER:
            setTargetTile(scatterCorner.x, scatterCorner.y);
            Ghost::update(level,map);
            break;
        case FRIGHTENED :
            Ghost::update(level,map);
            break;
        case EATEN:
            setTargetTile(13, 14);
            Ghost::update(level,map);
            break;
    }
}
