#include <allegro.h>
#include "allegro-joystick.h"

void AllegroJoystick::poll(){
    ::poll_joystick();
}

JoystickInput AllegroJoystick::readAll(){
    JoystickInput input;
    JOYSTICK_INFO * info = &joy[0];
    switch(info->num_buttons > 4 ? 4 : info->num_buttons){
        case 4: input.button4 = info->button[3].b;
        case 3: input.button3 = info->button[2].b;
        case 2: input.button2 = info->button[1].b;
        case 1: input.button1 = info->button[0].b;
        case 0: {
            break;
        }
    }
    if (info->num_sticks > 0){
        if (info->stick[0].num_axis > 0){
            int position = info->stick[0].axis[0].pos;
            if (position < 0){
                input.left = true;
            } else if (position > 0){
                input.right = true;
            }
        }
        if (info->stick[0].num_axis > 1){
            int position = info->stick[0].axis[1].pos;
            if (position < 0){
                input.up = true;
            } else if (position > 0){
                input.down = true;
            }
        }
    }
    return input;
}

bool AllegroJoystick::pressed(){
    JoystickInput input = readAll();
    return input.up || input.down || input.left || input.right ||
           input.button1 || input.button2 || input.button3 || input.button4; 
}

AllegroJoystick::~AllegroJoystick(){
}

AllegroJoystick::AllegroJoystick(){
}
