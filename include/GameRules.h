#pragma once

namespace GameRules {
    constexpr float GHOST_SPEED_NORMAL = 1.7;
    constexpr int GHOST_SPEED_FRIGHTENED = 1;
    constexpr int GHOST_SPEED_EATEN = 4;
    constexpr int PACMAN_SPEED = 2;
    constexpr int SMALL_DOTS_SCORE = 10;
    constexpr int BIG_DOTS_SCORE = 50;
    constexpr std::array<Uint32, 7> GHOST_CYCLES = {
            7000, 20000, 7000, 20000, 5000, 20000, 5000
    };
    constexpr std::array<Uint32, 4> FRIGHTENED_TIMES = {
            6000, 5000, 4000, 3000
    };

}
