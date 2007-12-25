#ifndef _paintown_player_h
#define _paintown_player_h

#include "character.h" 
#include "util/keyboard.h"
#include <deque>
#include "util/load_exception.h"
#include "network/network.h"

class Animation;
class World;

using namespace std;

struct keyState{
	keyState( int key, int facing ):key(key),facing(facing){}
	int key;
	int facing;
};

class Player: public Character{
public:

	Player( const char * filename ) throw( LoadException );
	Player( const string & str ) throw( LoadException );
	Player( const Player & pl ) throw( LoadException );
	Player( const Character & chr ) throw( LoadException );
	
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

	virtual void loseLife( int l = 1 );
	virtual void gainLife( int l = 1 );

	inline const void setInvincible( const bool b ){
		this->invincible = b;
	}

	inline const bool isInvincible() const {
		return this->invincible;
	}


	/* reset some stuff when the player dies */
	virtual void deathReset();
	
protected:
	void fillKeyCache();
	bool combo( Animation * ani );
	bool combo( Animation * ani, deque< keyState >::iterator cache_cur_key, deque< keyState >::iterator end );
	virtual int getKey( int x, int facing );
	virtual int getKey( int x );

	virtual bool careAboutKey( int key );

	bool canGrab( Object * enemy );
	void grabEnemy( Object * enemy );
			
	Network::Message thrownMessage( unsigned int id );
	
protected:

	/* store key presses in a stack with two ends*/
	deque< keyState > key_cache;
	map< int, bool > last_key;
	int acts;
	Keyboard keyboard;
	int show_life;

	int name_id;

	bool invincible;

	// int last_key;

};

#endif
