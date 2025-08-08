#pragma once
#include "Ghost.h"

class Clyde : public Ghost {
public:
    Clyde(int x, int y, int w, int h);
    void update(const Pacman& pacman, const Map& map);
};
