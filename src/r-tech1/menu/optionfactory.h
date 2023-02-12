#ifndef _rtech1_optionfactory_h
#define _rtech1_optionfactory_h

class MenuOption;
class Token;

namespace Gui{
    class ContextBox;
}

namespace Menu{

class OptionFactory{
public:
    OptionFactory();
    virtual MenuOption * getOption(const Gui::ContextBox & parent, const Token *token) const;
    virtual ~OptionFactory();
};

}

#endif
