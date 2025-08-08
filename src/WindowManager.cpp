#include "../include/WindowManager.h"
WindowManager::WindowManager() {}
WindowManager::~WindowManager() {
    if(renderer) SDL_DestroyRenderer(renderer);
    if(window) SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}
bool WindowManager::init(const char *title, int w, int h) {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) return false;
    if(!(IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG)) return false;

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);
    if(!window) return false;

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