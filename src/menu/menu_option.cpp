#include "menu/menu_option.h"
#include "util/bitmap.h"

MenuOption::MenuOption(const type t) : text("")
{
	setType(t);
}

MenuOption::~MenuOption()
{
	// Nothing
}

