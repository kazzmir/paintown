#ifndef _mugen_character_select_h
#define _mugen_character_select_h

#include <vector>
#include <map>

#include "mugen_exception.h"
#include "mugen_fadetool.h"

#include "menu/menu.h"

#include "input/input-map.h"

#include "mugen_util.h"
#include "mugen/character.h"
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
    
/* Handles the select screen grid */
class Grid{
    public:
	Grid();
	virtual ~Grid();
	
	virtual void render(const Bitmap &);
	
    private:
	int rows;
	int columns;
	bool wrapping;
	Mugen::Point position;
	bool showEmptyBoxes;
	bool moveOverEmptyBoxes;
	Mugen::Point cellSize;
	int cellSpacing;
	MugenSprite *cellBackgroundSprite;
	Bitmap *cellBackgroundBitmap;
	MugenSprite *cellRandomSprite;
	Bitmap *cellRandomBitmap;
	int cellRandomSwitchTime;
};

/* Handles player cursors */
class Cursor{
    public:
	Cursor();
	virtual ~Cursor();
	
	virtual void act();
	
	virtual void render(const Bitmap &);
	
	/*virtual inline void setInput(const InputMap<Command::Keys> & input){
	    this->input = input;
	}*/
	
	virtual inline void setStart(int x, int y){
	    this->start.x = x;
	    this->start.y = y;
	}
	virtual inline void setCursor(int x, int y){
	    this->cursor.x = x;
	    this->cursor.y = y;
	}
	
	virtual inline void setCursorActiveSprite(MugenSprite *spr){
	    this->cursorActiveSprite = spr;
	}
	
	virtual inline void setCursorDoneSprite(MugenSprite *spr){
	    this->cursorDoneSprite = spr;
	}
	
	virtual inline void setBlink(bool b){
	    this->blink = b;
	}
	
	virtual inline void setBlinkCounter(int b){
	    this->blinkCounter = b;
	}
	
	virtual inline void setFacingOffset(int x, int y){
	    this->faceOffset.x = x;
	    this->faceOffset.y = y;
	}
	
	virtual inline void setFaceScaleX(double x){
	    this->faceScalex = x;
	}
	
	virtual inline void setFaceScaleY(double y){
	    this->faceScaley = y;
	}
	
	virtual inline void setFacing(int f){
	    this->facing = f;
	}
	
	virtual inline void setSelecting(bool s){
	    this->selecting = s;
	}
	
	virtual inline void setActive(bool a){
	    this->active = a;
	}
	
	virtual inline void setNameOffset(int x, int y){
	    this->nameOffset.x = x;
	    this->nameOffset.y = y;
	}
	
	virtual inline void setFontInfo(Mugen::FontInfo info){
	    this->nameFont = info;
	}
	
    private:
	/*InputMap<Command::Keys> &input;*/
	// Cell
	Mugen::Point start;
	Mugen::Point cursor;
	MugenSprite *cursorActiveSprite;
	MugenSprite *cursorDoneSprite;
	bool blink;
	int blinkCounter;
	//Facing
	Mugen::Point faceOffset;
	double faceScalex;
	double faceScaley;
	int facing;
	bool selecting;
	bool active;
	// Name
	Mugen::Point nameOffset;
	Mugen::FontInfo nameFont;
};

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
	
	void loadCharacters(const std::string &selectFile);
	
	SelectedChars *characterList;
	
	/* Sprites */
	Mugen::SpriteMap sprites;
};
}
#endif


