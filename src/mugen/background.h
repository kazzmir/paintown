#ifndef _background_mugen_h
#define _background_mugen_h

#include <string>
#include <vector>
#include <map>
#include "mugen_animation.h"
#include "mugen_util.h"
#include "gui/rectarea.h"
#include "ast/all.h"

class Bitmap;

namespace Mugen{
    
//! Type of element
enum ElementType{
    Normal,
    Parallax,
    Anim,
    Dummy,
};

struct Sin {
    Sin():
    amp(0),period(0),offset(0){}
    double amp;
    double period;
    double offset;
};

/*! Base Element for backgrounds only uses a sprite as background */
class BackgroundElement : public Element{
    public:
	BackgroundElement();
	virtual ~BackgroundElement();
	
	virtual void act()=0;
	virtual void render(const Bitmap &)=0;
	
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
	    this->trans = trans;
	}
	
	virtual inline const TransType & getTrans() const {
	    return this->trans;
	}
	
	virtual inline void setMask(bool mask){
	    this->mask = mask;
	}
	
	virtual inline bool getMask() const {
	    return this->mask;
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
	    this->window = RectArea(x1,y1,x2-x1,y2-y1);
	}
	
	virtual inline RectArea getWindow() const {
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
	
    private:
	//! The starting coordinate relative to 0,0 which is center of screen
	Mugen::Point start;
	//! Delta values used for movement defaults to 1
	double deltaX;
	double deltaY;
	//! Transparency defaults to None
	Mugen::TransType trans;
	//! Masking?
	bool mask;
	//! Does this element tile 0 = no tile 1 = infinite above 1 = tile that many times
	Mugen::Point tile;
	//! Tile spacing
	Mugen::Point tileSpacing;
	//! Window for clipping
	RectArea window;
	//! Window delta
	double windowDeltaX;
	double windowDeltaY;
	//! linked?
	bool positionLink;
	//! Velocity of this element
	double velocityX;
	double velocityY;
	//! Sin components
	Sin sinX;
	Sin sinY;
};

/*! Our Background */
class Background{
    public:
	//! Pass in the file that has the background and the base name ie 'BG'
	Background(const std::string &file, const std::string &header);
	virtual ~Background();
	
	virtual void act();
	virtual void renderBackground(const Bitmap &);
	virtual void renderForeground(const Bitmap &);
	
	//! Set the position of the camera
	virtual inline void setCamera(const Mugen::Point & camera){
	    this->camera = camera;
	}
	
	//! Get the position of the camera
	virtual inline Mugen::Point getCamera() const {
	    return this->camera;
	}
	
    private:
	
	//! File where background is in
	std::string file;
	
	//! What's the section we are looking for ie 'BG'
	std::string header;
	
	//! Sprite file
	std::string spriteFile;
	
	//! Sprite map
	Mugen::SpriteMap sprites;
	
	//! Animations
	std::map< int, MugenAnimation * > animations;
	
	//! Debug State
	bool debug;
	
	//! Clear color
	int clearColor;
	
	//! Camera
	Mugen::Point camera;
	
	//! Backgrounds
	std::vector< BackgroundElement * > backgrounds;
	
	//! Foregrounds
	std::vector< BackgroundElement * > foregrounds;
};
    
}

#endif
