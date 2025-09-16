#pragma once
#include "GhostState.h"
#include <vector>
#include <SDL2/SDL.h>

class GhostManager {
public:
    GhostManager();

    void update();
    GhostState getCurrentMode() const;
    void startCycle();  // شروع چرخه Scatter/Chase بعد از خروج همه روح‌ها

private:
    GhostState currentMode;
    Uint32 modeStartTime;
    std::vector<Uint32> cycleTimes;
    size_t cycleIndex;
    bool cycleStarted = false;  // آیا چرخه Scatter/Chase شروع شده؟
};
