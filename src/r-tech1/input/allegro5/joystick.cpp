#ifdef USE_ALLEGRO5

#include "r-tech1/input/joystick.h"
#include "r-tech1/input/allegro5/joystick.h"
#include "r-tech1/debug.h"
#include <allegro5/allegro.h>

using std::vector;
using std::string;
using std::map;

class ButtonMapping{
public:
    ButtonMapping(){
    }

    virtual ~ButtonMapping(){
    }

    virtual int toNative(Joystick::Key key) = 0;
    virtual Joystick::Key toKey(int button) = 0;
    virtual void axisMotionEvents(int stick, int axis, float position, vector<Joystick::Event> & events) = 0;
    virtual void hatMotionEvents(int motion, vector<Joystick::Event> & events) = 0;
};

class DefaultMapping: public ButtonMapping {
public:
    DefaultMapping(){
    }
    
    virtual int toNative(Joystick::Key key){
        switch (key){
            case Joystick::Button1: return 0;
            case Joystick::Button2: return 1;
            case Joystick::Button3: return 2;
            case Joystick::Button4: return 3;
            case Joystick::Button5: return 4;
            case Joystick::Button6: return 5;
            case Joystick::Start: return 6;
            case Joystick::Quit: return 7;
            case Joystick::Up: return 8;
            case Joystick::Down: return 9;
            case Joystick::Left: return 10;
            case Joystick::Right: return 11;
            case Joystick::Invalid: return -1;
        }

        return -1;
    }
    
    virtual Joystick::Key toKey(int button){
        switch (button){
            case 0: return Joystick::Button1;
            case 1: return Joystick::Button2;
            case 2: return Joystick::Button3;
            case 3: return Joystick::Button4;
            case 4: return Joystick::Button5;
            case 5: return Joystick::Button6;
            case 6: return Joystick::Start;
            case 7: return Joystick::Quit;
            case 8: return Joystick::Up;
            case 9: return Joystick::Down;
            case 10: return Joystick::Left;
            case 11: return Joystick::Right;
        }
        return Joystick::Invalid;
    }
    
    virtual void axisMotionEvents(int stick, int axis, float position, vector<Joystick::Event> & events){
    }

    virtual void hatMotionEvents(int motion, vector<Joystick::Event> & events){
    }

    virtual ~DefaultMapping(){
    }
};

class LogitechPrecision: public ButtonMapping {
public:
    enum Buttons{
        Button1 = 0,
        Button2 = 1,
        Button3 = 2,
        Button4 = 3,
        Start = 8,
        Select = 9,
        R2 = 7,
        R1 = 5,
        L2 = 6,
        L1 = 4
    };

    int toNative(Joystick::Key key){
        switch (key){
            case Joystick::Button1: return Button1;
            case Joystick::Button2: return Button2;
            case Joystick::Button3: return Button3;
            case Joystick::Button4: return Button4;
            case Joystick::Button5: return L1;
            case Joystick::Button6: return R1;
            case Joystick::Start: return Start;
            case Joystick::Quit: return Select;
            case Joystick::Invalid: return -1;
            case Joystick::Left:
            case Joystick::Right:
            case Joystick::Up:
            case Joystick::Down: return -1;
        }
        return -1;
    }

    Joystick::Key toKey(int button){
        switch (button){
            case Button1: return Joystick::Button1;
            case Button2: return Joystick::Button2;
            case Button3: return Joystick::Button3;
            case Button4: return Joystick::Button4;
            case L1: return Joystick::Button5;
            case R1: return Joystick::Button6;
            case Start: return Joystick::Start;
            case Select: return Joystick::Quit;
        }
        return Joystick::Invalid;
    }
    
    /* axis 1. negative up, positive down
     * axis 0, negative left, positive right
     */
    void axisMotionEvents(int stick, int axis, float position, vector<Joystick::Event> & events){
        int tolerance = 10;

        const int LeftRightAxis = 0;
        const int UpDownAxis = 1;
        switch (stick){
            case 0: {
                switch (axis){
                    case UpDownAxis: {
                        if (position == -1){
                            events.push_back(Joystick::Event(Joystick::Up, true));
                            events.push_back(Joystick::Event(Joystick::Down, false));
                        } else if (position == 1){
                            events.push_back(Joystick::Event(Joystick::Up, false));
                            events.push_back(Joystick::Event(Joystick::Down, true));
                        } else if (position == 0){
                            events.push_back(Joystick::Event(Joystick::Up, false));
                            events.push_back(Joystick::Event(Joystick::Down, false));
                        }
                        break;
                    }
                    case LeftRightAxis: {
                        if (position == -1){
                            events.push_back(Joystick::Event(Joystick::Left, true));
                            events.push_back(Joystick::Event(Joystick::Right, false));
                        } else if (position == 1){
                            events.push_back(Joystick::Event(Joystick::Left, false));
                            events.push_back(Joystick::Event(Joystick::Right, true));
                        } else if (position == 0){
                            events.push_back(Joystick::Event(Joystick::Left, false));
                            events.push_back(Joystick::Event(Joystick::Right, false));
                        }
                        break;
                    }
                    default: {
                    }
                }
            }
            default: {
                break;
            }
        }
#if 0
        if (axis == 0){
            if (position == 0){
                events.push_back(Joystick::Event(Joystick::Left, false));
                events.push_back(Joystick::Event(Joystick::Right, false));
            } else if (motion == -32768){
                events.push_back(Joystick::Event(Joystick::Left, true));
            } else if (motion == 32767){
                events.push_back(Joystick::Event(Joystick::Right, true));
            } else if (motion == 128){
                events.push_back(Joystick::Event(Joystick::Up, false));
                events.push_back(Joystick::Event(Joystick::Down, false));
            } else if (motion == 1){
                events.push_back(Joystick::Event(Joystick::Up, true));
            } else if (motion == 255){
                events.push_back(Joystick::Event(Joystick::Down, true));
            }
            /*
            if (motion < -tolerance){
                events.push_back(Joystick::Event(Joystick::Left, true));
            } else if (motion > tolerance){
                events.push_back(Joystick::Event(Joystick::Right, true));
            } else {
                / * fake a release for left and right * /
                events.push_back(Joystick::Event(Joystick::Left, false));
                events.push_back(Joystick::Event(Joystick::Right, false));
            }
            */
        } else if (axis == 1){
            if (motion < -tolerance){
                events.push_back(Joystick::Event(Joystick::Up, true));
            } else if (motion > tolerance){
                events.push_back(Joystick::Event(Joystick::Down, true));
            } else {
                events.push_back(Joystick::Event(Joystick::Up, false));
                events.push_back(Joystick::Event(Joystick::Down, false));
            }
        }
#endif
    }
    
    virtual void hatMotionEvents(int motion, vector<Joystick::Event> & events){
    }
};

static Util::ReferenceCount<ButtonMapping> createMapping(ALLEGRO_JOYSTICK * joystick){
    string name = al_get_joystick_name(joystick);
    if (name.find("Logitech(R) Precision(TM) Gamepad") != string::npos){
        return Util::ReferenceCount<ButtonMapping>(new LogitechPrecision());
    }
    return Util::ReferenceCount<ButtonMapping>(new DefaultMapping());
}

Allegro5Joystick::Allegro5Joystick(int id):
id(id){
    queue = al_create_event_queue();
    if (al_is_joystick_installed()){
        al_register_event_source(queue, al_get_joystick_event_source());
    }
    buttons = createMapping(al_get_joystick(id));

    readCustomButtons();
    readCustomAxes();
}
    
void Allegro5Joystick::axis(int stick, int axis, float position){
    /*
    if (stick == 9){
        Global::debug(0) << "stick " << stick << " axis " << axis << " position " << position << std::endl;
    }
    */

    const std::set<JoystickListener*> & listeners = getListeners();
    for (std::set<JoystickListener*>::const_iterator it = listeners.begin(); it != listeners.end(); it++){
        (*it)->axisMotion(this, stick, axis, position);
    }

    bool handled = false;
    for (std::map<Key, Axis>::iterator it = customAxis.begin(); it != customAxis.end(); it++){
        Key key = it->first;
        Axis & use = it->second;
        if (use.stick == stick && use.axis == axis){
            handled = true;

            bool output = false;

            /* Only output an event if the new axis position is different from the last one */
            if (position >= use.low && position <= use.high){
                if (use.on == false){
                    use.on = true;
                    output = true;
                }
            } else {
                /* Not in range, so if we output an event before then output a false event */
                if (use.on == true){
                    use.on = false;
                    output = true;
                }
            }

            if (output){
                events.push_back(Joystick::Event(key, use.on));
            }
        }
    }

    /* If no custom axis then use the default joystick handler */
    if (!handled){
        buttons->axisMotionEvents(stick, axis, position, events);
    }
}

Joystick::Key Allegro5Joystick::getKey(int button){
    if (customButton.find(button) != customButton.end()){
        return customButton[button];
    }
    return buttons->toKey(button);
}
    
int Allegro5Joystick::getButton(Key key){
    for (std::map<int, Key>::iterator it = customButton.begin(); it != customButton.end(); it++){
        if (it->second == key){
            return it->first;
        }
    }

    return buttons->toNative(key);
}
    
void Allegro5Joystick::buttonDown(int button){
    // Global::debug(0) << "Button down " << button << std::endl;

    const std::set<JoystickListener*> & listeners = getListeners();
    for (std::set<JoystickListener*>::const_iterator it = listeners.begin(); it != listeners.end(); it++){
        (*it)->pressButton(this, button);
    }

    Key event = getKey(button);
    if (event != Invalid){
        events.push_back(Event(event, true));
    }
}

void Allegro5Joystick::buttonUp(int button){
    // Global::debug(0) << "Button up " << button << std::endl;

    for (std::set<JoystickListener*>::iterator it = listeners.begin(); it != listeners.end(); it++){
        (*it)->releaseButton(this, button);
    }

    Key event = getKey(button);
    if (event != Invalid){
        events.push_back(Event(event, false));
    }
}

void Allegro5Joystick::poll(){
    events.clear();
    ALLEGRO_EVENT event;
    while (al_get_next_event(queue, &event)){
        switch (event.type){
            case ALLEGRO_EVENT_JOYSTICK_AXIS: {
                if (event.joystick.id == al_get_joystick(id)){
                    axis(event.joystick.stick, event.joystick.axis, event.joystick.pos);
                }
                break;
            }
            case ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN: {
                if (event.joystick.id == al_get_joystick(id)){
                    buttonDown(event.joystick.button);
                }
                break;
            }
            case ALLEGRO_EVENT_JOYSTICK_BUTTON_UP: {
                if (event.joystick.id == al_get_joystick(id)){
                    buttonUp(event.joystick.button);
                }
                break;
            }
        }
    }
}
    
std::map<int, std::map<int, double> > Allegro5Joystick::getCurrentAxisValues() const {
    ALLEGRO_JOYSTICK_STATE state;
    ALLEGRO_JOYSTICK * joystick = al_get_joystick(id);
    al_get_joystick_state(joystick, &state);
    map<int, map<int, double> > out;

    int sticks = al_get_joystick_num_sticks(joystick);
    for (int stick = 0; stick < sticks; stick++){
        int axis = al_get_joystick_num_axes(joystick, stick);
        for (int i = 0; i < axis; i++){
            out[stick][i] = state.stick[stick].axis[i];
        }
    }

    return out;
}

int Allegro5Joystick::getDeviceId() const {
    return id;
}

std::string Allegro5Joystick::getName() const {
    return al_get_joystick_name(al_get_joystick(id));
}

Allegro5Joystick::~Allegro5Joystick(){
    al_destroy_event_queue(queue);
    queue = NULL;
}

int Joystick::numberOfJoysticks(){
    return al_get_num_joysticks();
}

#endif
