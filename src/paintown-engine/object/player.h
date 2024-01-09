#ifndef _paintown_player_h
#define _paintown_player_h

#include "character.h" 
#include "player-common.h"
#include <deque>
#include "r-tech1/network/network.h"
#include "r-tech1/input/input.h"
#include "r-tech1/file-system.h"
#include "r-tech1/pointer.h"
#include "r-tech1/thread.h"
#include "r-tech1/graphics/bitmap.h"
#include "../game/adventure_world.h"

class World;
class InputSource;

namespace Paintown{

class DrawGlowEffect;

class Animation;

class Player: public PlayerCommon {
public:

    Player(const char * filename);
    Player(const Filesystem::AbsolutePath & str, Util::ReferenceCount<InputSource> source);
    Player(const Player & pl);
    Player(const Character & chr);

    /* drawing */
    virtual void drawFront(const Graphics::Bitmap & work, int rel_x);

    using Character::drawLifeBar;
    virtual void drawLifeBar(int x, int y, const Graphics::Bitmap & work);

    virtual Object * copy();

    virtual void act(std::vector< Object * > * others, World * world, std::vector< Object * > * add);

    virtual void takeDamage(World & world, ObjectAttack * obj, int x, double forceX, double forceY);

    virtual void hurt(int x);

    virtual void attacked(World * world, Object * something, std::vector< Object * > & objects);

    virtual inline void setExplode(bool b){
        /* */
    }
    
    virtual void stimulate(const Stimulation & stim);
    
    virtual const Filesystem::RelativePath & getIntroFile();
    virtual const Filesystem::RelativePath & getEndingFile();

    virtual bool getExplode(){
        return false;
    }

    virtual void loseLife(int l = 1);
    virtual void gainLife(int l = 1);

    /* player has their own idea of invincibility because the user might
     * have set that as an option.
     */
    virtual void setInvincible(const bool b);
    virtual int getInvincibility() const;

    inline bool isInvincible() const {
        return this->invincible;
    }

    virtual void interpretMessage(World * world, Network::Message & m);

    /* reset some stuff when the player dies */
    virtual void deathReset();

    virtual Network::Message getCreateMessage();

    virtual inline void ignoreLives(){
        ignore_lives = true;
    }

    virtual inline bool ignoringLives() const {
        return ignore_lives;
    }

    virtual const InputSource & getInput() const;

    /* binds this object to 'player' so that they appear on the screen together */
    virtual void bindTo(Player * player);
    virtual std::vector<Player*> getBinds() const;
    
    virtual void resetInput();
    virtual bool isPlayer() const;
    
    virtual ~Player();

protected:
    Network::Message scoreMessage();

    /* more initialization called by all the constructors */
    void commonInitialize();
    void load(const Filesystem::AbsolutePath & path);

    std::map<Util::ReferenceCount<Animation>, int> possibleMovements(const std::string & name);
    Util::ReferenceCount<Animation> chooseLikelyAnimation(std::vector<Object*> * others, std::map<Util::ReferenceCount<Animation>, int> possible_animations, const std::string & current_name);

    void handleThrow(World * world);
    void handleGrab(World * world, std::vector<Object*> * others);

    std::vector<Input::PaintownInput> fillKeyCache();
    void debugDumpKeyCache(int level);
    const char * keyToName(Input::PaintownInput key);
    bool combo(Util::ReferenceCount<Animation> ani);
    bool combo(Util::ReferenceCount<Animation> ani, std::deque<Input::PaintownInput>::iterator cache_cur_key, std::deque<Input::PaintownInput>::iterator end );
    /*
    virtual int getKey( Input::PaintownInput x, int facing );
    virtual int getKey( Input::PaintownInput x );
    */

    bool canGrab(Object * enemy);
    void grabEnemy(Object * enemy);

    /* shows keys on the screen */
    void drawButtons(const Graphics::Bitmap & work, int x, int y);

    Network::Message thrownMessage(unsigned int id);

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
    bool ignore_lives;

    Util::ReferenceCount<InputSource> source;
    
    /* players this object is bound to */
    std::vector<Player*> binds;

    Filesystem::RelativePath intro;
    Filesystem::RelativePath ending;

    DrawGlowEffect * invincibilityEffect;
};

class PlayerFuture: public Util::Future<Object*> {
public:
    PlayerFuture(const Filesystem::AbsolutePath & path, bool invincible, int lives, int remap, Util::ReferenceCount<InputSource> source);

    typedef Util::Future<Object*> super;

    virtual ~PlayerFuture();

protected:
    virtual void compute();

    const Filesystem::AbsolutePath path;
    bool invincible;
    int lives;
    int remap;
    Util::ReferenceCount<InputSource> source;
};

}

#endif
