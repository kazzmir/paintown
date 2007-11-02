#include "util/funcs.h"
#include "util/bitmap.h"
#include "snow_atmosphere.h"

static int screenX(){
	return 320;
}

static int screenY(){
	return 240;
}

SnowAtmosphere::SnowAtmosphere():
Atmosphere(){
	for ( int i = 0; i < 100; i++ ){
		Flake * f = new Flake( Util::rnd( screenX() * 2 ) - screenX() / 2, Util::rnd( screenY() ), Util::rnd( 3 ) );
		flakes.push_back( f );
	}
}

SnowAtmosphere::~SnowAtmosphere(){
	for ( vector< Flake * >::iterator it = flakes.begin(); it != flakes.end(); it++ ){
		delete *it;
	}
}

static void drawFlake0( Flake * f, Bitmap * work ){
	work->circleFill( f->x, f->y, 2, Bitmap::makeColor( 255, 255, 255 ) );
}

void SnowAtmosphere::draw( Bitmap * work ){
	for ( vector< Flake * >::iterator it = flakes.begin(); it != flakes.end(); it++ ){
		Flake * f = *it;
		switch ( f->type ){
			default : {
				drawFlake0( f, work );
				break;
			}
		}
	}
}

void SnowAtmosphere::act(){
	for ( vector< Flake * >::iterator it = flakes.begin(); it != flakes.end(); it++ ){
		Flake * f = *it;
		f->y += 1;
		f->x += f->dir;
		f->dir += Util::rnd( 2 ) * 2 - 1;
		if ( f->dir > 2 ){
			f->dir = 2;
		}
		if ( f->dir < -2 ){
			f->dir = -2;
		}
		if ( f->y > screenY() ){
			f->y = - Util::rnd( 30 );
		}
		if ( f->x < -50 ){
			f->x = -50;
		}
		if ( f->x > screenX() + 50 ){
			f->x = screenX() + 50;
		}
	}
}
