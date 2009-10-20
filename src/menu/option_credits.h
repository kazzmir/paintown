#ifndef _paintown_option_credits_h
#define _paintown_option_credits_h

#include "menu_option.h"
#include "util/load_exception.h"
#include "return_exception.h"
#include "input/input-map.h"

#include <vector>

class Token;
class Bitmap;

/*! Handles key reconfiguration */
class OptionCredits : public MenuOption {
public:
    enum CreditKey{
        Exit
    };

		// Do logic before run part
		virtual void logic();
		
		// Finally it has been selected, this is what shall run 
		// endGame will be set true if it is a terminating option
		virtual void run(bool &endGame) throw (ReturnException);
		
		OptionCredits(Token *token)throw( LoadException );
	
		virtual ~OptionCredits();
	private:
		Bitmap *background;
		std::vector<std::string> credits;
		std::string music;
		int color, title;
        InputMap<CreditKey> input;
};

#endif

