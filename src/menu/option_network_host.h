#ifndef _paintown_option_network_host_h
#define _paintown_option_network_host_h

#include "menu_option.h"
#include "util/load_exception.h"
class Token;

/*! Handles key reconfiguration */
class OptionNetworkHost : public MenuOption{
public:
	OptionNetworkHost(Token *token)throw( LoadException );

	// Do logic before run part
	virtual void logic();
	
	// Finally it has been selected, this is what shall run 
	// endGame will be set true if it is a terminating option
	virtual void run(bool &endGame);
	
	virtual ~OptionNetworkHost();
};

#endif
