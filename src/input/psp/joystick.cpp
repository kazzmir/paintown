#ifdef MINPSPW

#include "joystick.h"

void PSPJoystick::poll(){
    sceCtrlPeekBufferPositive(&joystick, 1); 
}

JoystickInput PSPJoystick::readAll(){
    JoystickInput input;
    if(joystick.Buttons != 0){ 

        // WIIMOTE for now, we can add in nunchuck and classic later
        if(joystick.Buttons & PSP_CTRL_START){
            //input.button7 = true;
        } else if(joystick.Buttons & PSP_CTRL_SELECT){
            //input.button8 = true;
        } else if(joystick.Buttons & PSP_CTRL_SQUARE){
            input.button1 = true;
        } else if(joystick.Buttons & PSP_CTRL_CROSS){
            input.button2 = true;
        } else if(joystick.Buttons & PSP_CTRL_TRIANGLE){
            input.button3 = true;
        } else if(joystick.Buttons & PSP_CTRL_CIRCLE){
            input.button4 = true;
        } else if(joystick.Buttons & PSP_CTRL_RTRIGGER){
        } else if(joystick.Buttons & PSP_CTRL_LTRIGGER){
        } else if(joystick.Buttons & PSP_CTRL_LEFT){
            input.left = true;
        } else if(joystick.Buttons & PSP_CTRL_RIGHT){
            input.right = true;
        } else if(joystick.Buttons & PSP_CTRL_DOWN){
            input.down = true;
        } else if(joystick.Buttons & PSP_CTRL_UP){
            input.up = true;
        }
    }

    return input;
}

PSPJoystick::~PSPJoystick(){
    // no cleanup required
}

PSPJoystick::PSPJoystick(){
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
}

#endif
