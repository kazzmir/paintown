#ifndef _paintown_option_quit_h
#define _paintown_option_quit_h

#include "menu_option.h"
#include "util/load_exception.h"
#include "return_exception.h"
class Token;

/*! Handles key reconfiguration */
class OptionQuit : public MenuOption{
public:
	OptionQuit( Token *token ) throw( LoadException );
	OptionQuit( const std::string &name ) throw( LoadException );

	// Do logic before run part
	virtual void logic();

	// Finally it has been selected, this is what shall run 
	// endGame will be set true if it is a terminating option
	virtual void run(bool &endGame) throw (ReturnException);

	virtual ~OptionQuit();
};

#endif
