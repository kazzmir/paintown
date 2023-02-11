#pragma once

#include <SDL/SDL_video.h>

struct BitmapData{
};

typedef SDL_Color INTERNAL_COLOR;

bool operator==(const INTERNAL_COLOR&, const INTERNAL_COLOR&);
bool operator!=(const INTERNAL_COLOR&, const INTERNAL_COLOR&);
bool operator<(const INTERNAL_COLOR&, const INTERNAL_COLOR&);
