#ifndef _mugen_option_versus_h
#define _mugen_option_versus_h

#include "menu/menu_option.h"
#include "util/load_exception.h"
#include "return_exception.h"

#include <string>

class Token;
class Configuration;

/*! Handles key reconfiguration */
class MugenOptionVersus : public MenuOption
{
    public:
	// Finally it has been selected, this is what shall run 
	// endGame will be set true if it is a terminating option
	virtual void run(bool &endGame);
	
	virtual void logic();
	
	MugenOptionVersus(Token *token)throw( LoadException );
	MugenOptionVersus(const std::string &name)throw( LoadException);

	virtual ~MugenOptionVersus();
	
    private:
	    
};

#endif

