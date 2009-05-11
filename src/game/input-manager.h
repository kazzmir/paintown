#ifndef _paintown_input_manager
#define _paintown_input_manager

/* handles keyboard/joystick/whatever input during the game */
class InputManager{
public:
    /* main has one instance of this and thats it */
    friend int ::paintown_main(int, char**);
protected:
    InputManager();
};

#endif
