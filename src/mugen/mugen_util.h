#ifndef mugen_util_h
#define mugen_util_h

#include <string>
#include <map>
#include <vector>
#include <list>
#include "mugen_exception.h"

#include "input/input-manager.h"

class Bitmap;
class MugenAnimation;
class MugenBackground;
class MugenSprite;
class MugenSound;
class MugenItemContent;
class MugenSection;
class MugenCharacterSelect;
namespace Ast{
    class Section;
}

namespace Mugen{

/* Makes the use of the sprite maps easier */
typedef std::map< unsigned int, MugenSprite *> GroupMap;
typedef std::map< unsigned int, GroupMap> SpriteMap;
typedef std::map< unsigned int, std::map< unsigned int, MugenSound * > > SoundMap;
    
namespace Util{
    const std::string fixCase (const std::string &str);
    const std::string removeSpaces (const std::string &str);
    const std::string invertSlashes (const std::string &str);
    const std::string stripDir( const std::string &str );
    const std::string fixFileName( const std::string &dir, std::string str );
    const std::string getFileDir( const std::string &dir );
    // If you use this, please delete the item after you use it, this isn't java ok
    MugenItemContent *parseOpt( const std::string &opt );
    std::vector<Ast::Section*> collectBackgroundStuff(std::list<Ast::Section*>::iterator & section_it, const std::list<Ast::Section*>::iterator & end, const std::string & name = "bg");
    bool readPalette(const std::string &filename, unsigned char *pal);
    void readSprites(const std::string & filename, const std::string & palette, Mugen::SpriteMap & sprites) throw (MugenException);
    void readSounds(const std::string & filename, SoundMap & sounds);
    // Get background: The background must be deleted if used outside of stage/menus (Note: we give the background a ticker to whatever is running it)
    MugenBackground *getBackground( const unsigned long int &ticker, Ast::Section *section, Mugen::SpriteMap &sprites );
    // Get animation: The animation must be deleted if used outside of stage/animation (stage and character do the deletion in this case)
    MugenAnimation *getAnimation( Ast::Section *section, const Mugen::SpriteMap &sprites );

    std::map<int, MugenAnimation *> loadAnimations(const std::string & filename, const SpriteMap sprites);

    const std::string getCorrectFileLocation( const std::string &dir, const std::string &file );
    // Use to probe a def file, looking in section and looking for the item in that section and return it's value as a string
    // Usefull for getting names of maps, characters, etc without loading the entire Object....
    const std::string probeDef(const std::string &file, const std::string &section, const std::string &search) throw (MugenException);
    /*! Use to probe a SFF file for a specific sprite without loading the whole sprite list
       Throws exception if not found
    */
    MugenSprite *probeSff(const std::string &file, int groupNumber, int spriteNumber, const std::string &actFile="") throw (MugenException);

    /* convenient parser functions. throw MugenException on failure instead
     * of Ast::Exception.
     */
    std::list<Ast::Section*>* parseAir(const std::string & filename);
    std::list<Ast::Section*>* parseDef(const std::string & filename);

    /* returns the number of game ticks that have passed by.
     * speed adjusts the rate. lower values slow the game down,
     * higher values speed it up.
     */
    double gameTicks(double speed = 1);
}

class Point{
    public:
	int x;
	int y;
	Point();
	Point(int x, int y);
	Point &operator=(const Point &p);
	inline const bool operator==(const Point &p) const{
	    return ((this->x == p.x) && (this->y == p.y));
	}
	~Point();
};

enum TransType{
    NONE = 0,
    ADD,
    ADD1,
    SUB,
    ADDALPHA
};

class Effects{
    public:
	Effects();
        Effects(const Effects & copy);
	const Effects &operator=(const Effects &e);
	virtual ~Effects();

	TransType trans;
	int alphalow;
	int alphahigh;
	bool mask;
	int facing;
	int vfacing;
	double scalex;
	double scaley;
};

/* Use with fonts */
struct FontInfo{
    FontInfo():
        index(0),
        bank(0),
        position(0){
        }

    int index;
    int bank;
    int position;
};

/*! Game types currently in M.U.G.E.N */
enum GameType {
    Arcade,
    Versus,
    TeamArcade,
    TeamVersus,
    TeamCoop,
    Survival,
    SurvivalCoop,
    Training,
    Watch
};

/*! Key handler for keys for now change later*/
enum Keys{
    Up,
    Down,
    Left,
    Right,
    A,
    B,
    C,
    X,
    Y,
    Z,
    Start,
    Enter,
    Esc
};

/*! Player1 keys for menus including keyboard shortcuts */
inline static InputMap<Mugen::Keys> getPlayer1MenuKeys(){
    InputMap<Mugen::Keys> input;
    input.set(Keyboard::Key_UP, 20, false, Mugen::Up);
    input.set(Keyboard::Key_DOWN, 20, false, Mugen::Down);
    input.set(Keyboard::Key_RIGHT, 20, false, Mugen::Right);
    input.set(Keyboard::Key_LEFT, 20, false, Mugen::Left);

    input.set(Keyboard::Key_A, 0, true, Mugen::A);
    input.set(Keyboard::Key_S, 0, true, Mugen::B);
    input.set(Keyboard::Key_D, 0, true, Mugen::C);
    input.set(Keyboard::Key_Z, 0, true, Mugen::X);
    input.set(Keyboard::Key_X, 0, true, Mugen::Y);
    input.set(Keyboard::Key_C, 0, true, Mugen::Z);
    input.set(Keyboard::Key_ENTER, 0, true, Mugen::Start);
    input.set(Keyboard::Key_ESC, 0, true, Mugen::Esc);
    input.set(Keyboard::Key_ENTER, 0, true, Mugen::Enter);
    return input;
}

/*! Player2 keys for menus */
inline static InputMap<Mugen::Keys> getPlayer2MenuKeys(){
    InputMap<Mugen::Keys> input;
    input.set(Keyboard::Key_H, 20, false, Mugen::Up);
    input.set(Keyboard::Key_Y, 20, false, Mugen::Down);
    input.set(Keyboard::Key_J, 20, false, Mugen::Right);
    input.set(Keyboard::Key_G, 20, false, Mugen::Left);

    input.set(Keyboard::Key_I, 0, true, Mugen::A);
    input.set(Keyboard::Key_O, 0, true, Mugen::B);
    input.set(Keyboard::Key_P, 0, true, Mugen::C);
    input.set(Keyboard::Key_8, 0, true, Mugen::X);
    input.set(Keyboard::Key_9, 0, true, Mugen::Y);
    input.set(Keyboard::Key_0, 0, true, Mugen::Z);
    input.set(Keyboard::Key_L, 0, true, Mugen::Start);
    return input;
}

/*! Base Element Type */
class Element {
    public:
	Element();
	virtual ~Element();
	
	virtual void act()=0;
	virtual void render(int x, int y, const Bitmap &)=0;
	
	enum Layer {
	    Background,
	    Foreground,
	    Top
	};
	
	virtual inline void setID(int id){
	    this->ID = id;
	}
	
	virtual inline int getID() const {
	    return this->ID;
	}
	
	virtual inline void setLayer(const Layer &layer){
	    this->layer = layer;
	}
	
	virtual inline const Layer & getLayer() const {
	    return this->layer;
	}
	
	virtual inline void setName(const std::string & name){
	    this->name = name;
	}
	
	virtual inline const std::string & getName() const {
	    return this->name;
	}
    
    private:
	//! ID of said element
	int ID;
	//! Layer where element will be drawn (background or foreground)
	Layer layer;
	//! Name of element
	std::string name;
};

}

#endif
