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

