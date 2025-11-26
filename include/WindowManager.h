#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <string>
#include <iostream>

class WindowManager {
public:
    WindowManager();
    ~WindowManager();

    // Initialize SDL video, audio, and image support
    bool init(const char* title, int w, int h);

    // Rendering
    void clear();
    void present();
    SDL_Renderer* getRenderer() const;

    // Audio
    bool loadAudio(const std::string& filePath); // Load WAV/MP3
    void playAudio(int loops = 0);               // loops = 0 once, loops = -1 forever
    void stopAudio();

private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    Mix_Music* music = nullptr;
};
