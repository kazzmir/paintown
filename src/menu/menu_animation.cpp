#include "menu/menu_animation.h"

#include <vector>
#include <math.h>
#include <sstream>
#include "util/token.h"
#include "util/bitmap.h"
#include "globals.h"
#include "util/funcs.h"
#include "util/file-system.h"

using namespace std;

// Temporary solution
static void renderSprite(const Bitmap *bmp, const int x, const int y, const int alpha, const bool hflip, const bool vflip, Bitmap *work){
    if (alpha != 255){
	Bitmap::transBlender( 0, 0, 0, alpha );
	if (hflip && !vflip){
	    bmp->drawTransHFlip(x,y, *work);
	} else if (!hflip && vflip){
	    bmp->drawTransVFlip(x,y, *work);
	} else if (hflip && vflip){
	    bmp->drawTransHVFlip(x,y, *work);
	} else if (!hflip && !vflip){
	    bmp->drawTrans(x,y, *work);
	}
    }
    else {
	if (hflip && !vflip){
	    bmp->drawHFlip(x,y, *work);
	} else if (!hflip && vflip){
	    bmp->drawVFlip(x,y, *work);
	} else if (hflip && vflip){
	    bmp->drawHVFlip(x,y, *work);
	} else if (!hflip && !vflip){
	    bmp->draw(x,y, *work);
	}
    }
}

MenuPoint::MenuPoint():x(0),y(0){}
MenuArea::MenuArea():x1(0),y1(0),x2(0),y2(0){}

MenuFrame::MenuFrame(Token *the_token, imageMap &images) throw (LoadException):
bmp(0),
time(0),
horizontalFlip(false),
verticalFlip(false),
alpha(255){
    if ( *the_token != "frame" ){
        throw LoadException("Not an frame");
    }
    Token tok(*the_token);
    /* The usual setup of an animation frame is
    // use image -1 to not draw anything, it can be used to get a blinking effect
    (frame (image NUM) (alpha NUM) (offset x y) (hflip 0|1) (vflip 0|1) (time NUM))
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

void MenuFrame::act(const double xvel, const double yvel){
    scrollOffset.x +=xvel;
    scrollOffset.y +=yvel;
    if (scrollOffset.x >=bmp->getWidth()){
	scrollOffset.x = 0;
    } else if (scrollOffset.x <= -(bmp->getWidth())){
	scrollOffset.x = 0;
    }
    if (scrollOffset.y >=bmp->getHeight()){
	scrollOffset.y = 0;
    } else if (scrollOffset.y <= -(bmp->getHeight())){
	scrollOffset.y = 0;
    }
}

static bool closeFloat(double a, double b){
    const double epsilon = 0.001;
    return fabs(a-b) < epsilon;
}

void MenuFrame::draw(const int xaxis, const int yaxis, Bitmap *work){
    if (!bmp)return;

    if (!closeFloat(scrollOffset.x, 0) || !closeFloat(scrollOffset.y, 0)){

        // Lets do some scrolling
        Bitmap temp = Bitmap::temporaryBitmap(bmp->getWidth(), bmp->getHeight());
        MenuPoint loc;
        if (scrollOffset.x < 0){
            loc.x = scrollOffset.x + bmp->getWidth();
        } else if (scrollOffset.x > 0){
            loc.x = scrollOffset.x - bmp->getWidth();
        }
        if (scrollOffset.y < 0){
            loc.y = scrollOffset.y + bmp->getHeight();
        } else if (scrollOffset.y > 0){
            loc.y = scrollOffset.y - bmp->getHeight();
        }
        bmp->Blit(scrollOffset.x, scrollOffset.y, temp);
        bmp->Blit( scrollOffset.x, loc.y, temp);
        bmp->Blit( loc.x, scrollOffset.y, temp);
        bmp->Blit( loc.x, loc.y, temp);

        renderSprite(&temp, (int)(xaxis+offset.x), (int)(yaxis+offset.y), alpha, horizontalFlip, verticalFlip, work);

    } else {
        renderSprite(bmp, (int)(xaxis+offset.x), (int)(yaxis+offset.y), alpha, horizontalFlip, verticalFlip, work);
    }
}

MenuAnimation::MenuAnimation(Token *the_token) throw (LoadException):
id(0),
location(0),
ticks(0),
currentFrame(0),
loop(0),
allowReset(true){
    images[-1] = 0;
    std::string basedir = "";
    if ( *the_token != "anim" ){
        throw LoadException("Not an animation");
    }
    /* The usual setup of an animation is
	The images must be listed prior to listing any frames, basedir can be used to set the directory where the images are located
	loop will begin at the subsequent frame listed after loop
	axis is the location in which the drawing must be placed
	location - used to render in background or foreground (0 == background [default]| 1 == foreground)
	reset - used to allow resetting of animation (0 == no | 1 == yes [default])
	velocity - used to get a wrapping scroll effect while animating
	(anim (id NUM) 
	      (location NUM)
	      (basedir LOCATION)
	      (image NUM FILE) 
	      (velocity x y)
	      (axis x y) 
	      (frame "Read comments above in constructor") 
	      (loop)
	      (reset NUM)
	      (window x1 y1 x2 y2))
    */
    Token tok(*the_token);
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
            } else if (*token == "basedir"){
                // set the base directory for loading images
                *token >> basedir;
            } else if (*token == "image"){
                // add bitmaps by number to the map
                int number;
                std::string temp;
                *token >> number >> temp;
                Bitmap *bmp = new Bitmap(Filesystem::find(basedir + temp));
                if (bmp->getError()){
                    delete bmp;
                } else {
                    images[number] = bmp;
                }
            } else if (*token == "axis"){
                // Get the axis location it defaults to 0,0
                *token >> axis.x >> axis.y;
            } else if (*token == "window"){
                // time to display
                *token >> window.x1 >> window.y1 >> window.x2 >> window.y2;
            } else if (*token == "velocity"){
                // This allows the animation to get a wrapping scroll action going on
                *token >> velocity.x >> velocity.y;
            } else if (*token == "frame"){
                // new frame
                MenuFrame *frame = new MenuFrame(token,images);
                frames.push_back(frame);
            } else if (*token == "loop"){
                // start loop here
                loop = frames.size();
            } else if (*token == "reset"){
                // Allow reset of animation
                *token >> allowReset;
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
        ostringstream out;
        out << "Loop location is larger than the number of frames. Loop: " << loop << " Frames: " << frames.size();
        throw LoadException(out.str());
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
    // Used for scrolling
    for (std::vector<MenuFrame *>::iterator i = frames.begin(); i != frames.end(); ++i){
	(*i)->act(velocity.x, velocity.y);
    }
    if( frames[currentFrame]->time != -1 ){
	ticks++;
	if(ticks >= frames[currentFrame]->time){
		ticks = 0;
		forwardFrame();
	}
    }
}
void MenuAnimation::draw(Bitmap *work){
    /* should use sub-bitmaps here */
     // Set clip from the axis default is 0,0,bitmap width, bitmap height
    work->setClipRect(window.x1,window.y1,work->getWidth() + window.x2,work->getHeight() + window.y2);
    frames[currentFrame]->draw((int) axis.x, (int) axis.y,work);
    work->setClipRect(0,0,work->getWidth(),work->getHeight());
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


