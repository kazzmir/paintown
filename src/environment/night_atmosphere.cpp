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
}

NightAtmosphere::~NightAtmosphere(){
}

/* lights should not overlap! the effect completely messes up if they do
 */
void NightAtmosphere::drawLight(Bitmap * original, Bitmap * work, const int x, const int y, const int width, const int black, const int dark_alpha, const int light, const int light_alpha){
    int center_x = x;
    // int center_x = screenX();
    
    /*
    const int black = Bitmap::makeColor(0,0,0);
    const int light = Bitmap::makeColor(32,32,0);
    const int light_alpha = 0;
    const int dark_alpha = 128;
    */

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

    Bitmap save(*original, where_x, 0, total, screenY());
    Bitmap::transBlender(0, 0, 0, dark_alpha);
    Bitmap::drawingMode(Bitmap::MODE_TRANS);
    /*
    int top = -save.getHeight() / 3;
    top = 30;
    */
    int top = y;
    int lamp_height = save.getHeight() - top;
    int upper_lamp_width = 30;

    /* y = tan(theta) * x */
    int lamp_top = ((double)width * 2.0 / (double)lamp_height) * (double)upper_lamp_width;

    // int top = 0;
    save.triangle(left, top, middle, top, left, save.getHeight(), black);
    save.triangle(right, top, middle, top, right, save.getHeight(), black);
    int nwidth = (double) lamp_top / ((double) lamp_height / (double) width);
    save.triangle(middle, top, middle - nwidth, top + lamp_top, middle + nwidth, top + lamp_top, black);

    save.rectangleFill(0, 0, right, top, black);
    Bitmap::drawingMode(Bitmap::MODE_SOLID);
    int xwidth = (double) lamp_height / ((double)(save.getHeight() - top) / (double) width);
    save.light(middle, top, xwidth, lamp_height, lamp_top, light_alpha, dark_alpha, light, black);
    save.draw(where_x, 0, *work);
}

void NightAtmosphere::drawFront(Bitmap * work, int x){
    Bitmap::transBlender(0, 0, 0, 128);
    work->applyTrans(Bitmap::makeColor(0,0,0));
}

void NightAtmosphere::drawBackground(Bitmap * work, int x){
}
	
void NightAtmosphere::drawScreen(Bitmap * work, int x){
}

void NightAtmosphere::drawForeground(Bitmap * work, int x){
    const int black = Bitmap::makeColor(0,0,0);
    Bitmap save = Bitmap::temporaryBitmap(work->getWidth(), work->getHeight());
    work->Blit(save);
    Bitmap::transBlender(0, 0, 0, 128);
    work->applyTrans(Bitmap::makeColor(0,0,0));
    
    drawLight(&save, work, 500 - x, 30, 50, black, 128, Bitmap::makeColor(32, 32, 0), 0);
    drawLight(&save, work, 300 - x, 30, 70, black, 128, Bitmap::makeColor(0, 32, 192), 128);
}

void NightAtmosphere::act(){
}
