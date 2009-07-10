#include "util/funcs.h"
#include "util/bitmap.h"
#include "rain_atmosphere.h"
#include "level/scene.h"

using namespace std;

static int screenX(){
	return 320;
}

static int screenY(){
	return 240;
}

RainAtmosphere::RainAtmosphere():
Atmosphere(),
playing( false ){

	rain_sound = Sound( Util::getDataPath() + "sounds/rain.wav" );

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
	for ( vector<Puddle *>::iterator it = puddles.begin(); it != puddles.end(); it++ ){
		delete *it;
	}
}

void RainAtmosphere::drawBackground(Bitmap * work, int x){
    const int bluish = Bitmap::makeColor(106, 184, 225);
    Bitmap::transBlender(0, 0, 0, 192);
    Bitmap::drawingMode(Bitmap::MODE_TRANS);
    for (vector<Puddle*>::iterator it = puddles.begin(); it != puddles.end(); it++){
        Puddle * puddle = *it;
        work->circle(puddle->x, puddle->y, (int)puddle->current, bluish);
    }
    Bitmap::drawingMode(Bitmap::MODE_SOLID);
}

void RainAtmosphere::drawForeground(Bitmap * work, int x){
}
	
void RainAtmosphere::drawFront(Bitmap * work, int x){
}

void RainAtmosphere::drawScreen(Bitmap * work, int x){
    for ( vector< Drop * >::iterator it = rain_drops.begin(); it != rain_drops.end(); it++ ){
        Drop * d = *it;
        work->line( d->x, d->y, d->x + d->length * 2 / 3, d->y + d->length, d->color );
    }
}

void RainAtmosphere::act(const Scene & level){

	if ( ! playing ){
		rain_sound.playLoop();
		playing = true;
	}

        for (vector<Puddle*>::iterator it = puddles.begin(); it != puddles.end(); ){
            Puddle * puddle = *it;
            puddle->current += 0.3;
            if (puddle->current >= puddle->size){
                delete puddle;
                it = puddles.erase(it);
            } else {
                it++;
            }
        }

        while (puddles.size() < 20){
            int x = Util::rnd(screenX());
            int y = Util::rnd(level.getMinimumZ(), level.getMaximumZ());
            int size = Util::rnd(4) + 2;
            puddles.push_back(new Puddle(x, y, size));
        }

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
