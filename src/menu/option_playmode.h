#ifndef _paintown_option_playmode_h
#define _paintown_option_playmode_h

#include "menu_option.h"
#include "util/load_exception.h"

class Token;

/*! Handles key reconfiguration */
class OptionPlayMode : public MenuOption {
public:
    OptionPlayMode(Token *token) throw (LoadException);

    // Do logic before run part
    virtual void logic();

    // Finally it has been selected, this is what shall run 
    // endGame will be set true if it is a terminating option
    virtual void run(bool &endGame);

    // This is to pass paramaters to an option ie a bar or something
    virtual bool leftKey();
    virtual bool rightKey();

    virtual ~OptionPlayMode();

protected:
    virtual void changeMode();

private:
    // name
    std::string name;

    int lblue, lgreen;
    int rblue, rgreen;
};

#endif

