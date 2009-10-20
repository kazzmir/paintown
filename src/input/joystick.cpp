#include <stdlib.h>
#include "joystick.h"

/*
#ifdef LINUX
#include "linux_joystick.h"
#endif
*/

#include "allegro-joystick.h"

Joystick * Joystick::create(){
    return new AllegroJoystick();
/*
#ifdef LINUX
    return new LinuxJoystick();
#endif
    return NULL;
*/
}

Joystick::Joystick(){
}
    
Joystick::~Joystick(){
}
    
const char * Joystick::keyToName(Key key){
    switch (key){
        case Invalid : return "Invalid";
        case Up : return "Up";
        case Down : return "Down";
        case Left : return "Left";
        case Right : return "Right";
        case Button1 : return "Button1";
        case Button2 : return "Button2";
        case Button3 : return "Button3";
        case Button4 : return "Button4";
    }
}
