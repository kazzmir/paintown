#ifndef _paintown_rain_atmosphere_h
#define _paintown_rain_atmosphere_h

#include "atmosphere.h"
#include "util/sound.h"
#include <vector>

namespace Graphics{
class Bitmap;
}
class Token;

struct Drop{
	Drop( int x, int y, int length, int color ):x(x),y(y),length(length),color(color){}
	int x, y;
	int length;
	int color;
};

struct Puddle{
    Puddle(int x, int y, int size):
        x(x), y(y), size(size), current(1){
        }

    int x, y;
    int size;
    double current;
};

class RainAtmosphere: public Atmosphere {
public:

	RainAtmosphere();	
	virtual ~RainAtmosphere();

	virtual void drawBackground(Graphics::Bitmap * work, int x);
	virtual void drawForeground(Graphics::Bitmap * work, int x);
	virtual void drawFront(Graphics::Bitmap * work, int x);
	virtual void drawScreen(Graphics::Bitmap * work, int x);
	virtual void act(const Scene & level, const std::vector<Paintown::Object*>*);
    virtual void interpret(const Token * message);
	
protected:
        std::vector<Drop *> rain_drops;
        std::vector<Puddle *> puddles;
        std::vector<Puddle *> objectPuddles;
	Sound rain_sound;
	bool playing;
};

#endif
