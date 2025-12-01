#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

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

private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    std::unordered_map<std::string, Mix_Chunk*> sounds;
    std::unordered_map<std::string, Mix_Music*> musics;
    std::unordered_map<std::string, int> soundChannels;
};

#endif
