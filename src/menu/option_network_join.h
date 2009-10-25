#ifndef _paintown_option_network_join_h
#define _paintown_option_network_join_h

#include "menu_option.h"
#include "util/load_exception.h"
#include "return_exception.h"
class Token;

/*! Handles key reconfiguration */
class OptionNetworkJoin : public MenuOption{
public:
	OptionNetworkJoin(Token *token)throw( LoadException );

	// Do logic before run part
	virtual void logic();
	
	// Finally it has been selected, this is what shall run 
	// endGame will be set true if it is a terminating option
	virtual void run(bool &endGame);
	
	virtual ~OptionNetworkJoin();
};

#endif
