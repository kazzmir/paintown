#ifndef _paintown_option_level_h
#define _paintown_option_level_h

#include "menu_option.h"
#include "util/load_exception.h"
#include <string>

class Token;

/*! Handles key reconfiguration */
class OptionLevel : public MenuOption {
public:
	OptionLevel(Token *token, std::string * set) throw (LoadException);

	// Do logic before run part
	virtual void logic();

	// Finally it has been selected, this is what shall run 
	// endGame will be set true if it is a terminating option
	virtual void run(bool &endGame);

	virtual ~OptionLevel();

protected:
        /* string to set if this option is chosen */
        std::string * update;
};

#endif
