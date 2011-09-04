#ifndef _game_object_h
#define _game_object_h

#include "util/sound.h"
#include "util/network/network.h"
#include "util/bitmap.h"
#include <string>
#include <vector>

namespace Graphics{
class Bitmap;
}

class ECollide;
class World;

namespace Script{
    class Object;
}

#define MIN_(a,b) ((a) < (b) ? (a) : (b))
#define MAX_(a,b) ((a) > (b) ? (a) : (b))
#define MID_(a,b,c) MAX_(a,MIN_(b,c))

namespace Paintown{

class ObjectAttack;
class Stimulation;

/* generic game object
 * abstract for goodness
 */
class Object{
public:

    typedef unsigned int networkid_t;

	/* Constructors
         * alliance - team number for the object. all objects on the same
         * team cannot hurt each other.
         * x - x position in the world
         * z - z position in the world
         */
	Object( int alliance );
	Object( const int x, const int z, int alliance );

        /* copy constructor */
	Object( const Object & copy );

	/* act:
	 * Performs any logic the object wants to do
	 */
	virtual void act( std::vector< Object * > * others, World * world, std::vector< Object * > * add ) = 0;

	/* draw:
	 * Draws the object onto the specified Bitmap
	 */
	virtual void draw(Graphics::Bitmap * work, int rel_x, int rel_y) = 0;
	virtual void drawFront(Graphics::Bitmap * work, int rel_x);
        /* draw reflection. default behavior is to do nothing */
        virtual void drawReflection(Graphics::Bitmap * work, int rel_x, int rel_y, int intensity);

	/* FIXME: remove this method once mugen characters no longer extend
         * the paintown object heirarchy.
         * draw mugen style shadow. default behavior is to do nothing
         */
        virtual void drawMugenShade(Graphics::Bitmap * work, int rel_x, int intensity, Graphics::Color color, double scale, int fademid, int fadehigh);
	
	/* print:
	 * Print some debugging information
	 */
	virtual void print() const;

	/* weve been grabbed */
	virtual void grabbed( Object * obj ) = 0;
	/* stop grabbing */
	virtual void unGrab() = 0;
	virtual bool isGrabbed() = 0;

	/* copy:
	 * Return an exact copy of this object
	 */
	virtual Object * copy() = 0;

	/* called when this object touches 'obj' regardless of an attack.
	 * usually only called for things that are gettable, like items.
	 */
	virtual void touch( Object * obj );
	
	virtual const std::string getAttackName() = 0;

	/* ZDistance:
	 * Computes the distance in the Z coordinate between two
	 * objects.
	 */
	virtual double ZDistance( const Object * obj );
	virtual double XDistance( const Object * obj );

	/* collision:
	 * Returns true if this collides with obj
	 */
	virtual bool collision( ObjectAttack * obj ) = 0;

	/* collided
	 * You have collided with this object. Make your time.
	 */
	virtual void collided(World * world, ObjectAttack * obj, std::vector< Object * > & objects );

        /* true if the point touches the character.
         * x,y should be absolute coordinates.
         */
        virtual bool touchPoint(int x, int y);

	/* send an arbitrary stimulation to an object */
	virtual void stimulate( const Stimulation & stim );

	/* getCollide
	 * Returns the ECollide object that encompasses this object
	 */
	virtual std::vector<ECollide*> getCollide() const;

	virtual double currentDamage() const {
		return damage;
	}

	virtual void reduceDamage( const double much = 0.09 );

	/* takeDamage
	 * Take some damage and do other things
	 */
	virtual void takeDamage(World & world, ObjectAttack * obj, int x, double forceX, double forceY);

	/* getDamage:
	 * Gets the current amount of damage the object can do
	 */
	virtual int getDamage() const = 0;
        /* get the force for the current attack */
        virtual double getForceX() const = 0;
        virtual double getForceY() const = 0;

	/* isCollidable:
	 * Returns true if this object can collide with other objects
	 */
	virtual bool isCollidable( Object * obj ) = 0;

	virtual bool isGettable() = 0;

        virtual bool isGrabbable(Object * obj) = 0;

	/* isAttacking:
	 * Returns true if this object is performing an attack move
	 */
	virtual bool isAttacking() = 0;
		
	/* getAttackCoords:
	 * Sets x, y to the upper left hand corner of the attacking
	 * bounding box.
	 */
	// virtual void getAttackCoords( int & x, int & y) = 0;

	/* getWidth:
	 * Returns the width of the object
	 */
	virtual int getWidth() const = 0;

	/* getHeight:
	 * Returns the height of the object
	 */
	virtual int getHeight() const = 0;

	/* getRX:
	 * Returns the absolute X position of the object
	 */
	virtual inline int getRX() const {
		return (int) getX();
	}

	virtual inline int getRZ() const {
		return (int) getZ();
	}

	virtual Network::Message getCreateMessage() = 0;

	virtual void interpretMessage(World * world, Network::Message & m );

	virtual Network::Message movedMessage();
	virtual Network::Message collidedMessage();

        /* these are network ids */
	virtual inline void setId(networkid_t id){
		this->id = id;
	}

	virtual inline networkid_t getId() const {
		return this->id;
	}

    /* regular ids
    */
    virtual inline void setObjectId(int id){
        this->objectId = id;
    }

	virtual inline int getObjectId() const {
		return this->objectId;
	}

	/* getRY:
	 * Returns the absolute Y position of the object.
	 * This turns out to be the Z coordinate minus the Y coordinate.
	 * Z specifies how far into the screen the object is, Y is how high the
	 * object is off of the ground
	 */
	virtual inline int getRY() const {
		return (int)(getZ() - getY());
	}

	/* getX:
	 * Returns the X coordinate of the object
	 */
	inline double getX() const {
		return virtualx;
	}

	/* getY:
	 * Returns the Y coordinate of the object
	 * Y is how far the object is from the ground
	 */
	inline double getY() const {
            return virtualy;
	}

	/* getZ:
	 * Returns the Z coordinate of the object
	 * Z is how far the object is into the screen
	 */
	inline double getZ() const {
		return virtualz;
	}

	/* setX:
	 * Set the X coordinate
	 */
	inline void setX( const double x ){
		actualx = (int) x;
		virtualx = x;
	}

	/* setY:
	 * Set the Y coordinate
	 */
	inline void setY( const double y ){
		virtualy = y;
		if ( virtualy < 0 ){
			virtualy = 0;
		}
		actualy = (int) y;
	}

	/* setZ:
	 * Set the Z coordinate
	 */
	inline void setZ( const double z ){
		actualz = (int) z;
		virtualz = z;
	}

	/* this object died. this object can add arbitrary objects
	 * into 'objects'
	 */
	virtual void died( std::vector< Object * > & objects );

	/* set fall velocities */
	virtual void fall( double x_vel, double y_vel );

	/* object has been thrown */
	virtual void thrown();

	void moveRight( const int x );
	void moveLeft( const int x );
        void moveRight(const double x);
        void moveLeft(const double x);

	virtual void moveX( const int x );
	void moveY( const int y );
	void moveZ( const int z );
	void moveX( const double x );
	void moveY( const double y );
	void moveZ( const double z );

        /* for mugen support so we can have negative y values */
        virtual void moveYNoCheck(double y);

	void moveX( int dir, const int x );
        void moveX(int dir, const double x);

	/*
	inline void moveX( const int x ){
		setX( getX() + x );
	}

	inline void moveY( const int y ){
		setY( getY() + y );
	}

	inline void moveZ( const int z ){
		setZ( getZ() + z );
	}

	inline void moveX( const double x ){
		virtualx += x;
		actualx = (int) virtualx;
	}
	
	inline void moveY( const double y ){
		virtualy += y;
		actualy = (int) virtualy;
	}
	
	inline void moveZ( const double z ){
		virtualz += z;
		actualz = (int) virtualz;
	}
	*/

	inline void setHit( const Sound & s ){
		this->hit = s;
	}

	inline void setAlliance( const int x ){
		alliance = x;
	}

	virtual int getAlliance() const;

	/* setFacing:
	 * Sets the direction the object is facing, either left or right.
	 */
	inline virtual void setFacing( const int _f ){
		facing = _f;
		if ( facing != FACING_RIGHT && facing != FACING_LEFT )
			facing = FACING_RIGHT;
	}

	inline void reverseFacing(){
		setFacing( getOppositeFacing() );
	}

	inline int getOppositeFacing() const{
		if ( getFacing() == FACING_LEFT )
			return FACING_RIGHT;
		return FACING_LEFT;
	}

	/* getFacing:
	 * Returns the direction the object is facing.
	 */
	inline int getFacing() const{
		return facing;
	}

	/* getHealth:
	 * Returns the amount of health the object has left.
	 */
	inline virtual int getHealth() const {
		if ( health < 0 )
			return 0;
		return health;
		// return health;
	}

	inline int getMaxHealth() const {
		return max_health;
	}

	/* getLife:
	 * alternate spelling for getHealth()
	 */
	/*
	inline const int getLife() const {
		return getHealth();
		// return health;
	}
	*/

	/* setHealth:
	 * Sets the amount of health the object has
	 */
        inline void setHealth(int h){
            if (h < 0){
                health = 0;
            } else if (h > getMaxHealth()){
                health = getMaxHealth();
            } else {
                health = h;	
            }
        }

	virtual inline void setMaxHealth( int h ){
		max_health = h;
	}

        /* set this object to face obj, basically auto-turn */
	virtual void faceObject(const Object * obj);

        /* turn away from the object -- will be facing the same direction
         * as obj.
         */
	virtual void faceObjectOpposite(const Object * obj);

	/* hurt:
	 * take some damage
	 */
	virtual void hurt( int x );

        virtual void setScriptObject(void * object){
            scriptObject = object;
        }

        virtual void * getScriptObject(){
            return scriptObject;
        }

	/* destructor
	 * Nothing to cleanup.
	 */
	virtual ~Object();

        /* FIXME: make these enums */
	static const int FACING_LEFT = 0;
	static const int FACING_RIGHT = 1;

private:
	int actualx;
	int actualy;
	int actualz;
	double virtualx;
	double virtualy;
	double virtualz;
	int health;
	int max_health;
	double damage;
	
	int facing; // 0 for left, 1 for right

	int alliance;

	/* the sound played when this object gets hit */
	Sound hit;

        /* network id */
	networkid_t id;

        /* this id is assigned by the level */
        int objectId;

        void * scriptObject;
};

}

#endif
