#ifdef WII

#include "joystick.h"

#include <ogc/pad.h>
#include <wiiuse/wpad.h>

void WiiJoystick::poll(){
    PAD_ScanPads();
    WPAD_ScanPads();
}

JoystickInput WiiJoystick::readAll(){
    JoystickInput input;
    // Number passed should be a port 1-4 I believe same with gc pad this can be modified later for multiplayer
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
    
    // Gamecube Pad of course this assumes in the first position
    u16 gcheld = PAD_ButtonsHeld(0);
    if(gcheld & PAD_BUTTON_A){
        input.button1 = true;
    } else if(gcheld & PAD_BUTTON_B){
        input.button2 = true;
    } else if(gcheld & PAD_BUTTON_Y){
        input.button4 = true;
    } else if(gcheld & PAD_BUTTON_X){
        input.button3 = true;
    } else if(gcheld & PAD_BUTTON_UP){
        input.up = true;
    } else if(gcheld & PAD_BUTTON_DOWN){
        input.down = true;
    } else if(gcheld & PAD_BUTTON_LEFT){
        input.left = true;
    } else if(gcheld & PAD_BUTTON_RIGHT){
        input.right = true;
    } else if(gcheld & PAD_BUTTON_START){
        // enter
    } else if(gcheld & PAD_BUTTON_MENU){
        // esc
    }  

    return input;
}

WiiJoystick::~WiiJoystick(){
    WPAD_Shutdown();
}

WiiJoystick::WiiJoystick(){
    WPAD_Init();
    PAD_Init();
}

#endif
