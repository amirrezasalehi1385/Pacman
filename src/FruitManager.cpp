#include "FruitManager.h"
#include "TextureManager.h"
#include "SoundManager.h"
#include <cstdio>
#include "GameRules.h"

FruitManager::FruitManager()
        : visible(false), spawned1(false), spawned2(false),
          scoreValue(0), spawnTime(0), texManager(nullptr), renderer(nullptr),
          currentFruitIndex(-1)
{
    rect = { 13 * 16, 19 * 16 + 8, 32, 32 };
}


FruitManager::~FruitManager() {
    for (auto tex : fruitTextures) {
        if (tex) SDL_DestroyTexture(tex);
    }
    fruitTextures.clear();
}

void FruitManager::init(TextureManager* texManager, SDL_Renderer* renderer) {
    this->texManager = texManager;
    this->renderer = renderer;
    printf("[FruitManager] Initialized\n");
}

bool FruitManager::loadTextures(TextureManager* texManager) {
    const std::vector<std::string> fruitPaths = {
            "assets/fruits/cherry.png",
            "assets/fruits/strawberry.png",
            "assets/fruits/orange.png",
            "assets/fruits/apple.png",
            "assets/fruits/melon.png",
            "assets/fruits/galaxian.png",
            "assets/fruits/bell.png",
            "assets/fruits/key.png"
    };

    const std::vector<std::string> scorePaths = {
            "assets/scores/100.png",
            "assets/scores/300.png",
            "assets/scores/500.png",
            "assets/scores/700.png",
            "assets/scores/1000.png",
            "assets/scores/2000.png",
            "assets/scores/3000.png",
            "assets/scores/5000.png"
    };

    const std::vector<int> scores = {100, 300, 500, 700, 1000, 2000, 3000, 5000};

    fruitTextures.clear();
    scoreTextures.clear();
    fruitScores = scores;

    for (size_t i = 0; i < fruitPaths.size(); i++) {
        SDL_Texture* tex = texManager->loadTexture(fruitPaths[i]);
        if (!tex) return false;
        fruitTextures.push_back(tex);
    }

    for (size_t i = 0; i < scorePaths.size(); i++) {
        SDL_Texture* tex = texManager->loadTexture(scorePaths[i]);
        if (!tex) return false;
        scoreTextures.push_back(tex);
    }

    return true;
}

void FruitManager::reset(int level) {
    visible = false;
    spawned1 = false;
    spawned2 = false;
    spawnTime = 0;
    rect = { 13 * 16, 19 * 16 + 8, 32, 32 };

    selectFruitByLevel(level);

    printf("[FruitManager] Reset for level %d\n", level);
}
void FruitManager::selectFruitByLevel(int level) {
    int index = 0;

    if(level == 1)      index = 0;
    else if(level == 2) index = 1;
    else if(level <= 4) index = 2;
    else if(level <= 6) index = 3;
    else if(level <= 8) index = 4;
    else if(level <= 10)index = 5;
    else if(level <= 12)index = 6;
    else                index = 7;

    if (index < fruitTextures.size()) {
        scoreValue = fruitScores[index];
        currentFruitIndex = index;
    } else {
        currentFruitIndex = -1;
        scoreValue = 0;
    }
}


void FruitManager::renderScore(SDL_Renderer* renderer) {
    if (!showScorePopup || scorePopupIndex < 0) return;

    Uint32 now = SDL_GetTicks();
    const Uint32 duration = 1000;

    if (now - scorePopupStart <= duration) {
        SDL_RenderCopy(renderer, scoreTextures[scorePopupIndex], nullptr, &scorePopupRect);
    } else {
        showScorePopup = false;
    }
}
void FruitManager::renderHUD(SDL_Renderer* renderer) {
    const int maxFruitsOnHUD = 7;
    while (eatenFruits.size() > maxFruitsOnHUD) {
        eatenFruits.erase(eatenFruits.begin());
    }

    int startX = GameRules::MAP_WIDTH_TILES * GameRules::TILE_SIZE - 32;
    int startY = 576 - 32;
    int spacing = 36;

    for (size_t i = 0; i < eatenFruits.size(); i++) {
        int index = eatenFruits[i];
        if (index >= 0 && index < fruitTextures.size()) {
            SDL_Rect dst = { startX - (int)i * spacing, startY, 32, 32 };
            SDL_RenderCopy(renderer, fruitTextures[index], nullptr, &dst);
        }
    }
}
void FruitManager::render(SDL_Renderer* renderer) {
    if (visible && currentFruitIndex >= 0 && currentFruitIndex < fruitTextures.size()) {
        SDL_RenderCopy(renderer, fruitTextures[currentFruitIndex], nullptr, &rect);
    }
    renderScore(renderer);

}


void FruitManager::pauseFruit() {
    if(visible && spawnTime > 0) {
        Uint32 now = SDL_GetTicks();
        Uint32 elapsed = now - spawnTime;
        if(elapsed < duration) {
            pausedTimeRemaining = duration - elapsed;
            isPaused = true;
        }
    }
}

void FruitManager::update(int dotsEaten, const SDL_Rect& pacHitbox, int& score) {
    if(isPaused && visible) {
        spawnTime = SDL_GetTicks();
        isPaused = false;
    }

    if(showScorePopup) {
        if(SDL_GetTicks() - scorePopupStart >= 2000) {
            showScorePopup = false;
        }
        return;
    }

    if(dotsEaten >= spawn1 && !spawned1) {
        spawned1 = true;
        visible = true;
        spawnTime = SDL_GetTicks();
    }

    if(dotsEaten >= spawn2 && !spawned2) {
        spawned2 = true;
        visible = true;
        spawnTime = SDL_GetTicks();
    }

    if(visible) {
        Uint32 elapsed = SDL_GetTicks() - spawnTime;
        Uint32 effectiveDuration = isPaused ? pausedTimeRemaining : duration;

        if(elapsed >= effectiveDuration) {
            visible = false;
        }

        if(SDL_HasIntersection(&rect, &pacHitbox)) {
            SoundManager::get().playOnce("fruit");
            visible = false;
            score += scoreValue;
            eatenFruits.push_back(currentFruitIndex);

            showScorePopup = true;
            scorePopupStart = SDL_GetTicks();
            scorePopupRect = rect;
            scorePopupIndex = currentFruitIndex;
        }
    }
}

