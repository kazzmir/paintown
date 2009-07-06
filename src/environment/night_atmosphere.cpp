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
    int center_x = screenX() / 2;
    // int center_x = screenX();
    const int black = Bitmap::makeColor(0,0,0);
    int width = 50;
    Bitmap save = Bitmap::temporaryBitmap(width * 2, screenY());
    work->Blit(center_x - width, 0, width * 2, screenY(), 0, 0, save);
    Bitmap::transBlender(0, 0, 0, 128);
    night->drawTrans( 0, 0, *work );
    Bitmap::drawingMode(Bitmap::MODE_TRANS);
    int top = -save.getHeight() / 3;
    top = 30;
    int lamp_height = save.getHeight() - top;
    int lamp_top = 30;
    // int top = 0;
    save.triangle(0, top, save.getWidth() / 2, top, 0, save.getHeight(), black);
    save.triangle(save.getWidth(), top, save.getWidth() / 2, top, save.getWidth(), save.getHeight(), black);
    int nwidth = (double) lamp_top / ((double) lamp_height / (double) width);
    save.triangle(save.getWidth() / 2, top, save.getWidth() / 2 - nwidth, top + lamp_top, save.getWidth() / 2 + nwidth, top + lamp_top, black);

    save.rectangleFill(0, 0, save.getWidth(), top, black);
    Bitmap::drawingMode(Bitmap::MODE_SOLID);
    int xwidth = (double) lamp_height / ((double)(save.getHeight() - top) / (double) width);
    save.light(save.getWidth() / 2, top, xwidth, lamp_height, lamp_top, 0, 128, Bitmap::makeColor(32,32,0), Bitmap::makeColor(0,0,0));
    // save.light(save.getWidth() / 2, top, xwidth, lamp_height, 0, 128, Bitmap::makeColor(32,32,0), Bitmap::makeColor(0,0,0));
    // save.light(save.getWidth() / 2, top, width, save.getHeight(), 32, 128, Bitmap::makeColor(32,32,0), Bitmap::makeColor(0,0,0));
    // save.light(save.getWidth() / 2, 0, width, save.getHeight(), 0, 128, Bitmap::makeColor(0,0,0));
    // save.light(save.getWidth() / 2, 0, width, save.getHeight(), 128, 255, Bitmap::makeColor(0,0,0));
    save.draw(center_x - width, 0, *work);

    /*
    Bitmap::drawingMode(Bitmap::MODE_TRANS);
    Bitmap::transBlender(0,0,0, alpha);
    for (; width > 0; width -= 1){
        int bottom_y = screenY() - (150 - width * 3);
        work->triangle(center_x, -screenY() / 3, center_x - width * 2, bottom_y, center_x + width * 2, bottom_y, Bitmap::makeColor(255,255,255));
    }
    Bitmap::drawingMode(Bitmap::MODE_SOLID);
    */
}

void NightAtmosphere::act(){
}
