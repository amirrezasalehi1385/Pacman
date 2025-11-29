#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <string>
#include <unordered_map>

class WindowManager {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;

    std::unordered_map<std::string, Mix_Chunk*> sounds;
    std::unordered_map<std::string, Mix_Music*> musics;
    std::unordered_map<std::string, int> soundChannels; // ذخیره channel number هر صدا

public:
    WindowManager();
    ~WindowManager();

    bool init(const char* title, int width, int height);
    void clear();
    void present();
    SDL_Renderer* getRenderer() const;

    // Music
    bool loadMusic(const std::string& id, const std::string& filePath);
    void playMusic(const std::string& id, int loops = -1);
    void stopMusic();

    // Sound Effects
    bool loadSound(const std::string& id, const std::string& filePath);
    void playSound(const std::string& name, bool loop = false);
    void stopSound(const std::string& name);
    bool isSoundPlaying(const std::string& name);
};

#endif