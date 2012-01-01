#ifndef mugen_util_h
#define mugen_util_h

#include <string>
#include <map>
#include <vector>
#include <list>
#include "exception.h"
#include "configuration.h"
#include "util/bitmap.h"
#include "util/pointer.h"
#include "ast/extra.h"

#include "util/input/input-manager.h"

namespace PaintownUtil = ::Util;
typedef ::Util::ReferenceCount<Ast::AstParse> AstRef;

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

    /* find a character's .def file given just his name. first this function
     * will look in the <motif>/chars directory and then look in the standard
     * mugen/chars directory.
     */
    const Filesystem::AbsolutePath findCharacterDef(const std::string & name);
    /*
    const Filesystem::AbsolutePath findStageDef(const Filesystem::RelativePath & name);
    const Filesystem::AbsolutePath findFont(const Filesystem::RelativePath & name);
    */

    /* look for a font. check for font/name and just name */
    const Filesystem::AbsolutePath findFont(const Filesystem::RelativePath & name);

    /* search in various mugen places */
    const Filesystem::AbsolutePath findFile(const Filesystem::RelativePath & name);
    /* start searching in `base' and then resort to using various mugen places */
    const Filesystem::AbsolutePath findFile(const Filesystem::AbsolutePath & base, const Filesystem::RelativePath & path);

    const std::string getFileDir( const std::string &dir );
    // If you use this, please delete the item after you use it, this isn't java ok
    MugenItemContent *parseOpt( const std::string &opt );
    std::vector<Ast::Section*> collectBackgroundStuff(std::list<Ast::Section*>::iterator & section_it, const std::list<Ast::Section*>::iterator & end, const std::string & name = "bg");
    bool readPalette(const Filesystem::AbsolutePath &filename, unsigned char *pal);
    void readSprites(const Filesystem::AbsolutePath & filename, const Filesystem::AbsolutePath & palette, Mugen::SpriteMap & sprites, bool sprite);
    void readSounds(const Filesystem::AbsolutePath & filename, SoundMap & sounds);
    // Get background: The background must be deleted if used outside of stage/menus (Note: we give the background a ticker to whatever is running it)
    MugenBackground *getBackground( const unsigned long int &ticker, Ast::Section *section, Mugen::SpriteMap &sprites );
    // Get animation: The animation must be deleted if used outside of stage/animation (stage and character do the deletion in this case)
    PaintownUtil::ReferenceCount<MugenAnimation> getAnimation(Ast::Section *section, const Mugen::SpriteMap &sprites, bool mask);
    /* pull a sprite out of a const sprite map */
    MugenSprite * getSprite(const Mugen::SpriteMap & sprites, int group, int item);

    /* if mask is true, then effects.mask will be true by default */
    std::map<int, PaintownUtil::ReferenceCount<MugenAnimation> > loadAnimations(const Filesystem::AbsolutePath & filename, const SpriteMap sprites, bool mask);

    /* destroys raw pcx data in a MugenSprite */
    void destroyRaw(const std::map< unsigned int, std::map< unsigned int, MugenSprite * > > & sprites);

    // const Filesystem::AbsolutePath getCorrectFileLocation(const Filesystem::AbsolutePath & dir, const std::string &file );
    
    // Use to probe a def file, looking in section and looking for the item in that section and return it's value as a string
    // Usefull for getting names of maps, characters, etc without loading the entire Object....
    const std::string probeDef(const Filesystem::AbsolutePath &file, const std::string &section, const std::string &search);
    const std::string probeDef(const AstRef & parsed, const std::string & section, const std::string & search);

    /* Use to probe a SFF file for a specific sprite without loading the
     * whole sprite list
     * Throws exception if not found
     */
    MugenSprite *probeSff(const Filesystem::AbsolutePath &file, int groupNumber, int spriteNumber, bool mask, const Filesystem::AbsolutePath & actFile = Filesystem::AbsolutePath());
    /* similar to probeSff but searches for sprites 9000,0 and 9000,1 which are
     * the icon and the portrait respectively.
     */
    void getIconAndPortrait(const Filesystem::AbsolutePath & sffPath, const Filesystem::AbsolutePath & actPath, MugenSprite ** icon, MugenSprite ** portrait);

    /* convenient parser functions. throw MugenException on failure instead
     * of Ast::Exception.
     */
    AstRef parseAir(const std::string & filename);
    AstRef parseDef(const std::string & filename);
    AstRef parseCmd(const std::string & filename);

    /* returns the number of game ticks that have passed by.
     * speed adjusts the rate. lower values slow the game down,
     * higher values speed it up.
     */
    double gameTicks(double ticks, double speed = 1);
}

class Point{
    public:
	int x;
	int y;
	Point();
	Point(int x, int y);
        Point(const Point & copy);
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
    None = 0,
    Add,
    Add1,
    Sub,
    AddAlpha,
    /* used by the Trans controller to specify the trans should not change */
    Default
};

class Effects{
    public:
	Effects();
        Effects(const Effects & copy);
	const Effects &operator=(const Effects &e);
	virtual ~Effects();

        /* adds the mask from the parameter to this object */
        Effects operator+(const Effects &) const;

        /* adds the translucency */
        Effects operator+(const TransType & translucent) const;

        /* what kind of translucency to use */
	TransType trans;

        /* translucency values */
	int alphalow;
	int alphahigh;

        /* if the masking color should be shown or not, true is not shown */
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

        Graphics::Bitmap::Filter * filter;
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

/* thrown by options to start a game */
class StartGame: public std::exception {
public:
    StartGame(PlayerType player, GameType game):
    player(player),
    game(game){
    }

    virtual ~StartGame() throw (){
    }

    PlayerType player;
    GameType game;
};

/*! Player1 keys for menus including keyboard shortcuts */
inline static InputMap<Mugen::Keys> getPlayer1Keys(int delay = 0){
    InputMap<Mugen::Keys> input;
    input.set(Configuration::config(0).getUp(), delay, false, Mugen::Up);
    input.set(Configuration::config(0).getDown(), delay, false, Mugen::Down);
    input.set(Configuration::config(0).getRight(), delay, false, Mugen::Right);
    input.set(Configuration::config(0).getLeft(), delay, false, Mugen::Left);

    input.set(Configuration::config(0).getJoystickUp(), delay, false, Mugen::Up);
    input.set(Configuration::config(0).getJoystickLeft(), delay, false, Mugen::Left);
    input.set(Configuration::config(0).getJoystickRight(), delay, false, Mugen::Right);
    input.set(Configuration::config(0).getJoystickDown(), delay, false, Mugen::Down);

    input.set(Configuration::config(0).getAttack1(), delay, false, Mugen::A);
    input.set(Configuration::config(0).getAttack2(), delay, false, Mugen::B);
    input.set(Configuration::config(0).getAttack3(), delay, false, Mugen::C);
    input.set(Configuration::config(0).getAttack4(), delay, false, Mugen::X);
    input.set(Configuration::config(0).getAttack5(), delay, false, Mugen::Y);
    input.set(Configuration::config(0).getAttack6(), delay, false, Mugen::Z);
    /* FIXME: do we need start? maybe put it in the configuration */
    input.set(Keyboard::Key_ENTER, delay, false, Mugen::Start);
    input.set(Keyboard::Key_ESC, 0, true, Mugen::Esc);

    input.set(Configuration::config(0).getJoystickQuit(), 0, true, Mugen::Esc);
    input.set(Configuration::config(0).getJoystickAttack1(), delay, false, Mugen::A);
    input.set(Configuration::config(0).getJoystickAttack2(), delay, false, Mugen::B);
    input.set(Configuration::config(0).getJoystickAttack3(), delay, false, Mugen::C);
    input.set(Configuration::config(0).getJoystickAttack4(), delay, false, Mugen::X);
    input.set(Configuration::config(0).getJoystickAttack5(), delay, false, Mugen::Y);
    input.set(Configuration::config(0).getJoystickAttack6(), delay, false, Mugen::Z);

    return input;
}

/* kind of dumb to just copy/paste the above. find a better solution */
static InputMap<Mugen::Keys> getPlayer1InputLeft(){
    InputMap<Mugen::Keys> input;
    int delay = 0;
    input.set(Configuration::config(0).getUp(), delay, false, Mugen::Up);
    input.set(Configuration::config(0).getDown(), delay, false, Mugen::Down);
    input.set(Configuration::config(0).getLeft(), delay, false, Mugen::Right);
    input.set(Configuration::config(0).getRight(), delay, false, Mugen::Left);

    input.set(Configuration::config(0).getJoystickUp(), delay, false, Mugen::Up);
    input.set(Configuration::config(0).getJoystickRight(), delay, false, Mugen::Left);
    input.set(Configuration::config(0).getJoystickLeft(), delay, false, Mugen::Right);
    input.set(Configuration::config(0).getJoystickDown(), delay, false, Mugen::Down);

    input.set(Configuration::config(0).getAttack1(), delay, false, Mugen::A);
    input.set(Configuration::config(0).getAttack2(), delay, false, Mugen::B);
    input.set(Configuration::config(0).getAttack3(), delay, false, Mugen::C);
    input.set(Configuration::config(0).getAttack4(), delay, false, Mugen::X);
    input.set(Configuration::config(0).getAttack5(), delay, false, Mugen::Y);
    input.set(Configuration::config(0).getAttack6(), delay, false, Mugen::Z);

    input.set(Keyboard::Key_ENTER, 0, true, Mugen::Start);
    input.set(Keyboard::Key_ESC, 0, true, Mugen::Esc);

    input.set(Configuration::config(0).getJoystickQuit(), 0, true, Mugen::Esc);
    input.set(Configuration::config(0).getJoystickAttack1(), delay, false, Mugen::A);
    input.set(Configuration::config(0).getJoystickAttack2(), delay, false, Mugen::B);
    input.set(Configuration::config(0).getJoystickAttack3(), delay, false, Mugen::C);
    input.set(Configuration::config(0).getJoystickAttack4(), delay, false, Mugen::X);
    input.set(Configuration::config(0).getJoystickAttack5(), delay, false, Mugen::Y);
    input.set(Configuration::config(0).getJoystickAttack6(), delay, false, Mugen::Z);

    return input;
}

/*! Player2 keys for menus */
inline static InputMap<Mugen::Keys> getPlayer2Keys(int delay = 0){
    InputMap<Mugen::Keys> input;
    /*
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
    */

    input.set(Configuration::config(1).getUp(), delay, false, Mugen::Up);
    input.set(Configuration::config(1).getDown(), delay, false, Mugen::Down);
    input.set(Configuration::config(1).getRight(), delay, false, Mugen::Right);
    input.set(Configuration::config(1).getLeft(), delay, false, Mugen::Left);

    input.set(Configuration::config(1).getAttack1(), delay, false, Mugen::A);
    input.set(Configuration::config(1).getAttack2(), delay, false, Mugen::B);
    input.set(Configuration::config(1).getAttack3(), delay, false, Mugen::C);
    input.set(Configuration::config(1).getAttack4(), delay, false, Mugen::X);
    input.set(Configuration::config(1).getAttack5(), delay, false, Mugen::Y);
    input.set(Configuration::config(1).getAttack6(), delay, false, Mugen::Z);

    /* FIXME */
    input.set(Keyboard::Key_K, delay, false, Mugen::Start);
    input.set(Keyboard::Key_ESC, 0, true, Mugen::Esc);

    /* FIXME: joystick for player2 is broken right now */
    /*
    input.set(Configuration::config(1).getJoystickUp(), delay, false, Mugen::Up);
    input.set(Configuration::config(1).getJoystickLeft(), delay, false, Mugen::Left);
    input.set(Configuration::config(1).getJoystickRight(), delay, false, Mugen::Right);
    input.set(Configuration::config(1).getJoystickDown(), delay, false, Mugen::Down);

    input.set(Configuration::config(1).getJoystickQuit(), 0, true, Mugen::Esc);
    input.set(Configuration::config(1).getJoystickAttack1(), delay, false, Mugen::A);
    input.set(Configuration::config(1).getJoystickAttack2(), delay, false, Mugen::B);
    input.set(Configuration::config(1).getJoystickAttack3(), delay, false, Mugen::C);
    input.set(Configuration::config(1).getJoystickAttack4(), delay, false, Mugen::X);
    input.set(Configuration::config(1).getJoystickAttack5(), delay, false, Mugen::Y);
    input.set(Configuration::config(1).getJoystickAttack6(), delay, false, Mugen::Z);
    */

    return input;
}

static InputMap<Mugen::Keys> getPlayer2InputLeft(){
    InputMap<Mugen::Keys> input;
    int delay = 0;
    /*
    input.set(Keyboard::Key_Y, 0, false, Mugen::Up);
    input.set(Keyboard::Key_H, 0, false, Mugen::Down);
    input.set(Keyboard::Key_G, 0, false, Mugen::Right);
    input.set(Keyboard::Key_J, 0, false, Mugen::Left);

    input.set(Keyboard::Key_I, 0, false, Mugen::A);
    input.set(Keyboard::Key_O, 0, false, Mugen::B);
    input.set(Keyboard::Key_P, 0, false, Mugen::C);
    input.set(Keyboard::Key_8, 0, false, Mugen::X);
    input.set(Keyboard::Key_9, 0, false, Mugen::Y);
    input.set(Keyboard::Key_0, 0, false, Mugen::Z);
    */

    input.set(Configuration::config(1).getUp(), delay, false, Mugen::Up);
    input.set(Configuration::config(1).getDown(), delay, false, Mugen::Down);
    input.set(Configuration::config(1).getLeft(), delay, false, Mugen::Right);
    input.set(Configuration::config(1).getRight(), delay, false, Mugen::Left);

    
    input.set(Configuration::config(1).getAttack1(), delay, false, Mugen::A);
    input.set(Configuration::config(1).getAttack2(), delay, false, Mugen::B);
    input.set(Configuration::config(1).getAttack3(), delay, false, Mugen::C);
    input.set(Configuration::config(1).getAttack4(), delay, false, Mugen::X);
    input.set(Configuration::config(1).getAttack5(), delay, false, Mugen::Y);
    input.set(Configuration::config(1).getAttack6(), delay, false, Mugen::Z);

    /* FIXME */
    input.set(Keyboard::Key_K, 0, false, Mugen::Start);
    input.set(Keyboard::Key_ESC, 0, true, Mugen::Esc);

    /* FIXME */
    /*
    input.set(Configuration::config(1).getJoystickQuit(), 0, true, Mugen::Esc);
    input.set(Configuration::config(1).getJoystickAttack1(), delay, false, Mugen::A);
    input.set(Configuration::config(1).getJoystickAttack2(), delay, false, Mugen::B);
    input.set(Configuration::config(1).getJoystickAttack3(), delay, false, Mugen::C);
    input.set(Configuration::config(1).getJoystickAttack4(), delay, false, Mugen::X);
    input.set(Configuration::config(1).getJoystickAttack5(), delay, false, Mugen::Y);
    input.set(Configuration::config(1).getJoystickAttack6(), delay, false, Mugen::Z);

    input.set(Configuration::config(1).getJoystickUp(), delay, false, Mugen::Up);
    input.set(Configuration::config(1).getJoystickLeft(), delay, false, Mugen::Left);
    input.set(Configuration::config(1).getJoystickRight(), delay, false, Mugen::Right);
    input.set(Configuration::config(1).getJoystickDown(), delay, false, Mugen::Down);
    */

    return input;
}

/*! Base Element Type */
class Element {
    public:
	Element();
	virtual ~Element();
	
	virtual void act()=0;
	virtual void render(int x, int y, const Graphics::Bitmap &, Graphics::Bitmap::Filter * filter)=0;
	
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
