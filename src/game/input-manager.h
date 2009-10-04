#ifndef _paintown_input_manager
#define _paintown_input_manager

#include <vector>
#include "input.h"
#include "input-map.h"
#include "util/funcs.h"
#include "util/keyboard.h"

class Configuration;
class Joystick;

/* handles keyboard/joystick/whatever input during the game */
class InputManager{
public:
    /* main has one instance of this and thats it.
     * should the janitor have the reference instead?
     */
    friend int ::paintown_main(int, char**);

    static std::vector<PaintownInput> getInput(const Configuration & configuration, const int facing);
    static void poll();

    template <typename X>
    static typename InputMap<X>::Output getMap(InputMap<X> & input){
        if (manager){
            return manager->_getMap(input);
        }
        /* just crash hard.. who cares */
        *(char *)0 = 0;
        return *(typename InputMap<X>::Output*)0;
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

protected:
    InputManager();
    virtual ~InputManager();
    
    virtual std::vector<PaintownInput> _getInput(const Configuration & configuration, const int facing);

    template <typename X>
    typename InputMap<X>::Output _getMap(InputMap<X> & input){
        typename InputMap<X>::Output output;
        std::vector<int> all_keys;
        keyboard.readKeys(all_keys);

        input.read(all_keys, &output);

        if (joystick != NULL){
            JoystickInput all_joystick = joystick->readAll();
            input.read(all_joystick, &output);
        }

        return output;
    }

    template <typename X>
    bool _pressed(InputMap<X> & input, X result){
        std::vector<int> all_keys;
        keyboard.readKeys(all_keys);
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
    Joystick * joystick;
    Keyboard keyboard;
};

#endif
