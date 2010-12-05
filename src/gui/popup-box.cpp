#include "util/bitmap.h"
#include "popup-box.h"

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

void PopupBox::act(const Font & font){
    // do fade
    doFade();
}

void PopupBox::render(const Bitmap & work){
    board.render(work);
}

void PopupBox::open(){
    // Set the fade stuff
    fadeState = FadeIn;
    board.location = location;
    board.colors = colors;
    board.location.center(location);
    board.colors.borderAlpha = board.colors.bodyAlpha = 0;
}

void PopupBox::close(){
    fadeState = FadeOut;
}

void PopupBox::doFade(){
    switch (fadeState){
        case FadeIn: {
            board.location.growTo(location, 0.0025 * fadeSpeed);
            if (board.colors.borderAlpha < colors.borderAlpha){
                board.colors.borderAlpha += (int)(fadeSpeed/2);
                if (board.colors.borderAlpha >= colors.borderAlpha){
                    board.colors.borderAlpha = colors.borderAlpha;
                }
            }
            if (board.colors.bodyAlpha < colors.bodyAlpha){
                board.colors.bodyAlpha += (int)(fadeSpeed/2);
                if (board.colors.bodyAlpha >= colors.bodyAlpha){
                    board.colors.bodyAlpha = colors.bodyAlpha;
                }
            }

            if (board.location == location && board.colors.bodyAlpha == colors.bodyAlpha && board.colors.borderAlpha == colors.borderAlpha){
                fadeState = Open;
            }

            break;
        }
        case FadeOut: {
            Coordinate coord;
            coord.center(location);
            board.location.growTo(coord, 0.0025 * fadeSpeed);

            if (board.colors.borderAlpha > 0){
                board.colors.borderAlpha -= (int)(fadeSpeed/2);
                if (board.colors.borderAlpha <= 0){
                    board.colors.borderAlpha = 0;
                }
            }
            if (board.colors.bodyAlpha > 0){
                board.colors.bodyAlpha -= (int)(fadeSpeed/2);
                if (board.colors.bodyAlpha <= 0){
                    board.colors.bodyAlpha = 0;
                }
            }
            if (board.location.empty() && board.colors.borderAlpha == 0 && board.colors.bodyAlpha == 0){
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
