#pragma once
#include "Ghost.h"
#include "Direction.h"
class Pinky : public Ghost {
public:
    Pinky(int x, int y, int w, int h);
    void update(const Pacman& pacman, const Map& map);
};
