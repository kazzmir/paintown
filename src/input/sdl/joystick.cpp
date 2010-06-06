#ifdef USE_SDL

#include <SDL.h>
#include "joystick.h"

void SDLJoystick::poll(){
    /* TODO */
}

JoystickInput SDLJoystick::readAll(){
    /* TODO */
    JoystickInput ok;
    return ok;
}

SDLJoystick::~SDLJoystick(){
    if (joystick){
        SDL_JoystickClose(joystick);
    }
}

SDLJoystick::SDLJoystick():
joystick(NULL){
    if (SDL_NumJoysticks() > 0){
        joystick = SDL_JoystickOpen(0);
    }
}

#endif
