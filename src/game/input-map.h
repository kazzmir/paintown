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
    KeyState(int delay, bool block, X out, unsigned int last_read):
        delay(delay),
        block(block),
        out(out),
        pressed(false),
        last_read(last_read){
    }

    int delay;
    bool block;
    X out;
    bool pressed;
    unsigned int last_read;
};

template <typename X>
class InputMap{
public:
    typedef std::map<X,bool> Output;

    InputMap():
        last_read(0){
    }

    virtual ~InputMap(){
        for (typename std::map<Keyboard::KeyType, KeyState<X>*>::iterator it = key_states.begin(); it != key_states.end(); it++){
            delete (*it).second;
        }
    }

    void set(Keyboard::KeyType key, int delay, bool block, X out){
        key_states[key] = new KeyState<X>(delay, block, out, last_read);
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
                    state->last_read = last_read;
                } else {
                    use = true;
                }

                if (use){
                    (*output)[state->out] = true;
                }
            }
        }

        last_read += 1;
    }
    
    void read(const JoystickInput & joystick, Output * output){
    }
    
    bool pressed(const JoystickInput & joystick, X out){
        return false;
    }

private:
    std::map<Keyboard::KeyType, KeyState<X>* > key_states;
    unsigned int last_read;
};

#endif
