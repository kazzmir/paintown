#include <stdlib.h>
#include "r-tech1/configuration.h"
#include "r-tech1/input/joystick.h"

/*
#ifdef LINUX
#include "linux_joystick.h"
#endif
*/

#ifdef USE_ALLEGRO5
#include "r-tech1/input/allegro5/joystick.h"
#endif
    
JoystickListener::JoystickListener(){
}

JoystickListener::~JoystickListener(){
}

Joystick * Joystick::create(int i){
    /*
#ifdef USE_ALLEGRO
    return new AllegroJoystick();
#endif
#ifdef USE_SDL
#ifdef DEVKITPRO
    return new SDLJoystick(i);
    // return new WiiJoystick();
#elif MINPSPW
    return new PSPJoystick();
#else
    return new SDLJoystick(i);
#endif
#endif
#ifdef USE_ALLEGRO5
    return new Allegro5Joystick(i);
#endif
*/

    /* TODO: support allegro5 joystick */
/*
#ifdef LINUX
    return new LinuxJoystick();
#endif
    return NULL;
*/
    return NULL;
}

Joystick::Joystick(){
}
    
Joystick::~Joystick(){
}

void Joystick::readCustomButton(Key key){
    int button;
    if (Configuration::getCustomButton(key, getDeviceId(), getName(), button)){
        customButton[button] = key;
        if (customAxis.find(key) != customAxis.end()){
            customAxis.erase(key);
        }
    }
}

void Joystick::readCustomButtons(){
    readCustomButton(Up);
    readCustomButton(Down);
    readCustomButton(Left);
    readCustomButton(Right);
    readCustomButton(Button1);
    readCustomButton(Button2);
    readCustomButton(Button3);
    readCustomButton(Button4);
    readCustomButton(Button5);
    readCustomButton(Button6);
    readCustomButton(Quit);
    readCustomButton(Start);
}

void Joystick::readCustomAxis(Key key){
    int stick, axis;
    double low, high;
    if (Configuration::getCustomAxis(key, getDeviceId(), getName(), stick, axis, low, high)){
        Axis & use = customAxis[key];
        use.stick = stick;
        use.axis = axis;
        use.low = low;
        use.high = high;
        use.on = false;

        eraseCustomButton(key);
    }
}

void Joystick::readCustomAxes(){
    readCustomAxis(Up);
    readCustomAxis(Down);
    readCustomAxis(Left);
    readCustomAxis(Right);
    readCustomAxis(Button1);
    readCustomAxis(Button2);
    readCustomAxis(Button3);
    readCustomAxis(Button4);
    readCustomAxis(Button5);
    readCustomAxis(Button6);
    readCustomAxis(Quit);
    readCustomAxis(Start);

}
    
const char * Joystick::keyToName(Key key){
    switch (key){
        case Invalid: return "Invalid";
        case Up: return "Up";
        case Down: return "Down";
        case Left: return "Left";
        case Right: return "Right";
        case Button1: return "Button1";
        case Button2: return "Button2";
        case Button3: return "Button3";
        case Button4: return "Button4";
    case Button5: return "Button5";
    case Button6: return "Button6";
    case Start: return "Start";
        case Quit: return "Quit";
    }
    return "Unknown";
}

bool Joystick::pressed() const {
    return events.size() > 0;
}
    
void Joystick::pressButton(int button){
}

void Joystick::releaseButton(int button){
}

void Joystick::axisMotion(int axis, int motion){
}

void Joystick::hatMotion(int motion){
}
    
void Joystick::setCustomButton(int button, Key key){
    Configuration::setCustomButton(key, getDeviceId(), getName(), button);

    customButton[button] = key;

    /* Can only have one unique button/axis */
    if (customAxis.find(key) != customAxis.end()){
        customAxis.erase(key);
    }
}

void Joystick::eraseCustomButton(Key key){
    std::vector<int> keys;
    for (std::map<int, Key>::iterator it = customButton.begin(); it != customButton.end(); it++){
        if (it->second == key){
            keys.push_back(it->first);
        }
    }

    for (std::vector<int>::iterator it = keys.begin(); it != keys.end(); it++){
        customButton.erase(*it);
    }
}

void Joystick::setCustomAxis(Key key, int stick, int axis, double low, double high){
    Configuration::setCustomAxis(key, getDeviceId(), getName(), stick, axis, low, high);

    Axis & use = customAxis[key];
    use.stick = stick;
    use.axis = axis;
    use.low = low;
    use.high = high;
    use.on = false;

    /* Can only have one unique button/axis */
    eraseCustomButton(key);
}
    
bool Joystick::getAxis(Key key, int & stick, int & axis, double & low, double & high) const {
    std::map<Key, Axis>::const_iterator find = customAxis.find(key);
    if (find != customAxis.end()){
        const Axis & use = find->second;
        stick = use.stick;
        axis = use.axis;
        low = use.low;
        high = use.high;
        return true;
    } else {
        return false;
    }
}
    
void Joystick::addListener(JoystickListener * listener){
    listeners.insert(listener);
}

void Joystick::removeListener(JoystickListener * listener){
    listeners.erase(listener);
}
    
std::set<JoystickListener*> Joystick::listeners;
const std::set<JoystickListener*> & Joystick::getListeners(){
    return listeners;
}
