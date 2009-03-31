#ifndef _paintown_menu_animation_h
#define _paintown_menu_animation_h

#include <vector>
#include <map>

#include "util/load_exception.h"

class Token;
class Bitmap;

// To hold images by number easier to access and reuse
typedef std::map< int, Bitmap *> imageMap;

struct MenuPoint{
    int x;
    int y;
};

struct MenuArea{
    int x1;
    int y1;
    int x2;
    int y2;
};

class MenuFrame{
    public:
	MenuFrame(Token *token, imageMap &images) throw (LoadException);
	virtual ~MenuFrame();
	virtual void draw(int xaxis, int yaxis, Bitmap *work);
	Bitmap *bmp;
	MenuPoint offset;
	int time;
	bool horizontalFlip;
	bool verticalFlip;
	int alpha;
};

class MenuAnimation{
    public:
	MenuAnimation(Token *token) throw (LoadException);
	virtual ~MenuAnimation();
	// Logic
	virtual void act();
	virtual void draw(Bitmap *work);
	virtual void forwardFrame();
	virtual void backFrame();
	
	inline void reset(){ if (allowReset){ currentFrame = 0; } }
	inline const int getID() const { return id; }
	inline const int getLocation() const { return location; }
	
    private:
	int id;
	int location;
	int ticks;
	unsigned int currentFrame;
	unsigned int loop;
	bool allowReset;
	MenuPoint axis;
	MenuArea window;
	std::vector<MenuFrame *> frames;
	imageMap images;
};

#endif
