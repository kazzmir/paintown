#include "r-tech1/gui/fadetool.h"
#include "r-tech1/graphics/bitmap.h"
#include "r-tech1/token.h"
#include "r-tech1/exceptions/load_exception.h"

#include "r-tech1/debug.h"

using namespace Gui;

FadeTool::FadeTool():
currentState(FadeIn),
lastState(FadeIn),
fadeTime(255),
fadeInTime(0),
fadeOutTime(0),
fadeInColor(Graphics::makeColor(0,0,0)),
fadeOutColor(Graphics::makeColor(0,0,0)){
}

FadeTool::~FadeTool(){
}

void FadeTool::parseDefaults(const Token * token){
    if ( *token != "fade" ){
        throw LoadException(__FILE__, __LINE__, "Not a fader");
    }
    /*!
     * (fade 
        (in (color 0 0 0) (time 0) ) 
        (out (color 0 0 0) (time 0) ))
    */
    const Token & ourToken = *token;
    TokenView view = ourToken.view();
    
    while (view.hasMore()){
        try{
            const Token * tok;
            view >> tok;
            if (*tok == "in"){
                readFade(tok, FadeIn);
            } else if (*tok == "out"){
                readFade(tok, FadeOut);
            } 
        } catch ( const TokenException & ex ) {
            throw LoadException(__FILE__, __LINE__, ex, "Fade tool parse error");
        } catch ( const LoadException & ex ) {
            throw ex;
        }
    }
}

void FadeTool::setState( const State & f){
    lastState = currentState;
    currentState = f;
    
    switch (currentState){
        case FadeIn:
            fadeTime = 255;
            break;
        case FadeOut:
            fadeTime = 0;
            break;
        case NoFade:
        case EndFade:
        default:
            fadeTime = 0;
            break;
    }
}

void FadeTool::act(){
    switch (currentState){
        case FadeIn:
            fadeTime -= (255/(fadeInTime <= 0 ? 1 : fadeInTime));
            if (fadeTime<=0){
                setState(NoFade);
            }
            break;
        case FadeOut:
            fadeTime += (255/(fadeOutTime <= 0 ? 1 : fadeOutTime));
            if (fadeTime>=255){
                setState(EndFade);
            }
            break;
        case NoFade:
        case EndFade:
        default:
            break;
    }
}

void FadeTool::draw(const Graphics::Bitmap &bmp){
    switch (currentState){
        case FadeIn:
            // Graphics::Bitmap::transBlender(0,0,0, fadeTime);
            bmp.translucent(fadeTime).rectangleFill(0, 0, bmp.getWidth(), bmp.getHeight(), fadeInColor);
            break;
        case FadeOut:
            // Graphics::Bitmap::transBlender(0,0,0, fadeTime);
            bmp.translucent(fadeTime).rectangleFill(0, 0, bmp.getWidth(), bmp.getHeight(), fadeOutColor);
            break;
        case NoFade:
        case EndFade:
        default:
            break;
    }
}

void FadeTool::readFade(const Token * token, const State & type){
    TokenView view = token->view();
    while (view.hasMore()){
        try{
            const Token * tok;
            view >> tok;
            if (*tok == "color"){
                int red = 0, green = 0, blue = 0;
                try {
                    tok->view() >> red >> green >> blue;
                } catch (const TokenException & ex){
                }
                if (type == FadeIn){
                    setFadeInColor(Graphics::makeColor(red, green, blue));
                } else if (type == FadeOut){
                    setFadeOutColor(Graphics::makeColor(red, green, blue));
                }
            } else if ( *tok == "time" ){
                int time=0;
                try {
                    tok->view() >> time;
                } catch (const TokenException & ex){
                }
                if (type == FadeIn){
                    setFadeInTime(time);
                } else if (type == FadeOut){
                    setFadeOutTime(time);
                }
            } else {
                Global::debug(3) << "Unhandled FadeTool attribute: " << std::endl;
                if (Global::getDebug() >= 3){
                    tok->print(" ");
                }
            }
        } catch ( const TokenException & ex ) {
            throw LoadException(__FILE__, __LINE__, ex, "FadeTool parse error");
        } catch ( const LoadException & ex ) {
            throw ex;
        }
    }
}
