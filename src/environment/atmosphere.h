#ifndef _paintown_atmosphere_h
#define _paintown_atmosphere_h

class Bitmap;

#include <string>

class Atmosphere{
public:

	Atmosphere();	
	virtual ~Atmosphere();

	virtual void drawBackground(Bitmap * work, int x) = 0;
	virtual void drawForeground(Bitmap * work, int x) = 0;
	virtual void drawFront(Bitmap * work, int x) = 0;
	virtual void act() = 0;

	static Atmosphere * createAtmosphere( const std::string & name );
};

#endif
