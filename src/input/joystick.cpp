#include <stdlib.h>
#include "joystick.h"

/*
#ifdef LINUX
#include "linux_joystick.h"
#endif
*/

#ifdef USE_ALLEGRO
#include "allegro/allegro-joystick.h"
#endif
#ifdef USE_SDL
#ifdef WII
#include "wii/joystick.h"
#include "sdl/joystick.h"
#elif MINPSPW
#include "psp/joystick.h"
#else
#include "sdl/joystick.h"
#endif
#endif

Joystick * Joystick::create(){
#ifdef USE_ALLEGRO
    return new AllegroJoystick();
#endif
#ifdef USE_SDL
#ifdef WII
    return new SDLJoystick();
    // return new WiiJoystick();
#elif MINPSPW
    return new PSPJoystick();
#else
    return new SDLJoystick();
#endif
#endif
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

bool Joystick::pressed(){
    JoystickInput input = readAll();
    return input.up || input.down || input.left || input.right ||
           input.button1 || input.button2 || input.button3 || input.button4 ||
           input.quit; 
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
        case Quit: return "Quit";
    }
    return "Unknown";
}
    
void Joystick::pressButton(int button){
}

void Joystick::releaseButton(int button){
}

void Joystick::axisMotion(int axis, int motion){
}
