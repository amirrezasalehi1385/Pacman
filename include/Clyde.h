#pragma once
#include "Ghost.h"

class Clyde : public Ghost {
public:
    Clyde();
    void update(const Pacman& pacman, const Map& map);
};
