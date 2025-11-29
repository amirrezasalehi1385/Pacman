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

void WindowManager::playSound(const std::string& name, bool loop) {
    if(sounds.find(name) != sounds.end()) {
        int channel;
        if(loop) {
            channel = Mix_PlayChannel(-1, sounds[name], -1); // -1 = infinite loop
        } else {
            channel = Mix_PlayChannel(-1, sounds[name], 0); // 0 = play once
        }

        // ذخیره channel number برای این صدا
        if(channel != -1) {
            soundChannels[name] = channel;
        }
    }
}

void WindowManager::stopSound(const std::string& name) {
    // اگر این صدا در حال پخش است، فقط همان channel را متوقف کن
    if(soundChannels.find(name) != soundChannels.end()) {
        Mix_HaltChannel(soundChannels[name]);
        soundChannels.erase(name);
    }
}

bool WindowManager::isSoundPlaying(const std::string& name) {
    if(soundChannels.find(name) != soundChannels.end()) {
        return Mix_Playing(soundChannels[name]) != 0;
    }
    return false;
}