#ifndef _paintown_gib_h
#define _paintown_gib_h

#include "object_nonattack.h"

class Gib: public ObjectNonAttack{
public:
	Gib( const int x, const int y, const int z, double dx, double dy, Bitmap * image );
	Gib( const Gib & g );
	
	virtual void draw( Bitmap * work, int rel_x );
	
	virtual Object * copy();
	virtual bool isCollidable( Object * obj );
	virtual bool isGettable();
	virtual const int getWidth() const;
	virtual const int getHeight() const;
	
	virtual void act( vector< Object * > * others, World * world, vector< Object * > * add );

	virtual ~Gib();

protected:
	double dx, dy;
	Bitmap * image;

};

#endif
