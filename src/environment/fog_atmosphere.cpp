#include "util/funcs.h"
#include "util/bitmap.h"
#include "fog_atmosphere.h"

static int screenX(){
	return 320;
}

static int screenY(){
	return 240;
}

FogAtmosphere::FogAtmosphere():
Atmosphere(){
	fog = new Bitmap( 50, 50 );
	fog->fill( Bitmap::MaskColor );
	fog->circleFill( 25, 25, 20, Bitmap::makeColor( 0xbb, 0xbb, 0xcc ) );
}

FogAtmosphere::~FogAtmosphere(){
	delete fog;
}

void FogAtmosphere::draw( Bitmap * work ){
	Bitmap::transBlender( 0, 0, 0, 128 );
	screenX();
	fog->drawTrans( 30, screenY() - 50, *work );
}

void FogAtmosphere::act(){
}
