#ifndef _paintown_options_h
#define _paintown_options_h

#include "menu_option.h"
#include "return_exception.h"

#include <vector>

class Token;

/*! Handles key reconfiguration */
class OptionSound: public MenuOption {
public:
    OptionSound(Token *token);

    // Do logic before run part
    virtual void logic();

    // Finally it has been selected, this is what shall run 
    // endGame will be set true if it is a terminating option
    virtual void run(bool &endGame);

    // This is to pass parameters to an option ie a bar or something
    virtual bool leftKey();
    virtual bool rightKey();

    virtual ~OptionSound();

protected:
    void changeSound(int much);

private:
    int lblue, lgreen;
    int rblue, rgreen;
};

#endif
