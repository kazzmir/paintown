#ifndef _paintown_adventure_world_h
#define _paintown_adventure_world_h

namespace Paintown{
class Object;
class ObjectAttack;
class Enemy;
}

namespace Graphics{
class Bitmap;
}
class Scene;

namespace Effects{
class Gradient;
}

#include <vector>
#include <deque>
#include <map>
#include <string>

#include "r-tech1/file-system.h"
#include "r-tech1/network/network.h"
#include "world.h"
#include "../level/cacher.h"
#include "../level/block.h"

namespace Script{
    class Engine;
}

struct PlayerTracker{
    double min_x;
    Paintown::Object * player;
    void * script;

    double minimumX;
    double maximumX;
};

class Camera{
public:
    Camera(double x, double y);
    virtual ~Camera();

    /* move the camera to x/y */
    virtual void moveTo(double x, double y, int ticks);

    virtual double getX() const;
    virtual double getY() const;

    virtual void move();

protected:
    /* current position */
    double x, y;

    /* place to move to */
    double moveX, moveY;
    double speed;

    /* how many ticks to do it in */
    int ticksLeft;
};

class AdventureWorld: public World {
public:
	AdventureWorld();
	AdventureWorld(const std::vector< Paintown::Object * > & players, const Filesystem::AbsolutePath & path, Level::Cacher * cacher = new Level::DefaultCacher(), int screen_size = 320);

	virtual ~AdventureWorld();

	inline int getQuake() const {
		return quake_time;
	}
        
        virtual void begin();
        virtual void end();

	virtual void act();
	virtual void draw(const Graphics::Bitmap & work );
	virtual void addObject( Paintown::Object * o );

	virtual bool finished() const;

	virtual void reloadLevel();

    virtual Script::Engine * getEngine() const;

	/* upper left hand corner of the screen */
	virtual int getX();
	virtual int getY();
    
        virtual bool respawnPlayers(const std::vector<Paintown::Object*> & players);

    virtual void pause();
    virtual void unpause();
    virtual void changePause();
    virtual bool isPaused();

    virtual const std::deque<Graphics::Bitmap*> & getScreenshots();

    virtual int levelLength() const;
    virtual const Util::ReferenceCount<Block> currentBlock() const;

    virtual inline const std::vector<Paintown::Object*> & getObjects() const {
        return objects;
    }

    void addEnemy(Paintown::Enemy * obj);

	virtual void doScene( int min_x, int max_x );

        virtual Paintown::Object * findObject(int id);

	virtual int getMaximumZ();
	virtual int getMinimumZ();

	virtual void drawMiniMaps( bool b );
	virtual bool shouldDrawMiniMaps();
        
        virtual double ticks(const double in) const;

        /* handle players getting items */
        virtual void getItems();
        /* remove any objects that have health <= 0 */
        virtual void eraseDeadObjects(std::vector<Paintown::Object*> & added_effects);
        virtual void updateObject(Paintown::Object * good, std::vector<Paintown::Object*> & added_effects);
        virtual void handleCollisions(Paintown::ObjectAttack * o_good, std::vector<Paintown::Object*> & added_effects);

	void killAllHumans( Paintown::Object * player );

	virtual void addMessage( Network::Message m, Network::Socket from = 0, Network::Socket to = 0);
	Network::Message createBangMessage( int x, int y, int z );
        
        /* overloading virtual methods requires that all the original methods
         * are in scope. 'using' forces this.
         */
        using World::dyingObject;
        // virtual void dyingObject(const Object & obj);
        virtual void dyingObject(const Paintown::Player & obj);
        
protected:
        /* number of ticks to be in slow-motion */
        void enterSlowMotion(const int amount);

	Network::Message deleteMessage( unsigned int id );
	void loadLevel( const Filesystem::AbsolutePath & path );
	void threadedLoadLevel( const Filesystem::AbsolutePath & path );

	virtual void drawWorld( const PlayerTracker & tracker, const Graphics::Bitmap & where, const std::map< int, std::vector< Paintown::Object * > > & object_z, double cameraX);
    virtual void drawMiniMap(const Graphics::Bitmap & work, const PlayerTracker & player, const std::map<int, std::vector<Paintown::Object*> > & objects, int x, int y, int width, int height);
    virtual void showDescription(const Graphics::Bitmap & work, int time, const std::string & description);

	virtual void deleteObjects( std::vector< Paintown::Object * > * objects );

        bool randomLevel() const;

	virtual void doLogic();

	virtual bool isPlayer( Paintown::Object * o ) const;

    inline bool shouldTakeScreenshot() const {
        return takeAScreenshot;
    }

    inline void takeScreenshot(){
        takeAScreenshot = true;
    }

    virtual void doTakeScreenshot(const Graphics::Bitmap & work);

    static void * do_load_level(void * arg);

protected:
    /* add a player to the player tracker list*/
    void trackPlayer(Paintown::Player*);

    std::vector< PlayerTracker > players;
    /* when an attack hits show this graphic. this should be changed to a
     * per-character effect.
     */
    Paintown::Object * bang;

    Util::ReferenceCount<Scene> scene;

    // int min_x;
    int screen_size;
    /* path to the current level */
    Filesystem::AbsolutePath path;

    bool draw_minimaps;

    Util::ReferenceCount<Graphics::Bitmap> mini_map;

    /* screenshots that are shown at the end of the level */
    std::deque<Graphics::Bitmap*> screenshots;
    /* set during the logic but a screenshot actually happens in draw */
    bool takeAScreenshot;
    /* run the logic or not */
    bool is_paused;
    /* if the game should run in slow motion, like when a boss dies */
    int slowmotion;
    /* caches objects to make quick copies */
    Level::Cacher * cacher;
    /* for showing the current help */
    int descriptionTime;
    Effects::Gradient * descriptionGradient;
    /* count of ticks that have passed. how much time of gameplay can this hold
     * before overflowing?
     * there should be 90 logic loops per second and the max size of gameticks
     * is roughly 2^32.
     * 2^32 / (90 ticks * 60 seconds * 60 minutes * 24 hours)
     * = 552.33 days
     */
    unsigned int gameTicks;

    struct ReplayEvent{
        ReplayEvent(unsigned int time, const Network::Message & message):
        time(time),
        message(message){
        }
        /* what game tick the event occured on */
        unsigned int time;
        Network::Message message;
    };

    std::vector<ReplayEvent> replay;
    bool replayEnabled;

    Camera camera;
};

#endif
