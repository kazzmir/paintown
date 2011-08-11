#ifndef _paintown_scene_h
#define _paintown_scene_h

#include <string>
#include <vector>
#include <map>
#include <deque>
#include "util/load_exception.h"

namespace Graphics{
class Bitmap;
}

class Block;
class Heart;
class Atmosphere;
class Trigger;

namespace Paintown{
    class Enemy;
    class Object;
}

namespace Level{
    class Cacher;
}

struct Panel{
    Graphics::Bitmap * pic;
    Graphics::Bitmap * neon;
    Graphics::Bitmap * screen_overlay;

	Panel( Graphics::Bitmap * _pic = NULL, Graphics::Bitmap * _neon = NULL, Graphics::Bitmap * my_screen = NULL );
	~Panel();
};

class Scene{
public:
    Scene(const char * filename, const Level::Cacher & cacher);

    // void Draw( int x, Bitmap * work );
    void drawFront( int x, Graphics::Bitmap * work );
    void drawBack( int x, Graphics::Bitmap * work );

    int getFinished() const;

    inline std::string getDescription() const {
        return description;
    }

    inline int getMinimumZ() const {
        return minimum_z;
    }

    inline int getMaximumZ() const {
        return maximum_z;
    }

    inline int getBlock() const {
        return blockNumber;
    }

    inline Block * currentBlock() const {
        return current_block;
    }

    void startMusic();

    void addEnemy(Paintown::Enemy * const obj);

    virtual void advanceBlocks( int n );

    inline const std::vector<Atmosphere*> & getAtmospheres() const {
        return atmospheres;
    }

    int totalLength() const;

    int getLimit();
    void act( int min_x, int max_x, std::vector< Paintown::Object * > * objects );

    virtual ~Scene();

protected:

    /* true if the scene the next block is ready */
    bool canContinue( int x );

    inline double getBackgroundParallax() const {
        return backgroundParallax;
    }

    inline void setBackgroundParallax( double b ){
        backgroundParallax = b;
    }

    inline double getForegroundParallax() const {
        return foregroundParallax;
    }

    inline void setForegroundParallax( double b ){
        foregroundParallax = b;
    }

    inline void setMinimumZ( const int z ){
        minimum_z = z;
    }

    inline void setMaximumZ( const int z ){
        maximum_z = z;
    }

    void calculateLength();

    /* erase dead hearts */
    void clearHearts();

    /* try to execute the triggers */
    void doTriggers();

protected:
    Graphics::Bitmap * background;
    Graphics::Bitmap * arrow;
    int arrow_blink;
    std::vector< int > order;
    // vector< Panel * > front_panels;
    // vector< Graphics::Bitmap * > back_panels;
    std::vector< Graphics::Bitmap * > front_panels;
    std::map< int, Panel * > panels;

    std::deque< Block * > level_blocks;
    std::vector<Block*> old_level_blocks;

    std::vector< Heart * > hearts;
    std::vector<Paintown::Object*> added_objects;

    int scene_length;

    /* how far the scene has progressed */
    int block_length;

    /* minimum/maximum values characters can move within the z-plane */
    int minimum_z, maximum_z;

    Block * current_block;

    /* count of how many blocks have gone by */
    int blockNumber;

    double backgroundParallax;
    double foregroundParallax;

    Graphics::Bitmap * frontBuffer;

    std::vector<Atmosphere*> atmospheres;
    std::vector<Trigger*> triggers;
    std::vector<std::string> music;
    bool hasMusic;
    std::string description;
};

#endif
