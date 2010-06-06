#ifndef _paintown_sdl_joystick_h
#define _paintown_sdl_joystick_h

#ifdef USE_SDL

#include <SDL.h>
#include "../joystick.h"

class SDLJoystick: public Joystick {
public:
    virtual void poll();
    virtual JoystickInput readAll();

    virtual ~SDLJoystick();

    friend class Joystick;
protected:
    SDLJoystick();
    SDL_Joystick * joystick;
};

#endif

#endif
