#ifndef _paintown_input_manager
#define _paintown_input_manager

#include <vector>
#include <algorithm>
#include "input.h"
#include "input-map.h"
#include "util/funcs.h"
#include "input/keyboard.h"
#include "exceptions/exception.h"

class Configuration;
class Joystick;

/* handles keyboard/joystick/whatever input during the game */
class InputManager{
public:
    /* main has one instance of this and thats it.
     * should the janitor have the reference instead?
     */
    friend int paintown_main(int, char**);
    friend int main(int, char **);

    static std::vector<Input::PaintownInput> getInput(const Configuration & configuration, const int facing);
    static void poll();
    static void enableBufferInput();
    static void disableBufferInput();

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
    typename InputMap<X>::Output _getMap(InputMap<X> & input){
        typename InputMap<X>::Output output;

        if (capture != 0 && capture != &input){
            return output;
        }

        std::vector<int> all_keys;
        keyboard.readKeys(all_keys);

        all_keys.insert(all_keys.end(), bufferedKeys.begin(), bufferedKeys.end());
        std::sort(all_keys.begin(), all_keys.end());
        removeDuplicates(all_keys);
        // std::unique(all_keys.begin(), all_keys.end());
        bufferedKeys.clear();

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
        all_keys.insert(all_keys.end(), bufferedKeys.begin(), bufferedKeys.end());
        std::sort(all_keys.begin(), all_keys.end());
        removeDuplicates(all_keys);
        bufferedKeys.clear();
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
    std::vector<int> bufferedKeys;
    bool bufferKeys;
};

#endif
