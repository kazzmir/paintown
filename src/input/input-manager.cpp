#include "input-manager.h"
#include "configuration.h"
#include "object/object.h"
#include "input/joystick.h"
#include "util/events.h"
#include "globals.h"
#include <stdlib.h>
#include <vector>

using namespace std;

InputManager * InputManager::manager = 0;

InputManager::InputManager():
capture(0),
joystick(NULL){
    manager = this;
    if (Configuration::isJoystickEnabled()){
        joystick = Joystick::create();
    }
}

InputManager::~InputManager(){
    delete joystick;
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
    /* FIXME: not sure if its a good idea to put the event manager here */
    Util::EventManager eventManager;
    eventManager.run();

    keyboard.poll();
    if (joystick != NULL){
        joystick->poll();
    }
}

vector<Input::PaintownInput> InputManager::_getInput(const Configuration & configuration, const int facing){

    InputMap<Input::PaintownInput> input;

    enum Input::PaintownInput all[] = {Input::Forward, Input::Back, Input::Up, Input::Down, Input::Attack1, Input::Attack2, Input::Attack3, Input::Jump, Input::Grab};
    for (unsigned int i = 0; i < sizeof(all) / sizeof(Input::PaintownInput); i++){
        input.set(configuration.getKey(all[i], facing), 0, false, all[i]);
        input.set(configuration.getJoystickKey(all[i], facing), 0, false, all[i]);
    }
    
    InputMap<Input::PaintownInput>::Output state = InputManager::getMap(input);
    return InputMap<Input::PaintownInput>::getAllPressed(state);

    /*
    vector<int> all_keys;
    keyboard.readKeys( all_keys );

    vector<Input::PaintownInput> real_input = Input::convertKeyboard(configuration, facing, all_keys);

    if (joystick != NULL){
        vector<Input::PaintownInput> joystick_keys = convertJoystick(configuration, joystick->readAll(), facing);
        real_input.insert(real_input.begin(), joystick_keys.begin(), joystick_keys.end());
    }

    return real_input;
    */
}
