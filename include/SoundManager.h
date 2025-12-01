#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL.h>
#include <string>
#include <unordered_map>

class SoundManager {
public:
    static SoundManager& get();

    bool loadSound(const std::string& id, const std::string& path, int dedicatedChannel = -1);

    void playOnce(const std::string& id);
    void play(const std::string& id, int loops = 0);
    void stop(const std::string& id);
    bool isPlaying(const std::string& id) const;
    void stopAll();

    void setCooldown(Uint32 ms) { cooldown = ms; }

    ~SoundManager();

private:
    SoundManager();
    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;

    std::unordered_map<std::string, Mix_Chunk*> sounds;
    std::unordered_map<std::string, int> dedicatedChannels;
    std::unordered_map<std::string, Uint32> lastPlayTime;
    Uint32 cooldown = 100;
    int nextChannel = 0;
};

#endif
