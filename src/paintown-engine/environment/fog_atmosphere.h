#ifndef _paintown_fog_atmosphere_h
#define _paintown_fog_atmosphere_h

namespace Graphics{
class Bitmap;
}

#include "atmosphere.h"
#include <vector>

class Token;

struct Fog{
	Fog( int x, int y, unsigned int ang ):x(x),y(y),ang(ang){}
	int x, y;
	unsigned int ang;
};

class FogAtmosphere: public Atmosphere {
public:

	FogAtmosphere();	
	virtual ~FogAtmosphere();

	virtual void drawBackground(const Graphics::Bitmap & work, int x);
	virtual void drawForeground(const Graphics::Bitmap & work, int x);
	virtual void drawFront(const Graphics::Bitmap & work, int x);
	virtual void drawScreen(const Graphics::Bitmap & work, int x);
	virtual void act(const Scene & level, const std::vector<Paintown::Object*>*);
    virtual void interpret(const Token * message);

protected:
    Graphics::Bitmap * fog;
	std::vector< Fog * > fogs;
};

#endif
