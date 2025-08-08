#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include "TextureManager.h"
#include "Direction.h"
#include "Pacman.h"
#include "GhostState.h"
class Map;

class Ghost {
public:
    Ghost(int x, int y, int w, int h);
    bool loadTextures(TextureManager* texManager,
                      const std::string& upPath,
                      const std::string& downPath,
                      const std::string& leftPath,
                      const std::string& rightPath);
    void update(const Map& map);
    void render(SDL_Renderer* renderer);
    void setPosition(int x, int y);
    Direction bfsFindDirection(const Map& map, int startX, int startY, int targetX, int targetY);
    SDL_Point getCurrentTile() const;
    void setTarget(const SDL_Rect& targetRect);
    void setTargetTile(int tileX, int tileY);
        bool loadTargetTexture(SDL_Renderer* renderer, const std::string& path);
    void renderTarget(SDL_Renderer* renderer);
    void clearTargetTexture();
    void getOutOfHouse(const Map& map);
    void updateChaseScatter(const Map& map);
    GhostState state = CHASE;
    SDL_Point scatterCorner;
    void wait();
protected:
    SDL_Point targetTile;
    SDL_Point currentTile;
    Direction currentDirection = STOP;
private:
    const SDL_Rect ghostHouse = {11, 16, 8, 5}; // X, Y, width, height به تایل‌ها
    int pixelsMoved = 0; // متغیر عضو به جای static
    SDL_Rect rect;
    SDL_Rect hitbox;
    int speed = 1;
    SDL_Texture* targetTexture; // بافت برای نشانگر هدف
    SDL_Texture* texUp;
    SDL_Texture* texDown;
    SDL_Texture* texLeft;
    SDL_Texture* texRight;
    SDL_Texture* currentTex;

    void updateHitbox();
};
