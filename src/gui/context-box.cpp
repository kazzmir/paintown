#include "util/bitmap.h"

#include "gui/context-box.h"
#include "util/font.h"

static const double FONT_SPACER = 1.3;

using namespace std;
using namespace Gui;

ContextItem::ContextItem(){
}
ContextItem::~ContextItem(){
}

ContextBox::ContextBox():
current(0),
fadeState(NotActive),
fontWidth(0),
fontHeight(0),
fadeSpeed(12),
fadeAlpha(0),
cursorCenter(0),
cursorLocation(0),
scrollWait(4){
}
ContextBox::ContextBox( const ContextBox & copy ):
current(0),
fadeState(NotActive){
    this->context = copy.context;
    this->font = copy.font;
    this->fontWidth = copy.fontWidth;
    this->fontHeight = copy.fontHeight;
    this->fadeSpeed = copy.fadeSpeed;
    this->fadeAlpha = copy.fadeAlpha;
    this->cursorCenter = copy.cursorCenter;
    this->cursorLocation = copy.cursorLocation;
    this->scrollWait = copy.scrollWait;
}
ContextBox::~ContextBox(){
}
ContextBox & ContextBox::operator=( const ContextBox & copy){
    this->current = 0;
    this->fadeState = NotActive;
    this->context = copy.context;
    this->font = copy.font;
    this->fontWidth = copy.fontWidth;
    this->fontHeight = copy.fontHeight;
    this->fadeSpeed = copy.fadeSpeed;
    this->fadeAlpha = copy.fadeAlpha;
    this->cursorCenter = copy.cursorCenter;
    this->cursorLocation = copy.cursorLocation;
    this->scrollWait = copy.scrollWait;
    return *this;
}

void ContextBox::act(){
    // do fade
    doFade();

    // Calculate text info
    calculateText();
}

void ContextBox::render(const Bitmap & work){
    board.render(work);
    drawText(work);
}

bool ContextBox::next(){
    if (fadeState != Active){
	return false;
    }
    const Font & vFont = Font::getFont(font, fontWidth, fontHeight);
    cursorLocation += vFont.getHeight()/FONT_SPACER;
    if (current < context.size()-1){
        current++;
    } else {
        current = 0;
    }
    return true;
}
bool ContextBox::previous(){
    if (fadeState != Active){
	return false;
    }
    const Font & vFont = Font::getFont(font, fontWidth, fontHeight);
    cursorLocation -= vFont.getHeight()/FONT_SPACER;
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
    fadeState = FadeInBox;
    board.position = position;
    board.position.width = board.position.height = 0;
    board.position.x = position.x+(position.width/2);
    board.position.y = position.y+(position.height/2);
    board.position.borderAlpha = board.position.bodyAlpha = 0;
    fadeAlpha = 0;
}

void ContextBox::close(){
    fadeState = FadeOutText;
    fadeAlpha = 255;
}


void ContextBox::doFade(){
    switch ( fadeState ){
	case FadeInBox: {
	    if (board.position.x > position.x){
		board.position.x -= fadeSpeed;
	    } else if ( board.position.x < position.x ){
		board.position.x = position.x;
	    }

	    if (board.position.y > position.y){
		board.position.y -= fadeSpeed;
	    } else if (board.position.y < position.y){
		board.position.y = position.y;
	    }

	    if (board.position.width < position.width){
		board.position.width += (fadeSpeed*2);
	    } else if (board.position.width > position.width){
		board.position.width = position.width;
	    }

	    if (board.position.height < position.height){
		board.position.height += (fadeSpeed*2);
	    } else if (board.position.height > position.height){
		board.position.height = position.height;
	    }
	    
	    if (board.position.borderAlpha < position.borderAlpha){
                board.position.borderAlpha += (int)(fadeSpeed/2);
                if (board.position.borderAlpha >= position.borderAlpha){
                    board.position.borderAlpha = position.borderAlpha;
                }
            }
            if (board.position.bodyAlpha < position.bodyAlpha){
                board.position.bodyAlpha += (int)(fadeSpeed/2);
                if (board.position.bodyAlpha >= position.bodyAlpha){
                    board.position.bodyAlpha = position.bodyAlpha;
                }
            }

	    if (board.position == position){
		fadeState = FadeInText;
                fadeAlpha = 0;
	    }

	    break;
	}
	case FadeInText: {
	    if (fadeAlpha < 255){
		fadeAlpha += (fadeSpeed+2);
	    }

	    if (fadeAlpha >= 255){
		fadeAlpha = 255;
		fadeState = Active;
	    }
	    if (board.position.borderAlpha < position.borderAlpha){
                board.position.borderAlpha += (int)(fadeSpeed/4);
                if (board.position.borderAlpha >= position.borderAlpha){
                    board.position.borderAlpha = position.borderAlpha;
                }
            }
            if (board.position.bodyAlpha < position.bodyAlpha){
                board.position.bodyAlpha += (int)(fadeSpeed/4);
                if (board.position.bodyAlpha >= position.bodyAlpha){
                    board.position.bodyAlpha = position.bodyAlpha;
                }
            }
	    break;
	}
	case FadeOutText: {
	    if (fadeAlpha > 0){
		fadeAlpha -= (fadeSpeed+2);
	    }

	    if (fadeAlpha <= 0){
		fadeAlpha = 0;
		fadeState = FadeOutBox;
	    }
	    if (board.position.borderAlpha > 0){
                board.position.borderAlpha -= (int)(fadeSpeed/4);
                if (board.position.borderAlpha <= 0){
                    board.position.borderAlpha = 0;
                }
            }
            if (board.position.bodyAlpha < 0){
                board.position.bodyAlpha -= (int)(fadeSpeed/4);
                if (board.position.bodyAlpha <= 0){
                    board.position.bodyAlpha = 0;
                }
            }
	    break;
	}
	case FadeOutBox: {
	    const int positionX = position.x+(position.width/2);
	    const int positionY = position.y+(position.height/2);
	    if (board.position.x < positionX){
		board.position.x += fadeSpeed;
	    } else if ( board.position.x > positionX ){
		board.position.x = positionX;
	    }

	    if (board.position.y < positionY){
		board.position.y += fadeSpeed;
	    } else if (board.position.y < positionY){
		board.position.y = positionY;
	    }

	    if (board.position.width > 0){
		board.position.width -= (fadeSpeed*2);
	    } else if (board.position.width < 0){
		board.position.width = 0;
	    }

	    if (board.position.height > 0){
		board.position.height -= (fadeSpeed*2);
	    } else if (board.position.height < 0){
		board.position.height = 0;
	    }

	    if (board.position == RectArea()){
		fadeState = NotActive;
	    }
	    
	    if (board.position.borderAlpha > 0){
                board.position.borderAlpha -= (int)(fadeSpeed/2);
                if (board.position.borderAlpha <= 0){
                    board.position.borderAlpha = 0;
                }
            }
            if (board.position.bodyAlpha > 0){
                board.position.bodyAlpha -= (int)(fadeSpeed/2);
                if (board.position.bodyAlpha <= 0){
                    board.position.bodyAlpha = 0;
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

void ContextBox::calculateText(){
    if (context.empty()){
        return;
    } 
    
    const Font & vFont = Font::getFont(font, fontWidth, fontHeight);
    
    cursorCenter = (position.y + (int)position.height/2) - vFont.getHeight()/2;
    
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

void ContextBox::drawText(const Bitmap & bmp){
    const Font & vFont = Font::getFont(font, fontWidth, fontHeight);
    bmp.setClipRect(board.position.x+2, board.position.y+2, board.position.getX2()-2, board.position.getY2()-2);
    int locationY = cursorLocation;
    int currentOption = current;
    int count = 0;
    Bitmap::transBlender(0, 0, 0, (int)fadeAlpha/255.0);
    while (locationY < position.getX2() + vFont.getHeight()){
        const int startx = (position.width/2)-(vFont.textLength(context[currentOption]->getName().c_str())/2);
        if (count == 0){
            const int color = Bitmap::makeColor(0,255,255);
            vFont.printf(position.x + startx, locationY, color, bmp, context[currentOption]->getName(), 0 );
        } else {
            const int color = Bitmap::makeColor(255,255,255);
            vFont.printf(position.x + startx, locationY, color, bmp, context[currentOption]->getName(), 0 );
        }
        if (context.size() == 1){
            bmp.setClipRect(0, 0, bmp.getWidth(), bmp.getHeight());
            return;
        }
        currentOption++;
        if (currentOption == (int)context.size()){
            currentOption = 0;
        }
        locationY += vFont.getHeight()/FONT_SPACER;
        count++;
        /*if (context.size() < 2 && count == 2){
            break;
        }*/
    }
    locationY = cursorLocation - vFont.getHeight()/FONT_SPACER;
    currentOption = current;
    currentOption--;
    count = 0;
    while (locationY > position.x - vFont.getHeight()){
        if (currentOption < 0){
            currentOption = context.size()-1;
        }
        const int startx = (position.width/2)-(vFont.textLength(context[currentOption]->getName().c_str())/2);
        const int color = Bitmap::makeColor(255,255,255);
        vFont.printf(position.x + startx, locationY, color, bmp, context[currentOption]->getName(), 0 );
        currentOption--;
        locationY -= vFont.getHeight()/FONT_SPACER;
        count++;
        /*if (context.size() < 2 && count == 1){
            break;
        }*/
    }
    Bitmap::drawingMode( Bitmap::MODE_SOLID );
    bmp.setClipRect(0, 0, bmp.getWidth(), bmp.getHeight());
}
