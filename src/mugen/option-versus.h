#ifndef _mugen_option_versus_h
#define _mugen_option_versus_h

#include "mugen/menu.h"
#include "util/load_exception.h"

#include <string>

class Token;

namespace Mugen{
    
class Character;

/*! Handles key reconfiguration */
class OptionVersus: public Mugen::ItemOption {
public:
    OptionVersus(const std::string &name) throw (LoadException);
    virtual ~OptionVersus();
    void executeOption(const Mugen::PlayerType &, bool & endGame);
    
private:
	    
};

}

#endif
