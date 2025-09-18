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
    bool ghostEaten;
    bool endOfFrightening;
    Uint32 eatenUntil = 0;  // زمان پایان حالت EATEN
    Ghost(int x, int y, int w, int h);

    bool loadTextures(TextureManager* texManager, const std::vector<std::string>& paths);

    bool checkCollisionWithPacman(Pacman* pacman);
    void update(const Map& map);
    void render(SDL_Renderer* renderer);
    void setPosition(int x, int y);
//    Direction bfsFindDirection(const Map& map, int startX, int startY, int targetX, int targetY);
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
    void setMode(GhostState mode);
    SDL_Rect* getHitBox();
    void updateFrightened(const Map& map);
    void wait();
    GhostState getState() const { return state; }

protected:
    SDL_Point targetTile;
    SDL_Point currentTile;
    Direction currentDirection = STOP;
    SDL_Point scatterCorner;
    GhostState state = WAIT;

private:
    const SDL_Rect ghostHouse = {11, 16, 8, 5}; // X, Y, width, height به تایل‌ها
    int pixelsMoved = 0;
    SDL_Rect rect;
    SDL_Rect eyeRect;
    SDL_Texture* eyeTex;
    SDL_Rect hitbox;
    int speed = 1;
    SDL_Texture* bodyTex1;
    SDL_Texture* bodyTex2;
    SDL_Texture* frightenedTex;
    SDL_Texture* frightenedTex2;
    SDL_Texture* endFrightened;
    SDL_Texture* endFrightened2;
    int bodyFrame = 0;
    SDL_Texture* targetTexture;
    SDL_Texture* bodyTex;
    int frameCounter = 0;
    int frameSpeed = 8;
    SDL_Texture* eyeUp;
    SDL_Texture* eyeDown;
    SDL_Texture* eyeLeft;
    SDL_Texture* eyeRight;
    SDL_Texture* currentEye;

    void updateHitbox();
};
