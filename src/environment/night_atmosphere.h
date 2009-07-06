#ifndef _paintown_night_atmosphere_h
#define _paintown_night_atmosphere_h

class Bitmap;

#include "atmosphere.h"

class NightAtmosphere: public Atmosphere {
public:

	NightAtmosphere();	
	virtual ~NightAtmosphere();

	virtual void draw(Bitmap * work, int x);
	virtual void act();

protected:

        void drawLight(Bitmap * original, Bitmap * work, const int x);

protected:
	Bitmap * night;
};

#endif
