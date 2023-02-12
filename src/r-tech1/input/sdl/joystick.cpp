#ifdef USE_SDL

#include "../joystick.h"

int Joystick::numberOfJoysticks(){
    return 0;
}

Joystick * Joystick::create(int i){
    return nullptr;
}

#endif
