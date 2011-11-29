#ifndef _paintown_game_menu_options
#define _paintown_game_menu_options

#include "util/menu/optionfactory.h"
#include "paintown-engine/level/utils.h"

class MenuOption;
namespace Menu{
    class Context;
}

namespace Gui{
    class ContextBox;
}

namespace Paintown{

/* FIXME figure out what to do with this, since network depends on it." */
Level::LevelInfo doLevelMenu(const std::string dir, const Menu::Context & context);

class OptionFactory: public Menu::OptionFactory {
public:
    OptionFactory();
    virtual MenuOption * getOption(const Gui::ContextBox & parent, const Token *token) const;
    virtual ~OptionFactory();
};

}

#endif
