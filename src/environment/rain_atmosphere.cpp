#include "util/funcs.h"
#include "util/bitmap.h"
#include "rain_atmosphere.h"

static int screenX(){
	return 320;
}

static int screenY(){
	return 240;
}

RainAtmosphere::RainAtmosphere():
Atmosphere(){

	int colors[ 2 ];
	colors[ 0 ] = Bitmap::makeColor( 0x22, 0x66, 0x66 );
	colors[ 1 ] = Bitmap::makeColor( 0x11, 0x44, 0x77 );
	for ( int i = 0; i < 100; i++ ){
		Drop * d = new Drop( Util::rnd( screenX() * 2 ) - screenX() / 2, Util::rnd( screenY() ), Util::rnd( 4 ) + 3, colors[ Util::rnd( 2 ) ] );
		rain_drops.push_back( d );
	}
}

RainAtmosphere::~RainAtmosphere(){
	for ( vector< Drop * >::iterator it = rain_drops.begin(); it != rain_drops.end(); it++ ){
		delete *it;
	}
}

void RainAtmosphere::draw( Bitmap * work ){
	for ( vector< Drop * >::iterator it = rain_drops.begin(); it != rain_drops.end(); it++ ){
		Drop * d = *it;
		work->line( d->x, d->y, d->x + d->length * 2 / 3, d->y + d->length, d->color );
	}
}

void RainAtmosphere::act(){

	for ( vector< Drop * >::iterator it = rain_drops.begin(); it != rain_drops.end(); it++ ){
		Drop * d = *it;
		d->y += 7;
		d->x += 3;
		if ( d->y > screenY() ){
			d->y = -Util::rnd( 100 ) - 20;
		}
		if ( d->x > screenX() ){
			d->x -= screenX();
		}
	}

}
