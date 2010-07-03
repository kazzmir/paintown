#ifdef WII

#include "joystick.h"

#include <wiiuse/wpad.h>

void WiiJoystick::poll(){
    WPAD_ScanPads();
}

JoystickInput WiiJoystick::readAll(){
    JoystickInput input;
    u32 held = WPAD_ButtonsHeld(0);
    
    // WIIMOTE for now, we can add in nunchuck and classic later
    if(held & WPAD_BUTTON_HOME){
        //input.button7 = true;
    } else if(held & WPAD_BUTTON_1){
        input.button1 = true;
    } else if(held & WPAD_BUTTON_2){
        input.button2 = true;
    } else if(held & WPAD_BUTTON_A){
    } else if(held & WPAD_BUTTON_B){
        
    } else if(held & WPAD_BUTTON_MINUS){
        input.button4 = true;
    } else if(held & WPAD_BUTTON_PLUS){
        input.button3 = true;
    } else if(held & WPAD_BUTTON_UP){
        input.left = true;
    } else if(held & WPAD_BUTTON_DOWN){
        input.right = true;
    } else if(held & WPAD_BUTTON_LEFT){
        input.down = true;
    } else if(held & WPAD_BUTTON_RIGHT){
        input.up = true;
    }

    return input;
}

WiiJoystick::~WiiJoystick(){
    WPAD_Shutdown();
}

WiiJoystick::WiiJoystick(){
    WPAD_Init();
}

#endif
