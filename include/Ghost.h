#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include "TextureManager.h"
#include "Direction.h"
#include "Pacman.h"
#include "GhostState.h"
#include "GameRules.h"

class Map;
class Ghost {
public:
    float normalSpeed = GameRules::GHOST_SPEED_NORMAL;
    bool canGotoGhostHouse;
    void setNormalSpeed(float speed) { normalSpeed = speed; }
    void renderHitbox(SDL_Renderer* renderer);
    bool readyToExit;
    bool ghostEaten;
    bool endOfFrightening;
    float initialTileX;
    float initialTileY;
    bool visible = true;
    bool returningSoundPlaying = false;
    Ghost(float initX, float initY, int w, int h);
    bool ghostInGhostHouse();
    float posX, posY;
    float pixelsMoved;
    bool exitingHouse = false;
    float speed;
    bool loadTextures(TextureManager* texManager, const std::vector<std::string>& paths);
    bool checkCollisionWithPacman(Pacman* pacman);
    void update(int level,const Map& map);
    void render(SDL_Renderer* renderer);
    void setWindowManager(WindowManager* wm) {
        windowManager = wm;
    }

    void loadScoreTextures(TextureManager* textureManager,
                           const std::string& score200Path,
                           const std::string& score400Path,
                           const std::string& score800Path,
                           const std::string& score1600Path);

    void startShowingScore(int score);
    bool isShowingScore() const { return showingScore; }
    void updateScoreDisplay();
    void renderScore(SDL_Renderer* renderer);

    void setPosition(int x, int y);
    SDL_FPoint getCurrentTile() const;
    void setTarget(const SDL_Rect& targetRect);
    void setTargetTile(float tileX, float tileY);
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
    SDL_Rect rect;
    Uint32 returnSoundEndTime = 0;
    GhostState getState() const { return state; }
    void setFrozen(bool val) { frozen = val; }
    void reset();
    bool scoreBeingDisplayed = false;
    bool showingScore;
    Uint32 scoreDisplayStartTime;
    int scoreDisplayDuration;
    int currentScoreValue;
    bool isFrozenForScore = false;
    int scoreValue = 200;
protected:
    int w, h;
    SDL_FPoint  targetTile;
    SDL_FPoint currentTile;
    Direction currentDirection = STOP;
    SDL_Point scatterCorner;
    GhostState state = WAIT;

private:
    SDL_Texture* scoreTexture200;
    SDL_Texture* scoreTexture400;
    SDL_Texture* scoreTexture800;
    SDL_Texture* scoreTexture1600;
    const SDL_Rect ghostHouse = {11, 16, 8, 5};
    bool isGhostScoreShowing = false;
    Uint32 ghostScoreFreezeStart = 0;
    const Uint32 GHOST_SCORE_FREEZE_DURATION = 500;
    SDL_Rect eyeRect;
    SDL_Texture* eyeTex;
    SDL_Rect hitbox;
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
    WindowManager* windowManager;
    bool frozen = false;
    void updateHitbox();

    void updateCurrentTile();

    Direction getReverseDirection() const;

    bool isValidMove(int nx, int ny, const Map &map) const;

    void updatePositionAfterMove();

    void moveInDirection(Direction dir);

    Direction findBestDirection(const Map &map);

    int calculateDistance(int fromX, int fromY, int toX, int toY) const;

    Direction getRandomValidDirection(const Map &map);
};
