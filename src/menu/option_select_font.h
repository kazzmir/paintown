#ifndef _paintown_option_select_font_h
#define _paintown_option_select_font_h

#include <vector>

#include "menu_option.h"
#include "util/load_exception.h"
#include "return_exception.h"

class Token;

/*! Handles font selection */
class OptionSelectFont : public MenuOption {
public:
    OptionSelectFont(Token *token) throw (LoadException);

    // Do logic before run part
    virtual void logic();

    // Finally it has been selected, this is what shall run 
    // endGame will be set true if it is a terminating option
    virtual void run(bool &endGame);

    // This is to pass paramaters to an option ie a bar or something
    virtual bool leftKey();
    virtual bool rightKey();

    virtual ~OptionSelectFont();

protected:

private:
    // Type of selector
    enum Adjust{
      fontName=0,
      fontHeight,
      fontWidth
    };
    // Current type of menu to adjust
    Adjust typeAdjust;
    
    int lblue, lgreen;
    int rblue, rgreen;
    
    // fonts
    std::vector<std::string> fonts;
    
    void nextIndex(bool forward);

};

#endif

