#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class WindowManager {
public:
    WindowManager();
    ~WindowManager();
    bool init(const char* title, int w, int h);
    void clear();
    void present();
    SDL_Renderer* getRenderer() const;
private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
};


