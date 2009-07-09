#ifndef _paintown_night_atmosphere_h
#define _paintown_night_atmosphere_h

class Bitmap;

#include "atmosphere.h"

class NightAtmosphere: public Atmosphere {
public:

	NightAtmosphere();	
	virtual ~NightAtmosphere();

        virtual void drawBackground(Bitmap * work, int x);
	virtual void drawForeground(Bitmap * work, int x);
	virtual void drawFront(Bitmap * work, int x);
	virtual void drawScreen(Bitmap * work, int x);
	virtual void act();

protected:

        void drawLight(Bitmap * original, Bitmap * work, const int x, const int y, const int width, const int black, const int black_alpha, const int light, const int light_alpha);
};

#endif
