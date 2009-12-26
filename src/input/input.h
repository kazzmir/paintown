#ifndef _paintown_input_h
#define _paintown_input_h

#include <vector>
#include "input/joystick.h"

class Configuration;

class Input{
public:
    /* don't change these numbers, but you can add to them */
    enum PaintownInput{
        Unknown = -1,
        Forward = 0,
        Back = 1,
        Up = 2,
        Down = 3,
        Attack1 = 4,
        Attack2 = 5,
        Attack3 = 6,
        Jump = 7,
        Grab = 8,
    };

    /* TODO: remove this method.. useless now. */
    static std::vector<PaintownInput> convertKeyboard(const Configuration & configuration,int facing, const std::vector<int> & keys);
};

#endif
