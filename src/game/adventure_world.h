#ifndef _paintown_adventure_world_h
#define _paintown_adventure_world_h

class Object;
class Scene;
class Bitmap;
class Block;
class Enemy;

namespace Effects{
class Gradient;
}

#include <vector>
#include <deque>
#include <map>
#include <string>

#include "util/file-system.h"
#include "network/network.h"
#include "world.h"
#include "level/cacher.h"

namespace Script{
    class Engine;
}

struct PlayerTracker{
	double min_x;
	Object * player;
        void * script;
};

class AdventureWorld: public World {
public:
	AdventureWorld();
	AdventureWorld(const std::vector< Object * > & players, const Filesystem::AbsolutePath & path, Level::Cacher * cacher = new Level::DefaultCacher(), int screen_size = 320);

	virtual ~AdventureWorld();

	inline int getQuake() const {
		return quake_time;
	}

	virtual void act();
	virtual void draw( Bitmap * work );
	virtual void addObject( Object * o );

	virtual bool finished() const;

	virtual void reloadLevel();

    virtual Script::Engine * getEngine() const;

	/* upper left hand corner of the screen */
	virtual int getX();
	virtual int getY();

    virtual void pause();
    virtual void unpause();
    virtual void changePause();
    virtual bool isPaused();

    virtual const std::deque<Bitmap*> & getScreenshots();

    virtual int levelLength() const;
    virtual const Block * currentBlock() const;

    virtual inline const std::vector<Object*> & getObjects() const {
        return objects;
    }

    void addEnemy(Enemy * obj);

	virtual void doScene( int min_x, int max_x );

        virtual Object * findObject(int id);

	virtual int getMaximumZ();
	virtual int getMinimumZ();

	virtual void drawMiniMaps( bool b );
	virtual bool shouldDrawMiniMaps();
        
        virtual double ticks(const double in) const;

	void killAllHumans( Object * player );

	virtual void addMessage( Network::Message m, Network::Socket from = 0, Network::Socket to = 0);
	Network::Message createBangMessage( int x, int y, int z );
        
        /* overloading virtual methods requires that all the original methods
         * are in scope. 'using' forces this.
         */
        using World::dyingObject;
        // virtual void dyingObject(const Object & obj);
        virtual void dyingObject(const Player & obj);
        
protected:
        /* number of ticks to be in slow-motion */
        void enterSlowMotion(const int amount);

	Network::Message deleteMessage( unsigned int id );
	void loadLevel( const Filesystem::AbsolutePath & path );
	void threadedLoadLevel( const Filesystem::AbsolutePath & path );

	void drawWorld( const PlayerTracker & tracker, Bitmap * where, const std::map< int, std::vector< Object * > > & object_z );

	virtual void deleteObjects( std::vector< Object * > * objects );

	virtual void doLogic();

	virtual bool isPlayer( Object * o ) const;

    inline bool shouldTakeScreenshot() const {
        return takeAScreenshot;
    }

    inline void takeScreenshot(){
        takeAScreenshot = true;
    }

    virtual void doTakeScreenshot(Bitmap * work);

    static void * do_load_level(void * arg);

protected:
    std::vector< PlayerTracker > players;
    Object * bang;

    Scene * scene;

    // int min_x;
    int screen_size;
    Filesystem::AbsolutePath path;

    bool draw_minimaps;

    Bitmap * mini_map;

    std::deque<Bitmap*> screenshots;
    bool takeAScreenshot;
    bool is_paused;
    int slowmotion;
    Level::Cacher * cacher;
    int descriptionTime;
    Effects::Gradient * descriptionGradient;
};

#endif
