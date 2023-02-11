#ifdef LINUX

#include "joystick.h"

#include <stdint.h>
#include <vector>
#include <linux/input.h>
#include <linux/joystick.h>

#define NAME_LENGTH 1024

class LinuxJoystick: public Joystick {
public:
    virtual void poll();

    virtual ~LinuxJoystick();

    friend class Joystick;
protected:
    LinuxJoystick();

    /* file descriptor for /dev/input/jsX */
    int file;
    char name[NAME_LENGTH];
    unsigned char axes;
    unsigned char buttons;
    int version;
    uint16_t btnmap[KEY_MAX - BTN_MISC + 1];
    uint8_t axmap[ABS_MAX + 1];
    char * button;
    int * axis;
};

#endif
