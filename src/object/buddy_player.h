#ifndef _buddy_player_h
#define _buddy_player_h

#include "character.h" 
#include "util/keyboard.h"
#include <deque>
#include "util/load_exception.h"

class Animation;
class World;

using namespace std;

class BuddyPlayer: public Character{
public:

	BuddyPlayer( const Character & chr ) throw( LoadException );
	
	/* drawing */
	virtual void draw( Bitmap * work, int rel_x );

	using Character::drawLifeBar;
	virtual void drawLifeBar( int x, int y, Bitmap * work );
	
	virtual Object * copy();
	
	virtual void act( vector< Object * > * others, World * world, vector< Object * > * add );
	
	virtual void takeDamage( World * world, ObjectAttack * obj, int x );
	
	virtual void hurt( int x );

	virtual inline void setExplode( bool b ){
		/* */
	}

	virtual bool getExplode(){
		return false;
	}

	inline const void setInvincible( const bool b ){
		this->invincible = b;
	}

	inline const bool isInvincible() const {
		return this->invincible;
	}

protected:
	bool canGrab( Object * enemy );
	void grabEnemy( Object * enemy );

	const Object * findClosest( const vector< Object * > & enemies );
	
protected:

	/* store key presses in a stack with two ends*/
	int acts;
	int show_life;
	int name_id;
	bool invincible;
	int want_x, want_z;
};

#endif
