#include "menu/menu_animation.h"

#include <vector>
#include "util/token.h"
#include "util/bitmap.h"
#include "globals.h"
#include "util/funcs.h"

MenuFrame::MenuFrame(Token *token, imageMap &images) throw (LoadException):
bmp(0),
time(0),
horizontalFlip(false),
verticalFlip(false),
alpha(255){
    offset.x = 0;
    offset.y = 0;
    window.x1 = 0;
    window.y1 = 0;
    window.x2 = 0;
    window.y2 = 0;
    if ( *token != "frame" ){
	throw LoadException("Not an frame");
    }
    Token tok(*token);
    /* The usual setup of an animation frame is
	(frame (image NUM) (alpha NUM) (offset x y) (hflip 0|1) (vflip 0|1) (time NUM) (window x1 y1 x2 y2))
    */
    while ( tok.hasTokens() ){
	try{
	    Token * token;
	    tok >> token;
	    if (*token == "image"){
		// get the number
		int num;
		*token >> num;
		// now assign the bitmap
		bmp = images[num];
	    } else if (*token == "alpha"){
		// get alpha
		*token >> alpha;
	    } else if (*token == "offset"){
		// Get the offset location it defaults to 0,0
		*token >> offset.x >> offset.y;
	    } else if (*token == "hflip"){
		// horizontal flip
		*token >> horizontalFlip;
	    } else if (*token == "vflip"){
		// horizontal flip
		*token >> verticalFlip;
	    } else if (*token == "time"){
		// time to display
		*token >> time;
	    } else if (*token == "window"){
		// time to display
		*token >> window.x1 >> window.y1 >> window.x2 >> window.y2;
	    } else {
		Global::debug( 3 ) << "Unhandled menu attribute: "<<endl;
		if (Global::getDebug() >= 3){
		    token->print(" ");
		}
	    }
	} catch ( const TokenException & ex ) {
	    string m( "Menu parse error: " );
	    m += ex.getReason();
	    throw LoadException( m );
	} catch ( const LoadException & ex ) {
	    throw ex;
	}
    }
}
MenuFrame::~MenuFrame(){
}
void MenuFrame::draw(int xaxis, int yaxis, Bitmap *work){
    // Set clip from the axis default is 0,0,bitmap width, bitmap height
    work->setClipRect(window.x1,window.y1,work->getWidth() + window.x2,work->getHeight() + window.y2);
    Bitmap::transBlender( 0, 0, 0, alpha );
    if (horizontalFlip && !verticalFlip){
	bmp->drawTransHFlip(xaxis + offset.x, yaxis + offset.y, *work);
    } else if (!horizontalFlip && verticalFlip){
	bmp->drawTransVFlip(xaxis + offset.x, yaxis + offset.y, *work);
    } else if (horizontalFlip && verticalFlip){
	bmp->drawTransHVFlip(xaxis + offset.x, yaxis + offset.y, *work);
    } else if (!horizontalFlip && !verticalFlip){
	bmp->drawTrans(xaxis + offset.x, yaxis + offset.y, *work);
    }
    work->setClipRect(0,0,work->getWidth(),work->getHeight());
}

MenuAnimation::MenuAnimation(Token *token) throw (LoadException):
id(0),
location(0),
ticks(0),
currentFrame(0),
loop(0){
    axis.x = 0;
    axis.y = 0;
    if ( *token != "anim" ){
	throw LoadException("Not an animation");
    }
    /* The usual setup of an animation is
	The images must be listed prior to listing any frames
	loop will begin at the subsequent frame listed after loop
	axis is the location in which the drawing must be placed
	location - used to render in background or foreground (0 == background [default]| 1 == foreground)
	(anim (id NUM) 
	      (location NUM)
	      (image NUM FILE) 
	      (axis x y) 
	      (frame "Read comments above in constructor") 
	      (loop))
    */
    Token tok(*token);
    while ( tok.hasTokens() ){
	try{
	    Token * token;
	    tok >> token;
	    if (*token == "id"){
		// get the id
		*token >> id;
	    } else if (*token == "location"){
		// get the location
		*token >> location;
	    } else if (*token == "image"){
		// add bitmaps by number to the map
		int number;
		std::string temp;
		*token >> number >> temp;
		Bitmap *bmp = new Bitmap(Util::getDataPath() + temp);
		if (bmp->getError()){
		    delete bmp;
		} else {
		    images[number] = bmp;
		}
	    } else if (*token == "axis"){
		// Get the axis location it defaults to 0,0
		*token >> axis.x >> axis.y;
	    } else if (*token == "frame"){
		// new frame
		MenuFrame *frame = new MenuFrame(token,images);
		frames.push_back(frame);
	    } else if (*token == "loop"){
		// start loop here
		loop = frames.size();
	    } else {
		Global::debug( 3 ) << "Unhandled menu attribute: "<<endl;
		if (Global::getDebug() >= 3){
		    token->print(" ");
		}
	    }
	} catch ( const TokenException & ex ) {
	    string m( "Menu parse error: " );
	    m += ex.getReason();
	    throw LoadException( m );
	} catch ( const LoadException & ex ) {
	    throw ex;
	}
    }
    if (loop >= frames.size()){
	throw LoadException( "Problem with the loop location, it is beyond the last frame.." );
    }
}
MenuAnimation::~MenuAnimation(){
    for (std::vector<MenuFrame *>::iterator i = frames.begin(); i != frames.end(); ++i){
	if (*i){
	    delete *i;
	}
    }
    for (imageMap::iterator i = images.begin(); i != images.end(); ++i){
	if (i->second){
	    delete i->second;
	}
    }
}
void MenuAnimation::act(){
    if( frames[currentFrame]->time != -1 ){
	ticks++;
	if(ticks >= frames[currentFrame]->time){
		ticks = 0;
		forwardFrame();
	}
    }
}
void MenuAnimation::draw(Bitmap *work){
    frames[currentFrame]->draw(axis.x,axis.y,work);
}
void MenuAnimation::forwardFrame(){
    if (currentFrame < frames.size() -1){
	currentFrame++;
    } else {
	currentFrame = loop;
    }
}
void MenuAnimation::backFrame(){
    if (currentFrame > loop){
	currentFrame--;
    } else {
	currentFrame = frames.size() - 1;
    }
}


