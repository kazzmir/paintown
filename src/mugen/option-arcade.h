#ifndef _mugen_option_arcade_h
#define _mugen_option_arcade_h

#include "menu/menu_option.h"
#include "util/load_exception.h"

#include <string>

class Token;

namespace Mugen {

/*! Handles key reconfiguration */
class OptionArcade: public MenuOption {
public:
    // Finally it has been selected, this is what shall run 
    // endGame will be set true if it is a terminating option
    virtual void run(bool &endGame);
    
    virtual void logic();
    
    OptionArcade(Token *token) throw (LoadException);
    OptionArcade(const std::string &name) throw (LoadException);

    virtual ~OptionArcade();

private:
	    
};

}

#endif
