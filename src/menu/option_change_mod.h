#ifndef _paintown_option_change_mod_h
#define _paintown_option_change_mod_h

#include "menu_option.h"
#include "util/load_exception.h"
class Token;

/* Change game mod */
class OptionChangeMod: public MenuOption {
public:
    OptionChangeMod(Token *token) throw (LoadException);
    virtual ~OptionChangeMod();
    virtual void logic();
    virtual void run(bool &endGame);
};

#endif

