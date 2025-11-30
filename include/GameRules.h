#pragma once
#include <array>
#include <SDL2/SDL.h>

namespace GameRules {
    constexpr float GHOST_SPEED_NORMAL = 1.0f;
    constexpr float GHOST_SPEED_FRIGHTENED = 1.0f;
    constexpr float GHOST_SPEED_EATEN = 2.0f;
    constexpr int PACMAN_SPEED = 2;

    constexpr int SMALL_DOTS_SCORE = 10;
    constexpr int BIG_DOTS_SCORE = 50;

    constexpr std::array<Uint32, 7> GHOST_CYCLES = {
            7000, 20000, 7000, 20000, 5000, 20000, 5000
    };


    constexpr int TILE_SIZE = 16;
    constexpr int MAP_WIDTH_TILES = 28;
    constexpr int MAP_HEIGHT_TILES = 31;
    constexpr int TUNNEL_ROW = 14;

    inline float getGhostSpeed(int level) {
        float speed = 0.75f;
        float speeds[255];
        speeds[0] = 0.75f;
        int j = 1;
        for (int i = 1; i < 255; i++) {
            if(j % 5 == 0) {
                speeds[i] = speeds[i - 1] * 1.2;
            }else {
                speeds[i] = speeds[i - 1];
            }
            j++;
        }
        return speeds[level - 1];
    }

    inline float getFrightenedSpeed(int level) {
        if(level == 1) return 0.50f;
        if(level >= 2 && level <= 4) return 0.55f;
        if(level >= 5 && level <= 20) return 0.60f;
        return 0.0f;
    }



    inline Uint32 getFrightenedTime(int level) {
        if(level == 1) return 6000;
        if(level >= 2 && level <= 5) return 5000;
        if(level >= 6 && level <= 10) return 4000;
        if(level >= 11 && level <= 14) return 3000;
        if(level >= 15 && level <= 18) return 2000;
        if(level >= 19 && level <= 21) return 1000;
        if(level >= 22) return 0;
        return 6000;
    }

}