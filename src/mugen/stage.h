#ifndef paintown_mugen_stage_h
#define paintown_mugen_stage_h

#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <deque>
#include "exception.h"
#include "util.h"
#include "util/graphics/bitmap.h"
#include "common.h"
#include "stage-state.h"

namespace Graphics{
class Bitmap;
}
namespace Console{
    class Console;
}

namespace Mugen{
    class Animation;
    class Character;
    struct ResourceEffect;
    class Projectile;
    class Helper;
    class Object;
    class Background;
    class Spark;
    class Effect;
    class GameInfo;
    class World;

class Effect{
public:
    Effect(const Character * owner, PaintownUtil::ReferenceCount<Animation> animation, int id, int x, int y, double scaleX, double scaleY, int spritePriority);
    
    virtual void draw(const Graphics::Bitmap & work, int cameraX, int cameraY);
    virtual void logic();
    virtual bool isDead();
    virtual int getSpritePriority() const;

    virtual inline const Character * getOwner() const {
        return owner;
    }

    virtual void superPauseStart();
    virtual void superPauseEnd();

    virtual inline double getX() const {
        return x;
    }

    virtual inline double getY() const {
        return y;
    }

    virtual void setX(double x){
        this->x = x;
    }

    virtual void setY(double y){
        this->y = y;
    }

    virtual inline int getId() const {
        return id;
    }

    virtual ~Effect();
protected:
    const Character * owner;
    PaintownUtil::ReferenceCount<Animation> animation;
    int id;
    double x;
    double y;
    double scaleX;
    double scaleY;
    int spritePriority;
};

}

namespace Ast{
    class Section;
}

class MugenItemContent;
// class MugenBackgroundManager;

struct PlayerData {
    double oldx;
    double oldy;
    bool leftTension;
    bool rightTension;
    bool leftSide;
    bool rightSide;
    bool above;
    bool jumped;
};

namespace Mugen {

class Sound;
class Sprite;

class Stage{
public:
    // Location at dataPath() + "mugen/stages/"
    Stage(const Filesystem::AbsolutePath & s);
    // MugenStage( const char * location );

    virtual ~Stage();

    void load();

    inline const std::string &getName() const {
        return name;
    }
    /*
       inline const std::map<int, Animation*> & getAnimations() const {
       return animations;
       }*/
    
    inline const std::string & getMusic() const {
	return this->music;
    }
    
    inline const int getMusicVolume() const {
	return this->musicVolume;
    }

    virtual void begin();

    inline double getCameraX() const { return getStateData().camerax; }
    inline double getCameraY() const { return getStateData().cameray; }
    inline int getTension() const { return tension; }
    inline int getFloorTension() const { return floortension; }
    inline int getZ() const { return zoffset; }
    /* get position of the floor */
    int getFloor() const;

    inline unsigned long int getTicks() const { return getStateData().ticker; }

    void setCamera( const double x, const double y );
    void moveCamera( const double x, const double y );

    // Get background by ID
    //MugenBackground *getBackground( int ID );

    // This will update ticks and controllers. When ready pass the player objects to properly allow camera to follow
    // For now use view port, x and y screen size will be used to determine width/height
    void logic();

    void runCycle();

    // Render the backgrounds appropriately
    void render(Graphics::Bitmap *work);

    // Reset scenario
    void reset();
    
    virtual std::vector<Character *> getPlayers() const;

    // Add player1 people
    virtual void addPlayer1(Character * o);

    // Add player2 people
    virtual void addPlayer2(Character * o);

    // Set player health
    virtual void setPlayerHealth(int health);

    //! Set match
    virtual void setMatchOver(bool over){
        this->gameOver = over;
    }

    //! Check match
    virtual bool isMatchOver() const {
        return this->gameOver;
    }

    virtual GameInfo * getGameInfo() const {
        return gameHUD;
    }

    virtual void enableScreenBound(Character * who, bool offScreen, bool panX, bool panY);

    virtual void Quake(int q);

    //! Do continue screen return true to continue playing, false to end
    virtual bool doContinue(const PlayerType & type, InputMap<Keys> &);

    /* 1 is normal, 0<rate<1 is slower, 1<rate is faster */
    virtual void setGameRate(double rate);

    virtual int currentZOffset() const;

    /*
    virtual void pause();
    virtual void unpause();
    virtual void changePause();
    virtual bool isPaused();
    */

    // Console
    // virtual void toggleConsole();

    // Render debug stuff like lines and crap
    virtual void toggleDebug(int player);

    virtual PaintownUtil::ReferenceCount<World> snapshotState();
    virtual void updateState(const World & world);

    // Inherited world actions
    virtual void draw(Graphics::Bitmap * work);
    virtual void addObject(Character * o);
    virtual bool finished() const;
    virtual void reloadLevel();
    // virtual Character * findObject(int id);
    virtual int getMaximumZ();
    virtual int getMinimumZ();
    static const std::string getStageName(const std::string &filename);
    
    bool isaPlayer(Character * o) const;

    virtual Character * getCharacter(const CharacterId & id) const;

    virtual Character * getEnemy(const Character * who) const;
    virtual std::vector<Character *> getTargets(int id, const Character * from) const;

    virtual int getGameTime() const;

    virtual void doZoom(double x, double y, int zoomTime, int zoomOutTime, int time,
                        int bindTime, double scaleX, double scaleY,
                        double velocityX, double velocitY, double accelX, double accelY,
                        int superMoveTime, int pauseMoveTime, bool removeOnGetHit,
                        Character * bound, Character * owner);

    /* If currently zoomed in */
    virtual bool isZoomed() const;

    virtual int zoomX1() const;
    virtual int zoomY1() const;
    virtual int zoomX2() const;
    virtual int zoomY2() const;

    /* pause for dramatic effect */
    virtual void doSuperPause(int time, Character & guy, int animation, bool ownAnimation, int positionX, int positionY);
    virtual void doPause(int time, int buffer, int moveAllowed, bool pauseBackground, Character * who);

    /* make dust effects */
    virtual void createDust(int x, int y);

    /* get an animation from fightfx.sff */
    virtual PaintownUtil::ReferenceCount<Animation> getFightAnimation(int id);

    virtual void addProjectile(Projectile * projectile);
    virtual void addEffect(Effect * effect);
    virtual void removeEffects(const Character * owner, int id);

    virtual int countMyHelpers(const Character * owner) const;
    virtual std::vector<Projectile*> findProjectile(int id, const Character * owner) const;
    virtual std::vector<Effect*> findExplode(int id, const Character * owner) const;
    virtual std::vector<Helper*> findHelpers(const Character * owner) const;
    virtual std::vector<Helper*> findHelpers(const Character * owner, int id) const;
    virtual Effect * findEffect(const Character * owner, int id);
    virtual std::vector<Effect *> findEffects(const Character * owner, int id);

    virtual void setEnvironmentColor(Graphics::Color color, int time, bool under); 

    virtual void removeHelper(Character * who);

    // Alliance setting
    enum teams{
        Player1Side = 5,
        Player2Side
    };

    /* Edges of the visible screen */
    int maximumRight(const Character * who) const;
    int maximumLeft(const Character * who) const;
    /* Highest visible point on the screen */
    int maximumUp() const;
    int maximumDown() const;

    /* These two methods are hacks to support the mugen bug in the Explod controller
     * when bindtime is 0. If the postype is left, right, back, or front then the
     * explod will be created at the maximumLeft/maximumRight that existed at the
     * start of the match no matter what the current maximumLeft/Right are.
     *
     * Very strange but this is what I tested in the real mugen.
     */
    int getStartingLeft() const;
    int getStartingRight() const;

    virtual void setPaletteEffects(int time, int addRed, int addGreen, int addBlue, int multiplyRed, int multiplyGreen, int multiplyBlue, int sinRed, int sinGreen, int sinBlue, int period, int invert, int color);
    
    /* Remove references from this object to other objects, like through targetting */
    virtual void unbind(Character * what);

protected:
    struct cymk_holder{
        cymk_holder():c(0),m(0),y(0),k(0){}
        int c, m, y, k;
    };

    struct PaletteEffects{
        PaletteEffects():
            time(0),
            addRed(0),
            addGreen(0),
            addBlue(0),
            multiplyRed(0),
            multiplyGreen(0),
            multiplyBlue(0),
            sinRed(0),
            sinGreen(0),
            sinBlue(0),
            period(0),
            invert(0),
            color(0),
            counter(0){
            }

        PaletteEffects(const PaletteEffects & copy):
            time(copy.time),
            addRed(copy.addRed),
            addGreen(copy.addGreen),
            addBlue(copy.addBlue),
            multiplyRed(copy.multiplyRed),
            multiplyGreen(copy.multiplyGreen),
            multiplyBlue(copy.multiplyBlue),
            sinRed(copy.sinRed),
            sinGreen(copy.sinGreen),
            sinBlue(copy.sinBlue),
            period(copy.period),
            invert(copy.invert),
            color(copy.color),
            counter(copy.counter){
            }

        int time;
        int addRed;
        int addGreen;
        int addBlue;
        int multiplyRed;
        int multiplyGreen;
        int multiplyBlue;
        int sinRed;
        int sinGreen;
        int sinBlue;
        int period;
        int invert;
        int color;
        unsigned int counter;
    };

    PaletteEffects paletteEffects;

    void drawBackgroundWithEffects(int x, int y, const Graphics::Bitmap & board);
    void drawForegroundWithEffects(int x, int y, const Graphics::Bitmap & board);
    void drawBackgroundWithEffectsSide(int x, int y, const Graphics::Bitmap & board, void (Background::*render) (int, int, const Graphics::Bitmap &, Graphics::Bitmap::Filter *));

    /* section loaders */
    void loadSectionInfo(Ast::Section * section);
    void loadSectionCamera(Ast::Section * section);
    void loadSectionPlayerInfo(Ast::Section * section);
    void loadSectionBound(Ast::Section * section);
    void loadSectionStageInfo(Ast::Section * section);
    void loadSectionShadow(Ast::Section * section, cymk_holder & shadow);
    void loadSectionReflection(Ast::Section * section);
    void loadSectionMusic(Ast::Section * section);

    void updatePlayer(Character *o);
    void physics(Character * o);
    bool doBlockingDetection(Character * obj1, Character * obj2);
    bool doCollisionDetection(Character * obj1, Character * obj2);
    bool doReversalDetection(Character * obj1, Character * obj2);

    CharacterId nextId();
    
protected:

    bool exists(Character * who);
    bool exists(CharacterId id);

    /* Current level of zoom scaling */
    double zoomScale() const;
    void updateZoom();

    void addSpark(int x, int y, int sparkNumber, bool own, Character * owner);
    // void addSpark(int x, int y, const PaintownUtil::ReferenceCount<Animation> & animation);
    void addSpark(int x, int y, const ResourceEffect & resource, const ResourceEffect & default_, Character * owner);
    void playSound(Character * owner, int group, int item, bool own);
    void doProjectileCollision(Projectile * projectile, Character * mugen);
    void doProjectileToProjectileCollision(Projectile * mine, Projectile * his);

    int findMaximumSpritePriority();
    int findMinimumSpritePriority();
    std::vector<int> allSpritePriorities();

    std::vector<Character*> getOpponents(Object * who);

    /* Location is the directory passed in ctor
       This is where the def is loaded and all the relevant files
       are loaded from
       */
    Filesystem::AbsolutePath location;

    Filesystem::AbsolutePath baseDir;

    /* These items are taken from stage.def file */

    /* Base definitions */

    // Name of Stage
    std::string name;

    /*;Camera starting position: Usually 0 for both */
    int startx;
    int starty;

    /* Starting x coordinates */
    int originalMaxLeft;
    int originalMaxRight;

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
    /*
       zoffsetlink = BG_ID ;(optional)
       If this parameter is specified, it links the zoffset value to the y-position of a background element with ID number BG_ID. 
       For example, you can link the value to a dummy element (refer to "Other background element types" in section III: 
       Description of background elements) with a sin.y parameter to make the characters bob up and down in a sinusoidal fashion.
       */
    int zoffsetlink;

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

    // Reflection aparently newer versions have it under shadow
    bool reflect;

    /*;This is the shadow color given in r,g,b. Valid values for each
      ;range from 0 (lightest) to 255 (darkest).
      ;Defaults to 0,0,0 if omitted.
      ;intensity and color's effects add up to give the final shadow
      ;result.*/
    Graphics::Color shadowColor;  // this = 0,0,0 gets converted by Bitmap::makeColor

    /*;This is the scale factor of the shadow. Use a big scale factor
      ;to make the shadow longer. You can use a NEGATIVE scale factor
      ;to make the shadow fall INTO the screen.
      ;Defaults to 0.4 if omitted.*/
    double shadowYscale;

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
    int shadowFadeRangeHigh;
    int shadowFadeRangeMid;

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
    std::string music;

    /*;Adjust the volume. 0 is normal, negative for softer, and
      ;positive for louder (only for mp3, mods and CDA)
      Probably won't be used unless support for those items above get added*/
    int musicVolume;

    /* Main background definitions.. ie [BGdef] */

    // Sprite file
    //std::string sffFile;
    // This is debug for backgrounds probably won't use it, clears to magenta before drawing first layer 
    //bool debugbg;

    /* Sprites */
    //std::map< unsigned int, std::map< unsigned int, Sprite * > > sprites;

    /* Animation Lists stored by action number, ie [Begin Action 500] */
    //std::map< int, Animation * > animations;

    /* Backgrounds */
    //std::vector< MugenBackground * > backgrounds;

    /* Foregrounds */
    //std::vector< MugenBackground * > foregrounds;

    /* Sounds */
    Mugen::SoundMap sounds;

    // MugenBackgroundManager *background;
    Background * background;

    /* Commands, Triggers or whatever else we come up with */

    // Our working bitmap
    // Graphics::Bitmap *board;

    // Our real axis givin that we have to use negatives
    int xaxis;
    int yaxis;

    // Start the stage sometime
    bool stageStart;

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

private:

    // Our output Console
    // Console::Console * console;

    // Debug enabled?
    bool debugMode;

    void cleanup();

    std::vector<Projectile*> projectiles;

    std::vector<Character*> objects;

    // player list so we can distinguish
    std::vector<Character *> players;

    /* FIXME: sort of a hack which lets other classes with a reference to the
     * stage call stage.addObject(blah). really a vector should be passed
     * to the calling class that the class can add to and the stage will
     * insert all the objects from that vector into the `objects' vector
     * at some later point.
     */
    std::vector<Character*> addedObjects;
    // Hold information for players
    std::map<void *, PlayerData> playerInfo;

    bool loaded;

    // Controllers
    //std::vector<MugenBackgroundController *> controllers;

    // This is for controllers as sometimes backgrounds share IDs for this purpose
    //void getBackgrounds( std::vector<MugenBackground *> &bgs, int ID );

    void initializeName();

    SpriteMap effects;
    std::map<int, PaintownUtil::ReferenceCount<Animation> > sparks;
    std::vector<Effect*> showSparks;

    // Character huds
    GameInfo *gameHUD;

    bool gameOver;

    /* frequency of logic updates. 1 is normal, lower is slower */
    double gameRate;

    int objectId;

    StageStateData stateData;
    StageStateData & getStateData();
    const StageStateData & getStateData() const;
    void setStateData(const StageStateData & data);
};

}

#endif
