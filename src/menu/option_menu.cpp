#include "menu/option_menu.h"
#include "menu/menu.h"
#include "util/token.h"
#include "return_exception.h"

OptionMenu::OptionMenu(Token *token)throw( LoadException ) : MenuOption(event), _menu(0)
{
	_menu = new Menu();
	_menu->load(token);
	this->setText(_menu->getName());
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

void OptionMenu::draw(Bitmap *work)
{
	// Nothing
}

void OptionMenu::run(bool &endGame)
{
	// Do our new menu
	_menu->run();
}

