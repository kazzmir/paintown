#ifndef _paintown_gib_h
#define _paintown_gib_h

#include "object_nonattack.h"
#include "network/network.h"
#include <vector>

struct Point{
	Point( int x, int y, int life ):x(x), y(y), life(life){}
	int x, y;
	int life;
};

class Gib: public ObjectNonAttack{
public:
	Gib( const int x, const int y, const int z, double dx, double dy, double dz, Bitmap * image );
	Gib( const Gib & g );
	
	virtual void draw( Bitmap * work, int rel_x, int rel_y );
	
	virtual Object * copy();
	virtual bool isCollidable( Object * obj );
	virtual bool isGettable();
	virtual int getWidth() const;
	virtual int getHeight() const;
	virtual Network::Message getCreateMessage();
	
	virtual void act( std::vector< Object * > * others, World * world, std::vector< Object * > * add );

	virtual ~Gib();

protected:
	double dx, dy, dz;
	int angle;
	int fade;
	Bitmap * image;
    std::vector< Point > blood;
};

#endif
