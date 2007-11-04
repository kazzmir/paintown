#include "menu/menu_option.h"
#include "util/bitmap.h"

MenuOption::MenuOption(const type t) : text(""), ID(0)
{
	setType(t);
}

MenuOption::~MenuOption()
{
	// Nothing
}


// This is to pass paramaters to an option ie a bar or something
void MenuOption::leftKey()
{
	// Nothing
}
void MenuOption::rightKey()
{
	// Nothing
}

