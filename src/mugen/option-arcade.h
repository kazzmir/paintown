#ifndef _mugen_option_arcade_h
#define _mugen_option_arcade_h

#include "mugen/menu.h"
#include "util/load_exception.h"

#include <string>

class Token;

namespace Mugen {

class OptionArcade: public ItemOption {
public:
    OptionArcade(const std::string &name);
    virtual ~OptionArcade();
    
    void executeOption(const Mugen::PlayerType &, bool & endGame);

private:
	    
};

}

#endif
