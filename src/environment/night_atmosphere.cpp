#include "util/funcs.h"
#include "util/bitmap.h"
#include "night_atmosphere.h"
#include "../globals.h"

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

void NightAtmosphere::drawLight(Bitmap * original, Bitmap * work, const int x){
    int center_x = x;
    // int center_x = screenX();
    const int black = Bitmap::makeColor(0,0,0);
    int width = 50;
    int where_x = center_x - width;
    int total = width * 2;
    if (where_x < 0){
        total += where_x;
        where_x = 0;
    }
    if (total <= 0){
        return;
    }

    int left = total - width * 2;
    int middle = total - width;
    int right = total;
    if (center_x - width > 0){
        left = 0;
        middle = width;
        right = width * 2;
    }

    Global::debug(1) << "night at " << x << " left at " << where_x << " width was " << width * 2 << " now " << total << std::endl;
    Bitmap save(*original, where_x, 0, total, screenY());
    Global::debug(1) << " save: w " << save.getWidth() << " h: " << save.getHeight() << std::endl;
    Bitmap::drawingMode(Bitmap::MODE_TRANS);
    int top = -save.getHeight() / 3;
    top = 30;
    int lamp_height = save.getHeight() - top;
    int lamp_top = 30;
    // int top = 0;
    save.triangle(left, top, middle, top, left, save.getHeight(), black);
    save.triangle(right, top, middle, top, right, save.getHeight(), black);
    int nwidth = (double) lamp_top / ((double) lamp_height / (double) width);
    save.triangle(middle, top, middle - nwidth, top + lamp_top, middle + nwidth, top + lamp_top, black);

    save.rectangleFill(0, 0, right, top, black);
    Bitmap::drawingMode(Bitmap::MODE_SOLID);
    int xwidth = (double) lamp_height / ((double)(save.getHeight() - top) / (double) width);
    save.light(middle, top, xwidth, lamp_height, lamp_top, 0, 128, Bitmap::makeColor(32,32,0), Bitmap::makeColor(0,0,0));
    // save.light(save.getWidth() / 2, top, xwidth, lamp_height, 0, 128, Bitmap::makeColor(32,32,0), Bitmap::makeColor(0,0,0));
    // save.light(save.getWidth() / 2, top, width, save.getHeight(), 32, 128, Bitmap::makeColor(32,32,0), Bitmap::makeColor(0,0,0));
    // save.light(save.getWidth() / 2, 0, width, save.getHeight(), 0, 128, Bitmap::makeColor(0,0,0));
    // save.light(save.getWidth() / 2, 0, width, save.getHeight(), 128, 255, Bitmap::makeColor(0,0,0));
    save.draw(where_x, 0, *work);
}

void NightAtmosphere::draw(Bitmap * work, int x){
    const int black = Bitmap::makeColor(0,0,0);
    Bitmap save = Bitmap::temporaryBitmap(work->getWidth(), work->getHeight());
    work->Blit(save);
    Bitmap::transBlender(0, 0, 0, 128);
    night->drawTrans( 0, 0, *work );
    
    drawLight(&save, work, 520 - x);
}

void NightAtmosphere::act(){
}
