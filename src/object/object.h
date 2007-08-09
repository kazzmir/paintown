#ifndef _game_object_h
#define _game_object_h

class Bitmap;
class ECollide;
class ObjectAttack;
class World;
class Stimulation;

#include "util/sound.h"
#include <string>
#include <vector>

#define MIN_(a,b) ((a) < (b) ? (a) : (b))
#define MAX_(a,b) ((a) > (b) ? (a) : (b))
#define MID_(a,b,c) MAX_(a,MIN_(b,c))

// using namespace std;

/* generic game object
 * abstract for goodness
 */
class Object{
public:

	/* Constructors */
	Object( int alliance );
	Object( const int x, const int y, int alliance );
	Object( const Object & copy );

	/* act:
	 * Performs any logic the object wants to do
	 */
	virtual void act( vector< Object * > * others, World * world, vector< Object * > * add ) = 0;

	/* draw:
	 * Draws the object onto the specified Bitmap
	 */
	virtual void draw( Bitmap * work, int rel_x ) = 0;

	/* print:
	 * Print some debugging information
	 */
	virtual void print() const;

	/* weve been grabbed */
	virtual void grabbed( Object * obj ) = 0;
	/* stop grabbing */
	virtual void unGrab() = 0;

	/* copy:
	 * Return an exact copy of this object
	 */
	virtual Object * copy() = 0;

	/* called when this object touches 'obj' regardless of an attack.
	 * usually only called for things that are gettable, like items.
	 */
	virtual void touch( Object * obj );
	
	virtual const string & getAttackName() = 0;

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
	virtual void collided( ObjectAttack * obj, vector< Object * > & objects );

	/* send an arbitrary stimulation to an object */
	virtual void stimulate( const Stimulation & stim );

	/* getCollide
	 * Returns the ECollide object that encompasses this object
	 */
	virtual ECollide * getCollide() const;

	virtual const double currentDamage() const {
		return damage;
	}

	virtual void reduceDamage( const double much = 0.09 );

	/* takeDamage
	 * Take some damage and do other things
	 */
	virtual void takeDamage( ObjectAttack * obj, int x );

	/* getDamage:
	 * Gets the current amount of damage the object can do
	 */
	virtual int getDamage() const = 0;

	/* isCollidable:
	 * Returns true if this object can collide with other objects
	 */
	virtual bool isCollidable( Object * obj ) = 0;

	virtual bool isGettable() = 0;

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
	virtual const int getWidth() const = 0;

	/* getHeight:
	 * Returns the height of the object
	 */
	virtual const int getHeight() const = 0;

	/* getRX:
	 * Returns the absolute X position of the object
	 */
	virtual inline const int getRX() const {
		return (int) getX();
	}

	virtual inline const int getRZ() const {
		return (int) getZ();
	}

	/* getRY:
	 * Returns the absolute Y position of the object.
	 * This turns out to be the Z coordinate minus the Y coordinate.
	 * Z specifies how far into the screen the object is, Y is how high the
	 * object is off of the ground
	 */
	virtual inline const int getRY() const {
		return (int)(getZ() - getY());
	}

	/* getX:
	 * Returns the X coordinate of the object
	 */
	inline const double getX() const {
		return virtualx;
	}

	/* getY:
	 * Returns the Y coordinate of the object
	 * Y is how far the object is from the ground
	 */
	inline const double getY() const {
		return virtualy;
	}

	/* getZ:
	 * Returns the Z coordinate of the object
	 * Z is how far the object is into the screen
	 */
	inline const double getZ() const {
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
	virtual void died( vector< Object * > & objects );

	/* set fall velocities */
	virtual void fall( double x_vel, double y_vel );

	/* object has been thrown */
	virtual void thrown();

	void moveRight( const int x );
	void moveLeft( const int x );

	void moveX( const int x );
	void moveY( const int y );
	void moveZ( const int z );
	void moveX( const double x );
	void moveY( const double y );
	void moveZ( const double z );

	void moveX( int dir, const int x );

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

	virtual const int getAlliance() const;

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

	inline const int getOppositeFacing() const{
		if ( getFacing() == FACING_LEFT )
			return FACING_RIGHT;
		return FACING_LEFT;
	}

	/* getFacing:
	 * Returns the direction the object is facing.
	 */
	inline const int getFacing() const{
		return facing;
	}

	/* getHealth:
	 * Returns the amount of health the object has left.
	 */
	inline virtual const int getHealth() const {
		if ( health < 0 )
			return 0;
		return health;
		// return health;
	}

	inline const int getMaxHealth() const {
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
	inline void setHealth( int h ){
		health = MID_( 0, h, getMaxHealth() );
	}

	virtual inline void setMaxHealth( int h ){
		max_health = h;
	}

	virtual void faceObject( const Object * o );

	/* hurt:
	 * take some damage
	 */
	virtual void hurt( int x );

	/* destructor
	 * Nothing to cleanup.
	 */
	virtual ~Object();

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

};

#endif
