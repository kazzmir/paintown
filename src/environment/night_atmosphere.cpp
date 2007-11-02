#include "util/funcs.h"
#include "util/bitmap.h"
#include "night_atmosphere.h"

static int screenX(){
	return 320;
}

static int screenY(){
	return 240;
}

NightAtmosphere::NightAtmosphere():
Atmosphere(){
	night = new Bitmap( screenX(), screenY() );
	night->clear();
}

NightAtmosphere::~NightAtmosphere(){
	delete night;
}

void NightAtmosphere::draw( Bitmap * work ){
	Bitmap::transBlender( 0, 0, 0, 128 );
	night->drawTrans( 0, 0, *work );
}

void NightAtmosphere::act(){
}
