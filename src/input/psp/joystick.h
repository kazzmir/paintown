#ifndef _paintown_psp_joystick_h
#define _paintown_psp_joystick_h

#ifdef MINPSPW

#include "../joystick.h"

#include <pspctrl.h>

class PSPJoystick: public Joystick {
public:
    virtual void poll();
    virtual JoystickInput readAll();
    virtual int getDeviceId() const;

    virtual ~PSPJoystick();

    friend class Joystick;
protected:
    PSPJoystick();
    // joystick
    SceCtrlData joystick;
};

#endif

#endif
