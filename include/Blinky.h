#pragma once
#include "Ghost.h"

class Blinky : public Ghost {
public:
    Blinky(int x, int y, int w, int h);
    void update(const Pacman& pacman, const Map& map);
};
