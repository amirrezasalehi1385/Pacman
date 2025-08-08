
#include "../include/Blinky.h"
#include "../include/Map.h"

Blinky::Blinky(int x, int y, int w, int h)
        : Ghost(x, y, w, h) {
    scatterCorner = {27,0};
    state = CHASE; // مستقیم میره روی chase
}

void Blinky::update(const Pacman& pacman, const Map& map) {
    switch(state) {
        case WAIT:
            // Blinky هیچوقت داخل خونه نمی‌مونه
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
    }
}
