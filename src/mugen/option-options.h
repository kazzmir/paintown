#ifndef _mugen_option_options_h
#define _mugen_option_options_h

#include "mugen/util.h"
#include "menu/menu_option.h"
#include "util/load_exception.h"

#include <string>
#include <vector>

class Token;
class MugenFont;

namespace Mugen{
    
class Option {
    public:
	Option();
	virtual ~Option();
	
	virtual void next()=0;
	virtual void prev()=0;
	virtual void enter();
	virtual void render(MugenFont &, int x, int y, const Bitmap &);
	virtual inline const Mugen::Point & getPosition() const {
	    return this->position;
	}
	virtual inline void toggleSelected(){
	    this->selected = !this->selected;
	}
    protected:
	Mugen::Point position;
	std::string optionName;
	std::string currentValue;
	
    private:
	bool selected;
	// box
	int alpha,alphaMod;
};

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
    
    std::vector<Mugen::Option *> options;
    
    std::vector<Mugen::Option *>::iterator selectedOption;
    
    void doOptions(MugenFont & font, int x, int y, const Bitmap &);
    
    
	    
};

}

#endif
