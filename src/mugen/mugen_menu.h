#ifndef _mugen_menu_h
#define _mugen_menu_h

#include <vector>
#include <map>

#include "mugen_exception.h"
#include "mugen_fadetool.h"

#include "menu/menu.h"

#include "mugen/mugen_util.h"
#include "return_exception.h"

/*
 * We already have a setup for menus, might as well as utilize it to accomodate mugen
 */

class MugenAnimation;
class MugenFont;
class MugenSprite;
class MugenSection;

namespace Ast{
    class Section;
}

namespace Mugen{
    class Background;
    class Character;
    class CharacterSelect;
    class Storyboard;
}

class MugenStage;

struct MugenMenuArea{
    int x1;
    int y1;
    int x2;
    int y2;
    int alpha;
    int alphaMove;
};

class MugenMenu : public Menu {
    public:
	/*! ctor dtor */
        explicit MugenMenu(const MugenMenu & menu);
	MugenMenu(const std::string &filename);
	virtual ~MugenMenu();
	
	/*! load */
	virtual void loadData() throw (MugenException);
	
	/*! do logic, draw whatever */
	virtual void run();
	
        virtual void copyBackground(Bitmap & copyTo);
	
        virtual inline std::string getSelectInfoFile() const {
            return selectInfoFile;
        }
	
    private:
	// Option offset
	int optionLocation;
	
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
	//fadeout.time = 10
	// Utilize fade tool
	MugenFadeTool fader;
	
	//menu.pos = 159,158
	Mugen::Point position;
	/*menu.item.font = 3,0,0
	//menu.item.active.font = 3,5,0
	//menu.item.spacing = 0, 13
	*/
	Mugen::FontInfo fontItem;
	Mugen::FontInfo fontActive;
	Mugen::Point fontSpacing;
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
	Mugen::Point windowMarginX;
	Mugen::Point windowMarginY;
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
	
	// Our ticker/frames during duration of title
	unsigned long int ticker;
	
	/* Sprites */
	Mugen::SpriteMap sprites;
	
	// Title background
	Mugen::Background *background;
	
	// Logo
	Mugen::Storyboard *logo;
	
	// Intro
	Mugen::Storyboard *intro;
	
	// Select Info
	Mugen::CharacterSelect *characterSelect;
	
	// Cleanup
	void cleanup();
	
	// Cleanup sprites
	void cleanupSprites();
	
	// Draw text
	void renderText(Bitmap *bmp);

        std::string selectInfoFile;
};

#endif
