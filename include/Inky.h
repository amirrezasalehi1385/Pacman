#pragma once
#include "Ghost.h"

class Inky : public Ghost {
public:
    Inky(int x, int y, int w, int h);
    void update(const Pacman& pacman, const Ghost& blinky, const Map& map);
};
