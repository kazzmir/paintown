#include "util/bitmap.h"
#include "util/trans-bitmap.h"

#include "context-box.h"
#include "util/font.h"

static const double FONT_SPACER = 1.3;
static const int GradientMax = 50;

static int selectedGradientStart(){
    static int color = Bitmap::makeColor(19, 167, 168);
    return color;
}

static int selectedGradientEnd(){
    static int color = Bitmap::makeColor(27, 237, 239);
    return color;
}

using namespace std;
using namespace Gui;

ContextItem::ContextItem(){
}
ContextItem::~ContextItem(){
}
bool ContextItem::isAdjustable(){
    return false;
}
int ContextItem::getLeftColor(){
    return 0;
}
int ContextItem::getRightColor(){
    return 0;
}

ContextBox::ContextBox():
current(0),
fadeState(NotActive),
/*
fontWidth(0),
fontHeight(0),
*/
fadeSpeed(12),
fadeAlpha(0),
cursorCenter(0),
cursorLocation(0),
scrollWait(4),
selectedGradient(GradientMax, selectedGradientStart(), selectedGradientEnd()),
useGradient(true),
renderOnlyText(false){
}
ContextBox::ContextBox( const ContextBox & copy ):
current(0),
fadeState(NotActive),
selectedGradient(GradientMax, selectedGradientStart(), selectedGradientEnd()),
renderOnlyText(false){
    this->context = copy.context;
    /*
    this->font = copy.font;
    this->fontWidth = copy.fontWidth;
    this->fontHeight = copy.fontHeight;
    */
    this->fadeSpeed = copy.fadeSpeed;
    this->fadeAlpha = copy.fadeAlpha;
    this->cursorCenter = copy.cursorCenter;
    this->cursorLocation = copy.cursorLocation;
    this->scrollWait = copy.scrollWait;
    this->useGradient = copy.useGradient;
    this->renderOnlyText = copy.renderOnlyText;
}
ContextBox::~ContextBox(){
}
ContextBox & ContextBox::operator=( const ContextBox & copy){
    this->current = 0;
    this->fadeState = NotActive;
    this->context = copy.context;
    /*
    this->font = copy.font;
    this->fontWidth = copy.fontWidth;
    this->fontHeight = copy.fontHeight;
    */
    this->fadeSpeed = copy.fadeSpeed;
    this->fadeAlpha = copy.fadeAlpha;
    this->cursorCenter = copy.cursorCenter;
    this->cursorLocation = copy.cursorLocation;
    this->scrollWait = copy.scrollWait;
    this->useGradient = copy.useGradient;
    this->renderOnlyText = copy.renderOnlyText;
    return *this;
}

void ContextBox::act(const Font & font){
    // update board
    board.act(font);
    
    // Calculate text info
    calculateText(font);
    
    // do fade
    doFade();
    
    // Update gradient
    selectedGradient.update();
}

void ContextBox::render(const Bitmap & work){
}

void ContextBox::render(const Bitmap & work, const Font & font){

    if (!renderOnlyText){
	board.render(work);
    }
    drawText(work, font);
}

bool ContextBox::next(const Font & font){
    if (fadeState == FadeOut){
	return false;
    }

    /*
    const Font & vFont = Font::getFont(font, fontWidth, fontHeight);
    */
    cursorLocation += (int)(font.getHeight()/FONT_SPACER);

    if (current < context.size()-1){
        current++;
    } else {
        current = 0;
    }
    return true;
}

bool ContextBox::previous(const Font & font){
    if (fadeState == FadeOut){
	return false;
    }

    /*
    const Font & vFont = Font::getFont(font, fontWidth, fontHeight);
    */
    cursorLocation -= (int)(font.getHeight()/FONT_SPACER);

    if (current > 0){
        current--;
    } else {
        current = context.size()-1;
    }
    return true;
}

void ContextBox::adjustLeft(){
}

void ContextBox::adjustRight(){
}

void ContextBox::open(){
    // Set the fade stuff
    fadeState = FadeIn;
    //board.position = position;
    board.location = location;
    board.colors = colors;
    board.open();
    fadeAlpha = 0;
    cursorLocation = 0;
}

void ContextBox::close(){
    fadeState = FadeOut;
    board.close();
    fadeAlpha = 255;
    cursorLocation = 480;
}


void ContextBox::doFade(){
    switch ( fadeState ){
	case FadeIn: {
	    if (fadeAlpha < 255){
		fadeAlpha += (fadeSpeed+2);
	    }

	    if (fadeAlpha >= 255){
		fadeAlpha = 255;
                if (board.isActive()){
                    fadeState = Active;
                }
	    }
	    break;
	}
	case FadeOut: {
	    if (fadeAlpha > 0){
		fadeAlpha -= (fadeSpeed+2);
	    }

	    if (fadeAlpha <= 0){
		fadeAlpha = 0;
                if (!board.isActive()){
                    fadeState = NotActive;
                }
	    }
	    break;
	}
	case Active:
	case NotActive:
	default:
	    break;
    }
}

void ContextBox::calculateText(const Font & vFont){
    if (context.empty()){
        return;
    } 
    
    // const Font & vFont = Font::getFont(font, fontWidth, fontHeight);
    
    cursorCenter = (location.getY() + (int)location.getHeight()/2) - vFont.getHeight()/2;//(position.y + (int)position.height/2) - vFont.getHeight()/2;
    
    if (cursorLocation == cursorCenter){
        scrollWait = 4;
    } else {
	if (scrollWait <= 0){
	    cursorLocation = (cursorLocation + cursorCenter)/2;
	    scrollWait = 4;
	} else {
	    scrollWait--;
	}
    }
}

void ContextBox::drawText(const Bitmap & bmp, const Font & vFont){
    if (context.empty()){
        return;
    }
    // const Font & vFont = Font::getFont(font, fontWidth, fontHeight);
    const int x1 = board.getArea().getX()+(int)(board.getArea().getRadius()/2);
    const int y1 = board.getArea().getY()+2;//(board.getArea().radius/2);
    const int x2 = board.getArea().getX2()-(int)(board.getArea().getRadius()/2);
    const int y2 = board.getArea().getY2()-2;//(board.getArea().radius/2);
            
    bmp.setClipRect(x1, y1, x2, y2);
    int locationY = cursorLocation;
    int currentOption = current;
    int count = 0;
    while (locationY < location.getX2() + vFont.getHeight()){
        const int startx = (location.getWidth()/2)-(vFont.textLength(context[currentOption]->getName().c_str())/2);
        if (count == 0){
            Bitmap::transBlender(0, 0, 0, fadeAlpha);
            TranslucentBitmap translucent(bmp);
            // Bitmap::drawingMode( Bitmap::MODE_TRANS );
            const int color = useGradient ? selectedGradient.current() : selectedGradientStart();
            vFont.printf(location.getX() + startx, locationY, color, translucent, context[currentOption]->getName(), 0 );
            if (context[currentOption]->isAdjustable()){
                const int triangleSize = 14;
                int cx = (location.getX() + startx) - 15;
                int cy = (int)(locationY + (vFont.getHeight()/FONT_SPACER) / 2 + 2);

                /*
                int cx1 = cx + triangleSize / 2;
                int cy1 = cy - triangleSize / 2;
                int cx2 = cx - triangleSize;
                int cy2 = cy;
                int cx3 = cx + triangleSize / 2;
                int cy3 = cy + triangleSize / 2;
                */

                /* do the triangles need to be translucent? */
                // translucent.triangle(cx1, cy1, cx2, cy2, cx3, cy3, context[currentOption]->getLeftColor());
                translucent.equilateralTriangle(cx, cy, 180, triangleSize, context[currentOption]->getLeftColor());

                cx = (location.getX()+startx + vFont.textLength(context[currentOption]->getName().c_str()))+15;
                translucent.equilateralTriangle(cx, cy, 0, triangleSize, context[currentOption]->getLeftColor());
                // translucent.triangle( cx - triangleSize / 2, cy - triangleSize / 2, cx + triangleSize, cy, cx - triangleSize / 2, cy + triangleSize / 2, context[currentOption]->getRightColor() );
            }
            // Bitmap::drawingMode(Bitmap::MODE_SOLID);
        } else {
            int textAlpha = fadeAlpha - (count * 35);
            if (textAlpha < 0){
                textAlpha = 0;
            }
            Bitmap::transBlender(0, 0, 0, textAlpha);
            // Bitmap::drawingMode( Bitmap::MODE_TRANS );
            const int color = Bitmap::makeColor(255,255,255);
            vFont.printf(location.getX() + startx, locationY, color, bmp.translucent(), context[currentOption]->getName(), 0 );
            // Bitmap::drawingMode( Bitmap::MODE_SOLID );
        }
        if (context.size() == 1){
            bmp.setClipRect(0, 0, bmp.getWidth(), bmp.getHeight());
            return;
        }
        currentOption++;
        if (currentOption == (int)context.size()){
            currentOption = 0;
        }
        locationY += (int)(vFont.getHeight()/FONT_SPACER);
        count++;
        /*if (context.size() < 2 && count == 2){
            break;
        }*/
    }
    locationY = cursorLocation - (int)(vFont.getHeight()/FONT_SPACER);
    currentOption = current;
    currentOption--;
    count = 0;
    while (locationY > location.getX() - vFont.getHeight()){
        if (currentOption < 0){
            currentOption = context.size()-1;
        }
        const int startx = (location.getWidth()/2)-(vFont.textLength(context[currentOption]->getName().c_str())/2);
        int textAlpha = fadeAlpha - (count * 35);
        if (textAlpha < 0){
            textAlpha = 0;
        }
        Bitmap::transBlender(0, 0, 0, textAlpha);
        // Bitmap::drawingMode( Bitmap::MODE_TRANS );
        const int color = Bitmap::makeColor(255,255,255);
        vFont.printf(location.getX() + startx, locationY, color, bmp.translucent(), context[currentOption]->getName(), 0 );
        // Bitmap::drawingMode( Bitmap::MODE_SOLID );
        currentOption--;
        locationY -= (int)(vFont.getHeight()/FONT_SPACER);
        count++;
        /*if (context.size() < 2 && count == 1){
            break;
        }*/
    }
    bmp.setClipRect(0, 0, bmp.getWidth(), bmp.getHeight());
}

