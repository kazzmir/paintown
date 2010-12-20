#ifdef USE_SDL

#include <SDL.h>
#include "joystick.h"

void SDLJoystick::poll(){
    events.clear();
}

static bool read_button(SDL_Joystick * joystick, int button){
    return SDL_JoystickGetButton(joystick, button);
}

JoystickInput SDLJoystick::readAll(){
    JoystickInput input;
    if (joystick){
        int buttons = SDL_JoystickNumButtons(joystick);
        switch (buttons > 5 ? 5 : buttons){
            case 5: input.quit = read_button(joystick, 4);
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

    int hats = SDL_JoystickNumHats(joystick);
    if (hats > 0){
        int hat = SDL_JoystickGetHat(joystick, 0);
        if ((hat & SDL_HAT_UP) == SDL_HAT_UP){
#ifdef WII
            input.right = true;
#else
            input.up = true;
#endif
        }
        if ((hat & SDL_HAT_DOWN) == SDL_HAT_DOWN){
#ifdef WII
            input.left = true;
#else
            input.down = true;
#endif
        }
        if ((hat & SDL_HAT_LEFT) == SDL_HAT_LEFT){
#ifdef WII
            input.up = true;
#else
            input.left = true;
#endif
        }
        if ((hat & SDL_HAT_RIGHT) == SDL_HAT_RIGHT){
#ifdef WII
            input.down = true;
#else
            input.right = true;
#endif
        }
        if ((hat & SDL_HAT_RIGHTUP) == SDL_HAT_RIGHTUP){
#ifdef WII
            input.down = true;
            input.left = true;
#else
            input.right = true;
            input.up = true;
#endif
        }
        if ((hat & SDL_HAT_RIGHTDOWN) == SDL_HAT_RIGHTDOWN){
#ifdef WII
            input.down = true;
            input.left = true;
#else
            input.right = true;
            input.down = true;
#endif
        }
        if ((hat & SDL_HAT_LEFTDOWN) == SDL_HAT_LEFTDOWN){
#ifdef WII
            input.up = true;
            input.left = true;
#else
            input.left = true;
            input.down = true;
#endif
        }
        if ((hat & SDL_HAT_LEFTUP) == SDL_HAT_LEFTUP){
#ifdef WII
            input.up = true;
            input.right = true;
#else
            input.left = true;
            input.up = true;
#endif
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
    
void SDLJoystick::pressButton(int button){
    if (joystick){
        Event event = Invalid;
        switch (button){
            case 0: event = Button1; break;
            case 1: event = Button2; break;
            case 2: event = Button3; break;
            case 3: event = Button4; break;
            case 4: event = Quit; break;
            default: break;
        }
        events.push_back(event);
    }
}

void SDLJoystick::releaseButton(int button){
}

void SDLJoystick::axisMotion(int axis, int motion){
    if (joystick){
        if (axis == 0){
            if (motion < 0){
                events.push_back(Left);
            } else if (motion > 0){
                events.push_back(Right);
            }
        } else if (axis == 1){
            if (motion < 0){
                events.push_back(Up);
            } else if (motion > 0){
                events.push_back(Down);
            }
        }
    }
}

int SDLJoystick::getDeviceId() const {
    if (joystick){
        return SDL_JoystickIndex(joystick);
    }

    return -1;
}

#endif
