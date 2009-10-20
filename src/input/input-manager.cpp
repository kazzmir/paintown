#include "input-manager.h"
#include "configuration.h"
#include "object/object.h"
#include "input/joystick.h"
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

vector<Input::PaintownInput> InputManager::getInput(const Configuration & configuration, const int facing){
    if (manager == 0){
        Global::debug(0) << "*BUG* Input manager not set up" << endl;
        exit(0);
    }

    return manager->_getInput(configuration, facing);
}

/* these mappings should agree with configuration.cpp:defaultJoystick1Keys,
 * but otherwise they are completely arbitrary
 */
static Input::PaintownInput convertJoystickKey(const Joystick::Key key, const int facing){
    switch (key){
        case Joystick::Up : return Input::Up;
        case Joystick::Down : return Input::Down;
        case Joystick::Left : {
            if (facing == Object::FACING_RIGHT){
                return Input::Back;
            } else {
                return Input::Forward;
            }
        }
        case Joystick::Right : {
            if (facing == Object::FACING_RIGHT){
                return Input::Forward;
            } else {
                return Input::Back;
            }
        }
        case Joystick::Button1 : return Input::Attack1;
        case Joystick::Button2 : return Input::Attack2;
        case Joystick::Button3 : return Input::Attack3;
        case Joystick::Button4 : return Input::Jump;
        default : return Input::Unknown;
    }
}

static vector<Input::PaintownInput> convertJoystick(const Configuration & configuration, JoystickInput input, const int facing){
    vector<Input::PaintownInput> all;

    if (input.up){
        all.push_back(convertJoystickKey(configuration.getJoystickUp(), facing));
    }

    if (input.right){
        all.push_back(convertJoystickKey(configuration.getJoystickRight(), facing));
    }

    if (input.left){
        all.push_back(convertJoystickKey(configuration.getJoystickLeft(), facing));
    }

    if (input.down){
        all.push_back(convertJoystickKey(configuration.getJoystickDown(), facing));
    }

    if (input.button1){
        all.push_back(convertJoystickKey(configuration.getJoystickAttack1(), facing));
    }

    if (input.button2){
        all.push_back(convertJoystickKey(configuration.getJoystickAttack2(), facing));
    }

    if (input.button3){
        all.push_back(convertJoystickKey(configuration.getJoystickAttack3(), facing));
    }

    if (input.button4){
        all.push_back(convertJoystickKey(configuration.getJoystickJump(), facing));
    }

    return all;
}

void InputManager::poll(){
    if (manager == 0){
        Global::debug(0) << "*BUG* Input manager not set up" << endl;
        exit(0);
    }

    return manager->_poll();
}

void InputManager::_poll(){
    keyboard.poll();
    if (joystick != NULL){
        joystick->poll();
    }
}

vector<Input::PaintownInput> InputManager::_getInput(const Configuration & configuration, const int facing){
    vector<int> all_keys;
    keyboard.readKeys( all_keys );

    vector<Input::PaintownInput> real_input = Input::convertKeyboard(configuration, facing, all_keys);

    if (joystick != NULL){
        vector<Input::PaintownInput> joystick_keys = convertJoystick(configuration, joystick->readAll(), facing);
        real_input.insert(real_input.begin(), joystick_keys.begin(), joystick_keys.end());
    }

    return real_input;
}
