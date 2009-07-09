#ifndef _paintown_atmosphere_h
#define _paintown_atmosphere_h

class Bitmap;

#include <string>

class Atmosphere{
protected:
	Atmosphere();
public:
	virtual ~Atmosphere();

        /* drawn in front of all background things but before objects */
	virtual void drawBackground(Bitmap * work, int x) = 0;

        /* drawn in front of all objects */
	virtual void drawForeground(Bitmap * work, int x) = 0;
        
        /* drawn only on the foreground */
	virtual void drawFront(Bitmap * work, int x) = 0;

        /* drawn on the entire screen */
        virtual void drawScreen(Bitmap * work, int x) = 0;

	virtual void act() = 0;

	static Atmosphere * createAtmosphere( const std::string & name );
};

#endif
