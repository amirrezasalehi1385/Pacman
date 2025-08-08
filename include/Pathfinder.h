#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <queue>
#include "../include/Direction.h"
class Map;

Direction bfsFindDirection(const Map& map, int startX, int startY, int targetX, int targetY);
