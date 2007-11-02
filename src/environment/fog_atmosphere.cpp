#include "util/funcs.h"
#include "util/bitmap.h"
#include "fog_atmosphere.h"
#include <math.h>

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

	for ( int i = 0; i < screenX(); i += 20 ){
		for ( int q = 0; q < 3; q++ ){
			fogs.push_back( new Fog( i + Util::rnd( 9 ) - 4, screenY() - Util::rnd( 30 ) - 40, Util::rnd( 360 ) ) );
		}
	}
}

FogAtmosphere::~FogAtmosphere(){
	delete fog;
	for ( vector< Fog * >::iterator it = fogs.begin(); it != fogs.end(); it++ ){
		delete *it;
	}
}

void FogAtmosphere::draw( Bitmap * work ){
	Bitmap::transBlender( 0, 0, 0, 64 );
	for ( vector< Fog * >::iterator it = fogs.begin(); it != fogs.end(); it++ ){
		Fog * f = *it;
		int y = (int)(f->y + sin( f->ang * 3.14159 / 180.0 ) * 4);
		fog->drawTrans( f->x, y, *work );
	}
	/*
	screenX();
	for ( int i = -10; i < screenX() + 10; i += 15 ){
		fog->drawTrans( i, screenY() - 50, *work );
	}
	*/
}

void FogAtmosphere::act(){
	for ( vector< Fog * >::iterator it = fogs.begin(); it != fogs.end(); it++ ){
		Fog * f = *it;
		f->ang += 1;
	}
}
