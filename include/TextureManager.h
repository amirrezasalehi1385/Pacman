#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <map>
#include <string>
class TextureManager{
    public:
        TextureManager(SDL_Renderer* renderer);
        ~TextureManager();

        SDL_Texture* loadTexture(const std::string& path);
        void clear();
    private:
        SDL_Renderer* renderer;
        std::map<std::string, SDL_Texture*> textures;
};