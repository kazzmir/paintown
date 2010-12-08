#ifndef _paintown_sdl_joystick_h
#define _paintown_sdl_joystick_h

#ifdef USE_SDL

#include <SDL.h>
#include "../joystick.h"

class SDLJoystick: public Joystick {
public:
    virtual void poll();
    virtual JoystickInput readAll();
    virtual int getDeviceId() const;
    virtual void pressButton(int button);
    virtual void releaseButton(int button);
    virtual void axisMotion(int axis, int motion);

    virtual ~SDLJoystick();

    friend class Joystick;
protected:
    SDLJoystick();
    SDL_Joystick * joystick;
};

#endif

#endif
