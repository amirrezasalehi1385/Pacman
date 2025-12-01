#pragma once
#include "Ghost.h"

class Clyde : public Ghost {
public:
    Clyde();
    void update(int level,const Pacman& pacman, const Map& map);
};
