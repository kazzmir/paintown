#ifndef _paintown_player_h
#define _paintown_player_h

#include "character.h" 
#include "player-common.h"
#include <deque>
#include "util/load_exception.h"
#include "util/network/network.h"
#include "util/input/input.h"
#include "../game/adventure_world.h"
#include "util/thread.h"
#include "util/bitmap.h"

class World;

namespace Paintown{

class Animation;

class Player: public PlayerCommon {
public:

    Player( const char * filename, int config = 0 );
    Player( const Filesystem::AbsolutePath & str, int config = 0 );
    Player( const Player & pl );
    Player( const Character & chr );

    /* drawing */
    virtual void drawFront( Graphics::Bitmap * work, int rel_x );

    using Character::drawLifeBar;
    virtual void drawLifeBar( int x, int y, Graphics::Bitmap * work );

    virtual Object * copy();

    virtual void act( std::vector< Object * > * others, World * world, std::vector< Object * > * add );

    virtual void takeDamage(World & world, ObjectAttack * obj, int x, double forceX, double forceY);

    virtual void hurt( int x );

    virtual void attacked( World * world, Object * something, std::vector< Object * > & objects );

    virtual inline void setExplode( bool b ){
        /* */
    }

    virtual bool getExplode(){
        return false;
    }

    virtual void loseLife( int l = 1 );
    virtual void gainLife( int l = 1 );

    virtual void setInvincible(const bool b);
    virtual int getInvincibility() const;

    inline bool isInvincible() const {
        return this->invincible;
    }

    virtual void interpretMessage(World * world, Network::Message & m );

    /* reset some stuff when the player dies */
    virtual void deathReset();

    virtual Network::Message getCreateMessage();

    virtual inline void setConfig(int config){
        this->config = config;
    }

    virtual int getConfig() const {
        return this->config;
    }

    virtual inline void ignoreLives(){
        ignore_lives = true;
    }

    virtual inline bool ignoringLives() const {
        return ignore_lives;
    }
    
    virtual void resetInput();
    virtual bool isPlayer() const;
    
    virtual ~Player();

protected:
    Network::Message scoreMessage();

    /* more initialization called by all the constructors */
    void commonInitialize();

    std::vector<Input::PaintownInput> fillKeyCache();
    void debugDumpKeyCache(int level);
    const char * keyToName(Input::PaintownInput key);
    bool combo(Util::ReferenceCount<Animation> ani);
    bool combo(Util::ReferenceCount<Animation> ani, std::deque<Input::PaintownInput>::iterator cache_cur_key, std::deque<Input::PaintownInput>::iterator end );
    virtual int getKey( Input::PaintownInput x, int facing );
    virtual int getKey( Input::PaintownInput x );

    bool canGrab( Object * enemy );
    void grabEnemy( Object * enemy );

    /* shows keys on the screen */
    void drawButtons(Graphics::Bitmap * work, int x, int y);

    Network::Message thrownMessage( unsigned int id );

protected:

    struct Held{
        Held():
        up(false),
        down(false),
        left(false),
        right(false){
        }

        bool up;
        bool down;
        bool left;
        bool right;
    };

    /* store key presses in a stack with two ends*/
    std::deque<Input::PaintownInput> key_cache;
    std::map< Input::PaintownInput, bool > last_key;
    Held keyHold;
    int acts;
    int show_life;

    int name_id;
    double attack_bonus;
    static const int num_attack_gradient = 50;
    Graphics::Color attack_gradient[num_attack_gradient];

    bool invincible;
    int config;
    bool ignore_lives;

    // int last_key;
};

class PlayerFuture: public Util::Future<Object*> {
public:
    PlayerFuture(const Filesystem::AbsolutePath & path, bool invincible, int lives, int remap);

    typedef Util::Future<Object*> super;

    virtual ~PlayerFuture();

protected:
    virtual void compute();

    const Filesystem::AbsolutePath path;
    bool invincible;
    int lives;
    int remap;
};

}

#endif
