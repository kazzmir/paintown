#include "menu/menu_option.h"
#include "util/bitmap.h"

MenuOption::MenuOption(const type t) : currentState(Deselected), text(""), bmp(0), adjustLeftColor(Bitmap::makeColor( 255, 255, 255 )), adjustRightColor(Bitmap::makeColor( 255, 255, 255 )), runnable(true), ID(0)
{
	setType(t);
}

MenuOption::~MenuOption()
{
	// Nothing
}


// This is to pass paramaters to an option ie a bar or something
bool MenuOption::leftKey()
{
	return false;
}
bool MenuOption::rightKey()
{
	return false;
}

