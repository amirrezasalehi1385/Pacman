#pragma once
#include <array>
#include <SDL2/SDL.h>

namespace GameRules {

    // ------------------- Speeds -------------------
    constexpr float GHOST_SPEED_NORMAL = 1.2f;
    constexpr float GHOST_SPEED_EATEN = 2 * GHOST_SPEED_NORMAL;
    constexpr int PACMAN_SPEED = 2;
    constexpr float GHOST_SPEED_CRUISE_ELROY_1 = 1.2 * GHOST_SPEED_NORMAL;
    constexpr float GHOST_SPEED_CRUISE_ELROY_2 = 1.3 * GHOST_SPEED_NORMAL;

    // ------------------- Scoring -------------------
    constexpr int SMALL_DOTS_SCORE = 10;
    constexpr int BIG_DOTS_SCORE = 50;

    // ------------------- Ghost AI cycles -------------------
    constexpr std::array<Uint32, 7> GHOST_CYCLES = {
            7000, 20000, 7000, 20000, 5000, 20000, 5000
    };

    // ------------------- Debug -------------------
    static bool SHOW_DEBUG = false; // Show hitboxes/targets when true
    static bool PLAY_SOUND = true; // Show hitboxes/targets when true
    // ------------------- Map info -------------------
    constexpr int TILE_SIZE = 16;
    constexpr int MAP_WIDTH_TILES = 28;
    constexpr int MAP_HEIGHT_TILES = 31;
    constexpr int TUNNEL_ROW = 14;

    // ------------------- Speed functions -------------------
    inline float getGhostSpeed(int level) {
        static const float speeds[] = {
                0.75f, 0.85f, 0.85f, 0.95f, 0.95f, 1.05f,
                1.05f, 1.05f, 1.15f, 1.15f, 1.15f, 1.15f
        };
        if (level < 1) level = 1;
        if (level > 12) level = 12;
        return speeds[level - 1] * GHOST_SPEED_NORMAL;
    }

    inline float getFrightenedSpeed(int level) {
        float c;
        if(level <= 1) c = 0.5f;
        else if(level <= 4) c = 0.55f;
        else if(level <= 20) c = 0.6f;
        else c = 0.0f;
        return GHOST_SPEED_NORMAL * c;
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
