#include "mugen/fadetool.h"
#include "util/bitmap.h"

MugenFadeTool::MugenFadeTool():
currentState(FADEIN),
lastState(FADEIN),
fader(0),
fadeInTime(0),
fadeOutTime(0),
fadeInColor(Bitmap::makeColor(0,0,0)),
fadeOutColor(Bitmap::makeColor(0,0,0)){
}

MugenFadeTool::~MugenFadeTool(){
}
	

void MugenFadeTool::setState( FadeType f){
    lastState = currentState;
    currentState = f;
    
    switch (currentState){
	case FADEIN:
	    fader = 255;
	    break;
	case FADEOUT:
	    fader = 0;
	    break;
	case NOFADE:
	case RUNFADE:
	default:
	    fader = 0;
	    break;
    }
}

void MugenFadeTool::act(){
    switch (currentState){
	case FADEIN:
	    fader-=(255/(fadeInTime <= 0 ? 1 : fadeInTime));
	    if (fader<=0){
		setState(NOFADE);
	    }
	    break;
	case FADEOUT:
	    fader+=(255/(fadeOutTime <= 0 ? 1 : fadeOutTime));
	    if (fader>=255){
		setState(RUNFADE);
	    }
	    break;
	case NOFADE:
	case RUNFADE:
	default:
	    break;
    }
}

void MugenFadeTool::draw(const Bitmap &bmp){
    switch (currentState){
	case FADEIN:
	    Bitmap::drawingMode(Bitmap::MODE_TRANS);
	    Bitmap::transBlender(0,0,0,fader);
	    bmp.rectangleFill(0, 0, bmp.getWidth(),bmp.getHeight(),fadeInColor);
	    Bitmap::drawingMode(Bitmap::MODE_SOLID);
	    break;
	case FADEOUT:
	    Bitmap::drawingMode(Bitmap::MODE_TRANS);
	    Bitmap::transBlender(0,0,0,fader);
	    bmp.rectangleFill(0, 0, bmp.getWidth(),bmp.getHeight(),fadeOutColor);
	    Bitmap::drawingMode(Bitmap::MODE_SOLID);
	    break;
	case NOFADE:
	case RUNFADE:
	default:
	    break;
    }
}
