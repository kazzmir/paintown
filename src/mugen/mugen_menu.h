#ifndef _mugen_menu_h
#define _mugen_menu_h

#include <vector>
#include <map>

#include "util/load_exception.h"

#include "menu/menu.h"

/*
We already have a setup for menus, might as well as utilize it to accomodate mugen
*/

class MugenAnimation;
class MugenBackground;
class MugenFont;
class MugenSprite;

struct MugenMenuPoint{
    int x;
    int y;
};

struct MugenMenuArea{
    int x1;
    int y1;
    int x2;
    int y2;
};

struct MugenMenuFont{
    int index;
    int bank;
    int position;
};

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
	
	//spr = system.sff          ;Filename of sprite data
	std::string spriteFile;
	// snd = system.snd          ;Filename of sound data
	std::string soundFile;
	//logo.storyboard =         ;Logo storyboard definition (optional)
	std::string logoFile;
	//intro.storyboard =        ;Intro storyboard definition (optional)
	std::string introFile;
	//select = select.def       ;Character and stage selection list
	std::string selectFile;
	//fight = fight.def         ;Fight definition filename
	std::string fightFile;
	/*
	; Notes about fonts:
	;   - do not index fonts greater than 9 (crash) -- according to mugen..... we aren't them we can use more :P
	;   - fonts indexed here do not refer to the ones in fight.def
	*/
	std::vector<MugenFont *>fonts;
	
	//fadein.time = 10
	int fadeInTime;
	//fadeout.time = 10
	int fadeOutTime;
	//menu.pos = 159,158
	MugenMenuPoint position;
	/*menu.item.font = 3,0,0
	//menu.item.active.font = 3,5,0
	//menu.item.spacing = 0, 13
	*/
	MugenMenuFont fontItem;
	MugenMenuFont fontActive;
	MugenMenuPoint fontSpacing;
	/*
	; Names for each of the items in the menu. Names must be in quotes.
	; Use "" to disable an item. If all are disabled, goes straight to demo mode
	; (hold Esc to quit demos). *2001.11.14 NEW*
	menu.itemname.arcade = "ARCADE"
	menu.itemname.versus = "VS MODE"
	menu.itemname.teamarcade = "TEAM ARCADE"
	menu.itemname.teamversus = "TEAM VS"
	menu.itemname.teamcoop = "TEAM CO-OP"
	menu.itemname.survival = "SURVIVAL"
	menu.itemname.survivalcoop = "SURVIVAL CO-OP"
	menu.itemname.training = "TRAINING"
	menu.itemname.watch = "WATCH"
	menu.itemname.options = "OPTIONS"
	menu.itemname.exit = "EXIT"
	
	The menu items will be of type MenuOption so this will be easy
	*/
	
	/*
	; These parameters define the window in which the items are visible
	; in.*/
	//menu.window.margins.y = 12, 8 I can only assume there is an X version of this as well
	MugenMenuPoint windowMarginX;
	MugenMenuPoint windowMarginY;
	//menu.window.visibleitems = 5
	int windowVisibleItems;
	//menu.boxcursor.visible = 1     ;Set to 0 to disable default cursor display
	bool showBoxCursor;
	//menu.boxcursor.coords = -58,-10,57,2
	MugenMenuArea boxCursorCoords;
	//; These are the sounds for cursor movement (will be implemented later)
	//cursor.move.snd = 100,0
	//cursor.done.snd = 100,1
	//cancel.snd = 100,2
	
	// Clear color for the background worksurface
	int backgroundClearColor;
	
	// Our ticker/frames during duration of title
	unsigned long int ticker;
	
	/* Sprites */
	std::map< unsigned int, std::map< unsigned int, MugenSprite * > > sprites;
	
	/* Animation Lists stored by action number */
	std::map< int, MugenAnimation * > animations;
	
	// Backgrounds
	std::vector<MugenBackground *>backgrounds;
	
	// Cleanup
	void cleanup();
};
#endif


