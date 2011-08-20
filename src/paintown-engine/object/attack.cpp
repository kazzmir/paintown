#include "attack.h"
#include "util/load_exception.h"
#include "util/token.h"
#include "util/token_exception.h"

#include <iostream>

using namespace std;

Attack::Attack():
x1(0),
y1(0),
x2(0),
y2(0),
damage(0),
/* these values were the defaults in versions before 3.5 */
forceX(1.7),
forceY(4.4){
}

Attack::Attack( const int _x1, const int _y1, const int _x2, const int _y2, const int _damage, double forceX, double forceY):
x1( _x1 ),
y1( _y1 ),
x2( _x2 ),
y2( _y2 ),
damage(_damage),
forceX(forceX),
forceY(forceY){
}

Attack::Attack(const Attack & a):
x1(a.x1),
y1(a.y1),
x2(a.x2),
y2(a.y2),
damage(a.damage),
forceX(a.forceX),
forceY(a.forceY){
}

Attack::Attack(const Token & tok):
x1( 0 ),
y1( 0 ),
x2( 0 ),
y2( 0 ),
damage( 0 ),
forceX(1.7),
forceY(4.4){
    tok.match("_/x1", x1);
    tok.match("_/y1", y1);
    tok.match("_/x2", x2);
    tok.match("_/y2", y2);
    tok.match("_/damage", damage);

    double x = 0, y = 0;
    if (tok.match("_/force", x, y)){
        forceX = x;
        forceY = y;
    }
    // tok.match("_/force", force);
    
    /* TODO: read all attack boxes */
    /*
    tok.match("_/box/x1", x1);
    tok.match("_/box/y1", y1);
    tok.match("_/box/x2", x2);
    tok.match("_/box/y2", y2);
    tok.match("_/box/damage", damage);
    x = 0; y = 0;
    if (tok.match("_/box/force", x, y)){
        forceX = x;
        forceY = y;
    }
    */

    /* make sure x1, y1 is upper left and x2, y2 is lower right */
    if (x1 > x2){
        int p = x2;
        x2 = x1;
        x1 = p;
    }

    if (y1 > y2){
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

bool Attack::empty() const {
    return getXLen() == 0 &&
           getYLen() == 0;
}
	
int Attack::getXLen() const {
	return getX2() - getX1();
}
    
int Attack::getYLen() const {
	return getY2() - getY1();
}

Attack & Attack::operator=( const Attack & a ){
    setX1(a.getX1());
    setX2(a.getX2());
    setY1(a.getY1());
    setY2(a.getY2());
    setDamage(a.getDamage());
    setForceX(a.getForceX());
    setForceY(a.getForceY());

    return *this;
}

