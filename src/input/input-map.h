#ifndef _paintown_input_map_h
#define _paintown_input_map_h

#include <map>
#include <vector>
#include "joystick.h"
#include "keyboard.h"
#include "globals.h"
#include <iostream>

template <typename X>
struct KeyState{
    KeyState(unsigned int delay, bool block, X out, unsigned int last_read):
        delay(delay),
        block(block),
        out(out),
        pressed(false),
        last_read(0),
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
        last_read(0),
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
    typedef std::map<X, bool> Output;

    struct InputEvent{
        InputEvent(const X & out, Keyboard::unicode_t unicode, bool enabled):
        out(out),
        unicode(unicode),
        enabled(enabled){
        }

        InputEvent(const InputEvent & input):
        out(input.out),
        unicode(input.unicode),
        enabled(input.enabled){
        }

        InputEvent(){
        }

        bool operator[](const X & is) const {
            return this->out == is;
        }

        X out;
        Keyboard::unicode_t unicode;
        bool enabled;
    };

    InputMap():
        last_read(0){
    }
    
    InputMap(const InputMap & copy){
        for (typename std::map<Keyboard::KeyType, KeyState<X>* >::const_iterator it = copy.key_states.begin(); it != copy.key_states.end(); it++){
            key_states[(*it).first] = new KeyState<X>(*(*it).second);
        }
        for (typename std::map<typename Joystick::Key, JoystickState<X>* >::const_iterator it = copy.joy_states.begin(); it != copy.joy_states.end(); it++){
            joy_states[(*it).first] = new JoystickState<X>(*(*it).second);
        }
        last_read = copy.last_read;
    }

    InputMap & operator=(const InputMap & copy){
        for (typename std::map<Keyboard::KeyType, KeyState<X>* >::const_iterator it = copy.key_states.begin(); it != copy.key_states.end(); it++){
            key_states[(*it).first] = new KeyState<X>(*(*it).second);
        }
        for (typename std::map<typename Joystick::Key, JoystickState<X>* >::const_iterator it = copy.joy_states.begin(); it != copy.joy_states.end(); it++){
            joy_states[(*it).first] = new JoystickState<X>(*(*it).second);
        }
        last_read = copy.last_read;
        return *this;
    }

    virtual ~InputMap(){
        for (typename std::map<Keyboard::KeyType, KeyState<X>*>::iterator it = key_states.begin(); it != key_states.end(); it++){
            delete (*it).second;
        }

        for (typename std::map<typename Joystick::Key, JoystickState<X>* >::iterator it = joy_states.begin(); it != joy_states.end(); it++){
            delete (*it).second;
        }
    }

    /* key: the keyboard key to recognize, something like KEY_A
     * delay: time before successive keys are recognized (if held down)
     * block: if true then the key must be released before it will
     *   be recognized again. false allows repetition.
     * out: user defined value to set if this key is pressed
     */
    void set(Keyboard::KeyType key, int delay, bool block, X out){
        key_states[key] = new KeyState<X>(delay, block, out, last_read);
    }

    /* change an existing key */
    void update(Keyboard::KeyType key, int delay, bool block, X out){
        if (key_states[key] != 0){
            key_states[key]->delay = delay;
            key_states[key]->block = block;
            key_states[key]->out = out;
        }
    }

    /* mostly the same stuff but for joysticks.
     */
    void set(typename Joystick::Key key, int delay, bool block, X out){
        joy_states[key] = new JoystickState<X>(delay, block, out, last_read);
    }

    static std::vector<X> getAllPressed(const Output & output){
        std::vector<X> out;

        for (typename Output::const_iterator it = output.begin(); it != output.end(); it++){
            if ((*it).second){
                out.push_back((*it).first);
            }
        }

        return out;
    }

    /*
    static void observeKey(const Keyboard::KeyData & data, void * self){
        InputMap<X> * me = (InputMap<X>*) self;

        KeyState<X> * state = state = getState(data.key);
        if (state != NULL){
            events.push_back(InputEvent(state->out, data.unicode, data.enabled));
        }
    }
    */

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

    virtual KeyState<X> * getState(int key){
        return key_states[key];
    }

    virtual JoystickState<X> * getJoystickState(Joystick::Key key){
        return joy_states[key];
    }
    
    void read(const std::vector<int> & keys, Output * output){
        for (std::vector<int>::const_iterator it = keys.begin(); it != keys.end(); it++){
            Keyboard::KeyType key = *it;
            KeyState<X> * state = getState(key);
            if (state != NULL){
                bool use = false;
                // Global::debug(0) << "read " << key << " last read is " << state->last_read << " my last read is " << last_read << std::endl;
                if (state->block){
                    if (last_read - state->last_read > 1){
                        use = true;
                    }
                } else if (last_read - state->last_read > 1 || state->seen >= state->delay){
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
            } else if (last_read - state->last_read > 1 || state->seen >= state->delay){
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
