#pragma once

#include <SDL2/SDL_video.h>
#include <SDL2/SDL_render.h>

struct BitmapData{
    BitmapData(SDL_Texture* texture):texture(texture){
    }

    SDL_Texture* texture;

    ~BitmapData(){
        if (texture != nullptr){
            SDL_DestroyTexture(texture);
            texture = nullptr;
        }
    }
};

typedef SDL_Color INTERNAL_COLOR;

bool operator==(const INTERNAL_COLOR&, const INTERNAL_COLOR&);
bool operator!=(const INTERNAL_COLOR&, const INTERNAL_COLOR&);
bool operator<(const INTERNAL_COLOR&, const INTERNAL_COLOR&);
