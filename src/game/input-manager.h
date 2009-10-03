#ifndef _paintown_input_manager
#define _paintown_input_manager

#include <vector>
#include "input.h"
#include "input-map.h"
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

    virtual void _poll();

private:
    static InputManager * manager;
    Joystick * joystick;
    Keyboard keyboard;
};

#endif
