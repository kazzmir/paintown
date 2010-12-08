#ifdef USE_SDL

#include <SDL.h>
#include "joystick.h"

void SDLJoystick::poll(){
    /* TODO */
}

static bool read_button(SDL_Joystick * joystick, int button){
    return SDL_JoystickGetButton(joystick, button);
}

JoystickInput SDLJoystick::readAll(){
    JoystickInput input;
    if (joystick){
        int buttons = SDL_JoystickNumButtons(joystick);
        switch (buttons > 4 ? 4 : buttons){
            case 4: input.button4 = read_button(joystick, 3);
            case 3: input.button3 = read_button(joystick, 2);
            case 2: input.button2 = read_button(joystick, 1);
            case 1: input.button1 = read_button(joystick, 0);
            case 0: {
                break;
            }
        }
    }

    int axis = SDL_JoystickNumAxes(joystick);
    if (axis > 0){
        int position = SDL_JoystickGetAxis(joystick, 0);
        if (position < 0){
            input.left = true;
        } else if (position > 0){
            input.right = true;
        }
    }

    if (axis > 1){
        int position = SDL_JoystickGetAxis(joystick, 1);
        if (position < 0){
            input.up = true;
        } else if (position > 0){
            input.down = true;
        }
    }

    return input;
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

int SDLJoystick::getDeviceId() const {
    if (joystick){
        return SDL_JoystickIndex(joystick);
    }

    return -1;
}

#endif
