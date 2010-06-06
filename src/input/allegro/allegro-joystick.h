#ifndef _paintown_allegro_joystick_h
#define _paintown_allegro_joystick_h

#include "../joystick.h"

class AllegroJoystick: public Joystick {
public:
    virtual void poll();
    virtual JoystickInput readAll();

    virtual ~AllegroJoystick();

    friend class Joystick;
protected:
    AllegroJoystick();
};

#endif
