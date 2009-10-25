#include "menu/option_tabmenu.h"
#include "menu/tab_menu.h"
#include "util/token.h"
#include "return_exception.h"
#include "util/token_exception.h"
#include "util/funcs.h"
#include "util/file-system.h"
#include "globals.h"

OptionTabMenu::OptionTabMenu(Token *token) throw (LoadException):
MenuOption(token, Event),
_menu(0){
    // Check whether we have a menu or tabmenu
    if (*token == "tabmenu"){
	_menu = new TabMenu();
    } else {
	throw LoadException("Not a tabbed menu");
    }
    // Set this menu as an option
    _menu->setAsOption(true);
    
    /*
    // Lets try loading from a file
    std::string temp;
    // Filename
    *token >> temp;
    */

    if (token->numTokens() == 1){
        std::string temp;
        *token >> temp;
        _menu->load(Filesystem::find(temp));
    } else {
        _menu->load(token);
    }

    this->setText(_menu->getName());
    
    // Lets check if this menu is going bye bye
    if ( _menu->checkRemoval() ) setForRemoval(true);
}

OptionTabMenu::~OptionTabMenu()
{
	// Delete our menu
	if(_menu)delete _menu;
}

void OptionTabMenu::logic()
{
	// Nothing
}

void OptionTabMenu::run(bool &endGame){
	// Do our new menu
	_menu->run();
}

