#ifndef _paintown_option_screen_size_h
#define _paintown_option_screen_size_h

#include "menu_option.h"
#include "util/load_exception.h"

class Token;

/*! Handles key reconfiguration */
class OptionScreenSize : public MenuOption {
public:
    OptionScreenSize(Token *token) throw (LoadException);

    // Do logic before run part
    virtual void logic();

    // Do drawing before run part
    virtual void draw(Bitmap *work);

    // Finally it has been selected, this is what shall run 
    // endGame will be set true if it is a terminating option
    virtual void run(bool &endGame);

    // This is to pass paramaters to an option ie a bar or something
    virtual bool leftKey();
    virtual bool rightKey();

    virtual ~OptionScreenSize();

protected:

    void setMode(int width, int height);

private:
    // name
    std::string name;

    int lblue, lgreen;
    int rblue, rgreen;
};

#endif

