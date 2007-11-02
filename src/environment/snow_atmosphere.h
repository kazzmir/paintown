#ifndef _paintown_snow_atmosphere_h
#define _paintown_snow_atmosphere_h

class Bitmap;

#include "atmosphere.h"

struct Flake{
	Flake( int x, int y, int type ):x(x),y(y),type(type),dir(0){}
	int x, y;
	int type;
	int dir;
};

class SnowAtmosphere: public Atmosphere {
public:

	SnowAtmosphere();	
	virtual ~SnowAtmosphere();

	virtual void draw( Bitmap * work );
	virtual void act();
	
protected:
	vector< Flake * > flakes;
};

#endif
