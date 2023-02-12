#include "r-tech1/input/input-source.h"

using std::vector;

InputSource::InputSource(bool default_){
    if (default_){
        keyboard.push_back(0);
        keyboard.push_back(1);
        joystick.push_back(0);
        joystick.push_back(1);
    }
}
    
InputSource::InputSource(const InputSource & copy):
keyboard(copy.keyboard),
joystick(copy.joystick){
}

InputSource::InputSource(const vector<int> & keyboard, const vector<int> & joystick):
keyboard(keyboard),
joystick(joystick){
}
    
InputSource & InputSource::operator=(const InputSource & copy){
    this->keyboard = copy.keyboard;
    this->joystick = copy.joystick;
    return *this;
}
    
InputSource InputSource::addKeyboard(int keyboard){
    vector<int> keyboardCopy(this->keyboard);
    keyboardCopy.push_back(keyboard);

    return InputSource(keyboardCopy, joystick);
}

InputSource InputSource::addJoystick(int joystick){
    vector<int> joystickCopy(this->joystick);
    joystickCopy.push_back(joystick);

    return InputSource(keyboard, joystickCopy);
}

InputSource::~InputSource(){
}
    
bool InputSource::useKeyboard() const {
    return keyboard.size() > 0;
}

bool InputSource::useJoystick() const {
    return joystick.size() > 0;
}

const vector<int> & InputSource::getKeyboard() const {
    return keyboard;
}
    
const vector<int> & InputSource::getJoystick() const {
    return joystick;
}
