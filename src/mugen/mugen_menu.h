#ifndef _mugen_menu_h
#define _mugen_menu_h

#include "util/load_exception.h"

#include "menu/menu.h"

/*
We already have a setup for menus, might as well as utilize it to accomodate mugen
*/

class MugenBackground;

class MugenMenu : public Menu
{
    public:
	/*! ctor dtor */
	MugenMenu(const std::string &filename);
	virtual ~MugenMenu();
	
	/*! load */
	virtual void load() throw (LoadException);
	
	/*! do logic, draw whatever */
	virtual void run();	
    private:
	std::string location;
	
	std::vector<MugenBackground *>backgrounds;
};
#endif


