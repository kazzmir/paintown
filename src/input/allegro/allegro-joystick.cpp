#ifdef USE_ALLEGRO

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

    for (int stick = 0; stick < info->num_sticks; stick++){
        JOYSTICK_STICK_INFO * this_stick = &info->stick[stick];
        if (this_stick->num_axis > 0){
            int position = this_stick->axis[0].pos;
            if (position < 0){
                input.left = true;
            } else if (position > 0){
                input.right = true;
            }
        }
        if (this_stick->num_axis > 1){
            int position = this_stick->axis[1].pos;
            if (position < 0){
                input.up = true;
            } else if (position > 0){
                input.down = true;
            }
        }
    }

    return input;
}

AllegroJoystick::~AllegroJoystick(){
}

AllegroJoystick::AllegroJoystick(){
}
    
int AllegroJoystick::getDeviceId() const {
    return 0;
}

#endif
