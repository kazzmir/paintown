#ifndef _paintown_optionfactory_h
#define _paintown_optionfactory_h

#include "util/load_exception.h"

class MenuOption;
class Token;

namespace Menu{

class OptionFactory{
public:
    OptionFactory();
    virtual MenuOption * getOption(const Token *token) const;
    virtual ~OptionFactory();
};

}

#endif
