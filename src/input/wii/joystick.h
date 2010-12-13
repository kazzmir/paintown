#ifndef _paintown_wii_joystick_h
#define _paintown_wii_joystick_h

#ifdef WII

#include "../joystick.h"

class WiiJoystick: public Joystick {
public:
    virtual void poll();
    virtual JoystickInput readAll();
    virtual int getDeviceId() const;

    virtual ~WiiJoystick();

    friend class Joystick;
protected:
    WiiJoystick();
};

#endif

#endif
