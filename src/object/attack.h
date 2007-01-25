#ifndef _attack_h
#define _attack_h

#include "util/load_exception.h"

class Token;

/* a container class for an attack */
class Attack{
public:
	
	Attack();
	Attack( Token * tok ) throw( LoadException );
	Attack( const int x1, const int y1, const int x2, const int y2, const int damage, const int force );
	Attack( const Attack & a );

	void print();

	Attack & operator = ( const Attack & a );

	const int getXLen();
	const int getYLen();

public:

	inline const int getX1() const {
		return x1;
	}
	
	inline const int getX2() const {
		return x2;
	}
	
	inline const int getY1() const {
		return y1;
	}
	
	inline const int getY2() const {
		return y2;
	}

	inline const int getDamage() const {
		return damage;
	}

	inline const int getForce() const {
		return force;
	}

public:
	inline void setX1( const int x ){
		x1 = x;
	}

	inline void setY1( const int y ){
		y1 = y;
	}

	inline void setX2( const int x ){
		x2 = x;
	}

	inline void setY2( const int y ){
		y2 = y;
	}

	inline void setDamage( const int d ){
		damage = d;
	}

	inline void setForce( const int f ){
		force = f;
	}

protected:
	int x1, y1, x2, y2;
	int damage;
	int force;
};

#endif
