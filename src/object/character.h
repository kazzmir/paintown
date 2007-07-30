#ifndef _character_h
#define _character_h

#include <string>
#include <vector>
#include <map>
#include "object_attack.h"
#include "util/load_exception.h"

class Bitmap;
class Animation;
class ObjectAttack;
class Stimulation;
class World;
class Sound;
class Projectile;

using namespace std;

/* these should be self explanatory.. */
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

class Character: public ObjectAttack {
public:

	/* constructors and destructor */
	Character( int alliance ); 
	Character( const char * filename, int alliance ) throw( LoadException );
	Character( const string & filename, int alliance ) throw ( LoadException );
	Character( const Character & chr ) throw( LoadException );
	
	virtual ~Character();

public:

	virtual void createProjectile( Projectile * projectile );

	/* logic */
	virtual void act( vector< Object * > * others, World * world, vector< Object * > * add );

	/* drawing */
	virtual void draw( Bitmap * work, int rel_x );
	
	// virtual bool collision( Object * obj );

	/* get a mirror image of this character */
	virtual Object * copy();

	/* make the character jump with an x velocity of x_ */
	virtual void doJump( double x_, double z_ );
	
	virtual void stimulate( const Stimulation & stim );

	/* inherited from object */
	virtual void thrown();

	/* you have collided with obj */
	virtual void collided( ObjectAttack * obj, vector< Object * > & objects );
	
	virtual void grabbed( Object * obj );
	virtual void unGrab();
	
	virtual void attacked( Object * something, vector< Object * > & objects );

	/* obj hurt you, take some damage */
	virtual void takeDamage( ObjectAttack * obj, int x );

	/* make the character jump! */
	virtual void jump();

public:
	/* debug methods */

	void print() const;

public:
	/* probers */

	/* am I doing an attacking move? */
	virtual bool isAttacking();

	inline const bool isMoving() const{
		return moving;
	}
	
	virtual const double minZDistance() const;

	/* can I be hit? */
	virtual bool isCollidable( Object * obj );

	/* have I hit something? */
	virtual bool collision( ObjectAttack * obj );

	virtual bool isJumping();
	
	virtual const int getAlliance() const;

public:
	/* setters */
	
	virtual inline void setStatus( const int _status ){
		status = _status;
	}

	virtual const int getToughness() const {
		return toughness;
	}

	virtual const int getRX() const;
	virtual const int getRZ() const;
	virtual const int getRY() const;

	virtual inline void setShadow( int i ){
		shadow = i;
	}

	virtual void setMap( const unsigned int x );
	virtual void nextMap();

	inline void setLink( Object * o ){
		linked = o;
	}

	inline void setThrown( const bool b ){
		thrown_status = b;
	}

	/* should these two methods be private? probably.. */
	inline void setMaxJumpHeight( const int j ){
		max_jump_height = j;
	}
	
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

	inline void initJumpingYVelocity(){
		setYVelocity( jumping_starting_velocity );
	}
		
	void decreaseYVelocity();
	void decreaseXVelocity();
	void decreaseZVelocity();

public:
	/* for testing purposes only */
	virtual bool testAnimation();
	virtual void testAnimation( unsigned int x );
	virtual void testAnimation( string name );
	void testReset();


public:
	/* getters */

	/* how much damage I do */
	virtual int getDamage() const;

	/* collision detection object */
	virtual ECollide * getCollide() const;

	inline Object * getLink(){
		return linked;
	}

	inline const bool getThrown() const{
		return thrown_status;
	}

	inline const map< int, map<string,Animation*> > getMapper() const {
		return mapper;
	}

	virtual bool isGettable();
	virtual Animation * getCurrentMovement();
	virtual Animation * getMovement( const string & str );
	virtual Animation * getMovement( const unsigned int x );
	virtual const map<string,Animation*> & getMovements();

	virtual inline const int getShadow() const {
		return shadow;
	}
	virtual void getAttackCoords( int & x, int & y);
	const string getName() const;
	void setName( const string & str );

	inline void setSpeed( const double s ){
		speed = s;
	}

	inline void setDeath( int i ){
		death = i;
	}

	inline const double getSpeed() const {
		return speed;
	}

	virtual const int getWidth() const;
	virtual const int getHeight() const;
	virtual const string & getAttackName();
	virtual inline const int getStatus() const {
		return status;
	}

	inline const double getMaxJumpHeight() const{
		return max_jump_height;
	}

	inline const double getMaxJumpingVelocity() const {
		return jumping_starting_velocity;
	}

	inline const double getYVelocity() const {
		return y_velocity;
	}
	
	inline const double getXVelocity() const {
		return x_velocity;
	}

	inline const double getZVelocity() const {
		return z_velocity;
	}

protected:
	
	const int getShadowX();
	const int getShadowY();

	virtual void filterEnemies( vector< Object * > & mine, vector< Object * > * all );
	virtual void reMap( const string & from, const string & to, int id );
	void loadSelf( const char * filename ) throw ( LoadException );
	bool realCollision( ObjectAttack * obj );

	virtual void landed( World * world );

	virtual void fall( double x_vel, double y_vel );

	void drawLifeBar( int x, int y, int he, Bitmap * work );
	
	virtual ECollide * getNormalCollide() const;

	/* helper functions */

	// void parseObject( string object_string ) throw( exception );
	void upperCase( string & who );

	inline const int getInvincibility() const {
		return invincibility;
	}

	inline void setInvincibility( const int x ){
		invincibility = x;
	}

protected:
	string name;
	// int speed;
	int type;
	int shadow;
	Bitmap * icon;

	// int actualx, actualy;

	/* last_obj: last object to attack us */
	// Object * last_obj;

	/* last_collide: keep track of last animation ticket that hurt us */
	// unsigned long last_collide;
	map< Object *, unsigned long > collision_objects;

	/* map from name of animation to animation */
	// map<string,Animation *> movements;

	/* current animation */
	Animation * animation_current;

	bool own_stuff;

/* specific to character */
private:
	
	int max_jump_height;
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
	map< int, map<string,Animation*> > mapper;
	vector< Object * > projectiles;
	
	Sound * die_sound;
	Sound * landed_sound;
	int invincibility;
	int toughness;
};

#endif
