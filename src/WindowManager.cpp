#include "../include/WindowManager.h"

WindowManager::WindowManager() : window(nullptr), renderer(nullptr), music(nullptr) {}

WindowManager::~WindowManager() {
    if (music) Mix_FreeMusic(music);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);

    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}

bool WindowManager::init(const char* title, int w, int h) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return false;
    }

    int imgFlags = IMG_INIT_JPG | IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "IMG_Init Error: " << IMG_GetError() << std::endl;
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "Mix_OpenAudio Error: " << Mix_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

void WindowManager::clear() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void WindowManager::present() {
    SDL_RenderPresent(renderer);
}

SDL_Renderer* WindowManager::getRenderer() const {
    return renderer;
}

// Audio
bool WindowManager::loadAudio(const std::string& filePath) {
    music = Mix_LoadMUS(filePath.c_str());
    if (!music) {
        std::cerr << "Mix_LoadMUS Error: " << Mix_GetError() << std::endl;
        return false;
    }
    return true;
}

void WindowManager::playAudio(int loops) {
    if (music) {
        if (Mix_PlayMusic(music, loops) == -1) {
            std::cerr << "Mix_PlayMusic Error: " << Mix_GetError() << std::endl;
        }
    }
}

void WindowManager::stopAudio() {
    if (Mix_PlayingMusic()) Mix_HaltMusic();
}
