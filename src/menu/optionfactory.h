#ifndef _paintown_optionfactory_h
#define _paintown_optionfactory_h

#include "util/load_exception.h"

class MenuOption;
class Token;

namespace OptionFactory{
    MenuOption * getOption(Token *token);
}

#endif

