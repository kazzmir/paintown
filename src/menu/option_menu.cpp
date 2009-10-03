#include "menu/option_menu.h"
#include "menu/menu.h"
#include "util/token.h"
#include "return_exception.h"
#include "util/token_exception.h"
#include "util/funcs.h"
#include "util/file-system.h"
#include "globals.h"

OptionMenu::OptionMenu(Token *token) throw (LoadException):
MenuOption(token, Event),
menu(0){
    // Check whether we have a menu or tabmenu
    if ( *token == "menu" ){
	menu = new Menu();
    } else {
	throw LoadException("Not a menu");
    }
    // Set this menu as an option
    menu->setAsOption(true);
    
    /*
    // Lets try loading from a file
    std::string temp;
    // Filename
    *token >> temp;
    */

    if (token->numTokens() == 1){
        std::string temp;
        *token >> temp;
        menu->load(Filesystem::find(temp));
    } else {
        menu->load(token);
    }

    this->setText(menu->getName());
    
    // Lets check if this menu is going bye bye
    if ( menu->checkRemoval() ) setForRemoval(true);
}

OptionMenu::~OptionMenu()
{
	// Delete our menu
	if(menu)delete menu;
}

void OptionMenu::logic()
{
	// Nothing
}

void OptionMenu::run(bool &endGame) throw (ReturnException) {
	// Do our new menu
	menu->run();
}

void OptionMenu::setParent(Menu *menu){
    this->parent = menu;
    this->menu->setParent(menu);
}

