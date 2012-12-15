#ifndef _paintown_buddy_player_h
#define _paintown_buddy_player_h

#include "character.h" 
#include "player-common.h"
#include <deque>
#include <vector>
#include "util/thread.h"

class World;

namespace Paintown{

class Player;
class Animation;

class BuddyPlayer: public PlayerCommon {
public:

    BuddyPlayer( const Character * leader, const Character & chr );

    /* drawing */
    virtual void draw( Graphics::Bitmap * work, int rel_x, int rel_y );

    using Character::drawLifeBar;
    virtual void drawLifeBar( int x, int y, Graphics::Bitmap * work );

    virtual Object * copy();

    virtual void act( std::vector< Object * > * others, World * world, std::vector< Object * > * add );

    // virtual void takeDamage( World & world, ObjectAttack * obj, int x );

    virtual void hurt( int x );

    virtual inline void setExplode( bool b ){
        /* */
    }
    
    virtual void resetInput();
    virtual bool isPlayer() const;

    virtual bool getExplode(){
        return false;
    }

    virtual int spawnTime();

    virtual void deathReset();

    inline void setInvincible( const bool b ){
        this->invincible = b;
    }

    inline bool isInvincible() const {
        return this->invincible;
    }

protected:
    bool canGrab( Object * enemy );
    void grabEnemy( Object * enemy );

    const Object * findClosest( const std::vector< Object * > & enemies );

protected:

    /* store key presses in a stack with two ends*/
    int acts;
    int show_life;
    int name_id;
    bool invincible;
    int want_x, want_z;
    int spawn_time;
    const Character * leader;
};

class BuddyFuture: public Util::Future<Object*> {
public:
    BuddyFuture(const Filesystem::AbsolutePath & path, Util::Future<Object*> * playerFuture, int remap, int id);

    typedef Util::Future<Object*> super;

    virtual ~BuddyFuture();

protected:
    virtual void compute();

    const Filesystem::AbsolutePath path;
    Util::Future<Object*> * playerFuture;
    int remap;
    int id;
    Player * base;
};

}

#endif
