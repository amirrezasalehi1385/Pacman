#pragma once
#include "Ghost.h"

class Blinky : public Ghost {
public:
    Blinky();
    void update(int level,const Pacman& pacman, const Map& map);
};
