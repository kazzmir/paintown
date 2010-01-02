#ifndef paintown_mugen_character_h
#define paintown_mugen_character_h

#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "mugen_exception.h"

// Implement object_attack
#include "object/object_attack.h"
#include "network/network.h"

namespace Ast{
    class Key;
}

class Bitmap;
class MugenItemContent;
class MugenSprite;
class MugenSound;
class MugenAnimation;

namespace Mugen{

struct Constant{
    enum ConstantType{
        None,
        Double,
        ListOfDouble,
    };

    Constant():
    type(None){
    }

    Constant(double value):
    type(Double),
    double_(value){
    }

    Constant(std::vector<double> doubles):
    type(ListOfDouble),
    doubles(doubles){
    }

    ConstantType type;

    double double_;
    std::vector<double> doubles;
};

/* key command */
class Command{
public:
    Command(std::string name, Ast::Key * key, int maxTime, int bufferTime);
};

class Character: public ObjectAttack {
public:
	// Location at dataPath() + "mugen/chars/"
	Character(const std::string & s );
	Character(const char * location );
	Character(const std::string & s, int alliance );
	Character(const std::string & s, const int x, const int y, int alliance );
	Character(const Character &copy );

	virtual ~Character();
	
	// Convert to paintown character or whatever
	// Do code
	
	virtual void load();
	
	virtual void renderSprite(const int x, const int y, const unsigned int group, const unsigned int image, Bitmap *bmp, 
			   const int flip=1, const double scalex = 1, const double scaley = 1);
			   
	// Change palettes
	virtual void nextPalette();
	virtual void priorPalette();
	
	virtual inline const std::string &getName() const {
            return name;
        }
	
	virtual inline const std::string &getDisplayName() const {
            return displayName;
        }
	
	virtual inline const unsigned int &getCurrentPalette() const {
            return currentPalette;
        }

        virtual inline const std::map<int, MugenAnimation*> & getAnimations() const {
            return animations;
        }
	
	virtual inline const std::map<unsigned int, std::map<unsigned int, MugenSound *> >& getSounds() const {
            return sounds;
        }
	
	/*! This all the inherited members */
	virtual void act(std::vector<Object*, std::allocator<Object*> >*, World*, std::vector<Object*, std::allocator<Object*> >*);                       
	virtual void draw(Bitmap*, int);                      
	virtual void grabbed(Object*);
	virtual void unGrab();
	virtual bool isGrabbed();
	virtual Object* copy();
	virtual const std::string& getAttackName();
	virtual bool collision(ObjectAttack*);
	virtual int getDamage() const;
	virtual bool isCollidable(Object*);
	virtual bool isGettable();
	virtual bool isGrabbable(Object*);
	virtual bool isAttacking();
	virtual int getWidth() const;
	virtual int getHeight() const;
	virtual Network::Message getCreateMessage();
	virtual void getAttackCoords(int&, int&);
	virtual double minZDistance() const;
	virtual void attacked(World*, Object*, std::vector<Object*, std::allocator<Object*> >&);

protected:
    void initialize();

    /* This creates the animations no need for a new class to handle this */
    virtual void bundleAnimations();
    virtual void loadCmdFile(const std::string & path);
    virtual void loadCnsFile(const std::string & path);
    virtual void loadStateFile(const std::string & path);

    virtual void addCommand(Command * command);

    virtual void setConstant(std::string name, const std::vector<double> & values);
    virtual void setConstant(std::string name, double value);

protected:

	/* Location is the directory passed in ctor
	This is where the def is loaded and all the relevant files
	are loaded from
	*/
	std::string location;
	
	std::string baseDir;
	
	/* These items are taken from character.def file */
	
	/* Base definitions */
	
	// Name of Character
	std::string name;
	// Name of Character to Display why there is two is beyond me, retards
	std::string displayName;
	// Version date (unused)
	std::string versionDate;
	// Version that works with mugen (this isn't mugen)
	std::string mugenVersion;
	// Author 
	std::string author;
	// Palette defaults
	std::vector< unsigned int> palDefaults;
	unsigned int currentPalette;
	
	/* Relevant files */
	
	// Command set file
	std::string cmdFile;
	// Constants
	std::string constantsFile;
	// States
	std::string stateFile;
	// Common States
	std::string commonStateFile;
	// Other state files? I can't find documentation on this, in the meantime we'll wing it
	std::string stFile[12];
	// Sprites
	std::string sffFile;
	// animations
	std::string airFile;
	// Sounds
	std::string sndFile;
	// Palettes max 12
        std::map<int, std::string> palFile;
	
	// Arcade mode ( I don't think we will be using this anytime soon )
	std::string introFile;
	std::string endingFile;
	
	/* Now on to the nitty gritty */
	
	/* Player Data and constants comes from cns file */
	
	/* Section [Data] */
	
	// Life
	int life;
	// Attack
	int attack;
	// Defence
	int defence;
	// How much to bring up defese on fall
	int falldefenseup;
	// How long to lie down when fall
	int liedowntime;
	// Air juggle points this charcter is worth?
	int airjuggle;
	// Default Hit Spark Number for hitdefs ???
	int sparkno;
	// Default guard spark number
	int guardsparkno;
	// Echo on KO (I guess is for the death sound)
	bool koecho;
	// Volume offset on characters sounds
	int volumeoffset;
	// Maybe used in VS mode later
	/* According to the definition: 
	Variables with this index and above will not have their values
	reset to 0 between rounds or matches. There are 60 int variables,
	indexed from 0 to 59, and 40 float variables, indexed from 0 to 39.
	If omitted, then it defaults to 60 and 40 for integer and float
	variables repectively, meaning that none are persistent, i.e. all
	are reset. If you want your variables to persist between matches,
	you need to override state 5900 from common1.cns.
	*/
	int intpersistindex;
	int floatpersistindex;
	
	/* Section [Size] */
	
	// Horizontal Scaling Factor
	int xscale;
	//Vertical scaling factor.
	int yscale;
	//      ;Player width (back, ground)
	int groundback;
	//   ;Player width (front, ground)
	int groundfront;
	//      ;Player width (back, air)
	int airback;
	//     ;Player width (front, air)
	int airfront;
	//  = 60          ;Height of player (for opponent to jump over)
	int height;
	// = 160    ;Default attack distance
	int attackdist;
	//  = 90 ;Default attack distance for projectiles
	int projattackdist;
	//  = 0     ;Set to 1 to scale projectiles too
	bool projdoscale;
	// = -5, -90   ;Approximate position of head
	int headposx;
	int headposy;
	//  = -5, -60    ;Approximate position of midsection
	int midposx;
	int midposy;
	//  = 0     ;Number of pixels to vertically offset the shadow
	int shadowoffset;
	// = 0,0    ;Player drawing offset in pixels (x, y)
	int drawoffsetx;
	int drawoffsety;
	
	/* Section [Velocity] */
	
	//   = 2.4      ;Walk forward
	double walkfwd;
	// = -2.2     ;Walk backward
	double walkback;
	//  = 4.6, 0    ;Run forward (x, y)
	double runfwdx;
	double runfwdy;
	// = -4.5,-3.8 ;Hop backward (x, y)
	double runbackx;
	double runbacky;
	// = 0,-8.4    ;Neutral jumping velocity (x, y)
	double jumpneux;
	double jumpneuy;
	// = -2.55    ;Jump back Speed (x, y)
	double jumpbackx;
	double jumpbacky;
	// = 2.5       ;Jump forward Speed (x, y)
	double jumpfwdx;
	double jumpfwdy;
	// = -2.55,-8.1 ;Running jump speeds (opt)
	double runjumpbackx;
	double runjumpbacky;
	// = 4,-8.1      ;.
	double runjumpfwdx;
	double runjumpfwdy;
	// = 0,-8.1      ;.
	double airjumpneux;
	double airjumpneuy;
	// Air jump speeds (opt)
	double airjumpback;
	double airjumpfwd;
	
	/* Movement */
	
	//  = 1      ;Number of air jumps allowed (opt)
	int airjumpnum;
	//  = 35  ;Minimum distance from ground before you can air jump (opt)
	int airjumpheight;
	// = .44         ;Vertical acceleration
	double yaccel;
	//  = .85  ;Friction coefficient when standing
	double standfriction;
	//  = .82 ;Friction coefficient when crouching
	double crouchfriction;

	/* Sprites */
	std::map< unsigned int, std::map< unsigned int, MugenSprite * > > sprites;
	// Bitmaps of those sprites
	std::map< unsigned int, std::map< unsigned int, Bitmap * > > bitmaps;
	
	/* Animation Lists stored by action number, ie [Begin Action 500] */
	std::map< int, MugenAnimation * > animations;
	
	/* Sounds */
	std::map< unsigned int, std::map< unsigned int, MugenSound * > > sounds;
	
	/* Commands, Triggers or whatever else we come up with */
        std::map<std::string, Constant> constants;

        int currentState;
};

}

#endif
