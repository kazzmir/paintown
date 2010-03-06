#include "util/bitmap.h"

#include "gui/context-box.h"
#include "util/font.h"

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
fadeAlpha(0){
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
    return *this;
}

void ContextBox::act(){
    // do fade
    doFade();
}

void ContextBox::render(const Bitmap & work){
    board.render(work);
    work.setClipRect(position.x+2, position.y+2, position.getX2()-2, position.getY2()-2);
    drawText(work);
    work.setClipRect(0, 0, work.getWidth(), work.getHeight());
}

void ContextBox::next(){
    if (fadeState != Active){
	return;
    }
}
void ContextBox::previous(){
    if (fadeState != Active){
	return;
    }
}
void ContextBox::open(){
    // Set the fade stuff
    fadeState = FadeInBox;
    board.position = position;
    board.position.width = board.position.height = 0;
    board.position.x = position.x+(position.width/2);
    board.position.y = position.y+(position.height/2);
    fadeAlpha = 0;
}

void ContextBox::close(){
    fadeState = FadeOutText;
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

	    if (board.position == position){
		fadeState = FadeInText;
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

	    break;
	}
	case Active:
	case NotActive:
	default:
	    break;
    }
}

void ContextBox::drawText(const Bitmap & bmp){
    const Font & vFont = Font::getFont(font, fontWidth, fontHeight);
    const double spacing = 1.3;
}
