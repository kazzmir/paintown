#ifndef _paintown_move_list_h
#define _paintown_move_list_h

#include <vector>
#include "util/menu/menu_option.h"

namespace Gui{
    class ContextBox;
}

namespace Menu{
    class Context;
}

class Token;

namespace Paintown{
class Player;
}

class OptionMoveList: public MenuOption {
public:
    OptionMoveList(const Gui::ContextBox & parent, const Token * token, const std::vector<Paintown::Player *> & players);

    virtual ~OptionMoveList();
    virtual void logic();
    virtual void run(const Menu::Context & context);

protected:
    std::vector<Paintown::Player *> players;
};

#endif
