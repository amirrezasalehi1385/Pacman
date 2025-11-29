#include "SoundManager.h"
#include <SDL2/SDL.h>

SoundManager::SoundManager() {
    Mix_AllocateChannels(16);
    nextChannel = 0;
}

SoundManager& SoundManager::get() {
    static SoundManager instance;
    return instance;
}

bool SoundManager::loadSound(const std::string& id, const std::string& path, int dedicatedChannel) {
    Mix_Chunk* chunk = Mix_LoadWAV(path.c_str());
    if (!chunk) {
        SDL_Log("Failed to load sound %s: %s", path.c_str(), Mix_GetError());
        return false;
    }
    sounds[id] = chunk;
    if (dedicatedChannel == -1) {
        dedicatedChannels[id] = nextChannel++;
    } else {
        dedicatedChannels[id] = dedicatedChannel;
    }
    return true;
}

void SoundManager::playOnce(const std::string& id) {
    auto it = sounds.find(id);
    if (it == sounds.end()) return;
    Uint32 now = SDL_GetTicks();
    if (now - lastPlayTime[id] < cooldown) return;
    int channel = dedicatedChannels[id];
    Mix_PlayChannel(channel, it->second, 0);
    lastPlayTime[id] = now;
}

void SoundManager::play(const std::string& id, int loops) {
    auto it = sounds.find(id);
    if (it == sounds.end()) return;

    Uint32 now = SDL_GetTicks();

    int channel = dedicatedChannels[id];

    if (isPlaying(id)) return;

    Mix_PlayChannel(channel, it->second, loops);
    lastPlayTime[id] = now;
}

void SoundManager::stop(const std::string& id) {
    auto it = dedicatedChannels.find(id);
    if (it != dedicatedChannels.end()) {
        Mix_HaltChannel(it->second);
    }
}

bool SoundManager::isPlaying(const std::string& id) const {
    auto it = dedicatedChannels.find(id);
    if (it == dedicatedChannels.end()) {
        return false;
    }
    return Mix_Playing(it->second) != 0;
}

void SoundManager::stopAll() {
    Mix_HaltChannel(-1);
}

SoundManager::~SoundManager() {
    for (auto& sound : sounds) {
        if (sound.second) {
            Mix_FreeChunk(sound.second);
        }
    }
    sounds.clear();
    dedicatedChannels.clear();
}