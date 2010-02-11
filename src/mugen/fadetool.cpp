#include "mugen/fadetool.h"
#include "util/bitmap.h"

using namespace Mugen;

FadeTool::FadeTool():
currentState(FadeIn),
lastState(FadeIn),
fader(0),
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
	    fader = 255;
	    break;
	case FadeOut:
	    fader = 0;
	    break;
	case NoFade:
	case RunFade:
	default:
	    fader = 0;
	    break;
    }
}

void FadeTool::act(){
    switch (currentState){
	case FadeIn:
	    fader-=(255/(fadeInTime <= 0 ? 1 : fadeInTime));
	    if (fader<=0){
		setState(NoFade);
	    }
	    break;
	case FadeOut:
	    fader+=(255/(fadeOutTime <= 0 ? 1 : fadeOutTime));
	    if (fader>=255){
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
	    Bitmap::drawingMode(Bitmap::MODE_TRANS);
	    Bitmap::transBlender(0,0,0,fader);
	    bmp.rectangleFill(0, 0, bmp.getWidth(),bmp.getHeight(),fadeInColor);
	    Bitmap::drawingMode(Bitmap::MODE_SOLID);
	    break;
	case FadeOut:
	    Bitmap::drawingMode(Bitmap::MODE_TRANS);
	    Bitmap::transBlender(0,0,0,fader);
	    bmp.rectangleFill(0, 0, bmp.getWidth(),bmp.getHeight(),fadeOutColor);
	    Bitmap::drawingMode(Bitmap::MODE_SOLID);
	    break;
	case NoFade:
	case RunFade:
	default:
	    break;
    }
}

