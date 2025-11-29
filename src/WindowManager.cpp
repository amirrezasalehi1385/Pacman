#include "WindowManager.h"

WindowManager::WindowManager() : window(nullptr), renderer(nullptr) {}

WindowManager::~WindowManager() {
    for (auto& s : sounds)
        Mix_FreeChunk(s.second);

    for (auto& m : musics)
        Mix_FreeMusic(m.second);

    Mix_CloseAudio();
    IMG_Quit();
    SDL_Quit();
}


bool WindowManager::init(const char* title, int width, int height) {

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
        return false;

    if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) & (IMG_INIT_PNG | IMG_INIT_JPG)))
        return false;

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
        return false;

    Mix_AllocateChannels(32);

    window = SDL_CreateWindow(title,
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              width, height,
                              SDL_WINDOW_SHOWN);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    return renderer != nullptr;
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



