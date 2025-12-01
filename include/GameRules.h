#pragma once
#include <array>
#include <SDL2/SDL.h>

namespace GameRules {
    constexpr float GHOST_SPEED_NORMAL = 1.0f;
    constexpr float GHOST_SPEED_EATEN = 2 * GHOST_SPEED_NORMAL;
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

        static const float speeds[] = {
                0.75f, // L1
                0.85f, // L2
                0.85f, // L3
                0.95f, // L4
                0.95f, // L5
                1.05f, // L6
                1.05f, // L7
                1.05f, // L8
                1.15f, // L9
                1.15f, // L10
                1.15f, // L11
                1.15f  // L12+
        };

        if (level < 1) level = 1;
        if (level > 12) level = 12;

        return speeds[level - 1] * GHOST_SPEED_NORMAL;
    }


    inline float getFrightenedSpeed(int level) {
        return getGhostSpeed(level) * 0.5f;
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