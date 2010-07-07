#ifndef _paintown_gui_animation_h
#define _paintown_gui_animation_h

#include <vector>
#include <map>

#include "util/load_exception.h"

class Token;
class Bitmap;

namespace Gui{

// To hold images by number easier to access and reuse
typedef std::map< int, Bitmap *> imageMap;

struct AnimationPoint{
    AnimationPoint();
    double x;
    double y;
};

struct AnimationArea{
    AnimationArea();
    int x1;
    int y1;
    int x2;
    int y2;
};

class Frame{
    public:
	Frame(Token *token, imageMap &images) throw (LoadException);
	virtual ~Frame();
	virtual void act(const double xvel, const double yvel);
	virtual void draw(const int xaxis, const int yaxis, const Bitmap &);
	Bitmap *bmp;
	AnimationPoint offset;
	AnimationPoint scrollOffset;
	int time;
	bool horizontalFlip;
	bool verticalFlip;
	int alpha;
};

class Animation{
public:
	Animation(Token *token) throw (LoadException);
	virtual ~Animation();
	// Logic
	virtual void act();
	virtual void draw(const Bitmap &);
	virtual void forwardFrame();
	virtual void backFrame();
	
	inline void reset(){ if (allowReset){ currentFrame = 0; } }
	inline int getID() const { return id; }

    enum Depth {
        Background0,
        Background1,
        Foreground0,
        Foreground1,
    };
	inline const Depth & getDepth() const { return this->depth; }
	
private:
	int id;
	Depth depth;
	int ticks;
	unsigned int currentFrame;
	unsigned int loop;
	bool allowReset;
	AnimationPoint axis;
	// This allows the frames to scroll in place
	AnimationPoint velocity;
	AnimationArea window;
	std::vector<Frame *> frames;
	imageMap images;
};
}
#endif
