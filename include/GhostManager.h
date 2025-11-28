#pragma once
#include "GhostState.h"
#include <vector>
#include <SDL2/SDL.h>

class GhostManager {
public:
    GhostManager();

    void update();
    GhostState getCurrentMode() const;
    void startCycle();

private:
    GhostState currentMode;
    Uint32 modeStartTime;
    std::vector<Uint32> cycleTimes;
    size_t cycleIndex;
    bool cycleStarted = false;
};
