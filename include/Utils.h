#pragma once
#include <SDL2/SDL.h>

inline SDL_Point toTileCoords(int x, int y, int tileSize = 16) {
    SDL_Point tile;
    tile.x = x / tileSize;
    tile.y = y / tileSize;
    return tile;
}

inline SDL_Point toPixelCoords(int tileX, int tileY, int tileSize = 16) {
    SDL_Point pixel;
    pixel.x = tileX * tileSize;
    pixel.y = tileY * tileSize;
    return pixel;
}
