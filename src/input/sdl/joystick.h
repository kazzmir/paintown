#ifndef _paintown_sdl_joystick_h
#define _paintown_sdl_joystick_h

#include "../joystick.h"

class SDLJoystick: public Joystick {
public:
    virtual void poll();
    virtual JoystickInput readAll();
    virtual bool pressed();

    virtual ~SDLJoystick();

    friend class Joystick;
protected:
    SDLJoystick();
};

#endif
