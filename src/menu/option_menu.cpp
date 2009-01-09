#include "menu/option_menu.h"
#include "menu/menu.h"
#include "util/token.h"
#include "return_exception.h"
#include "util/token_exception.h"
#include "util/funcs.h"

OptionMenu::OptionMenu(Token *token) throw (LoadException): MenuOption(token, Event), _menu(0)
{
	_menu = new Menu();
	
	// Set this menu as an option
	_menu->setAsOption(true);
	
	// Lets try loading from a file
	std::string temp;
	// Filename
	*token >> temp;
	if(temp=="name")
	{
		token->resetToken();
		_menu->load(token);
	}
	else _menu->load(Util::getDataPath() + temp);
	this->setText(_menu->getName());
	
	// Lets check if this menu is going bye bye
	if ( _menu->checkRemoval() ) setForRemoval(true);
}

OptionMenu::~OptionMenu()
{
	// Delete our menu
	if(_menu)delete _menu;
}

void OptionMenu::logic()
{
	// Nothing
}

void OptionMenu::run(bool &endGame)
{
	// Do our new menu
	_menu->run();
}

