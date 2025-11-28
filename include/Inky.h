#pragma once
#include "Ghost.h"

class Inky : public Ghost {
public:
    Inky();
    void update(const Pacman& pacman, const Ghost& blinky, const Map& map);
};
