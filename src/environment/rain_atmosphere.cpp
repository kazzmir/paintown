#include "util/funcs.h"
#include "util/bitmap.h"
#include "rain_atmosphere.h"

RainAtmosphere::RainAtmosphere():
Atmosphere(){

	for ( int i = 0; i < 100; i++ ){
		Drop * d = new Drop( Util::rnd( 320 ), Util::rnd( 240 ), Util::rnd( 4 ) + 3 );
		rain_drops.push_back( d );
	}
}

RainAtmosphere::~RainAtmosphere(){
}

void RainAtmosphere::draw( Bitmap * work ){
	int color = Bitmap::makeColor( 0, 0, 250 );
	for ( vector< Drop * >::iterator it = rain_drops.begin(); it != rain_drops.end(); it++ ){
		Drop * d = *it;
		work->vLine( d->y, d->x, d->y + d->length, color );
	}
}

void RainAtmosphere::act(){

	for ( vector< Drop * >::iterator it = rain_drops.begin(); it != rain_drops.end(); it++ ){
		Drop * d = *it;
		d->y += 8;
		if ( d->y > 240 ){
			d->y = -Util::rnd( 100 ) - 20;
		}
	}

}
