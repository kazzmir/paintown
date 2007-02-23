#include "attack.h"
#include "util/load_exception.h"
#include "util/token.h"
#include "util/token_exception.h"

#include <iostream>

Attack::Attack():
x1( 0 ),
y1( 0 ),
x2( 0 ),
y2( 0 ),
damage( 0 ),
force( 0 ){
}

Attack::Attack( const int _x1, const int _y1, const int _x2, const int _y2, const int _damage, const int _force ):
x1( _x1 ),
y1( _y1 ),
x2( _x2 ),
y2( _y2 ),
damage( _damage ),
force( _force ){
}

Attack::Attack( const Attack & a ){
	setX1( a.getX1() );
	setX2( a.getX2() );
	setY1( a.getY1() );
	setY2( a.getY2() );
	setDamage( a.getDamage() );
	setForce( a.getForce() );
}

Attack::Attack( Token * tok ) throw( LoadException ):
x1( 0 ),
y1( 0 ),
x2( 0 ),
y2( 0 ),
damage( 0 ),
force( 0 ){
	Token * current;
	while ( tok->hasTokens() ){
		try{
			*tok >> current;

			int thing;
			*current >> thing;
			if ( *current == "x1" )
				setX1( thing );
			else if ( *current == "x2" )
				setX2( thing );
			else if ( *current == "y1" )
				setY1( thing );
			else if ( *current == "y2" )
				setY2( thing );
			else if ( *current == "damage" )
				setDamage( thing );
			else if ( *current == "force" )
				setForce( thing );

		} catch ( const TokenException & te ){
			string k = "Could not parse attack: ";
			k += te.getReason();
			throw LoadException( k );
		}
	}

	if ( x1 > x2 ){
		int p = x2;
		x2 = x1;
		x1 = p;
	}
	if ( y1 > y2 ){
		int p = y2;
		y2 = y1;
		y1 = p;
	}
}
	
void Attack::print(){
	cout<<"X1: "<<getX1()<<endl;
	cout<<"Y1: "<<getY1()<<endl;
	cout<<"X2: "<<getX2()<<endl;
	cout<<"Y2: "<<getY2()<<endl;
	cout<<"Damage: "<<getDamage()<<endl;
}
	
const int Attack::getXLen() const {
	return getX2() - getX1();
}

const int Attack::getYLen() const {
	return getY2() - getY1();
}

Attack & Attack::operator = ( const Attack & a ){

	setX1( a.getX1() );
	setX2( a.getX2() );
	setY1( a.getY1() );
	setY2( a.getY2() );
	setDamage( a.getDamage() );
	setForce( a.getForce() );

	return *this;
}

