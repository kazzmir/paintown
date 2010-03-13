#include "util/bitmap.h"

#include "gui/popup-box.h"

using namespace std;
using namespace Gui;

PopupBox::PopupBox():
fadeState(Closed),
fadeSpeed(12){
}
PopupBox::PopupBox( const PopupBox & copy ):
fadeState(Closed){
    this->fadeSpeed = copy.fadeSpeed;
}
PopupBox::~PopupBox(){
}
PopupBox & PopupBox::operator=( const PopupBox & copy){
    this->fadeState = Closed;
    this->fadeSpeed = copy.fadeSpeed;
    return *this;
}

void PopupBox::act(){
    // do fade
    doFade();
}

void PopupBox::render(const Bitmap & work){
    board.render(work);
}

void PopupBox::open(){
    // Set the fade stuff
    fadeState = FadeIn;
    board.position = position;
    board.position.width = board.position.height = 0;
    board.position.x = position.x+(position.width/2);
    board.position.y = position.y+(position.height/2);
    board.position.borderAlpha = board.position.bodyAlpha = 0;
}

void PopupBox::close(){
    fadeState = FadeOut;
}


void PopupBox::doFade(){
    switch ( fadeState ){
	case FadeIn: {
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

	    if (board.position == position && board.position.bodyAlpha == position.bodyAlpha && board.position.borderAlpha == position.borderAlpha){
		fadeState = Open;
	    }

	    break;
	}
	case FadeOut: {
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
            
            if (board.position == RectArea() && board.position.borderAlpha == 0 && board.position.bodyAlpha == 0){
                fadeState = Closed;
            }
	    break;
	}
	case Open:
	case Closed:
	default:
	    break;
    }
}
