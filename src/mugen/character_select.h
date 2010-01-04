#ifndef _mugen_character_select_h
#define _mugen_character_select_h

#include <vector>
#include <map>

#include "mugen_exception.h"
#include "mugen_fadetool.h"

#include "menu/menu.h"

#include "mugen_util.h"
#include "return_exception.h"

#include "ast/all.h"

/*
 * Character Select Screen
 */

class MugenAnimation;
class MugenBackgroundManager;
class MugenFont;
class MugenSprite;
class MugenStoryboard;
class MugenSection;

namespace Ast{
    class Section;
}

class MugenStage;

/* Encapsulate in Mugen namespace */
namespace Mugen{
    
class Character;

struct PlayerCell{
    // Cell
    Mugen::Point start;
    Mugen::Point cursor;
    MugenSprite *cursorActiveSprite;
    MugenSprite *cursorDoneSprite;
    bool blink;
    int blinkCounter;
    // The bitmaps
    Bitmap *active;
    Bitmap *done;
    //Facing
    Mugen::Point faceOffset;
    double faceScalex;
    double faceScaley;
    int facing;
    bool selecting;
    bool show;
    // Name
    Mugen::Point nameOffset;
    Mugen::FontInfo nameFont;
};

struct Cell{
    // Location
    Mugen::Point position;
    Mugen::Character *character;
    bool random;
    bool empty;
};

struct StageInfo{
    Mugen::Point stagePosition;
    Mugen::FontInfo stageActiveFont;
    Mugen::FontInfo stageActiveFont2;
    Mugen::FontInfo stageDoneFont;
    bool selected;
    int altCounter;
};

struct SelectedChars{
    std::vector<Mugen::Character *> team1;
    std::vector<Mugen::Character *> team2;
    MugenStage *selectedStage;
};
    
typedef std::map< unsigned int, std::map< unsigned int, MugenSprite * > > MugenSprites;

class CharacterSelect{
    public:
	CharacterSelect(const unsigned long int &ticker, const std::string &filename);
	virtual ~CharacterSelect();
	
	/*! load */
	virtual void load();
	
	/*! do logic, draw whatever */
	virtual SelectedChars *run(const std::string &title, const int players, const bool selectStage, Bitmap *work);
	
    private:
	void parseSelectInfo(const std::vector<Ast::Section*> & sections);
	
	// Location of file
	std::string location;
	
	// sprite file
	std::string sffFile;
	
	// music
	std::string sndFile;
	
	// Select file
	std::string selectFile;
	
	// Fonts
	std::vector<MugenFont *>fonts;
	
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
	Mugen::Point position;
	//showEmptyBoxes = 1
	bool showEmptyBoxes;
	//moveOverEmptyBoxes = 1
	bool moveOverEmptyBoxes;
	//cell.size = 27,27         ;x,y size of each cell (in pixels)
	Mugen::Point cellSize;
	//cell.spacing = 2          ;space between each cell
	int cellSpacing;
	//cell.bg.spr = 150,0       ;Note: Don't use animation for cells
	MugenSprite *cellBackgroundSprite;
	Bitmap *cellBackgroundBitmap;
	//cell.random.spr = 180,0   ;Icon for random select (don't use animation)
	MugenSprite *cellRandomSprite;
	Bitmap *cellRandomBitmap;
	//Random Switch Time
	int cellRandomSwitchTime;
	//P1 Cell Cursor
	PlayerCell p1Cursor;
	//P2 Cell Cursor
	PlayerCell p2Cursor;
	// Portrait offset
	Mugen::Point portraitOffset;
	//portrait.scale = 1,1
	Mugen::Point portraitScale;
	//title.offset = 159,13     ;Position of title (Arcade Mode, etc)
	Mugen::Point titleOffset;
	//title.font = 3,0,0        ;Font of title (-1 for none)
	Mugen::FontInfo titleFont;
	// Stage information
	StageInfo stageInfo;
	// Select background
	MugenBackgroundManager *background;
	
	const unsigned long int &selectTicker;
	
	std::vector< std::vector< Cell *> > cells;
	
	std::vector< Mugen::Character *> characters;
	
	std::vector< MugenStage *> stages;
	
	void drawCursors(const int players, Bitmap *work);
	
	void movePlayer1Cursor(int x, int y);
	
	void movePlayer2Cursor(int x, int y);
	
	void loadCharacters(const std::string &selectFile)throw (MugenException);
	
	SelectedChars *characterList;
	
	/* Sprites */
	Mugen::SpriteMap sprites;
};
}
#endif


