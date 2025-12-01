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
    const std::vector<std::string> paths = {
            "assets/fruits/cherry.png",
            "assets/fruits/strawberry.png",
            "assets/fruits/orange.png",
            "assets/fruits/apple.png",
            "assets/fruits/melon.png",
            "assets/fruits/galaxian.png",
            "assets/fruits/bell.png",
            "assets/fruits/key.png"
    };

    const std::vector<int> scores = {100, 300, 500, 700, 1000, 2000, 3000, 5000};

    fruitTextures.clear();
    fruitScores.clear();

    for (size_t i = 0; i < paths.size(); ++i) {
        SDL_Texture* tex = texManager->loadTexture(paths[i]);
        if (!tex) {
            printf("[FruitManager] ERROR: Failed to load %s\n", paths[i].c_str());
            return false;
        }
        fruitTextures.push_back(tex);
        fruitScores.push_back(scores[i]);
        printf("[FruitManager] Loaded %s (score=%d)\n", paths[i].c_str(), scores[i]);
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



void FruitManager::update(int dotsEaten, const SDL_Rect& pacHitbox, int& score) {
    Uint32 now = SDL_GetTicks();

    if (!spawned1 && dotsEaten >= spawn1) {
        visible = true;
        spawned1 = true;
        spawnTime = now;
    }

    if (!spawned2 && dotsEaten >= spawn2) {
        visible = true;
        spawned2 = true;
        spawnTime = now;
    }

    if (visible && currentFruitIndex >= 0) {
        if (now - spawnTime > duration) {
            visible = false;
        }

        if (SDL_HasIntersection(&rect, &pacHitbox)) {
            SoundManager::get().playOnce("fruit");
            score += scoreValue;
            visible = false;

            // ذخیره میوه خورده شده فقط اگر کمتر از 7 تا باشد
            if(eatenFruits.size() < 7) {
                eatenFruits.push_back(currentFruitIndex);
            }
        }
    }
}

void FruitManager::renderHUD(SDL_Renderer* renderer) {
    int startX = GameRules::MAP_WIDTH_TILES * GameRules::TILE_SIZE - 32; // شروع از سمت راست
    int startY = 576 - 32;
    int spacing = 36;

    for(size_t i = 0; i < eatenFruits.size(); i++) {
        int index = eatenFruits[i];
        if(index >= 0 && index < fruitTextures.size()) {
            SDL_Rect dst = { startX - (int)i * spacing, startY, 32, 32 };
            SDL_RenderCopy(renderer, fruitTextures[index], nullptr, &dst);
        }
    }
}


void FruitManager::render(SDL_Renderer* renderer) {
    if (visible && currentFruitIndex >= 0 && currentFruitIndex < fruitTextures.size()) {
        SDL_RenderCopy(renderer, fruitTextures[currentFruitIndex], nullptr, &rect);
    }
}
