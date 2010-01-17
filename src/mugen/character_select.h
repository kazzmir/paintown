#ifndef _mugen_character_select_h
#define _mugen_character_select_h

#include <vector>
#include <map>

#include "mugen_exception.h"
#include "mugen_fadetool.h"

#include "menu/menu.h"

#include "input/input-map.h"

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

/*! Key handler for now change later*/
enum CharacterKeys{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    A,
    B,
    C,
    X,
    Y,
    Z,
    START
};
    
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

/* Forward declaration of Cell */
class Cell;

/* Character Info handler, portrait name and etc */
class CharacterInfo {
    public:
        CharacterInfo(const std::string &definitionFile);
        virtual ~CharacterInfo();
	
	inline bool operator==(CharacterInfo &character){
	    return (this->definitionFile.compare(character.definitionFile) == 0);
	}

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
	
	virtual inline void setReferenceCell(Cell *cell){
	    this->referenceCell = cell;
	}

    private:
        /* The characters definition File to pass on to stage or anything else */
        const std::string definitionFile;
        /* Character base directory */
        const std::string baseDirectory;
        /* Sprite File */
        std::string spriteFile;
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
	
	//! Reference Cell mainly for random so that we can light it up when it selected
	Cell *referenceCell;
};

/* Handle an individual cell which contains the data required to render itself */
class Cell{
    public:
        Cell(int x, int y);
        virtual ~Cell();

        virtual void act(std::vector<CharacterInfo *> &characters);
        virtual void render(const Bitmap &);
	
	virtual inline const bool operator==(const Cell &cell) const{
	    return (this->location == cell.location);
	}
	
	virtual inline const Mugen::Point &getLocation() const{
	    return this->location;
	}
	
	virtual inline void setBackground(MugenSprite *sprite){
	    this->background = sprite;
	}
	
	virtual inline void setCharacter(CharacterInfo *character){
	    this->empty = false;
	    this->character = character;
	}
	
	virtual inline const CharacterInfo *getCharacter() const{
	    return this->character;
	}
	
	virtual inline void setRandomSprite(MugenSprite *random){
	    randomSprite = random;
	}
        
        virtual inline void setPosition(int x, int y){
            position.x = x;
            position.y = y;
        }
	
	virtual inline const Mugen::Point &getPosition() const {
	    return this->position;
	}
	
        virtual inline void setRandom(bool r){
	    empty = false;
            random = r;
        }
	virtual inline bool isRandom() const {
	    return random;
	}
        virtual inline void setEmpty(bool e){
            empty = e;
        }
	virtual inline bool isEmpty() const {
	    return empty;
	}
	virtual inline void setCharacterOffset(int x, int y){
	    this->characterOffset.x = x;
	    this->characterOffset.y = y;
	}
	virtual inline void setCharacterScale(double x, double y){
	    this->characterScaleX = x;
	    this->characterScaleY = y;
	}
    private:
	//! Location on grid
	const Mugen::Point location;
	
	//! Set sprite background
	MugenSprite *background;
	
	//! Set Character
        CharacterInfo *character;
	
	//! Set Random Sprite
	MugenSprite *randomSprite;
	
	//! Position of this cell
        Mugen::Point position;
	
	//! Is this a random select cell
        bool random;
	
	//! Is this cell empty
        bool empty;
	
	//! Offset in which the portrait is placed in the cell
	Mugen::Point characterOffset;
	
	//! Scale values of the portrait placed in the cell
	double characterScaleX;
	double characterScaleY;
	
};

/* Cell map */
typedef std::vector< std::vector< Cell * > > CellMap;

/* forward declare Cursor */
class Cursor;

/* Handles the select screen grid */
class Grid{
    public:
	Grid();
	virtual ~Grid();
	
	virtual void initialize();

        virtual void act();
	
	virtual void render(const Bitmap &);
	
	virtual void addCharacter(CharacterInfo *character, bool isRandom = false);
	
	virtual void setCursorStart(Cursor &cursor);
	
	virtual void moveCursorLeft(Cursor &cursor);
	virtual void moveCursorRight(Cursor &cursor);
	virtual void moveCursorUp(Cursor &cursor);
	virtual void moveCursorDown(Cursor &cursor);
        
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
        virtual inline void setCellRandomSprite(MugenSprite *s){
            this->cellRandomSprite = s;
        }
        virtual inline void setCellRandomSwitchTime(int t){
            this->cellRandomSwitchTime = t;
        }
	
	virtual inline void setPortraitOffset(int x, int y){
	    portraitOffset.x = x;
	    portraitOffset.y = y;
	}
	
	virtual inline void setPortraitScale(double x, double y){
	    this->portraitScaleX = x;
	    this->portraitScaleY = y;
	}
    
    private:
	
	Cell *getCell(int row, int column) throw (MugenException);
	
	//! Total rows
	int rows;
	
	//! Total columns
	int columns;
	
	//! Allow a cursor to wrap?
	bool wrapping;
	
	//! Starting position of the grid
	Mugen::Point position;
	
	//! Show empty boxes?
	bool showEmptyBoxes;
	
	//! Allow cursor to move over empty boxes?
	bool moveOverEmptyBoxes;
	
	//! Size of cell
	Mugen::Point cellSize;
	
	//! Spacing between cells
	int cellSpacing;
	
	//! The background of the cell
	MugenSprite *cellBackgroundSprite;
	
	//! Random sprite portrait
	MugenSprite *cellRandomSprite;
	
	//! Random sprite switch time
	int cellRandomSwitchTime;
	
	//! Portrait offset for placement of the picture
	Mugen::Point portraitOffset;
	
	//! portrait scale for resizing if need be
	double portraitScaleX;
	double portraitScaleY;

        /* Character list */
        std::vector< CharacterInfo * > characters;
	
        //! Cells of the grid
        CellMap cells;
};

/* Handles player cursors */
class Cursor{
    public:
	Cursor();
	virtual ~Cursor();
	
	virtual void act(Grid &grid);
	
	virtual void render(Grid &grid, const Bitmap &);
	
	virtual inline void setInput(const InputMap<CharacterKeys> & input){
	    this->input = input;
	}
	
	virtual inline void setStart(int x, int y){
	    this->start.x = x;
	    this->start.y = y;
	}
	
	virtual inline const Mugen::Point &getStart() const {
	    return this->start;
	}
	
	virtual inline void setCurrentCell(Cell *cell){
	    this->currentCell = cell;
	}
	
	virtual const Cell *getCurrentCell() const {
	    return this->currentCell;
	}
	
	virtual inline void setActiveSprite(MugenSprite *spr){
	    this->activeSprite = spr;
	}
	
	virtual inline void setDoneSprite(MugenSprite *spr){
	    this->doneSprite = spr;
	}
	
	virtual inline void setBlink(bool b){
	    this->blink = b;
	}
	
	virtual inline void setBlinkCounter(int b){
	    this->blinkCounter = b;
	}
	
	virtual inline void setFaceOffset(int x, int y){
	    this->faceOffset.x = x;
	    this->faceOffset.y = y;
	}
	
	virtual inline void setFaceScale(double x, double y){
	    this->faceScaleX = x;
	    this->faceScaleY = y;
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
	
	virtual inline FontHandler &getFontHandler(){
	    return this->font;
	}
	
    private:
	InputMap<CharacterKeys> input;
	//! Starting cell position in terms of row and column
	Mugen::Point start;
	
	//! Cell
	Cell *currentCell;
	
	//! Sprite to display while selecting
	MugenSprite *activeSprite;
	
	//! Sprite to display when selected
	MugenSprite *doneSprite;
	
	//! Blink when overlapping player1 ?
	bool blink;
	
	//! Our blink counter at 10
	int blinkCounter;
	
	//! the placement of the portrait
	Mugen::Point faceOffset;
	
	//! Scale data of the portrait
	double faceScaleX;
	double faceScaleY;
	
	//! Which direction facing
	int facing;
	
	//! Is currently selecting?
	bool selecting;
	
	//! Is active?
	bool active;
	
	//! Font
	FontHandler font;
};

/* Temporary namespace remove later */

namespace New{

class CharacterSelect {
    public:
        CharacterSelect(const std::string & file);
        virtual ~CharacterSelect();
	
	virtual void load() throw (MugenException);
	
	virtual void run(const std::string & title, bool player2Enabled, bool selectStage, const Bitmap &);
	
	virtual void parseSelect(const std::string &selectFile);
	
	virtual inline void setPlayer1Keys(const InputMap<CharacterKeys> &input){
	    player1.setInput(input);
	}
	
	virtual inline void setPlayer2Keys(const InputMap<CharacterKeys> &input){
	    player2.setInput(input);
	}
	
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
	
	//! Grid (Cell container)
	Grid grid;
	
	//!Player 1 Cursor
	Cursor player1;
	
	//!Player 2 Cursor
	Cursor player2;
	
	//! Title font handler
	FontHandler titleFont;
	
	//! Stage Font Handler
	FontHandler stageFont;
	
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


