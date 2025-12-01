#include "Fruit.h"
#include "TextureManager.h"

Fruit::Fruit(int level) {
    selectFruitByLevel(level);
    rect = { 13 * 16, 17 * 16, 16, 16 };
    spawnTime = SDL_GetTicks();
    visible = true;
}

void Fruit::selectFruitByLevel(int level) {
    if(level == 1)      { score = 100;  /* cherry */ }
    else if(level == 2) { score = 300;  /* strawberry */ }
    else if(level <= 4) { score = 500;  /* orange */ }
    else if(level <= 6) { score = 700;  /* apple */ }
    else if(level <= 8) { score = 1000; /* melon */ }
    else if(level <= 10){ score = 2000; /* galaxian */ }
    else if(level <= 12){ score = 3000; /* bell */ }
    else                { score = 5000; /* key */ }
}
bool Fruit::checkCollision(const SDL_Rect& pacmanRect) {
    return SDL_HasIntersection(&rect, &pacmanRect);
}