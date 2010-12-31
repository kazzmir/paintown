#ifndef _paintown_rain_atmosphere_h
#define _paintown_rain_atmosphere_h

#include "atmosphere.h"
#include "util/sound.h"
#include <vector>

class Bitmap;
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

	virtual void drawBackground(Bitmap * work, int x);
	virtual void drawForeground(Bitmap * work, int x);
	virtual void drawFront(Bitmap * work, int x);
	virtual void drawScreen(Bitmap * work, int x);
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
