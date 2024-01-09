#pragma once

#include <SDL2/SDL_video.h>
#include <SDL2/SDL_render.h>

struct BitmapData{
    BitmapData(SDL_Surface* surface, SDL_Texture* texture):
    surface(surface),
    texture(texture){
    }

    /* if loaded in a thread then this will be non-null */
    SDL_Surface* surface;
    /* if available to use on the gpu this will be non-null */
    SDL_Texture* texture;

    ~BitmapData(){
        if (texture != nullptr){
            SDL_DestroyTexture(texture);
            texture = nullptr;
        }
        if (surface != nullptr){
            SDL_FreeSurface(surface);
            surface = nullptr;
        }
    }
};

typedef SDL_Color INTERNAL_COLOR;

bool operator==(const INTERNAL_COLOR&, const INTERNAL_COLOR&);
bool operator!=(const INTERNAL_COLOR&, const INTERNAL_COLOR&);
bool operator<(const INTERNAL_COLOR&, const INTERNAL_COLOR&);
