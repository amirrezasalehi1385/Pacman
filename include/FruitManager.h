#ifndef FRUITMANAGER_H
#define FRUITMANAGER_H

#include <SDL2/SDL.h>
#include <string>
#include <vector>

class TextureManager;

class FruitManager {
private:
    std::vector<SDL_Texture*> fruitTextures;
    std::vector<SDL_Texture*> scoreTextures;
    std::vector<int> fruitScores;
    SDL_Rect rect;

    bool visible; // True if the fruit is currently visible on the screen
    bool spawned1; // True if the first fruit has appeared (after the required number of dots eaten)
    bool spawned2; // True if the second fruit has appeared (after the required number of dots eaten)

    int scoreValue;        // The score the player gets when the fruit is eaten
    Uint32 spawnTime;      // The exact time (SDL_GetTicks) when the fruit appeared or should appear

    TextureManager* texManager;  // Used to load and access fruit textures
    SDL_Renderer* renderer;      // The renderer on which the fruit is drawn

    const int spawn1 = 70;       // Number of dots eaten after which the first fruit appears
    const int spawn2 = 170;      // Number of dots eaten after which the second fruit appears
    const Uint32 duration = 9500; // Duration (in milliseconds) that the fruit remains visible
    int currentFruitIndex = -1;   // Index of the currently spawned fruit, -1 if none
    bool showScorePopup = false;
    Uint32 scorePopupStart = 0;
    SDL_Rect scorePopupRect;
    int scorePopupIndex = -1;   // index تصویر امتیاز (100, 300 ...)



public:
    void pauseFruit();
    Uint32 pausedTimeRemaining = 0;
    bool isPaused = false;
    std::vector<int> eatenFruits;
    FruitManager();
    ~FruitManager();
    void renderHUD(SDL_Renderer* renderer);
    // Initialize the fruit manager with textures and renderer
    void init(TextureManager* texManager, SDL_Renderer* renderer);
    // Reset the fruit state for the new level
    void reset(int level);
    // Update fruit visibility, check collisions with Pacman, and update the score
    void update(int dotsEaten, const SDL_Rect& pacHitbox, int& score);
    // Draw the fruit on the screen
    void render(SDL_Renderer* renderer);
    bool loadTextures(TextureManager* texManager);
    bool isVisible() const { return visible; }
    void selectFruitByLevel(int level);
private:
    void renderScore();

    void renderScore(SDL_Renderer *renderer);
};

#endif
