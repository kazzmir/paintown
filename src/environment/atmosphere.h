#ifndef _paintown_atmosphere_h
#define _paintown_atmosphere_h

class Bitmap;

class Atmosphere{
public:

	Atmosphere();	
	virtual ~Atmosphere();

	virtual void draw( Bitmap * work ) = 0;
	virtual void act() = 0;

};

#endif
