#include "gui/fadetool.h"
#include "util/bitmap.h"
#include "util/trans-bitmap.h"
#include "util/token.h"
#include "util/load_exception.h"

using namespace Gui;

FadeTool::FadeTool():
currentState(FadeIn),
lastState(FadeIn),
fadeTime(0),
fadeInTime(0),
fadeOutTime(0),
fadeInColor(Bitmap::makeColor(0,0,0)),
fadeOutColor(Bitmap::makeColor(0,0,0)){
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
    /* FIXME: this code looks like its expecting a structure like
     *  (in (color ...) (out ...))
     * But its not parsing that way, rewrite it
     */
    while (view.hasMore()){
        try{
            const Token * tok;
            view >> tok;
            if (*tok == "in"){
                TokenView inView = tok->view();
                /* FIXME: this loop is pretty wierd, shouldn't the if's be on the outside? */
                while (inView.hasMore()){
                    if (*tok == "color"){
                        int r=0,g=0,b=0;
                        try {
                            inView >> r >> g >> b;
                        } catch (const TokenException & ex){
                        }
                        setFadeInColor(Bitmap::makeColor(r,b,g));
                    } else if (*tok == "time"){
                        int time=0;
                        try {
                            inView >> time;
                        } catch (const TokenException & ex){
                        }
                        setFadeInTime(time);
                    }
                }
            } else if (*tok == "out"){
                /*
                Token & out = *tok;
                while (out.hasTokens()){
                    if (out == "color"){
                        int r=0,g=0,b=0;
                        try {
                            out >> r >> g >> b;
                        } catch (const TokenException & ex){
                        }
                        setFadeOutColor(Bitmap::makeColor(r,b,g));
                    } else if (out == "time"){
                        int time=0;
                        try {
                            out >> time;
                        } catch (const TokenException & ex){
                        }
                        setFadeOutTime(time);
                    }
                }
            */
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
	    fadeTime-=(255/(fadeInTime <= 0 ? 1 : fadeInTime));
	    if (fadeTime<=0){
		setState(NoFade);
	    }
	    break;
	case FadeOut:
	    fadeTime+=(255/(fadeOutTime <= 0 ? 1 : fadeOutTime));
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

void FadeTool::draw(const Bitmap &bmp){
    switch (currentState){
	case FadeIn:
	    // Bitmap::drawingMode(Bitmap::MODE_TRANS);
	    Bitmap::transBlender(0,0,0,fadeTime);
	    bmp.translucent().rectangleFill(0, 0, bmp.getWidth(),bmp.getHeight(),fadeInColor);
	    // Bitmap::drawingMode(Bitmap::MODE_SOLID);
	    break;
	case FadeOut:
	    // Bitmap::drawingMode(Bitmap::MODE_TRANS);
	    Bitmap::transBlender(0,0,0,fadeTime);
	    bmp.translucent().rectangleFill(0, 0, bmp.getWidth(),bmp.getHeight(),fadeOutColor);
	    // Bitmap::drawingMode(Bitmap::MODE_SOLID);
	    break;
	case NoFade:
	case EndFade:
	default:
	    break;
    }
}

