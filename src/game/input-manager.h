#ifndef _paintown_input_manager
#define _paintown_input_manager

#include <vector>
#include "input.h"
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
protected:
    InputManager();
    virtual ~InputManager();
    
    virtual std::vector<PaintownInput> _getInput(const Configuration & configuration, const int facing);

private:
    static InputManager * manager;
    Joystick * joystick;
    Keyboard keyboard;
};

#endif
