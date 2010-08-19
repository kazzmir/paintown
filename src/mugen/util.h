#ifndef mugen_util_h
#define mugen_util_h

#include <string>
#include <map>
#include <vector>
#include <list>
#include "exception.h"

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
    class AstParse;
}

namespace Mugen{

/* Makes the use of the sprite maps easier */
typedef std::map< unsigned int, MugenSprite *> GroupMap;
typedef std::map< unsigned int, GroupMap> SpriteMap;
typedef std::map< unsigned int, std::map< unsigned int, MugenSound * > > SoundMap;

/* FIXME: add descriptions of every function here */
namespace Util{
    /* Converts string to all lower case */
    const std::string fixCase (const std::string &str);

    /* Removes extra whitespace at the front and end of a string */
    const std::string removeSpaces (const std::string &str);

    /* Converts backslash \ to forward slash / */
    const std::string invertSlashes (const std::string &str);
    const std::string stripDir( const std::string &str );
    const Filesystem::AbsolutePath fixFileName( const Filesystem::AbsolutePath &dir, std::string str );
    const std::string getFileDir( const std::string &dir );
    // If you use this, please delete the item after you use it, this isn't java ok
    MugenItemContent *parseOpt( const std::string &opt );
    std::vector<Ast::Section*> collectBackgroundStuff(std::list<Ast::Section*>::iterator & section_it, const std::list<Ast::Section*>::iterator & end, const std::string & name = "bg");
    bool readPalette(const Filesystem::AbsolutePath &filename, unsigned char *pal);
    void readSprites(const Filesystem::AbsolutePath & filename, const Filesystem::AbsolutePath & palette, Mugen::SpriteMap & sprites);
    void readSounds(const Filesystem::AbsolutePath & filename, SoundMap & sounds);
    // Get background: The background must be deleted if used outside of stage/menus (Note: we give the background a ticker to whatever is running it)
    MugenBackground *getBackground( const unsigned long int &ticker, Ast::Section *section, Mugen::SpriteMap &sprites );
    // Get animation: The animation must be deleted if used outside of stage/animation (stage and character do the deletion in this case)
    MugenAnimation *getAnimation( Ast::Section *section, const Mugen::SpriteMap &sprites );

    std::map<int, MugenAnimation *> loadAnimations(const Filesystem::AbsolutePath & filename, const SpriteMap sprites);

    /* destroys raw pcx data in a MugenSprite */
    void destroyRaw(const std::map< unsigned int, std::map< unsigned int, MugenSprite * > > & sprites);

    const Filesystem::AbsolutePath getCorrectFileLocation(const Filesystem::AbsolutePath & dir, const std::string &file );
    // Use to probe a def file, looking in section and looking for the item in that section and return it's value as a string
    // Usefull for getting names of maps, characters, etc without loading the entire Object....
    const std::string probeDef(const Filesystem::AbsolutePath &file, const std::string &section, const std::string &search);
    const std::string probeDef(const Ast::AstParse & parsed, const std::string & section, const std::string & search);
    /*! Use to probe a SFF file for a specific sprite without loading the whole sprite list
       Throws exception if not found
    */
    MugenSprite *probeSff(const Filesystem::AbsolutePath &file, int groupNumber, int spriteNumber, const Filesystem::AbsolutePath & actFile = Filesystem::AbsolutePath());

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
	inline void set(int x, int y){
	    this->x = x;
	    this->y = y;
	}
	inline bool operator==(const Point &p) const{
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

        /* what kind of translucency to use */
	TransType trans;

        /* translucency values */
	int alphalow;
	int alphahigh;

        /* ??? */
	bool mask;

        /* horizontal flip */
	int facing;

        /* vertical flip */
	int vfacing;

        /* scale horizontally */
	double scalex;

        /* scale vertically */
	double scaley;

        /* clipping. -1 means dont alter clipping */
        struct Dimension{
            Dimension():x1(-1), x2(-1), y1(-1), y2(-1){}

            int x1, x2;
            int y1, y2;
        } dimension;
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
    Esc,
};

/*! Player type */
enum PlayerType{
    Player1,
    Player2,
    CPU,
    NoControl,
};

/*! Player1 keys for menus including keyboard shortcuts */
inline static InputMap<Mugen::Keys> getPlayer1Keys(int delay = 0){
    InputMap<Mugen::Keys> input;
    input.set(Keyboard::Key_UP, delay, false, Mugen::Up);
    input.set(Keyboard::Key_DOWN, delay, false, Mugen::Down);
    input.set(Keyboard::Key_RIGHT, delay, false, Mugen::Right);
    input.set(Keyboard::Key_LEFT, delay, false, Mugen::Left);

    input.set(Joystick::Up, delay, false, Mugen::Up);
    input.set(Joystick::Left, delay, false, Mugen::Left);
    input.set(Joystick::Right, delay, false, Mugen::Right);
    input.set(Joystick::Down, delay, false, Mugen::Down);

    input.set(Keyboard::Key_A, delay, false, Mugen::A);
    input.set(Keyboard::Key_S, delay, false, Mugen::B);
    input.set(Keyboard::Key_D, delay, false, Mugen::C);
    input.set(Keyboard::Key_Z, delay, false, Mugen::X);
    input.set(Keyboard::Key_X, delay, false, Mugen::Y);
    input.set(Keyboard::Key_C, delay, false, Mugen::Z);
    input.set(Keyboard::Key_ENTER, delay, false, Mugen::Start);
    input.set(Keyboard::Key_ESC, 0, true, Mugen::Esc);

    input.set(Joystick::Button1, delay, false, Mugen::A);
    input.set(Joystick::Button2, delay, false, Mugen::B);
    input.set(Joystick::Button3, delay, false, Mugen::X);
    input.set(Joystick::Button4, delay, false, Mugen::Y);
    return input;
}

/*! Player2 keys for menus */
inline static InputMap<Mugen::Keys> getPlayer2Keys(int delay = 0){
    InputMap<Mugen::Keys> input;
    input.set(Keyboard::Key_Y, delay, false, Mugen::Up);
    input.set(Keyboard::Key_H, delay, false, Mugen::Down);
    input.set(Keyboard::Key_J, delay, false, Mugen::Right);
    input.set(Keyboard::Key_G, delay, false, Mugen::Left);

    input.set(Keyboard::Key_I, delay, false, Mugen::A);
    input.set(Keyboard::Key_O, delay, false, Mugen::B);
    input.set(Keyboard::Key_P, delay, false, Mugen::C);
    input.set(Keyboard::Key_8, delay, false, Mugen::X);
    input.set(Keyboard::Key_9, delay, false, Mugen::Y);
    input.set(Keyboard::Key_0, delay, false, Mugen::Z);
    input.set(Keyboard::Key_K, delay, false, Mugen::Start);
    input.set(Keyboard::Key_ESC, 0, true, Mugen::Esc);
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
