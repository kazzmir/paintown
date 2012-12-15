#ifndef _paintown_enemy_guy_h
#define _paintown_enemy_guy_h

#include "character.h"
#include "util/file-system.h"
#include "heart.h"

#include <string>
#include <vector>

class World;

namespace Paintown{

class Object;
class Enemy: public Character{
public:
	Enemy( ); 
	Enemy( const char * filename );
	Enemy( const Filesystem::AbsolutePath & filename );
	Enemy( const Enemy & chr );
	Enemy( const Character & chr );
	
	virtual Object * copy();
	
	virtual void act( std::vector< Object * > * others, World * world, std::vector< Object * > * add );

	using Character::drawLifeBar;
	virtual void drawLifeBar( int x, int y, Graphics::Bitmap * work );
	
	virtual void drawFront( Graphics::Bitmap * work, int rel_x );
	
	virtual void hurt( int x );

	virtual inline void setMaxHealth( int h ){
		Character::setMaxHealth( h );
		show_life = h;
	}

	virtual inline Heart * getHeart() const{
		return heart;
	}
	
	virtual ~Enemy();

	/* the chances this enemy will try to attack */
	virtual int getAggression() const {
		return aggression;
	}

	virtual void setAggression( const int a ){
		aggression = a;
	}

protected:

	// void filterEnemies( vector< Object * > * mine, vector< Object * > * all );
	void constructSelf();

	const Object * findClosest( const std::vector< Object * > & enemies );

protected:

	Heart * heart;
	int want_x, want_z;
	bool want_path;

	int show_name_time;
	int id;
	int show_life;
	int aggression;

};

}

#endif
