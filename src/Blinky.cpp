
#include "../include/Blinky.h"
#include "../include/Map.h"

Blinky::Blinky()
        : Ghost(14, 10.5, 28, 28) {
    scatterCorner = {27,0};
    state = SCATTER;
}

void Blinky::update(const Pacman& pacman, const Map& map) {
    switch(state) {
        case WAIT:
            state = CHASE;
            break;

        case EXIT:
            getOutOfHouse(map);
            break;

        case CHASE:
            setTarget(pacman.getHitbox());
            Ghost::update(map);
            break;

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
