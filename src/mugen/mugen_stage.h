#ifndef mugen_stage_h
#define mugen_stage_h

#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "mugen_exception.h"

// Implement world because raf is a kind man
#include "world.h"

class Bitmap;
class MugenItemContent;
class MugenSprite;
class MugenSound;
class MugenAnimation;
class MugenBackground;

class MugenStage : public World{
public:
	// Location at dataPath() + "mugen/stages/"
	MugenStage( const string & s );
	MugenStage( const char * location );

	~MugenStage();
	
	void load() throw( MugenException );
	
	inline const std::string &getName() const {
            return name;
        }

        inline const std::map<int, MugenAnimation*> & getAnimations() const {
            return animations;
        }
	
	inline const int getCameraX() { return camerax; }
	inline const int getCameraY() { return cameray; }
	
	inline void setCamera( const int &x, const int &y ) { camerax = x; cameray = y; }
	inline void moveCamera( const int &x, const int &y ) { camerax += x; cameray += y; }
	
	int getTicks();
	
	// This will update ticks and controllers. When ready pass the player objects to properly allow camera to follow
	// For now use view port, x and y screen size will be used to determine width/height
	void logic();
	
	// Render the backgrounds appropriately
	void render( Bitmap *work );
	
	// Reset scenario
	void reset();
	
	// Inherited world actions
	virtual void act();
	virtual void draw( Bitmap * work );
	
protected:

	/* Location is the directory passed in ctor
	This is where the def is loaded and all the relevant files
	are loaded from
	*/
	std::string location;
	
	std::string baseDir;
	
	/* These items are taken from stage.def file */
	
	/* Base definitions */
	
	// Name of Stage
	std::string name;
	
	/*;Camera starting position: Usually 0 for both */
	int startx;
	int starty;

	/*;Left and right bound of camera */
 
	int boundleft;
	int boundright;

	/*;High and low bound of camera 
	;High is a negative number. Make is more negative if you want to
	;camera to be able to move higher.
	;Low should usually be 0.
	;If omitted, defaults to -25 and 0 respectively */
	int boundhigh;
	int boundlow;

	/*
	;This is how much the camera will move vertically towards the
	;highest player. Valid values are from 0 to 1. A value of 0 will mean
	;the camera does not move up at all. A value of 1 will makes the camera
	;follow the highest player.
	;Typically .2 for normal-sized backgrounds. You may need to pull this
	;value up for taller backgrounds.*/
	double verticalfollow;

	/*
	;Minimum vertical distance the highest player has to be from the floor,
	;before the camera starts to move up to follow him. For extremely
	;tall stages, a floor tension of about 20-30 coupled with a
	;vertical-follow of .8 allows the camera to follow the action. */
	int floortension;

	/*;Horizontal distance player is from edge before camera starts to move
	;left or right. Typically 50 or 60.*/
	int tension;

	/*
	;--------------------------------------------------------
	[PlayerInfo]
	;--- Player 1 ---
	;Player 1 starting coordinates.
	;p1startx is typically -70 and p2startx is 70.
	;p1starty and p1startz should be 0.*/
	int p1startx; //            ;Starting x coordinates
	int p1starty; //            ;Starting y coordinates
	int p1startz; //            ;Starting z coordinates
	int p1facing; //            ;Direction player faces: 1=right, -1=left

	/*;--- Player 2 --- */
	int p2startx;
	int p2starty;
	int p2startz;
	int p2facing;

	/*;--- Common ---
	;Don't change these values.*/
	int leftbound; // ;Left bound (x-movement)
	int rightbound; // ;Right bound
	int topbound; //     ;Top bound (z-movement)
	int botbound; //     ;Bottom bound

	/*
	;--------------------------------------------------------
	[Scaling]
	;No need to change these values
	int topz;	//     = 0       ;Top z-coordinate for scaling
	int botz; //   ;Bottom z-coordinate for scaling
	double topscale; //      ;Scale to use at top
	double botscale; //     ;Scale to use at bottom*/
	
	/*
	;--------------------------------------------------------
	[Bound]
	;Distance from left/right edge of screen that player can move to
	;Typically 15 */
	int screenleft; //    ;Dist from left of screen that player can move to
	int screenright; //   ;Right edge

	/*;--------------------------------------------------------
	[StageInfo]
	;Z offset for drawing
	;Adjust this value to move the ground level up/down in the screen.
	;It's the position where the players stand at.
	;Up - smaller, Down - larger
	;*** */
	int zoffset;

	//;Leave this at 1. It makes the players face each other
	bool autoturn;

	/*;Set the following to 1 to have the background be reset between
	;rounds.*/
	bool resetBG;

	/*;--------------------------------------------------------
	[Shadow]
	;This is the shadow darkening intensity. Valid values range from
	;0 (lightest) to 256 (darkest). Defaults to 128 if omitted.*/
	int shadowIntensity;

	/*;This is the shadow color given in r,g,b. Valid values for each
	;range from 0 (lightest) to 255 (darkest).
	;Defaults to 0,0,0 if omitted.
	;intensity and color's effects add up to give the final shadow
	;result.*/
	int color;  // this = 0,0,0 gets converted by Bitmap::makeColor

	/*;This is the scale factor of the shadow. Use a big scale factor
	;to make the shadow longer. You can use a NEGATIVE scale factor
	;to make the shadow fall INTO the screen.
	;Defaults to 0.4 if omitted.*/
	double yscale;

	/*;This parameter lets you set the range over which the shadow is
	;visible. The first value is the high level, and the second is
	;the middle level. Both represent y-coordinates of the player.
	;A shadow is invisible if the player is above the high level,
	;and fully visible if below the middle level. The shadow is
	;faded in between the two levels. This gives an effect of the
	;shadow fading away as the player gets farther away from the
	;ground. If omitted, defaults to no level effects (shadow is
	;always fully visible).*/
	//fade.range = -400,-100
	int fadeRangeHigh;
	int fadeRangeMid;

	/*;--------------------------------------------------------
	[Reflection]
	;Intensity of reflection (from 0 to 256). Set to 0 to have no
	;reflection. Defaults to 0.*/
	int reflectionIntensity;

	/*;--------------------------------------------------------
	[Music]
	;Put a filename for a MOD, MP3 or MIDI here, or just leave it
	;blank if you don't want music. If an invalid filename is
	;given, then no music will play. To play CD audio, put
	;the track number followed by ".da". Using a track number of
	;0 will play a random audio track. For example, to play
	;track 3 from a music CD, use:
	;  bgmusic = 3.da 
	This option is ignored in paintown, no support for those formats*/
	//int bgmusic;

	/*;Adjust the volume. 0 is normal, negative for softer, and
	;positive for louder (only for mp3, mods and CDA)
	Probably won't be used unless support for those items above get added*/
	//int bgvolume;
	
	/* Main background definitions.. ie [BGdef] */
	
	// Sprite file
	std::string sffFile;
	// This is debug for backgrounds probably won't use it, clears to magenta before drawing first layer 
	int debugbg;
	
	/* Sprites */
	std::map< unsigned int, std::map< unsigned int, MugenSprite * > > sprites;
	
	/* Animation Lists stored by action number, ie [Begin Action 500] */
	std::map< int, MugenAnimation * > animations;
	
	/* Backgrounds */
	std::vector< MugenBackground * > backgrounds;
	
	/* Foregrounds */
	std::vector< MugenBackground * > foregrounds;
	
	/* Sounds */
	//std::map< unsigned int, std::map< unsigned int, MugenSound * > > sounds;
	
	/* Commands, Triggers or whatever else we come up with */
	
	// Our working bitmap
	Bitmap *board;
	
	// Our real axis givin that we have to use negatives
	int xaxis;
	int yaxis;
	
	// Camera location on stage
	int camerax;
	int cameray;
	
	// Total rounds set to -1 for infinite rounds
	int totalRounds;
	
	// Current round
	int round;
	
	// Total Time per round -1 for infinite time
	int totalTime;
	
	// Current time
	int time;
	
	// p1 round points (this also goes for team players ie 2v1 etc)
	int p1points;
	
	// p2 round points
	int p2points;
	
	
};

#endif
