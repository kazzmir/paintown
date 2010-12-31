#ifndef _paintown_input_manager
#define _paintown_input_manager

#include <vector>
#include <algorithm>
#include "input.h"
#include "input-map.h"
#include "util/funcs.h"
#include "keyboard.h"
#include "exceptions/exception.h"

class Configuration;
class Joystick;

template <class Output>
class InputHandler{
public:
    InputHandler(){
    }

    virtual ~InputHandler(){
    }

    virtual void press(const Output & out, Keyboard::unicode_t unicode) = 0;
    virtual void release(const Output & out, Keyboard::unicode_t unicode) = 0;
};

/* handles keyboard/joystick/whatever input during the game */
class InputManager{
public:
    /* main has one instance of this and thats it.
     * should the janitor have the reference instead?
     */
    friend int paintown_main(int, char**);
    friend int main(int, char **);

    /* returns true if any input device is activated (keys pressed, joystick button */
    static bool anyInput();

    static std::vector<Input::PaintownInput> getInput(const Configuration & configuration, const int facing);
    static void poll();
    /*
    static void enableBufferInput();
    static void disableBufferInput();
    */
    static void waitForKeys(int key1, int key2);
    static void waitForKeys(int key1);
    static int readKey();
    static void waitForClear();

    /*
    template <class X>
    static void observeKeyboard(InputMap<X> & input){
        manager->keyboard.addObserver(InputMap<X>::observeKey, &input);
    }
    
    template <class X>
    static void unObserveKeyboard(InputMap<X> & input){
        manager->keyboard.removeObserver(InputMap<X>::observeKey, &input);
    }
    */

    static std::vector<Keyboard::unicode_t> readText(){
        return manager->keyboard.readText();
    }

    template <typename X>
    static std::vector<Keyboard::unicode_t> readText(InputMap<X> & input, typename InputMap<X>::Output & output){
        return manager->_readText(input, output);
    }

    template <typename X>
    static typename InputMap<X>::Output getMap(InputMap<X> & input){
        if (manager){
            return manager->_getMap(input);
        }
        /* just crash hard.. who cares */
        throw Exception::Base(__FILE__, __LINE__);
        /* make the compiler happy about returning something */
        return *(typename InputMap<X>::Output*)1;
    }

    template <typename X>
    static typename std::vector<typename InputMap<X>::InputEvent> getEvents(InputMap<X> & input){
        return manager->_getEvents(input);
    }

    template <typename X>
    static void handleEvents(InputMap<X> & input, InputHandler<X> & handler){
        typename std::vector<typename InputMap<X>::InputEvent> events = getEvents(input);
        for (typename std::vector<typename InputMap<X>::InputEvent>::iterator it = events.begin(); it != events.end(); it++){
            const typename InputMap<X>::InputEvent & event = *it;
            if (event.enabled){
                handler.press(event.out, event.unicode);
            } else {
                handler.release(event.out, event.unicode);
            }
        }
    }

    template <typename X>
    static bool pressed(InputMap<X> & input, X out){
        if (manager){
            return manager->_pressed(input, out);
        }
        return false;
    }

    /* wait for a key to be released
     * really this waits for all inputs that would result in `out'
     * being generated to stop.
     */
    template <typename X>
    static void waitForRelease(InputMap<X> & input, X out){
        while (InputManager::pressed(input, out)){
            Util::rest(1);
            InputManager::poll();
        }
    }

    template <typename X>
    static void waitForPress(InputMap<X> & input, X out){
        while (!InputManager::pressed(input, out)){
            Util::rest(1);
            InputManager::poll();
        }
    }

    template <typename X>
    static void captureInput(InputMap<X> & input){
        manager->_captureInput(input);
    }

    template <typename X>
    static void releaseInput(InputMap<X> & input){
        manager->_releaseInput(input);
    }

protected:
    InputManager();
    virtual ~InputManager();
    
    virtual bool _anyInput();

    virtual int _readKey();
    
    virtual std::vector<Input::PaintownInput> _getInput(const Configuration & configuration, const int facing);

    template <typename X>
    void _captureInput(InputMap<X> & input){
        capture = (void*) &input;
    }

    template <typename X>
    void _releaseInput(InputMap<X> & input){
        if (capture == (void*) &input){
            capture = 0;
        }
    }

    template <typename X>
    std::vector<Keyboard::unicode_t> _readText(InputMap<X> & input, typename InputMap<X>::Output & output){
        std::vector<Keyboard::unicode_t> text;
        std::vector<Keyboard::KeyData> all = keyboard.readData();
        for (std::vector<Keyboard::KeyData>::iterator it = all.begin(); it != all.end(); it++){
            const Keyboard::KeyData & data = *it;
            KeyState<X> * state = input.getState(data.key);
            if (state != NULL && output[state->out]){
                text.push_back(data.unicode);
            }
        }

        return text;
    }

    void removeDuplicates(std::vector<int> & storage){
        std::vector<int> output;
        int last = -1;
        for (std::vector<int>::iterator it = storage.begin(); it != storage.end(); it++){
            if (*it != last){
                output.push_back(*it);
                last = *it;
            }
        }
        storage = output;
    }

    template <typename X>
    typename std::vector<typename InputMap<X>::InputEvent> _getEvents(InputMap<X> & input){
        std::vector<typename InputMap<X>::InputEvent> events;
        if (capture != NULL && capture != &input){
            return events;
        }

        const std::vector<typename Keyboard::KeyData> & buffer = keyboard.getBufferedKeys();
        for (std::vector<Keyboard::KeyData>::const_iterator it = buffer.begin(); it != buffer.end(); it++){
            const Keyboard::KeyData & data = *it;
            KeyState<X> * state = input.getState(data.key);
            if (state != NULL){
                events.push_back(typename InputMap<X>::InputEvent(state->out, data.unicode, data.enabled));
            }
        }

        if (joystick){
            const std::vector<typename Joystick::Event> & joystickEvents = joystick->getEvents();
            for (std::vector<Joystick::Event>::const_iterator it = joystickEvents.begin(); it != joystickEvents.end(); it++){
                Joystick::Event event = *it;
                JoystickState<X> * state = input.getJoystickState(event);
                if (state != NULL){
                    events.push_back(typename InputMap<X>::InputEvent(state->out, -1, true));
                }
            }
        }

        return events;
    }

    template <typename X>
    typename InputMap<X>::Output _getMap(InputMap<X> & input){
        typename InputMap<X>::Output output;

        if (capture != 0 && capture != &input){
            return output;
        }

        std::vector<int> all_keys;
        keyboard.readKeys(all_keys);
        keyboard.readBufferedKeys(all_keys);
        // all_keys.insert(all_keys.end(), bufferedKeys.begin(), bufferedKeys.end());
        // all_keys.insert(all_keys.end(), keyboard.currentKeys().begin(), keyboard.currentKeys.end());
        std::sort(all_keys.begin(), all_keys.end());
        removeDuplicates(all_keys);
        // std::unique(all_keys.begin(), all_keys.end());
        // bufferedKeys.clear();

        input.read(all_keys, &output);

        if (joystick != NULL){
            JoystickInput all_joystick = joystick->readAll();
            input.read(all_joystick, &output);
        }

        /* just bumps an internal counter */
        input.update();

        return output;
    }

    template <typename X>
    bool _pressed(InputMap<X> & input, X result){

        if (capture != 0 && capture != &input){
            return false;
        }

        std::vector<int> all_keys;
        keyboard.readKeys(all_keys);
        // all_keys.insert(all_keys.end(), bufferedKeys.begin(), bufferedKeys.end());
        std::sort(all_keys.begin(), all_keys.end());
        removeDuplicates(all_keys);
        // bufferedKeys.clear();
        bool out = false;

        out = input.pressed(all_keys, result);

        if (joystick != NULL){
            JoystickInput all_joystick = joystick->readAll();
            out |= input.pressed(all_joystick, result);
        }

        return out;
    }

    virtual void _poll();

private:
    static InputManager * manager;
    void * capture;
    Joystick * joystick;
    Keyboard keyboard;
    // std::vector<int> bufferedKeys;
    // bool bufferKeys;
};

#endif
