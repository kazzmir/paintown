#ifndef _enemy_guy_h
#define _enemy_guy_h

#include "character.h"
#include "util/load_exception.h"
#include "heart.h"

#include <string>
#include <vector>

class Object;
class World;

using namespace std;

class Enemy: public Character{
public:
	Enemy( ); 
	Enemy( const char * filename ) throw( LoadException );
	Enemy( const string & filename ) throw( LoadException );
	Enemy( const Enemy & chr ) throw( LoadException );
	
	virtual Object * copy();
	
	virtual void act( vector< Object * > * others, World * world );
	
	virtual void draw( Bitmap * work, int rel_x );
	
	// virtual void takeDamage( ObjectAttack * obj, int x );

	virtual void hurt( int x );

	virtual inline Heart * getHeart() const{
		return heart;
	}
	
	virtual ~Enemy();

	/* the chances this enemy will try to attack */
	virtual const int getAggression() const {
		return aggression;
	}

	virtual void setAggression( const int a ){
		aggression = a;
	}

protected:

	// void filterEnemies( vector< Object * > * mine, vector< Object * > * all );
	void constructSelf();

	const Object * findClosest( const vector< Object * > & enemies );

protected:

	Heart * heart;
	int want_x, want_z;
	bool want_path;

	int show_name_time;
	int id;
	int show_life;
	int aggression;

};

#endif
