#include "util/funcs.h"
#include "util/bitmap.h"
#include "snow_atmosphere.h"
#include <math.h>

using namespace std;

static int screenX(){
	return 320;
}

static int screenY(){
	return 240;
}

SnowAtmosphere::SnowAtmosphere():
Atmosphere(){
	for ( int i = 0; i < 150; i++ ){
		Flake * f = new Flake(Util::rnd( screenX() * 2 ) - screenX() / 2, Util::rnd( screenY() ), Util::rnd(360), Util::rnd( 3 ) );
		flakes.push_back( f );
	}
}

SnowAtmosphere::~SnowAtmosphere(){
	for ( vector< Flake * >::iterator it = flakes.begin(); it != flakes.end(); it++ ){
		delete *it;
	}
}

static void drawFlake0( Flake * f, Bitmap * work ){
    int c = (int)(200 + 3 * log((double)(f->y < 1 ? 1 : 2 * f->y)));
    if (c > 255){
        c = 255;
    }
    int color = Bitmap::makeColor(c, c, c);
    // work->circleFill( f->x, f->y, 1, color );
    double pi = 3.141592526;
    double rads = f->angle * pi / 180;
    double rads2 = rads + pi / 2.0;
    double length = 1.5;

    int x1 = (int)(f->x - length * cos(rads));
    int y1 = (int)(f->y + length * sin(rads));
    int x2 = (int)(f->x + length * cos(rads));
    int y2 = (int)(f->y - length * sin(rads));
    work->line(x1, y1, x2, y2, color);

    x1 = (int)(f->x - length * cos(rads2));
    y1 = (int)(f->y + length * sin(rads2));
    x2 = (int)(f->x + length * cos(rads2));
    y2 = (int)(f->y - length * sin(rads2));
    work->line(x1, y1, x2, y2, color);

    /*
    work->line( f->x - 1, f->y - 1, f->x + 1, f->y + 1, color );
    work->line( f->x + 1, f->y - 1, f->x - 1, f->y + 1, color );
    */
}

void SnowAtmosphere::draw(Bitmap * work, int x){
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
		f->dy += 0.40;
		f->y = (int) f->dy;
		f->dx += (double)f->dir / 4.3;
		f->x = (int) f->dx;
		f->dir += Util::rnd( 2 ) * 2 - 1;
                f->angle = (f->angle + f->spin + 360) % 360;
                f->spin += Util::rnd(11) - 5;
		if ( f->dir > 3 ){
			f->dir = 3;
		}
		if ( f->dir < -3 ){
			f->dir = -3;
		}
		if ( f->y >= screenY() ){
			f->y = - Util::rnd( 30 );
			f->dy = f->y;
                        f->x = Util::rnd(screenX());
                        f->dx = f->x;
		}
		if ( f->x < -50 ){
			f->x = -50;
			f->dx = f->x;
		}
		if ( f->x > screenX() + 50 ){
			f->x = screenX() + 50;
			f->dx = f->x;
		}
	}
}
