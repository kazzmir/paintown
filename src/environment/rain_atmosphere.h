#ifndef _paintown_rain_atmosphere_h
#define _paintown_rain_atmosphere_h

class Bitmap;

#include "atmosphere.h"

struct Drop{
	Drop( int x, int y, int length, int color ):x(x),y(y),length(length),color(color){}
	int x, y;
	int length;
	int color;
};

class RainAtmosphere: public Atmosphere {
public:

	RainAtmosphere();	
	virtual ~RainAtmosphere();

	virtual void draw( Bitmap * work );
	virtual void act();
	
protected:
	vector< Drop * > rain_drops;
};

#endif
