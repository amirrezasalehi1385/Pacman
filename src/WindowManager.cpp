#include "../include/WindowManager.h"
#include <iostream>

WindowManager::WindowManager() : window(nullptr), renderer(nullptr) {}

WindowManager::~WindowManager() {
    // پاک کردن صداها
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

    Mix_AllocateChannels(32); // اجازه پخش ۳۲ صدا همزمان

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


// ------------------------ MUSIC ------------------------

bool WindowManager::loadMusic(const std::string& id, const std::string& filePath) {
    Mix_Music* m = Mix_LoadMUS(filePath.c_str());
    if (!m) {
        std::cerr << "Mix_LoadMUS Error: " << Mix_GetError() << std::endl;
        return false;
    }
    musics[id] = m;
    return true;
}

void WindowManager::playMusic(const std::string& id, int loops) {
    if (musics.count(id))
        Mix_PlayMusic(musics[id], loops);
}

void WindowManager::stopMusic() {
    Mix_HaltMusic();
}



// ------------------------ SOUND EFFECTS ------------------------

bool WindowManager::loadSound(const std::string& id, const std::string& filePath) {
    Mix_Chunk* s = Mix_LoadWAV(filePath.c_str());
    if (!s) {
        std::cerr << "Mix_LoadWAV Error: " << Mix_GetError() << std::endl;
        return false;
    }
    sounds[id] = s;
    return true;
}

int WindowManager::playSound(const std::string& id, int loops) {
    if (!sounds.count(id))
        return -1;

    return Mix_PlayChannel(-1, sounds[id], loops);
}

void WindowManager::stopSound(int channel) {
    Mix_HaltChannel(channel);
}
