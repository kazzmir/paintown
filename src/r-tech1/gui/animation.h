#ifndef _rtech1_gui_animation_h
#define _rtech1_gui_animation_h

#include <vector>
#include <map>
#include <string>

#include "coordinate.h"
#include "r-tech1/pointer.h"
#include "r-tech1/graphics/gradient.h"

class Filesystem;
namespace Path{
    class AbsolutePath;
}
typedef Path::AbsolutePath AbsolutePath;

namespace Graphics{
    class Bitmap;
}

namespace Util{
    class Draw;
}

class Token;

namespace Gui{

// To hold images by number easier to access and reuse
typedef std::map<int, Util::ReferenceCount<Graphics::Bitmap> > ImageMap;

/*! Base Element class for frame types */
class Element{
public:
    virtual ~Element();
    virtual void act(double xvel, double yvel);
    virtual void draw(int xaxis, int yaxis, const Graphics::Bitmap &) = 0;
    virtual void draw(const Graphics::Bitmap &) = 0;
    virtual void reset() = 0;
    virtual void setToEnd(const RelativePoint &) = 0;
    virtual const std::string getInfo() = 0;

    virtual inline const RelativePoint getOffset() const {
        return this->offset;
    }

    virtual inline const RelativePoint getScrollOffset() const {
        return this->offset;
    }
    
    virtual inline int getTime() const {
        return this->time;
    }

    virtual inline int getAlpha() const {
        return this->alpha;
    }
protected:
    Element(const Token *);
    RelativePoint offset;
    RelativePoint scrollOffset;
    int time;
    int alpha;
private:
    void parseToken(const Token *);
};

/*! Image Frame */
class ImageFrame : public Element{
public:
    ImageFrame(const Token *, ImageMap &, const std::string &);
    ImageFrame(Util::ReferenceCount<Graphics::Bitmap> bmp);
    virtual ~ImageFrame();
    virtual void act(double xvel, double yvel);
    virtual void draw(int xaxis, int yaxis, const Graphics::Bitmap &);
    virtual void draw(const Graphics::Bitmap &);
    virtual void reset();
    virtual void setToEnd(const RelativePoint &);
    virtual const std::string getInfo();
    virtual inline const Util::ReferenceCount<Graphics::Bitmap> & getBitmap() const {
        return this->bmp;
    }
protected:
    virtual void parseToken(const Token *, const std::string &, ImageMap &);

protected:
    Util::ReferenceCount<Graphics::Bitmap> bmp;
    bool horizontalFlip;
    bool verticalFlip;
};

/*! Text Frame */
class TextFrame: public Element {
public:
    TextFrame(const Token *);
    virtual ~TextFrame();
    
    virtual void act(double xvel, double yvel);
    virtual void draw(int xaxis, int yaxis, const Graphics::Bitmap &);
    virtual void draw(const Graphics::Bitmap &);
    virtual void reset();
    virtual void setToEnd(const RelativePoint &);
    virtual const std::string getInfo();
protected:
    virtual void parseToken(const Token *);

    /* FIXME: default this to Globals::DEFAULT_FONT */
    std::string font;
    std::string message;
    int fontWidth, fontHeight;
    Effects::Gradient gradient;
};

/* Iterates over a series of items */
class Sequence{
public:
    Sequence();

    virtual Util::ReferenceCount<Element> getCurrentFrame() const = 0;
    virtual int totalTicks() const = 0;
    virtual void setToEnd() = 0;

    /* Move the sequence along by the number of ticks and at the specified speed
     * Returns true if the sequence can't move any farther.
     */ 
    virtual bool forward(int tickCount, double velocityX, double velocityY) = 0;
    virtual bool reverse(int tickCount, double velocityX, double velocityY) = 0;
    
    virtual void draw(int xaxis, int yaxis, const Graphics::Bitmap &) const = 0;

    virtual void reset() = 0;
    virtual void resetTicks() = 0;

    /* Forcifully move to the next/previous frame */
    virtual void forwardFrame() = 0;
    virtual void backFrame() = 0;

    virtual ~Sequence();
};

class SequenceFrame: public Sequence {
public:
    SequenceFrame(const Util::ReferenceCount<Element> & frame);
    virtual Util::ReferenceCount<Element> getCurrentFrame() const;

    virtual int totalTicks() const;
    virtual void reset();
    virtual void resetTicks();
    virtual void setToEnd();
    
    virtual void draw(int xaxis, int yaxis, const Graphics::Bitmap &) const;

    /* Move the sequence along by the number of ticks and at the specified speed */
    virtual bool forward(int tickCount, double velocityX, double velocityY);
    virtual bool reverse(int tickCount, double velocityX, double velocityY);

    /* Forcifully move to the next/previous frame */
    virtual void forwardFrame();
    virtual void backFrame();

protected:
    Util::ReferenceCount<Element> frame;
    int ticks;
};

/* Shows sequences in a loop */
class SequenceLoop: public Sequence {
public:
    SequenceLoop(int loops);
    
    virtual Util::ReferenceCount<Element> getCurrentFrame() const;

    virtual void reset();
    virtual void resetTicks();
    virtual void setToEnd();
    virtual void addSequence(const Util::ReferenceCount<Sequence> & sequence);
    virtual void parse(const Token * token, ImageMap & map, const std::string & baseDir);
    
    virtual void draw(int xaxis, int yaxis, const Graphics::Bitmap &) const;
    
    virtual int totalTicks() const;

    /* Move the sequence along by the number of ticks and at the specified speed */
    virtual bool forward(int tickCount, double velocityX, double velocityY);
    virtual bool reverse(int tickCount, double velocityX, double velocityY);

    /* Forcifully move to the next/previous frame */
    virtual void forwardFrame();
    virtual void backFrame();

protected:
    void resetChildrenTicks();
    virtual Util::ReferenceCount<Sequence> getCurrentSequence() const;

    /* The current frame to display */
    unsigned int currentFrame;

    /* The number of times left to loop */
    unsigned int currentLoop;

    /* The total number of times to loop */
    const unsigned int loopTimes;

    std::vector<Util::ReferenceCount<Sequence> > frames;
};

/* Shows all sequences simaltaneously */
class SequenceAll: public Sequence {
public:
    SequenceAll(const Token * token, ImageMap & images, const std::string & baseDir);
    
    virtual Util::ReferenceCount<Element> getCurrentFrame() const;

    virtual void reset();
    virtual void resetTicks();
    virtual void setToEnd();
    virtual void addSequence(const Util::ReferenceCount<Sequence> & sequence);
    
    virtual void draw(int xaxis, int yaxis, const Graphics::Bitmap &) const;
    
    virtual int totalTicks() const;

    /* Move the sequence along by the number of ticks and at the specified speed */
    virtual bool forward(int tickCount, double velocityX, double velocityY);
    virtual bool reverse(int tickCount, double velocityX, double velocityY);

    /* Forcifully move to the next/previous frame */
    virtual void forwardFrame();
    virtual void backFrame();

protected:
    std::vector<Util::ReferenceCount<Sequence> > sequences;
    typedef std::vector<Util::ReferenceCount<Sequence> >::iterator SequenceIterator;
    typedef std::vector<Util::ReferenceCount<Sequence> >::const_iterator SequenceConstIterator;
};

/* Displays a random child node for its duration */
class SequenceRandom: public Sequence {
public:
    SequenceRandom(const Token * token, ImageMap & images, const std::string & baseDir);
    
    virtual Util::ReferenceCount<Element> getCurrentFrame() const;

    virtual void reset();
    virtual void resetTicks();
    virtual void setToEnd();
    virtual void addSequence(const Util::ReferenceCount<Sequence> & sequence);
    
    virtual void draw(int xaxis, int yaxis, const Graphics::Bitmap &) const;
    
    virtual int totalTicks() const;

    /* Move the sequence along by the number of ticks and at the specified speed */
    virtual bool forward(int tickCount, double velocityX, double velocityY);
    virtual bool reverse(int tickCount, double velocityX, double velocityY);

    /* Forcifully move to the next/previous frame */
    virtual void forwardFrame();
    virtual void backFrame();

protected:
    unsigned int current;
    std::vector<Util::ReferenceCount<Sequence> > sequences;
    typedef std::vector<Util::ReferenceCount<Sequence> >::iterator SequenceIterator;
    typedef std::vector<Util::ReferenceCount<Sequence> >::const_iterator SequenceConstIterator;
};

/* Handles a series of bitmaps */
class Animation{
protected:
    Animation();
public:
    Animation(const Token *token);
    /*! Load only a single bitmap (for backwards compatibility of backgrounds in menu) */
    Animation(const std::string &);
    Animation(const AbsolutePath &);
    /*! use an existing bitmap */
    Animation(Util::ReferenceCount<Graphics::Bitmap> image);
    virtual ~Animation();
    /*! Reverse with ticks */
    virtual void reverse(int tickCount = 1);
    /*! Forward with ticks */
    virtual void forward(int tickCount = 1);
    /*! Logic ticking per iteration and moving to subsequent frames */
    virtual void act();
    /*! Draw */
    virtual void draw(const Graphics::Bitmap &) const;
    virtual void draw(int x, int y, int width, int height, const Graphics::Bitmap &) const;
    /*! Forward to next frame with no regard to ticks */
    virtual void forwardFrame();
    /*! Back a frame with no regard to ticks */
    virtual void backFrame();
    /*! Reset everything to the beginning of the start of the animation */
    virtual void resetAll();
    /*! Set everything to the end of the animation */
    virtual void setToEnd();
    /*! Get printable information regarding current frame */
    virtual const std::string getInfo();

    /*! Reset only frame ticks and other things are ignored */
    virtual void reset();

    /* Total number of ticks used by this animation. If any frames have a time
     * of -1 then the total time will also be -1, meaning infinity.
     */
    virtual int totalTicks() const;

    /*! Return ID */
    virtual inline int getID() const { 
        return id;
    }
    /*! Depth of animation
        TODO make depth unlimited and just use integers for weight of depth
    */
    enum Depth {
        BackgroundBottom,
        BackgroundMiddle,
        BackgroundTop,
        ForegroundBottom,
        ForegroundMiddle,
        ForegroundTop,
    };
    /*! Get depth */
    inline const Depth & getDepth() const { 
        return this->depth;
    }

protected:
    int id;
    Depth depth;
    unsigned int currentSequence;
    bool allowReset;
    RelativePoint axis;
    // This allows the frames to scroll in place
    RelativePoint velocity;
    Coordinate window;
    // std::vector<Util::ReferenceCount<Frame> > frames;
    SequenceLoop sequence;
    ImageMap images;
};

/* Handles a Util::Draw, which really can't be controlled in any way. This just wraps the drawer.draw() method */
class DrawableAnimation: public Animation {
public:
    DrawableAnimation(const Util::ReferenceCount<Util::Draw> & drawer, Depth depth);
    DrawableAnimation(const Util::ReferenceCount<Util::Draw> & drawer);

    virtual void draw(const Graphics::Bitmap &) const;
    virtual void draw(int x, int y, int width, int height, const Graphics::Bitmap &) const;

protected:
    const Util::ReferenceCount<Util::Draw> drawer;
};

/*! Generalized to for re-use in other contexts (menu, cutscene, characterselect, etc) */
class AnimationManager{
public:
    AnimationManager();
    AnimationManager(const AnimationManager &);
    virtual ~AnimationManager();
    
    const AnimationManager & operator=(const AnimationManager &);
    
    void forward(int tickCount = 1);
    void reverse(int tickCount = 1);
    void act();
    void render(const Gui::Animation::Depth &, const Graphics::Bitmap &) const;
    
    void add(const Util::ReferenceCount<Gui::Animation > & animation);
    
    void reset();
    void setToEnd();

    int totalTicks() const;
    
    const std::string getInfo(int id, bool all = false);
    
    const std::vector<int> getIdList();
    
    virtual inline const bool empty() const{
        return this->animations.empty();
    }

protected:
    int countTicks(const std::vector<Util::ReferenceCount<Gui::Animation> > & toCount) const;

private:
    std::map< Gui::Animation::Depth, std::vector< Util::ReferenceCount<Gui::Animation> > > animations;
};

}
#endif
