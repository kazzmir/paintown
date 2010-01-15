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
    
class FontHandler{
    public:
	FontHandler();
	virtual ~FontHandler();
	
	void act();
	void render(const std::string &text, const Bitmap &);
	
	enum State{
	    Normal,
	    Blink,
	    Done
	};
	
	virtual inline void setLocation(int x, int y){
	    location.x = x;
	    location.y = y;
	}
	
	virtual inline void setState(State state){
	    this->state = state;
	}
	
	virtual inline State getState() const {
	    return state;
	}
	
	virtual inline void setBlinkTime(int time){
	    this->blinkTime = time;
	}
	
	virtual inline void setPrimary(MugenFont *font, int bank, int position){
	    this->font = font;
	    this->bank = bank;
	    this->position = position;
	}
	virtual inline void setBlink(MugenFont *font, int bank, int position){
	    this->state = Blink;
	    this->blinkFont = font;
	    this->blinkBank = bank;
	    this->blinkPosition = position;
	}
	virtual inline void setDone(MugenFont *font, int bank, int position){
	    this->doneFont = font;
	    this->doneBank = bank;
	    this->donePosition = position;
	}
    private:
	//! Current state
	State state;
	//! Position
	Mugen::Point location;
	//! primary font
	MugenFont *font;
	int bank;
	int position;
	//! secondary font for blinking
	MugenFont *blinkFont;
	int blinkBank;
	int blinkPosition;
	//! third font for when completed
	MugenFont *doneFont;
	int doneBank;
	int donePosition;
	//! ticker for font switching when blinking
	int ticker;
	//! blink time defaults to 10
	int blinkTime;
	//! Blink setting
	State blinkState;
};

/* Character Info handler, portrait name and etc */
class CharacterInfo {
    public:
        CharacterInfo(const std::string &definitionFile);
        virtual ~CharacterInfo();

        virtual inline MugenSprite * getIcon() const{
            return icon;
        }

        virtual inline MugenSprite * getPortrait() const{
            return portrait;
        }

        virtual inline const std::string & getName() const{
            return name;
        }

        virtual inline const std::string & getDisplayName() const{
            return displayName;
        }

        virtual void setAct(int index);

        virtual inline void setRandomStage(bool r){
            randomStage = r;
        }

        virtual inline const bool hasRandomStage() const{
            return randomStage;
        }

        virtual inline void setStage(const std::string &str){
            stageFile = str;
        }

        virtual inline const std::string &getStage() const{
            return stageFile;
        }

        virtual inline void setMusic(const std::string &str){
            music = str;
        }

        virtual inline const std::string &getMusic() const{
            return music;
        }

        virtual inline void setOrder(int o){
            order = o;
        }

        virtual inline const int getOrder() const{
            return order;
        }

    private:
        /* The characters definition File to pass on to stage or anything else */
        const std::string definitionFile;
        /* Character base directory */
        const std::string baseDirectory;
        /* Sprite File */
        const std::string spriteFile;
        /* Characters Name */
        std::string name;
        /* Characters Display Name */
        std::string displayName;
        /* Current Act */
        int currentAct;
        /* Act Collection */
        std::vector< std::string > actCollection;
        /* image 9000 */
        MugenSprite *icon;
        /* image 9001 */
        MugenSprite *portrait;
        /* Random Stage */
        bool randomStage;
        /* Stage File */
        std::string stageFile;
        /* Music for stage */
        std::string music;
        /* Order in which to be set during Arcade mode */
        int order;
};

/* Handle an individual cell which contains the data required to render itself */
class Cell{
    public:
        Cell();
        virtual ~Cell();

        virtual void act();
        virtual void render(const Bitmap &);
        
        virtual inline void setSpriteIndex(int x, int y){
            spriteIndex.x = x;
            spriteIndex.y = y;
        }
        virtual inline void setPosition(int x, int y){
            position.x = x;
            position.y = y;
        }
        virtual inline void setRandom(bool r){
            random = r;
        }
        virtual inline void setEmpty(bool e){
            empty = e;
        }
    private:
        // Character table so that we can rotate through the characters on random
        
        Mugen::Point spriteIndex;
        Mugen::Point position;
        bool random;
        bool empty;
};

typedef std::vector< std::vector< Cell * > > CellMap;
    
/* Handles the select screen grid */
class Grid{
    public:
	Grid();
	virtual ~Grid();

        virtual void act();
	
	virtual void render(const Bitmap &);
        
        virtual inline void setRows(int r){
                this->rows = r;
        }
        virtual inline void setColumns(int c){
                this->columns = c;
        }
        virtual inline void setWrapping(bool w){
                this->wrapping = w;
        }
        virtual inline void setPosition(int x, int y){
                this->position.x = x;
                this->position.y = y;
        }
        virtual inline void setShowEmptyBoxes(bool s){
                this->showEmptyBoxes = s;
        }
        virtual inline void setMoveOverEmptyBoxes(bool m){
                this->moveOverEmptyBoxes = m;
        }
        virtual inline void setCellSize(int x, int y){
                this->cellSize.x = x;
                this->cellSize.y = y;
        }
        virtual inline void setCellSpacing(int c){
                this->cellSpacing = c;
        }
        virtual inline void setCellBackgroundSprite(MugenSprite *s){
                this->cellBackgroundSprite = s;
        }
        virtual inline void setRandomSprite(MugenSprite *s){
                this->cellRandomSprite = s;
        }
        virtual inline void setRandomSwitchTime(int t){
                this->cellRandomSwitchTime = t;
        }
    
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
	MugenSprite *cellRandomSprite;
	int cellRandomSwitchTime;

        /* Character list */
        std::vector< CharacterInfo * > characters;
        /* Cells of the grid */
        CellMap cells;
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
};

/* Temporary namespace remove later */

namespace New{

class CharacterSelect {
    public:
        CharacterSelect(const std::string & file);
        virtual ~CharacterSelect();
	
	virtual void load() throw (MugenException);
	
	virtual void run(const std::string & title, bool player2Enabled, bool selectStage, const Bitmap &);
	
    private:
	
	//! Location of file
	const std::string systemFile;
	
	//! sprite file
	std::string sffFile;
	
	//! music
	std::string sndFile;
	
	//! Select file
	std::string selectFile;
	
	//! Fonts
	std::vector<MugenFont *>fonts;
	
	//! Fade tool
	MugenFadeTool fader;
	
	//!Player 1 Cursor
	Cursor player1;
	
	//!Player 2 Cursor
	Cursor player2;
	
	//! Portrait offset -- need to find out what this is for
	Mugen::Point portraitOffset;
	
	//! portrait.scale = 1,1 -- need to find out what this is for
	Mugen::Point portraitScale;
	
	//! Title
	FontHandler titleFont;
	
	// Stage
	//StageInfo stageInfo;
	
	std::vector< CharacterInfo *> characters;
	
	std::vector< MugenStage *> stages;
	
	//! Sprites
	Mugen::SpriteMap sprites;
	
	//! Select background
	MugenBackgroundManager *background;
};

}

/* Older data */

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

struct OldCell{
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
	
	std::vector< std::vector< OldCell *> > cells;
	
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


