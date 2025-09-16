#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include "TextureManager.h"
#include "Direction.h"
#include "Pacman.h"
#include "GhostState.h"
#include "GhostManager.h"

class Map;

class Ghost {
public:
    bool canGotoGhostHouse;
    bool readyToExit;
    Ghost(int x, int y, int w, int h);
    bool loadTextures(TextureManager* texManager,
                      const std::string& upPath,
                      const std::string& downPath,
                      const std::string& leftPath,
                      const std::string& rightPath,
                      const std::string& bodyPath1,
                      const std::string& bodyPath2);
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
    void updateBodyAnimation();
    void setState(GhostState ghostState);

    GhostState state = WAIT;
    SDL_Point scatterCorner;
    void setMode(GhostState mode);
    void wait();
    GhostState getState() const { return state; }

protected:
    SDL_Point targetTile;
    SDL_Point currentTile;
    Direction currentDirection = STOP;

private:
    const SDL_Rect ghostHouse = {11, 16, 8, 5}; // X, Y, width, height به تایل‌ها
    int pixelsMoved = 0;
    SDL_Rect rect;
    SDL_Rect eyeRect;
    SDL_Texture* eyeTex;
    SDL_Rect hitbox;
    int speed = 1;
    SDL_Texture* bodyTex1; // تصویر اولیه
    SDL_Texture* bodyTex2; // تصویر پاهای باز
    int bodyFrame = 0;     // 0 یا 1
    SDL_Texture* targetTexture;
    SDL_Texture* bodyTex;
    int frameCounter = 0;    // شمارنده برای تغییر frame
    int frameSpeed = 8;      // هر چند tick یکبار تغییر کنه
    SDL_Texture* eyeUp;
    SDL_Texture* eyeDown;
    SDL_Texture* eyeLeft;
    SDL_Texture* eyeRight;
    SDL_Texture* currentEye;

    void updateHitbox();
};
