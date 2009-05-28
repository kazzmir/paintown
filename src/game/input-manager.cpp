#include "input-manager.h"
#include "configuration.h"
#include "object/object.h"
#include "util/joystick.h"
#include "globals.h"
#include <stdlib.h>
#include <vector>

using namespace std;

InputManager * InputManager::manager = 0;

InputManager::InputManager(){
    manager = this;
    joystick = Joystick::create();
}

InputManager::~InputManager(){
}

vector<PaintownInput> InputManager::getInput(const Configuration & configuration, const int facing){
    if (manager == 0){
        Global::debug(0) << "*BUG* Input manager not set up" << endl;
        exit(0);
    }

    return manager->_getInput(configuration, facing);
}

static vector<PaintownInput> convertJoystick(JoystickInput input, const int facing){
    vector<PaintownInput> all;
    if (input.up){
        all.push_back(Up);
    }
    if (input.right){
        if (facing == Object::FACING_RIGHT){
            all.push_back(Forward);
        } else {
            all.push_back(Back);
        }
    }
    if (input.left){
        if (facing == Object::FACING_RIGHT){
            all.push_back(Back);
        } else {
            all.push_back(Forward);
        }
    }
    if (input.down){
        all.push_back(Down);
    }
    if (input.button1){
        all.push_back(Attack1);
    }
    if (input.button2){
        all.push_back(Attack2);
    }
    if (input.button3){
        all.push_back(Attack3);
    }
    if (input.button4){
        all.push_back(Jump);
    }

    return all;
}

vector<PaintownInput> InputManager::_getInput(const Configuration & configuration, const int facing){

    keyboard.poll();
    if (joystick != NULL){
        joystick->poll();
    }

    vector<int> all_keys;
    keyboard.readKeys( all_keys );

    vector<PaintownInput> real_input = Input::convertKeyboard(configuration, facing, all_keys);

    if (joystick != NULL){
        vector<PaintownInput> joystick_keys = convertJoystick(joystick->readAll(), facing);
        for (vector<PaintownInput>::iterator it = joystick_keys.begin(); it != joystick_keys.end(); it++){
            Global::debug(2) << "Read joystick key " << *it << endl;
        }
        all_keys.insert(all_keys.begin(), joystick_keys.begin(), joystick_keys.end());
    }

    return real_input;
}
