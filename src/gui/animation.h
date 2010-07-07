#ifndef _paintown_gui_animation_h
#define _paintown_gui_animation_h

#include <vector>
#include <map>

#include "util/load_exception.h"
#include "gui/coordinate.h"

class Token;
class Bitmap;

namespace Gui{

// To hold images by number easier to access and reuse
typedef std::map< int, Bitmap *> imageMap;

class Frame{
    public:
	Frame(Token *token, imageMap &images) throw (LoadException);
    Frame(Bitmap *);
	virtual ~Frame();
	virtual void act(double xvel, double yvel);
	virtual void draw(int xaxis, int yaxis, const Bitmap &);
	Bitmap *bmp;
    RelativePoint offset;
    RelativePoint scrollOffset;
	int time;
	bool horizontalFlip;
	bool verticalFlip;
	int alpha;
};

class Animation{
public:
	Animation(Token *token) throw (LoadException);
    /*! Load only a single bitmap (for bacwards compatibility of backgrounds in menu) */
    Animation(const std::string &) throw (LoadException);
	virtual ~Animation();
	// Logic
	virtual void act();
	virtual void draw(const Bitmap &);
	virtual void forwardFrame();
	virtual void backFrame();
	
	inline void reset(){ if (allowReset){ currentFrame = 0; } }
	inline int getID() const { return id; }

    enum Depth {
        BackgroundBottom,
        BackgroundMiddle,
        BackgroundTop,
        ForegroundBottom,
        ForegroundMiddle,
        ForegroundTop,
    };
	inline const Depth & getDepth() const { return this->depth; }
	
private:
	int id;
	Depth depth;
	int ticks;
	unsigned int currentFrame;
	unsigned int loop;
	bool allowReset;
    RelativePoint axis;
	// This allows the frames to scroll in place
    RelativePoint velocity;
    Coordinate window;
	std::vector<Frame *> frames;
	imageMap images;
};
}
#endif
