#include "gui/fadetool.h"
#include "util/bitmap.h"
#include "util/trans-bitmap.h"

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
	case RunFade:
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
		setState(RunFade);
	    }
	    break;
	case NoFade:
	case RunFade:
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
	case RunFade:
	default:
	    break;
    }
}

