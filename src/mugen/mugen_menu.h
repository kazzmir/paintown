#ifndef _mugen_menu_h
#define _mugen_menu_h

#include <vector>
#include <map>

#include "mugen_exception.h"
#include "mugen_fadetool.h"

#include "menu/menu.h"

/*
We already have a setup for menus, might as well as utilize it to accomodate mugen
*/

class MugenAnimation;
class MugenBackgroundManager;
class MugenFont;
class MugenSprite;
class MugenStoryboard;
class MugenSection;

struct MugenMenuPoint{
    int x;
    int y;
};

struct MugenMenuArea{
    int x1;
    int y1;
    int x2;
    int y2;
    int alpha;
    int alphaMove;
};

struct MugenMenuFont{
    int index;
    int bank;
    int position;
};

struct MugenPlayerCell{
    // Cell
    MugenMenuPoint cursor;
    MugenSprite *cursorActiveSprite;
    MugenSprite *cursorDoneSprite;
    bool blink;
    // The bitmaps
    Bitmap *active;
    Bitmap *done;
    //Facing
    MugenMenuPoint faceOffset;
    double faceScalex;
    double faceScaley;
    int facing;
    // Name
    MugenMenuPoint nameOffset;
    MugenMenuFont nameFont;
};

class MugenCharacterSelect{
    public:
	MugenCharacterSelect(const unsigned long int &ticker);
	virtual ~MugenCharacterSelect();
	
	/*! load */
	virtual void load(const std::string &selectFile, unsigned int &index, std::vector< MugenSection * > &collection, 
			   std::map< unsigned int, std::map< unsigned int, MugenSprite * > > &sprites) throw (MugenException);
	
	/*! do logic, draw whatever */
	virtual void run(Bitmap *work);	
    private:
	//fadein.time = 10
	//fadeout.time = 10
	MugenFadeTool fader;
	
	//rows = 6
	int rows;
	//columns = 10
	int columns;
	//wrapping = 1              ;1 to let cursor wrap around
	bool wrapping;
	//pos = 17,24               ;Position to draw to
	MugenMenuPoint position;
	//showEmptyBoxes = 1
	bool showEmptyBoxes;
	//moveOverEmptyBoxes = 1
	bool moveOverEmptyBoxes;
	//cell.size = 27,27         ;x,y size of each cell (in pixels)
	MugenMenuPoint cellSize;
	//cell.spacing = 2          ;space between each cell
	int cellSpacing;
	//cell.bg.spr = 150,0       ;Note: Don't use animation for cells
	MugenSprite *cellBackgroundSprite;
	Bitmap *cellBackgroundBitmap;
	//cell.random.spr = 180,0   ;Icon for random select (don't use animation)
	MugenSprite *cellRandomSprite;
	Bitmap *cellRandomBitmap;
	//cell.random.switchtime = 4;Time to wait before changing to another random portrait
	int cellRandomSwitchTime;
	//p1.cursor.startcell = 0,0
	//p1.cursor.active.spr = 160,0
	//p1.cursor.done.spr = 161,0
	//p1.cursor.move.snd = 100,0
	//p1.cursor.done.snd = 100,1
	//p1.random.move.snd = 100,0
	MugenPlayerCell p1Cursor;
	/*p2.cursor.startcell = 0,4
	p2.cursor.active.spr = 170,0
	p2.cursor.done.spr = 171,0
	p2.cursor.blink = 1       ;1 to blink p2's cursor if overlapping p1's
	p2.cursor.move.snd = 100,0
	p2.cursor.done.snd = 100,1
	p2.random.move.snd = 100,0*/
	MugenPlayerCell p2Cursor;
	//random.move.snd.cancel = 0;1 to have random move sound cancel itself when played repeatedly
	//stage.move.snd = 100,0
	//stage.done.snd = 100,1
	//cancel.snd = 100,2
	//portrait.offset = 0,0
	MugenMenuPoint portraitOffset;
	//portrait.scale = 1,1
	MugenMenuPoint portraitScale;
	//title.offset = 159,13     ;Position of title (Arcade Mode, etc)
	MugenMenuPoint titleOffset;
	//title.font = 3,0,0        ;Font of title (-1 for none)
	MugenMenuFont titleFont;
	/*;Big portraits
	p1.face.offset = 1,193    ;Position to put big portrait
	p1.face.scale = .35, .33
	p1.face.facing = 1
	p2.face.offset = 318,193
	p2.face.scale = .35, .33
	p2.face.facing = -1
	;Name
	p1.name.offset = 3,236    ;Position to put name
	p1.name.font = 3,4,1      ;Set to -1 for no display
	p2.name.offset = 316,236
	p2.name.font = 3,1,-1
	;Stage select
	stage.pos = 160,216
	stage.active.font = 3,0,0
	stage.active2.font = 3,2  ;Second font color for blinking
	stage.done.font = 3,0
	*/
	MugenMenuPoint stagePosition;
	MugenMenuFont stageActiveFont;
	MugenMenuFont stageActiveFont2;
	MugenMenuFont stageDoneFont;
	/*
	;Team menu
	teammenu.move.wrapping = 1
	p1.teammenu.pos = 20, 30
	;p1.teammenu.bg.spr =
	p1.teammenu.selftitle.font = 3,0, 1
	p1.teammenu.selftitle.text = Select team mode
	p1.teammenu.enemytitle.font = 3,0, 1
	p1.teammenu.enemytitle.text = Opponent team mode
	p1.teammenu.move.snd = 100,0
	p1.teammenu.value.snd = 100,0
	p1.teammenu.done.snd = 100,1
	p1.teammenu.item.offset = 0,20
	p1.teammenu.item.spacing = 0,15
	p1.teammenu.item.font = 3,0, 1
	p1.teammenu.item.active.font = 3,3, 1
	p1.teammenu.item.active2.font = 3,0, 1 ;Second font color for blinking
	p1.teammenu.item.cursor.offset = -10, 0
	p1.teammenu.item.cursor.anim = 180
	p1.teammenu.value.icon.offset = 60,1
	p1.teammenu.value.icon.spr = 181,0
	p1.teammenu.value.empty.icon.offset = 60,1
	p1.teammenu.value.empty.icon.spr = 182,0
	p1.teammenu.value.spacing = 6,0
	;p2 team
	p2.teammenu.pos = 299, 30
	;p2.teammenu.bg.spr =
	p2.teammenu.selftitle.font = 3,0, -1
	p2.teammenu.selftitle.text = Select team mode
	p2.teammenu.enemytitle.font = 3,0, -1
	p2.teammenu.enemytitle.text = Opponent team mode
	p2.teammenu.move.snd = 100,0
	p2.teammenu.value.snd = 100,0
	p2.teammenu.done.snd = 100,1
	p2.teammenu.item.offset = 0,20
	p2.teammenu.item.spacing = 0,15
	p2.teammenu.item.font = 3,0, -1
	p2.teammenu.item.active.font = 3,1, -1
	p2.teammenu.item.active2.font = 3,0, -1 ;Second font color for blinking
	p2.teammenu.item.cursor.offset = 10, 0
	p2.teammenu.item.cursor.anim = 190
	p2.teammenu.value.icon.offset = -60,1
	p2.teammenu.value.icon.spr = 191,0
	p2.teammenu.value.empty.icon.offset = -60,1
	p2.teammenu.value.empty.icon.spr = 192,0
	p2.teammenu.value.spacing = -6,0
	*/
	// Select background
	MugenBackgroundManager *background;
	
	const unsigned long int &selectTicker;
};

class MugenMenu : public Menu
{
    public:
	/*! ctor dtor */
	MugenMenu(const std::string &filename);
	virtual ~MugenMenu();
	
	/*! load */
	virtual void load() throw (MugenException);
	
	/*! do logic, draw whatever */
	virtual void run();	
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
	
	// Our ticker/frames during duration of title
	unsigned long int ticker;
	
	/* Sprites */
	std::map< unsigned int, std::map< unsigned int, MugenSprite * > > sprites;
	
	// Title background
	MugenBackgroundManager *background;
	
	// Logo
	MugenStoryboard *logo;
	
	// Intro
	MugenStoryboard *intro;
	
	// Select Info
	MugenCharacterSelect *characterSelect;
	
	// Cleanup
	void cleanup();
	
	// Cleanup sprites
	void cleanupSprites();
	
	// Draw text
	void drawText(Bitmap *bmp);
};
#endif


