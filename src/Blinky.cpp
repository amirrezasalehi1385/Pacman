
#include "../include/Blinky.h"
#include "../include/Map.h"

Blinky::Blinky(int x, int y, int w, int h)
        : Ghost(x, y, w, h) {
    scatterCorner = {27,0};
    state = SCATTER;
}

void Blinky::update(const Pacman& pacman, const Map& map) {
    switch(state) {
        case WAIT:
            state = CHASE;
            break;

        case EXIT:
            state = CHASE;
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
    }
}
