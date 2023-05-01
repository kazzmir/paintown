#pragma once

#include <SDL2/SDL_video.h>
#include <SDL2/SDL_render.h>

struct BitmapData{
    BitmapData(SDL_Window* window, SDL_Renderer* renderer):window(window), renderer(renderer){
    }

    SDL_Window* window;
    SDL_Renderer* renderer;
};

typedef SDL_Color INTERNAL_COLOR;

bool operator==(const INTERNAL_COLOR&, const INTERNAL_COLOR&);
bool operator!=(const INTERNAL_COLOR&, const INTERNAL_COLOR&);
bool operator<(const INTERNAL_COLOR&, const INTERNAL_COLOR&);
