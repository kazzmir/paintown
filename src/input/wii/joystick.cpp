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
    
    // Wii data
    WPADData * wpad;
    wpad = WPAD_Data(0);
    
    // Gamecube
    u16 gcheld = PAD_ButtonsHeld(0);
    
    // Wii controller
    if (wpad->exp.type == WPAD_EXP_CLASSIC){
        // Classic controller
        if(wpad->btns_h & WPAD_CLASSIC_BUTTON_UP){
            input.up = true;
        }
        if(wpad->btns_h & WPAD_CLASSIC_BUTTON_DOWN){
            input.down = true;
        }
        if(wpad->btns_h & WPAD_CLASSIC_BUTTON_LEFT){
            input.left = true;
        }
        if(wpad->btns_h & WPAD_CLASSIC_BUTTON_RIGHT){
            input.right = true;
        }
    } else if (wpad->exp.type == WPAD_EXP_NUNCHUK){
        // With nunchuck
        if(wpad->exp.nunchuk.js.pos.y >= 0xB0){
            input.up = true;
        }
        if(wpad->exp.nunchuk.js.pos.y <= 0x40){
            input.down = true;
        }
        if(wpad->exp.nunchuk.js.pos.x <= 0x40){
            input.left = true;
        }
        if(wpad->exp.nunchuk.js.pos.x >= 0xB0){
            input.right = true;
        }
    } else {
        // No attachment
        if(wpad->btns_h & WPAD_BUTTON_UP){
            if(wpad->ir.valid){
                input.up = true;
            } else {
                input.left = true;
            }
        }
        if(wpad->btns_h & WPAD_BUTTON_DOWN){
            if(wpad->ir.valid){
                input.down = true;
            } else {
                input.right = true;
            }
        }
        if(wpad->btns_h & WPAD_BUTTON_LEFT){
            if(wpad->ir.valid){
                input.left = true;
            } else {
                input.down = true;
            }
        }
        if(wpad->btns_h & WPAD_BUTTON_RIGHT){
            if(wpad->ir.valid){
                input.right = true;
            } else {
                input.up = true;
            }
        }
    } 
    
    // Gamecube controller
    if(gcheld & PAD_BUTTON_UP){
        input.up = true;
    }
    if(gcheld & PAD_BUTTON_DOWN){
        input.down = true;
    }
    if(gcheld & PAD_BUTTON_LEFT){
        input.left = true;
    }
    if(gcheld & PAD_BUTTON_RIGHT){
        input.right = true;
    }
    
    // Check all input buttons
    if (wpad->btns_h & WPAD_CLASSIC_BUTTON_X || wpad->btns_h & WPAD_BUTTON_1 || gcheld & PAD_BUTTON_X){
        input.button1 = true;
    }
    if (wpad->btns_h & WPAD_CLASSIC_BUTTON_Y || wpad->btns_h & WPAD_BUTTON_2 || gcheld & PAD_BUTTON_Y){
        input.button2 = true;
    }
    if (wpad->btns_h & WPAD_CLASSIC_BUTTON_A || wpad->btns_h & WPAD_BUTTON_A || gcheld & PAD_BUTTON_A){
        input.button3 = true;
    }
    if (wpad->btns_h & WPAD_CLASSIC_BUTTON_B || wpad->btns_h & WPAD_BUTTON_B || gcheld & PAD_BUTTON_B){
        input.button4 = true;
    }
    if (wpad->btns_h & WPAD_CLASSIC_BUTTON_PLUS || wpad->btns_h & WPAD_BUTTON_PLUS || gcheld & PAD_BUTTON_START){
        // enter
    }
    if (wpad->btns_h & WPAD_CLASSIC_BUTTON_HOME || wpad->btns_h & WPAD_BUTTON_HOME || gcheld & PAD_BUTTON_MENU){
        // esc
    }
    if (wpad->btns_h & WPAD_CLASSIC_BUTTON_MINUS || wpad->btns_h & WPAD_BUTTON_MINUS){
        // ?
    }  

    return input;
}
    
int WiiJoystick::getDeviceId() const {
    /* FIXME! Figure out the device id */
    return 0;
}

WiiJoystick::~WiiJoystick(){
    WPAD_Shutdown();
}

WiiJoystick::WiiJoystick(){
    WPAD_Init();
    PAD_Init();
}

#endif
