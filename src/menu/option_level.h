#ifndef _paintown_option_level_h
#define _paintown_option_level_h

#include "menu_option.h"
#include "util/load_exception.h"
#include "return_exception.h"
#include <string>

class Token;

/* Should rename this class to OptionSelect */

/*! Handles key reconfiguration */
class OptionLevel : public MenuOption {
public:
	OptionLevel(Token *token, int * set, int value) throw (LoadException);

	// Do logic before run part
	virtual void logic();

	// Finally it has been selected, this is what shall run 
	// endGame will be set true if it is a terminating option
	virtual void run(bool &endGame) throw (ReturnException);

	virtual ~OptionLevel();

protected:
    /* integer to set if this option is chosen */
    int * set;
    int value;
};

#endif
