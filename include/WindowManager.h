#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <string>
#include <unordered_map>

class WindowManager {
public:
    WindowManager();
    ~WindowManager();

    bool init(const char* title, int width, int height);

    void clear();
    void present();
    SDL_Renderer* getRenderer() const;

    // --- Audio Music ---
    bool loadMusic(const std::string& id, const std::string& filePath);
    void playMusic(const std::string& id, int loops = -1);
    void stopMusic();

    // --- Sound Effects ---
    bool loadSound(const std::string& id, const std::string& filePath);
    int  playSound(const std::string& id, int loops = 0);
    void stopSound(int channel);

private:
    SDL_Window* window;
    SDL_Renderer* renderer;

    // موسیقی
    std::unordered_map<std::string, Mix_Music*> musics;

    // افکت‌ها
    std::unordered_map<std::string, Mix_Chunk*> sounds;
};
