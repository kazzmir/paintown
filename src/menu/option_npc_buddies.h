#ifndef _paintown_option_npc_buddies_h
#define _paintown_option_npc_buddies_h

#include "menu_option.h"
#include "util/load_exception.h"
#include "return_exception.h"
class Token;

/*! Handles key reconfiguration */
class OptionNpcBuddies : public MenuOption {
public:
	OptionNpcBuddies( Token *token ) throw( LoadException );

	// Do logic before run part
	virtual void logic();
	
	// Finally it has been selected, this is what shall run 
	// endGame will be set true if it is a terminating option
	virtual void run(bool &endGame);
	
	// This is to pass paramaters to an option ie a bar or something
	virtual bool leftKey();
	virtual bool rightKey();
	
	virtual ~OptionNpcBuddies();
private:
	// name
	std::string name;
	
	int lblue, lgreen;
	int rblue, rgreen;
};

#endif
