#ifndef _mugen_option_options_h
#define _mugen_option_options_h

#include "menu/menu_option.h"
#include "util/load_exception.h"

#include <string>

class Token;

namespace Mugen{

/*! Handles Mugen Options */
class OptionOptions: public MenuOption {
public:
    // Finally it has been selected, this is what shall run 
    // endGame will be set true if it is a terminating option
    virtual void run(bool &endGame);
    
    virtual void logic();
    
    OptionOptions(Token *token) throw (LoadException);
    OptionOptions(const std::string &name) throw (LoadException);

    virtual ~OptionOptions();

private:
	    
};

}

#endif
