#ifndef _character_h
#define _character_h

#include <string>
#include <vector>
#include <map>
#include "util/bitmap.h"
#include "object_attack.h"
#include "util/load_exception.h"
#include "util/file-system.h"
#include "util/pointer.h"
#include "util/network/network.h"

namespace Graphics{
class Bitmap;
}
class Stimulation;
class World;
class Sound;

namespace Script{
    class Character;
}

namespace Paintown{

class Animation;
class AnimationTrail;
class ObjectAttack;
class Projectile;
class DrawEffect;

/* used for gibs */
struct BodyPart{
	BodyPart( int x, int y, Graphics::Bitmap * image ):x(x), y(y), image(image){}
	/* relative to getX()/getY() */
	int x, y;
        Graphics::Bitmap * image;
};

/* these should be self explanatory.. */
/* TODO: make these an enum probably */
const int Status_Ground = 0; /* normal status, on the ground */
const int Status_Jumping = 1; /* jumping */
const int Status_Grab = 2; /* grabbing another character */
const int Status_Grabbed = 3; /* this character is being grabbed */
const int Status_Hurt = 4; /* just got hurt */
const int Status_Rise = 5; /* getting up from ground */
const int Status_Fell = 6; /* fell down due to damage */
const int Status_Dead = 7; /* dying */
const int Status_Get = 8; /* getting an object */
const int Status_Falling = 9; /* falling due to lack of ground beneath them */

class Character;

/* Handles palette swaps */
class Remap: public Graphics::Bitmap::Filter {
public:
    Remap(const Filesystem::RelativePath & from, const Filesystem::RelativePath & to);
    Remap(const Remap & copy);

    virtual ~Remap();
            
    virtual Graphics::Color filter(Graphics::Color color) const;

    virtual inline const Filesystem::RelativePath & getFrom() const {
        return remapFrom;
    }
    
    virtual inline const Filesystem::RelativePath & getTo() const {
        return remapTo;
    }

protected:
    std::map<Graphics::Color, Graphics::Color> computeRemapColors(const Filesystem::RelativePath & from, const Filesystem::RelativePath & to);


    /*
    virtual void setAnimation(const std::string & name, Animation * animation);
    virtual Animation * getAnimation(const std::string & name);
    virtual const inline std::map<std::string, Animation*> & getAnimations() const {
        return mapper;
    }
    virtual void use(Character * from);
protected:
    virtual void doRemap(Character * from);

    std::map<std::string, Animation*> mapper;
    bool needRemap;
    Remap * original;
    */
    Filesystem::RelativePath remapFrom;
    Filesystem::RelativePath remapTo;
    std::map<Graphics::Color, Graphics::Color> colors;
};

class Character: public ObjectAttack {
public:
    /* constructors and destructor */
    Character( int alliance ); 
    Character( const char * filename, int alliance );
    Character(const Filesystem::AbsolutePath & filename, int alliance );
    Character( const Character & chr );

    virtual ~Character();

public:

    virtual void createProjectile( Projectile * projectile );

    /* logic */
    virtual void act( std::vector< Object * > * others, World * world, std::vector< Object * > * add );

    /* drawing */
    virtual void draw( Graphics::Bitmap * work, int rel_x, int rel_y );

    /* intensity is the amount of alpha blending to do.
     * 0 = translucent
     * 255 = opaque
     */
    virtual void drawReflection(Graphics::Bitmap * work, int rel_x, int rel_y, int intensity);

    /* draw the character in a solid color. red, green, blue, intensity change
     * what color the pixels are drawn.
     * 0, 0, 0, 255 will draw all non-masking pixels as black
     */
    virtual void drawOutline(Graphics::Bitmap * work, int rel_x, int rel_y, int red, int green, int blue, int intensity);

    virtual void drawShade(Graphics::Bitmap * work, int rel_x, int intensity, Graphics::Color color, double scale, int fademid, int fadehigh);
    virtual void drawMugenShade(Graphics::Bitmap * work, int rel_x, int intensity, Graphics::Color color, double scale, int fademid, int fadehigh);

    virtual const Graphics::Bitmap * getCurrentFrame() const;

    // virtual bool collision( Object * obj );

    /* get a mirror image of this character */
    virtual Object * copy();

    /* make the character jump with an x velocity of x_ */
    virtual void doJump( double x_, double z_ );

    /* implements the visitor OOP pattern:
     * stimulation calls stimulate on character, then character
     * calls stimulate on stimulation with the proper type
     */
    virtual void stimulate( const Stimulation & stim );

    virtual bool touchPoint(int x, int y);

    /* inherited from object */
    virtual void thrown();

    /* you have collided with obj */
    virtual void collided(World * world, ObjectAttack * obj, std::vector< Object * > & objects );

    inline virtual void setFacing( const int f ){
        if ( getStatus() == Status_Ground || getStatus() == Status_Grabbed ){
            Object::setFacing( f );
        }
    }

    virtual bool isGrabbed();

    virtual void grabbed( Object * obj );
    virtual void unGrab();

    virtual void attacked( World * world, Object * something, std::vector< Object * > & objects );

    /* obj hurt you, take some damage */
    virtual void takeDamage( World & world, ObjectAttack * obj, int x, double forceX, double forceY );

    /* make the character jump! */
    virtual void jump();

    virtual void died( std::vector< Object * > & objects );

public:
    /* debug methods */

    void print() const;

public:
    /* probers */

    /* am I doing an attacking move? */
    virtual bool isAttacking();

    inline bool isMoving() const {
        return moving;
    }

    virtual void setSpeedBoost(double boost, int ticks);

    virtual double minZDistance() const;

    /* can I be hit? */
    virtual bool isCollidable( Object * obj );

    /* have I hit something? */
    virtual bool collision( ObjectAttack * obj );

    virtual bool isGrabbable(Object * obj);

    virtual bool isJumping();

    virtual int getAlliance() const;

public:
    /* setters */

    virtual inline void setStatus( const int _status ){
        status = _status;
    }

    virtual int getToughness() const {
        return toughness;
    }

    virtual int getRX() const;
    virtual int getRZ() const;
    virtual int getRY() const;

    virtual inline void setShadow( int i ){
        shadow = i;
    }

    virtual void setMap( const unsigned int x );
    virtual void nextMap();
    virtual void nextMap( int x );
    virtual int getNextMap() const;
    virtual int getNextMap( unsigned int x ) const;

    inline void setLink( Object * o ){
        linked = o;
    }

    inline void setThrown( const bool b ){
        thrown_status = b;
    }

    /* should these two methods be private? probably.. */
    /*
       inline void setMaxJumpHeight( const int j ){
       max_jump_height = j;
       }
       */

    inline void setXVelocity( const double j ){
        moving = true;
        x_velocity = j;
    }

    inline void setZVelocity( const double j ){
        moving = true;
        z_velocity = j;
    }

    inline void setYVelocity( const double j ){
        moving = true;
        y_velocity = j;
    }

    inline void setMoving( const bool x ){
        if ( x == false ){
            setXVelocity( 0 );
            setYVelocity( 0 );
            setZVelocity( 0 );
        }
        moving = x;
    }

    inline void setJumpingYVelocity( const double j ){
        jumping_starting_velocity = j;
    }

    virtual void initJumpingYVelocity();

    void decreaseYVelocity();
    void decreaseXVelocity();
    void decreaseZVelocity();

    virtual void setTrails(const int produce, const int life);

public:
    /* for testing purposes only */
    virtual bool testAnimation();
    virtual void testAnimation( unsigned int x );
    virtual void testAnimation( std::string name );
    void testReset();


public:
    /* getters */

    /* how much damage I do */
    virtual int getDamage() const;
    virtual double getForceX() const;
    virtual double getForceY() const;

    /* collision detection object */
    virtual std::vector<ECollide*> getCollide() const;

    inline Object * getLink(){
        return linked;
    }

    inline bool getThrown() const{
        return thrown_status;
    }

    virtual Remap * getCurrentRemap() const;

    inline const std::map< int, Remap*> & getMapper() const {
        return mapper;
    }

    virtual bool isGettable();
    virtual Util::ReferenceCount<Animation> getCurrentMovement() const;
    virtual void setMovement( Animation * animation, const std::string & name );
    virtual Util::ReferenceCount<Animation> getMovement( const std::string & str );
    // virtual Animation * getMovement( const unsigned int x );
    virtual const std::map<std::string, Util::ReferenceCount<Animation> > & getMovements();

    virtual inline int getShadow() const {
        return shadow;
    }
    // virtual void getAttackCoords( int & x, int & y);
    const std::string getName() const;
    void setName( const std::string & str );

    inline void setSpeed( const double s ){
        speed = s;
    }

    inline void setDeath( int i ){
        death = i;
    }

    inline virtual int getDeath() const {
        return death;
    }

    virtual double getSpeed() const;

    inline virtual unsigned int getCurrentMap() const {
        return current_map;
    }

    virtual int getWidth() const;
    virtual int getHeight() const;
    virtual const std::string getAttackName();
    virtual inline int getStatus() const {
        return status;
    }

    /*
       inline const double getMaxJumpHeight() const{
       return max_jump_height;
       }
       */

    inline double getMaxJumpingVelocity() const {
        return jumping_starting_velocity;
    }

    inline double getYVelocity() const {
        return y_velocity;
    }

    inline double getXVelocity() const {
        return x_velocity;
    }

    inline double getZVelocity() const {
        return z_velocity;
    }

    virtual void drawLifeBar( int x, int y, Graphics::Bitmap * work );

    virtual inline const Filesystem::AbsolutePath & getPath() const {
        return path;
    }

    virtual int spawnTime(){
        return 0;
    }

    virtual double getSpriteScale() const;
    virtual void setSpriteScale(double scale);
    
    /* true for human controlled characters */
    virtual bool isPlayer() const;

    virtual void deathReset();

    virtual inline int getLives(){
        return lives;
    }

    inline void setLives( const int x ){
        lives = x;
    }

    virtual inline bool drawShadow() const {
        return draw_shadow;
    }

    virtual inline void setDrawShadow( bool b ){
        draw_shadow = b;
    }

    virtual Network::Message getCreateMessage();
    virtual Network::Message movedMessage();
    virtual Network::Message explodeMessage();
    virtual Network::Message healthMessage();
    virtual Network::Message ungrabMessage();
    virtual Network::Message animationMessage();
    virtual Network::Message showNameMessage( int amount );
    virtual Network::Message fallMessage( double x, double y );
    virtual Network::Message grabMessage(Object::networkid_t from, Object::networkid_t who);
    virtual Network::Message nameMessage() const;

    virtual void interpretMessage(World * world, Network::Message & m );

    virtual void fall( double x_vel, double y_vel );

    /* Invincibility makes the player glow and is usually given to the player
     * through picking up an item.
     */
    virtual int getInvincibility() const;
    virtual void setInvincibility(const int x);

    virtual void addEffect(DrawEffect * effect);

    virtual bool showDebugging() const;

protected:

    virtual Network::Message jumpMessage( double x, double y );

    int getShadowX();
    int getShadowY();

    virtual void filterEnemies( std::vector< Object * > & mine, std::vector< Object * > * all );
    // virtual void reMap( const std::string & from, const std::string & to, int id );
    virtual void addRemap(Remap * remap);
    /* true if a mapping between `from' and `to' doesn't already exist */
    virtual bool newRemap(const std::string & from, const std::string & to);
    void loadSelf(const Filesystem::AbsolutePath & filename );
    bool realCollision( ObjectAttack * obj );

    std::vector< BodyPart > getBodyParts(Util::ReferenceCount<Animation> animation );

    virtual void landed( World * world );

    virtual void drawLifeBar( int x, int y, int he, Graphics::Bitmap * work );

    virtual std::vector<ECollide*> getNormalCollide() const;

    /* helper functions */

    // void parseObject( string object_string ) throw( exception );
    void upperCase( std::string & who );


    virtual inline void setExplode( bool b ){
        explode = b;
    }

    virtual bool getExplode();

protected:
    std::string name;
    // int speed;
    int type;
    int shadow;
    Graphics::Bitmap * icon;

    // int actualx, actualy;

    /* last_obj: last object to attack us */
    // Object * last_obj;

    /* last_collide: keep track of last animation ticket that hurt us */
    // unsigned long last_collide;
    std::map< Object *, unsigned long > collision_objects;

private:
    /* map from name of animation to animation */
    std::map<std::string, Util::ReferenceCount<Animation> > movements;

protected:

    /* current animation */
    Util::ReferenceCount<Animation> animation_current;

    bool own_stuff;

    /* specific to character */
protected:

    // int max_jump_height;
    double x_velocity;
    double y_velocity;
    double z_velocity;

    /*
       double jumping_x; // velocity in the X direction
       double jumping_velocity; // velocity in the Y direction
       */
    double jumping_starting_velocity;

    /* status: jumping or on the ground or anything else */
    int status;

    /* when health reaches 0, death is set to a number such that the character blinks
     * for a little while before going away
     */
    int death;

    /* can be linked to another object, in some way */
    /* Total time this object has been grabbed */
    int grab_time;

    /* being thrown or not */
    bool thrown_status;

    double speed;

    /* object this is linked to */
    Object * linked;

    /* object that threw this */
    Object * thrown_from;

    bool moving;

    unsigned int current_map;
    /* map from id to map of animations */
    // std::map< int, std::map<std::string, Animation*> > mapper;
    std::map<int, Remap*> mapper;
    std::vector< Object * > projectiles;
    std::vector< BodyPart > body_parts;

    Sound * die_sound;
    Sound * landed_sound;
    Sound * squish_sound;
    int invincibility;
    int toughness;
    bool explode;
    int lives;

    bool draw_shadow;

    Filesystem::AbsolutePath path;

    /* list of faded animations that act as a trail */
    std::vector<AnimationTrail*> trails;
    /* starting time before a trail is produced, 0 means don't produce any */
    int trail_generator;
    /* countdown before a trail is produced */
    int trail_counter;
    /* lifetime for trails */
    int trail_life;

    std::vector<DrawEffect*> effects;
    Util::ReferenceCount<Graphics::Bitmap> gibBloodImage;

    struct SpeedBoost{
        SpeedBoost():
        boost(0),
        ticks(0){
        }

        double boost;
        int ticks;
    } speedBoost;

    /* scale the size of the images (and all related events like attack boxes) */
    double spriteScale;
};

}

#endif
