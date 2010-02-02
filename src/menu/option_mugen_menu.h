#ifndef _paintown_option_mugen_menu_h
#define _paintown_option_mugen_menu_h

#include "menu_option.h"
#include "util/load_exception.h"
#include "return_exception.h"

class Token;
class MugenMenu;

/*! Handles sub menus */
class OptionMugenMenu : public MenuOption {
public:
    // Do logic before run part
    virtual void logic();

    // Finally it has been selected, this is what shall run 
    // endGame will be set true if it is a terminating option
    virtual void run(bool &endGame);

    OptionMugenMenu(Token *token)throw( LoadException );

    virtual ~OptionMugenMenu();

private:
    MugenMenu *_menu;
};

#endif

