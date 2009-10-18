#ifndef _paintown_option_joystick_h
#define _paintown_option_joystick_h

#include "menu_option.h"
#include "util/load_exception.h"
#include "return_exception.h"

#include <string>

class Token;
class Configuration;

/*! Handles joystick reconfiguration */
class OptionJoystick : public MenuOption {
public:
    // Do logic before run part
    virtual void logic();

    // Finally it has been selected, this is what shall run 
    // endGame will be set true if it is a terminating option
    virtual void run(bool &endGame) throw (ReturnException);

    OptionJoystick(Token *token)throw( LoadException );

    virtual ~OptionJoystick();

    //! keys
    enum JoystickType{
        up=0,
        down,
        left,
        right,
        jump,
        attack1,
        attack2,
        attack3,
        attack4,
        attack5,
        attack6,
        invalidkey
    };

private:
    //! name
    std::string name;
    int player;

    JoystickType type;
    int keyCode;
};

#endif

