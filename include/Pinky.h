#pragma once
#include "Ghost.h"
#include "Direction.h"
class Pinky : public Ghost {
public:
    Pinky();
    void update(int level, const Pacman& pacman, const Map& map);
};
