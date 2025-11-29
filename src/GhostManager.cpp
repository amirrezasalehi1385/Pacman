#include "../include/GhostManager.h"
#include "../include/TextureManager.h"
#include <cmath>
#include "../include/Map.h"
#include <queue>
#include <vector>
#include "../include/Direction.h"
#include "../include/GhostState.h"
#include <SDL2/SDL.h>

GhostManager::GhostManager()
        : currentMode(SCATTER), modeStartTime(SDL_GetTicks()), cycleIndex(0), cycleStarted(false) {
    cycleTimes = {7000, 20000, 7000, 20000, 5000, 20000, 5000};
}

void GhostManager::startCycle() {
    if(!cycleStarted) {
        cycleStarted = true;
        currentMode = SCATTER;
        modeStartTime = SDL_GetTicks();
        cycleIndex = 0;
    }
}

void GhostManager::update() {
    if(!cycleStarted) return;

    Uint32 now = SDL_GetTicks();
    if(cycleIndex >= cycleTimes.size()) return;

    if(now - modeStartTime >= cycleTimes[cycleIndex]) {
        currentMode = (currentMode == SCATTER) ? CHASE : SCATTER;
        modeStartTime = now;
        cycleIndex++;
    }
}

GhostState GhostManager::getCurrentMode() const {
    return currentMode;
}
