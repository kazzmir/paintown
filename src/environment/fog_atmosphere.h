#ifndef _paintown_fog_atmosphere_h
#define _paintown_fog_atmosphere_h

class Bitmap;

#include "atmosphere.h"

class FogAtmosphere: public Atmosphere {
public:

	FogAtmosphere();	
	virtual ~FogAtmosphere();

	virtual void draw( Bitmap * work );
	virtual void act();

protected:
	Bitmap * fog;
};

#endif
