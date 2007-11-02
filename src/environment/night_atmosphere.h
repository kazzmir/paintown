#ifndef _paintown_night_atmosphere_h
#define _paintown_night_atmosphere_h

class Bitmap;

#include "atmosphere.h"

class NightAtmosphere: public Atmosphere {
public:

	NightAtmosphere();	
	virtual ~NightAtmosphere();

	virtual void draw( Bitmap * work );
	virtual void act();

protected:
	Bitmap * night;
};

#endif
