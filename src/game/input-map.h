#ifndef _paintown_input_map_h
#define _paintown_input_map_h

#include <map>
#include <vector>
#include "util/joystick.h"
#include "util/keyboard.h"
#include "globals.h"
#include <iostream>

template <typename X>
struct KeyState{
    KeyState(unsigned int delay, bool block, X out, unsigned int last_read):
        delay(delay),
        block(block),
        out(out),
        pressed(false),
        last_read(last_read),
        seen(0){
    }

    unsigned int delay;
    bool block;
    X out;
    bool pressed;
    unsigned int last_read;
    unsigned int seen;
};

/* cant typedef a template'd struct so we have to copy/paste the definition
 */
template <typename X>
struct JoystickState{
    JoystickState(unsigned int delay, bool block, X out, unsigned int last_read):
        delay(delay),
        block(block),
        out(out),
        pressed(false),
        last_read(last_read),
        seen(0){
    }

    unsigned int delay;
    bool block;
    X out;
    bool pressed;
    unsigned int last_read;
    unsigned int seen;
};

/* maps a raw input device (keyboard, joystick, etc.) to some user-defined type
 * the template parameter, X, is that user-defined type
 */
template <typename X>
class InputMap{
public:
    typedef std::map<X,bool> Output;

    InputMap():
        last_read(0){
    }
    
    InputMap(const InputMap & copy){
        Global::debug(0) << "*BUG* InputMap should not be copied! Pass it by reference instead" << std::endl;
    }

    virtual ~InputMap(){
        for (typename std::map<Keyboard::KeyType, KeyState<X>*>::iterator it = key_states.begin(); it != key_states.end(); it++){
            delete (*it).second;
        }

        for (typename std::map<typename Joystick::Key, JoystickState<X>* >::iterator it = joy_states.begin(); it != joy_states.end(); it++){
            delete (*it).second;
        }
    }

    void set(Keyboard::KeyType key, int delay, bool block, X out){
        key_states[key] = new KeyState<X>(delay, block, out, last_read);
    }

    void set(typename Joystick::Key key, int delay, bool block, X out){
        joy_states[key] = new JoystickState<X>(delay, block, out, last_read);
    }

    bool pressed(const std::vector<int> & keys, X out){
        for (std::vector<int>::const_iterator it = keys.begin(); it != keys.end(); it++){
            Keyboard::KeyType key = *it;
            KeyState<X> * state = key_states[key];
            if (state != 0){
                if (state->out == out){
                    return true;
                }
            }
        }
        return false;
    }
    
    void read(const std::vector<int> & keys, Output * output){
        for (std::vector<int>::const_iterator it = keys.begin(); it != keys.end(); it++){
            Keyboard::KeyType key = *it;
            KeyState<X> * state = key_states[key];
            if (state != 0){
                bool use = false;
                // Global::debug(0) << "read " << key << " last read is " << state->last_read << " my last read is " << last_read << std::endl;
                if (state->block){
                    if (last_read - state->last_read > 1){
                        use = true;
                    }
                } else if (last_read - state->last_read > 1 || state->seen > state->delay){
                    use = true;
                    state->seen = 0;
                } else {
                    state->seen += 1;
                }

                state->last_read = last_read;

                // state->last_read = last_read;

                (*output)[state->out] = use;
            }
        }
    }

    /* called by the input manager when the map is read */
    void update(){
        last_read += 1;
    }

    void read(const JoystickInput & joystick, Output * output){
#define do_joy(field, key) if (joystick.field){\
    JoystickState<X> * state = joy_states[Joystick::key];\
    doJoyState(state, output);\
}

        do_joy(down, Down);
        do_joy(up, Up);
        do_joy(left, Left);
        do_joy(right, Right);
        do_joy(button1, Button1);
        do_joy(button2, Button2);
        do_joy(button3, Button3);
        do_joy(button4, Button4);
    /*
        if (joystick.up){
            JoystickState<X> * state = joy_states[Joystick::Up];
            doJoyState(state);
        }
        
        if (joystick.down){
            JoystickState<X> * state = joy_states[Joystick::Down];
            doJoyState(state);
        }
        */
#undef do_joy

    }
    
    bool pressed(const JoystickInput & joystick, X out){
#define do_joy(field, key) if (joystick.field){\
    JoystickState<X> * state = joy_states[Joystick::key];\
    if (state != 0 && state->out == out){\
        return true;\
    }\
}

        do_joy(down, Down);
        do_joy(up, Up);
        do_joy(left, Left);
        do_joy(right, Right);
        do_joy(button1, Button1);
        do_joy(button2, Button2);
        do_joy(button3, Button3);
        do_joy(button4, Button4);


#undef do_joy
        return false;
    }

protected:
    void doJoyState(JoystickState<X> * state, Output * output){
        if (state != 0){
            bool use = false;
            if (state->block){
                if (last_read - state->last_read > 1){
                    use = true;
                }
            } else if (last_read - state->last_read > 1 || state->seen > state->delay){
                use = true;
                state->seen = 0;
            } else {
                state->seen += 1;
            }

            state->last_read = last_read;

            // state->last_read = last_read;

            (*output)[state->out] = use;
        }
    }

private:
    std::map<Keyboard::KeyType, KeyState<X>* > key_states;
    std::map<typename Joystick::Key, JoystickState<X>* > joy_states;
    unsigned int last_read;
};

#endif
