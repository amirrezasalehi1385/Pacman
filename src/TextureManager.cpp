#include "TextureManager.h"

TextureManager::TextureManager(SDL_Renderer *renderer): renderer(renderer){

}
TextureManager::~TextureManager() {
    clear();
}

SDL_Texture* TextureManager::loadTexture(const std::string &path) {
    if(textures.count(path)) return textures[path];
    SDL_Surface* surface = IMG_Load(path.c_str());
    if(!surface) return nullptr;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer,surface);
    SDL_FreeSurface(surface);
    if(texture) textures[path] = texture;
    return texture;
}

void TextureManager::clear() {
    for(auto& pair : textures){
        SDL_DestroyTexture(pair.second);
    }
    textures.clear();
}
