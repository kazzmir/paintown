#include "r-tech1/input/input-manager.h"
#include "r-tech1/configuration.h"
#include "r-tech1/input/joystick.h"
#include "r-tech1/events.h"
#include "r-tech1/debug.h"
#include <stdlib.h>
#include <vector>

using namespace std;

InputManager * InputManager::manager = 0;

InputManager::InputManager():
capture(0){
    manager = this;
    if (Configuration::isJoystickEnabled()){
        installJoysticks();
    }

    touch = DeviceInput::getTouchDevice();
}

const Util::ReferenceCount<DeviceInput::Touch> & InputManager::getTouch(){
    if (manager != NULL){
        return manager->touch;
    }

    throw 1;
}

void InputManager::installJoysticks(){
    joysticks.clear();
    for (int i = 0; i < Joystick::numberOfJoysticks(); i++){
        joysticks[i] = Joystick::create(i);
    }
}
    
const std::map<int, Util::ReferenceCount<Joystick> > & InputManager::getJoysticks(){
    if (manager != NULL){
        return manager->joysticks;
    }
    Global::debug(0) << "*BUG* Input manager not set up" << endl;
    exit(0);
}

#ifdef PS3
#include <io/pad.h>
extern "C" int SDL_JoystickInit();
extern "C" int SDL_JoystickQuit();
#endif
static bool needJoystickUpdate(){
#ifdef PS3
    padInfo pad;
    if (ioPadGetInfo(&pad) == 0){
        /* re-initialize the joystick stuff if we have a different
         * number of joysticks according to lv2
         */
        if (pad.connected != (unsigned) SDL_NumJoysticks()){
            SDL_JoystickQuit();
            SDL_JoystickInit();
            return true;
        }
    }
#endif
    return false;
}

void InputManager::checkJoysticks(){
    if (needJoystickUpdate()){
        installJoysticks();
    }
}

InputManager::~InputManager(){
}
    
void InputManager::deferResizeEvents(bool defer){
    if (manager != NULL){
        manager->eventManager.deferResizeEvents(defer);
    }
}

bool InputManager::anyInput(){
    if (manager == 0){
        Global::debug(0) << "*BUG* Input manager not set up" << endl;
        exit(0);
    }

    return manager->_anyInput();
}

bool InputManager::_anyInput(){
    if (keyboard.keypressed()){
        return true;
    }

    for (map<int, Util::ReferenceCount<Joystick> >::iterator it = joysticks.begin(); it != joysticks.end(); it++){
        Util::ReferenceCount<Joystick> joystick = it->second;
        if (joystick != NULL){
            return joystick->pressed();
        }
    }

    return false;
}

void InputManager::waitForClear(){
    manager->keyboard.clear();
    while (anyInput()){
        poll();
        Util::rest(1);
    }
}

void InputManager::waitForKeys(int key1, int key2, const InputSource & source){
    InputMap<int> wait;
    wait.set(key1, 0, false, 1);
    wait.set(key2, 0, false, 1);
    InputManager::waitForRelease(wait, source, 1);
    InputManager::waitForPress(wait, source, 1);
    InputManager::waitForRelease(wait, source, 1);
}

void InputManager::waitForKeys(int key, const InputSource & source){
    InputMap<int> wait;
    wait.set(key, 0, false, 1);
    InputManager::waitForRelease(wait, source, 1);
    InputManager::waitForPress(wait, source, 1);
    InputManager::waitForRelease(wait, source, 1);
}

void InputManager::poll(){
    if (manager == 0){
        Global::debug(0) << "*BUG* Input manager not set up" << endl;
        exit(0);
    }

    return manager->_poll();
}

int InputManager::readKey(){
    return manager->_readKey();
}

int InputManager::_readKey(){
    std::vector<int> keys;
    do{
        keyboard.readKeys(keys);
        if (keys.size() == 0){
            Util::rest(1);
            poll();
        }
    } while (keys.size() == 0);
    return keys.front();
}

std::vector<DeviceInput::Touch::Event> InputManager::getTouchEvents(){
    if (touch != NULL){
        return touch->getEvents();
    }

    vector<DeviceInput::Touch::Event> empty;
    return empty;

    /* we used to just throw here, not sure why really */
    throw 1;
}

void InputManager::_poll(){
#ifdef PS3
    checkJoysticks();
#endif
    if (touch != nullptr){
        touch->poll();
    }
    eventManager.run(keyboard, joysticks);
}
