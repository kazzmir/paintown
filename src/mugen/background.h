#ifndef _background_mugen_h
#define _background_mugen_h

#include <string>
#include <vector>
#include <map>
#include <math.h>
#include "animation.h"
#include "util.h"
#include "util/gui/rectarea.h"
#include "ast/all.h"
#include "util/bitmap.h"
#include "util/pointer.h"

namespace PaintownUtil = ::Util;

namespace Mugen{
    
struct Sin {
    Sin():
    amp(0),period(0),offset(0),angle(0){}

    inline void act(){
        angle += 1;
    }

    double get() const;

    double amp;
    double period;
    double offset;
    double angle;
};

/*! Base Element for backgrounds only uses a sprite as background */
class BackgroundElement: public Element {
    public:
	BackgroundElement(const std::string & name, Ast::Section * data);
        BackgroundElement(const BackgroundElement &);
	virtual ~BackgroundElement();
	
	virtual void act();
	virtual void render(int x, int y, const Graphics::Bitmap &, Graphics::Bitmap::Filter * filter = NULL) = 0;

        //! Set the passed element to this elements values, this is called when the next element is linked to this one
        virtual void setLink(BackgroundElement *element);

        // Copy operator so we can make an initial copy of are starting values to restore on a reset
        const BackgroundElement & operator=(const BackgroundElement &);

        virtual double getCurrentX() const;
        virtual double getCurrentY() const;
	
	virtual inline void setVisible(bool visible){
            this->visible = visible;
        }

        virtual inline bool getVisible(){
            return this->visible && this->enabled;
        }

        virtual inline void setEnabled(bool enabled){
            this->enabled = enabled;
        }

        virtual inline bool getEnabled(){
            return this->enabled;
        }

        virtual inline void setStart(const Mugen::Point &point){
	    this->start = point;
	}
	
	virtual inline Mugen::Point getStart() const{
	    return start;
	}
	
	virtual inline void setDelta(double x, double y){
	    this->deltaX = x;
	    this->deltaY = y;
	}
	
	virtual inline double getDeltaX() const{
	    return this->deltaX;
	}
	
	virtual inline double getDeltaY() const{
	    return this->deltaY;
	}
	
	virtual inline void setTrans(const TransType &trans){
	    this->effects.trans = trans;
	}
	
	virtual inline void setAlpha(int low, int high){
	    this->effects.alphalow = low;
	    this->effects.alphahigh = high;
	}
	
	virtual inline const TransType & getTrans() const {
	    return this->effects.trans;
	}
	
	virtual inline void setMask(bool mask){
	    this->effects.mask = mask;
	}
	
	virtual inline bool getMask() const {
	    return this->effects.mask;
	}

        virtual inline const Mugen::Effects & getEffects() const{
            return this->effects;
        }
	
	virtual inline void setTile(const Mugen::Point &point){
	    this->tile = point;
	}
	
	virtual inline Mugen::Point getTile() const {
	    return this->tile;
	}
	
	virtual inline void setTileSpacing(const Mugen::Point &point){
	    this->tileSpacing = point;
	}
	
	virtual inline Mugen::Point getTileSpacing() const {
	    return this->tileSpacing;
	}
	
	virtual inline void setWindow(int x1, int y1, int x2, int y2){
	    this->window = Gui::RectArea(x1,y1,x2-x1,y2-y1);
	}
	
	virtual inline const Gui::RectArea & getWindow() const {
	    return this->window;
	}
	
	virtual inline void setWindowDelta(double x, double y){
	    this->windowDeltaX = x;
	    this->windowDeltaY = y;
	}
	
	virtual inline double getWindowDeltaX() const{
	    return this->windowDeltaX;
	}
	
	virtual inline double getWindowDeltaY() const{
	    return this->windowDeltaY;
	}
	
	virtual inline void setPositionLink(bool link){
	    this->positionLink = link;
	}
	
	virtual inline bool getPositionLink() const {
	    return this->positionLink;
	}
	
	virtual inline void setVelocity(double x, double y){
	    this->velocityX = x;
	    this->velocityY = y;
	}
	
	virtual inline double getVelocityX() const{
	    return this->velocityX;
	}
	
	virtual inline double getVelocityY() const{
	    return this->velocityY;
	}
	
	virtual inline void setSinX(const Sin &sin){
	    this->sinX = sin;
	}
	
	virtual inline Sin & getSinX() {
	    return this->sinX;
	}
	
        virtual inline const Sin & getSinX() const {
	    return this->sinX;
	}
	
	virtual inline void setSinY(const Sin &sin){
	    this->sinY = sin;
	}
	
	virtual inline Sin & getSinY() {
	    return this->sinY;
	}
	
        virtual inline const Sin & getSinY() const {
	    return this->sinY;
	}
	
	virtual inline void setX(int x){
	    this->x = x;
	}
	
	virtual inline void setY(int y){
	    this->y = y;
	}

        virtual inline int getX() const {
            return (int) x;
        }

        virtual inline int getY() const {
            return (int) y;
        }

        virtual void reset();

    private:
        //! get linked element
        BackgroundElement *getLinkedElement();
	//! Visible
        bool visible;
        //! Enabled
        bool enabled;
        //! The starting coordinate relative to 0,0 which is center of screen
	Mugen::Point start;

        /* virtual coordinates, affected by velocity */
        double x;
        double y;

	//! Delta values used for movement defaults to 1
	double deltaX;
	double deltaY;
	//! Transparency defaults to None
	Mugen::Effects effects;
	//! Does this element tile 0 = no tile 1 = infinite above 1 = tile that many times
	Mugen::Point tile;
	//! Tile spacing
	Mugen::Point tileSpacing;
	//! Window for clipping
    Gui::RectArea window;
	//! Window delta
	double windowDeltaX;
	double windowDeltaY;
	//! The current element is linked to the previous element in the definion file. It takes on the start,delta,and sin properties of the linked element
	bool positionLink;
	//! Velocity of this element
	double velocityX;
	double velocityY;
	//! Sin components (sinusodial (sp?) values to simulate the effect of bobbing or weaving back and forth)
	Sin sinX;
	Sin sinY;
        //! The previous element that this one is linked to it used by setLink
        BackgroundElement *linkedElement;
};

/*! Normal element */
class NormalElement : public BackgroundElement {
    public:
	NormalElement(const std::string & name, Ast::Section * data, const Mugen::SpriteMap & sprites);
	virtual ~NormalElement();
	virtual void act();
	virtual void render(int x, int y, const Graphics::Bitmap &, Graphics::Bitmap::Filter * filter = NULL);
	virtual inline void setSprite(MugenSprite *sprite){
	    this->sprite = sprite;
	}
    private:
	//! Sprite Based
	MugenSprite *sprite;
};

/*! Animation Element */
class AnimationElement: public BackgroundElement {
public:
    AnimationElement(const PaintownUtil::ReferenceCount<Ast::AstParse> & parse, const std::string & name, Ast::Section * data, const Mugen::SpriteMap & sprites);

    virtual ~AnimationElement();
    virtual void act();
    virtual void render(int x, int y, const Graphics::Bitmap &, Graphics::Bitmap::Filter * filter = NULL);
    virtual void setAnimation(PaintownUtil::ReferenceCount<MugenAnimation> animation);
private:
    PaintownUtil::ReferenceCount<MugenAnimation> animation;
};

/*! Parallax Element */
class ParallaxElement : public BackgroundElement {
    public:
	ParallaxElement(const std::string & name, Ast::Section * data, const Mugen::SpriteMap & sprites);
	virtual ~ParallaxElement();
	virtual void act();
	virtual void render(int x, int y, const Graphics::Bitmap &, Graphics::Bitmap::Filter * filter = NULL);
	virtual inline void setSprite(MugenSprite *sprite){
	    this->sprite = sprite;
	}
	virtual inline void setXScale(double x, double y){
	    this->xscaleX = x;
	    this->xscaleY = y;
	    width.x = 0;
	    width.y = 0;
	}
	virtual inline void setWidth(const Mugen::Point &point){
	    this->width = point;
	    xscaleX = 0;
	    xscaleY = 0;
	}
	virtual inline void setYScale(double x){
	    this->yscale = x;
	}
	virtual inline void setYScaleDelta(double x){
	    this->yscaleDelta = x;
	}

        virtual inline double getYScaleDelta() const {
            return this->yscaleDelta;
        }
    private:
	//! Sprite Based
	MugenSprite *sprite;
	//! xscale x = top y = bottom
	double xscaleX;
	double xscaleY;
	//! Same as xscale but just ints x = top y = bottom
	Mugen::Point width;
	//! yscale starting y-scale in percent (defaults to 100) use either xscale or width but not both
	double yscale;
	//! Delta for yscale per unit in percent (defaults to 0)
	double yscaleDelta;
};

/*! Dummy Element - Not an interactive element, it used mostly as support in Position Link chains */
class DummyElement : public BackgroundElement {
    public:
	DummyElement(const std::string & name, Ast::Section * data);
	virtual ~DummyElement();
	virtual void act();
	virtual void render(int x, int y, const Graphics::Bitmap &, Graphics::Bitmap::Filter * filter = NULL);
    private:
};

// Forward declare Background Controller and Background
class Background;
class BackgroundController;

/*! Controller */
class Controller {
    public:
        Controller(const std::string & name, Ast::Section * data, BackgroundController & control, Background & background);
        virtual ~Controller();

        virtual void act()=0;
        /*! Reset Counter */
        virtual void reset();
        
        virtual inline void addElements(const std::vector<BackgroundElement *> & elements){
	    this->elements.insert(this->elements.end(), elements.begin(), elements.end());
	}

    protected:
        /*! Name of controller */
        std::string name;
        /*! How many ticks before controller becomes active */
        int timeStart;
        /*! How many ticks before controller is deactivated */
	int endTime;
        /*! How many ticks before controller is reset (-1 means no reset) */
	int loopTime;
        /*! Tick handler */
	int ticker;
        /*! Current Elements this controller has governance over */
        std::vector< BackgroundElement * > elements;

        /* dont allow the global reset time affect this controller */
        bool dontReset;
};

/*! Background Controller */
class BackgroundController{
    public:
	BackgroundController(const std::string & name, Ast::Section * data, Background & background);
	virtual ~BackgroundController();
	
	virtual void act();

        virtual inline const std::string & getName() const {
            return this->name;
        }
	
	virtual inline void addElements(const std::vector<BackgroundElement *> & elements){
	    this->elements.insert(this->elements.end(), elements.begin(), elements.end());
	}

        virtual inline const std::vector< BackgroundElement *> & getElements() const {
            return this->elements;
        }

        virtual inline void addController(Controller * controller){
            this->controllers.push_back(controller);
        }
    private:
        /*! Name of controller */
        std::string name;
        /*! Controller ID */
        int ID;
	/*! Global Looptime if not given then it will be disabled. At looptime it will reset itself and all controllers. */
	int globalLooptime;
	/*! Ticker of Background Controller */ 
	int ticker;
	/*! BackgroundElement list */
	std::vector < BackgroundElement *> elements;
	/*! Controllers */
	std::vector < Controller *> controllers;
};

/*! Our Background */
class Background{
    public:
	//! Pass in the file that has the background and the base name ie 'BG'
	Background(const Filesystem::AbsolutePath & file, const std::string &header);
	Background(const PaintownUtil::ReferenceCount<Ast::AstParse> & parse, const std::string & header, const SpriteMap & sprites);

	virtual ~Background();
	
	virtual void act();
	virtual void renderBackground(int cameraX, int cameraY, const Graphics::Bitmap &, Graphics::Bitmap::Filter * filter = NULL);
	virtual void renderForeground(int cameraX, int cameraY, const Graphics::Bitmap &, Graphics::Bitmap::Filter * filter = NULL);
	
        //! Returns a vector of Elements by given ID
        std::vector< BackgroundElement * > getIDList(int ID);
        
        //! Returns a vector of all elements in backgrounds and foregrounds
        inline std::vector< BackgroundElement * > getElements() { 
            std::vector< BackgroundElement *> temp;
            temp.insert(temp.end(),backgrounds.begin(),backgrounds.end());
            temp.insert(temp.end(),foregrounds.begin(),foregrounds.end());
            return temp;
        }

    protected:

        void destroyRaw(const std::map< unsigned int, std::map< unsigned int, MugenSprite * > > & sprites);

    private:
	
	//! File where background is in
        Filesystem::AbsolutePath file;
	
	//! What's the section we are looking for ie 'BG'
	std::string header;
	
	//! Sprite file
	std::string spriteFile;
	
        /* these are the sprites that come from a bgdef like
         * [BGdef]
         * spr = stages/venice.sff
         *
         * the background will own these sprites and should delete them.
         */
	Mugen::SpriteMap sprites;
	
	//! Animations
	// std::map< int, MugenAnimation * > animations;
	
	//! Debug State
	bool debug;
	
	//! Clear color
        Graphics::Color clearColor;
	
	//! Backgrounds
	std::vector< BackgroundElement * > backgrounds;
	
	//! Foregrounds
	std::vector< BackgroundElement * > foregrounds;

        //! Controllers
        std::vector< BackgroundController *> controllers;
};
    
}

#endif
