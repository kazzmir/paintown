#ifndef mugen_util_h
#define mugen_util_h

#include <string>
#include <map>
#include <vector>
#include <list>
#include <queue>
#include "exception.h"
#include "configuration.h"
#include "sound.h"
#include "sprite.h"
#include "common.h"
#include "util/bitmap.h"
#include "util/pointer.h"
#include "ast/extra.h"

#include "util/input/input-manager.h"

namespace PaintownUtil = ::Util;
typedef ::Util::ReferenceCount<Ast::AstParse> AstRef;

class MugenBackground;
class MugenItemContent;
class MugenSection;
class MugenCharacterSelect;
namespace Ast{
    class Section;
}

namespace Mugen{

    class Animation;
    class Sprite;

/* Makes the use of the sprite maps easier */
typedef std::map< unsigned int, PaintownUtil::ReferenceCount<Sprite> > GroupMap;
typedef std::map< unsigned int, GroupMap> SpriteMap;
typedef std::map< unsigned int, std::map< unsigned int, PaintownUtil::ReferenceCount<Sound> > > SoundMap;

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

    void destroySprites(const SpriteMap & sprites);

    // Get background: The background must be deleted if used outside of stage/menus (Note: we give the background a ticker to whatever is running it)
    MugenBackground *getBackground( const unsigned long int &ticker, Ast::Section *section, Mugen::SpriteMap &sprites );
    // Get animation: The animation must be deleted if used outside of stage/animation (stage and character do the deletion in this case)
    PaintownUtil::ReferenceCount<Animation> getAnimation(Ast::Section *section, const Mugen::SpriteMap &sprites, bool mask);
    /* pull a sprite out of a const sprite map */
    PaintownUtil::ReferenceCount<Sprite> getSprite(const Mugen::SpriteMap & sprites, int group, int item);

    /* if mask is true, then effects.mask will be true by default */
    std::map<int, PaintownUtil::ReferenceCount<Animation> > loadAnimations(const Filesystem::AbsolutePath & filename, const SpriteMap sprites, bool mask);

    /* destroys raw pcx data in a Sprite */
    void destroyRaw(const SpriteMap &);

    // const Filesystem::AbsolutePath getCorrectFileLocation(const Filesystem::AbsolutePath & dir, const std::string &file );
    
    // Use to probe a def file, looking in section and looking for the item in that section and return it's value as a string
    // Usefull for getting names of maps, characters, etc without loading the entire Object....
    const std::string probeDef(const Filesystem::AbsolutePath &file, const std::string &section, const std::string &search);
    const std::string probeDef(const AstRef & parsed, const std::string & section, const std::string & search);

    /* Use to probe a SFF file for a specific sprite without loading the
     * whole sprite list
     * Throws exception if not found
     */
    PaintownUtil::ReferenceCount<Sprite>probeSff(const Filesystem::AbsolutePath &file, int groupNumber, int spriteNumber, bool mask, const Filesystem::AbsolutePath & actFile = Filesystem::AbsolutePath());
    /* similar to probeSff but searches for sprites 9000,0 and 9000,1 which are
     * the icon and the portrait respectively.
     */
    void getIconAndPortrait(const Filesystem::AbsolutePath & sffPath, const Filesystem::AbsolutePath & actPath, PaintownUtil::ReferenceCount<Mugen::Sprite> * icon, PaintownUtil::ReferenceCount<Mugen::Sprite> * portrait);

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
// End namespace Util

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

/*! Game types currently in M.U.G.E.N */
enum GameType {
    Arcade,
    Versus,
    VersusSelect,
    NetworkVersusServer,
    NetworkVersusClient,
    TeamArcade,
    TeamVersus,
    TeamCoop,
    Survival,
    SurvivalCoop,
    Training,
    Watch,
    Demo,
    Undefined,
    Options,
    Quit,
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

/*! Arcade Matchup Data */
namespace ArcadeData{

/*! Information regarding a character */
class CharacterInfo{
public:
    CharacterInfo();
    CharacterInfo(const Filesystem::AbsolutePath &);
    CharacterInfo(const CharacterInfo &);
    virtual ~CharacterInfo();
    
    const CharacterInfo & operator=(const CharacterInfo &);
    
    bool operator==(const CharacterInfo &) const;
    bool operator!=(const CharacterInfo &) const;
    
    virtual void drawIcon(int x, int y, const Graphics::Bitmap &, const Mugen::Effects &) const;
    virtual void drawPortrait(int x, int y, const Graphics::Bitmap &, const Mugen::Effects &) const;
    
    virtual inline const Filesystem::AbsolutePath & getDef() const {
        return this->definition;
    }
    virtual inline void setStage(const Filesystem::AbsolutePath & stage){
        this->stage = stage;
    }
    virtual inline const Filesystem::AbsolutePath & getStage() const {
        return this->stage;
    }
    virtual inline void setMusic(const Filesystem::AbsolutePath & music){
        this->stage = music;
    }
    virtual inline const Filesystem::AbsolutePath & getMusic() const {
        return this->music;
    }
    virtual inline void setOrder(int order){
        this->order = order;
    }
    virtual inline int getOrder() const {
        return this->order;
    }
    virtual inline void setRandomStage(bool random){
        this->randomStage = random;
    }
    virtual inline bool getRandomStage() const {
        return this->randomStage;
    }
    virtual inline void setIncludeStage(bool include){
        this->includeStage = include;
    }
    virtual inline bool getIncludeStage() const {
        return this->includeStage;
    }
    virtual inline const std::string & getName() const {
        return this->name;
    }
    virtual inline const std::string & getDisplayName() const {
        return this->displayName;
    }
    virtual inline void setAct(int act){
        this->act = act;
    }
    virtual inline int getAct() const {
        return this->act;
    }
protected:
    //! Load images portrait and profile
    virtual void loadImages();
    //! Definition file
    Filesystem::AbsolutePath definition;
    //! Stage file
    Filesystem::AbsolutePath stage;
    //! Music file (overrides the BGM of the stage)
    Filesystem::AbsolutePath music;
    //! order
    unsigned int order;
    //! random stage?
    bool randomStage;
    //! Include stage in select stage collection?
    bool includeStage;
    //! Name
    std::string name;
    //! Display name
    std::string displayName;
    //! Act position
    int act;
    //! Icon
    PaintownUtil::ReferenceCount<Sprite> icon;
    //! Portrait
    PaintownUtil::ReferenceCount<Sprite> portrait;
};

/*! Single, Simultaneous or Turns collection */
class CharacterCollection{
public:
    enum Type{
        Single,
        Simultaneous,
        Turns2,
        Turns3,
        Turns4,
    };
    
    CharacterCollection(const Type &);
    CharacterCollection(const CharacterCollection &);
    virtual ~CharacterCollection();
    
    const CharacterCollection & operator=(const CharacterCollection &);
    
    virtual bool checkSet() const;
    
    virtual void setNext(const CharacterInfo &, int act = 0);
    
    virtual const CharacterInfo & getLastSet() const;
    
    virtual inline void setType(const Type & type){
        this->type = type;
    }
    virtual inline const Type & getType() const {
        return this->type;
    }
    virtual inline void setFirst(const CharacterInfo & character, int act = 0){
        this->first = character;
        this->first.setAct(act);
        this->firstSet = true;
    }
    virtual inline const CharacterInfo & getFirst() const {
        return this->first;
    }
    virtual inline void setSecond(const CharacterInfo & character, int act = 0){
        if (this->type == Single){
            return;
        }
        this->second = character;
        this->second.setAct(act);
        this->secondSet = true;
    }
    virtual inline const CharacterInfo & getSecond() const {
        return this->second;
    }
    virtual inline void setThird(const CharacterInfo & character, int act = 0){
        if (this->type == Single || this->type == Simultaneous){
            return;
        }
        this->third = character;
        this->third.setAct(act);
        this->thirdSet = true;
    }
    virtual inline const CharacterInfo & getThird() const {
        return this->third;
    }
    virtual inline void setFourth(const CharacterInfo & character, int act = 0){
        if (this->type == Single || this->type == Simultaneous){
            return;
        }
        this->fourth = character;
        this->fourth.setAct(act);
        this->fourthSet = true;
    }
    virtual inline const CharacterInfo & getFourth() const {
        return this->fourth;
    }
    virtual inline bool getFirstSet() const {
        return this->firstSet;
    }
    virtual inline bool getSecondSet() const {
        return this->secondSet;
    }
    virtual inline bool getThirdSet() const {
        return this->thirdSet;
    }
    virtual inline bool getFourthSet() const {
        return this->fourthSet;
    }
    
protected:
    Type type;
    CharacterInfo first;
    CharacterInfo second;
    CharacterInfo third;
    CharacterInfo fourth;
    bool firstSet;
    bool secondSet;
    bool thirdSet;
    bool fourthSet;
};

/*! Arcade Match Handling */
class MatchPath{
public:
    MatchPath();
    MatchPath(const CharacterCollection::Type &, const std::vector<int> &, const std::vector<CharacterInfo> &, const std::vector<Filesystem::AbsolutePath> &);
    MatchPath(const MatchPath &);
    virtual ~MatchPath();
    const MatchPath & operator=(const MatchPath &);
    virtual bool hasMore();
    virtual CharacterCollection next();
    
protected:
    std::queue<CharacterCollection> opponents;
};

}
// End namespace ArcadeData


/*! Configuration Helper */
namespace Configuration{
void set(const std::string &, const std::string &);
void set(const std::string &, bool value);
void set(const std::string &, int value);
void set(const std::string &, double value);
bool check(const std::string &);
PaintownUtil::ReferenceCount<std::istringstream> get(const std::string &);
}
// End namespace Configuration

/*! Sound system to make playing and using sounds easier.
 */
template <typename Type>
class SoundSystem{
public:
    SoundSystem(){
    }

    SoundSystem(const SoundSystem & copy):
    sounds(copy.sounds),
    soundLookup(copy.soundLookup){
    }

    virtual ~SoundSystem(){
    }

    const SoundSystem & operator=(const SoundSystem & copy){
        sounds = copy.sounds;
        soundLookup = copy.soundLookup;
        return *this;
    }

    //! Init the sound system given a .snd file
    void init(const std::string & file){
        Mugen::Util::readSounds(Util::findFile(Filesystem::RelativePath(file)), sounds);
        Global::debug(1) << "Got Sound File: '" << file << "'" << std::endl;
    }

    //! Play
    void play(const Type & type){
        PaintownUtil::ReferenceCount<Mugen::Sound> sound = sounds[soundLookup[type].group][soundLookup[type].index];
        if (sound != NULL){
            sound->play();
        }
    }

    //! Sets a sound
    virtual void set(const Type & type, int group, int sound){
        IndexValue values;
        values.group = group;
        values.index = sound;
        soundLookup[type] = values;
    }

protected:
    //! Sounds
    Mugen::SoundMap sounds;
    //! Sound lookup
    struct IndexValue{
        int group;
        int index;
    };
    std::map<Type, IndexValue> soundLookup;
};

class Font;

/*! Font system */
class FontSystem{
public:
    FontSystem();
    FontSystem(const FontSystem &);
    virtual ~FontSystem();
    
    virtual const FontSystem & operator=(const FontSystem &);
    
    /*! Font */
    class Font{
    public:
        Font();
        Font(PaintownUtil::ReferenceCount<Mugen::Font> font, int bank, int position);
        Font(const Font &);
        virtual ~Font();
        
        virtual const Font & operator=(const Font &);
        
        virtual void draw(int x, int y, const std::string &, const Graphics::Bitmap &) const;
        virtual void draw(int x, int y, int bank, int position, const std::string &, const Graphics::Bitmap &) const;
        virtual void draw(int x, int y, int position, const std::string &, const Graphics::Bitmap &) const;
        
        virtual int getHeight() const;
        virtual int getWidth(const std::string &) const;
    protected:
        PaintownUtil::ReferenceCount<Mugen::Font> font;
        int bank;
        int position;
    };
    
    virtual void add(const std::string &);
    virtual PaintownUtil::ReferenceCount<Mugen::Font> get(int index);
    virtual const Font getFont(int index, int bank, int position);
    virtual inline bool empty() const {
        return this->fonts.empty();
    }
protected:
    std::vector< PaintownUtil::ReferenceCount<Mugen::Font> > fonts;
};

}
// End namespace Mugen

#endif
