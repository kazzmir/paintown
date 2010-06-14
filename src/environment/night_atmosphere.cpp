#include "util/funcs.h"
#include "util/bitmap.h"
#include "night_atmosphere.h"
#include "util/file-system.h"
#include "util/token.h"
#include "globals.h"
#include <vector>

using namespace std;

/*
static int screenX(){
	return 320;
}

static int screenY(){
	return 240;
}
*/

NightAtmosphere::NightAtmosphere():
Atmosphere(),
darkness(128),
lightning(false){

    lamp = new Bitmap(Filesystem::find(Filesystem::RelativePath("sprites/lamp.png")).path());
    /*
    addLight(500, 30, 50, 30, Bitmap::makeColor(32,32,0), 0);
    addLight(300, 30, 70, 30, Bitmap::makeColor(0,32,192), 128);
    */
}

NightAtmosphere::~NightAtmosphere(){
    for (vector<Light*>::iterator it = lights.begin(); it != lights.end(); it++){
        delete (*it);
    }
    delete lamp;
}

/* lights should not overlap! the effect completely messes up if they do
 */
void NightAtmosphere::drawLight(Bitmap * original, Bitmap * work, const int x, const int y, const int lower_width, const int upper_width, const int black, const int dark_alpha, const int light, const int light_alpha, bool draw_light){
    int center_x = x;
    // int center_x = screenX();
    
    /*
    const int black = Bitmap::makeColor(0,0,0);
    const int light = Bitmap::makeColor(32,32,0);
    const int light_alpha = 0;
    const int dark_alpha = 128;
    */

    int where_x = center_x - lower_width;
    int total = lower_width * 2;
    if (where_x < 0){
        total += where_x;
        where_x = 0;
    }
    if (total <= 0){
        return;
    }

    int left = total - lower_width * 2;
    int middle = total - lower_width;
    int right = total;
    if (center_x - lower_width > 0){
        left = 0;
        middle = lower_width;
        right = lower_width * 2;
    }

    Bitmap save(*original, where_x, 0, total, work->getWidth());
    int top = y;

    if (draw_light){
        Bitmap::transBlender(0, 0, 0, dark_alpha);
        Bitmap::drawingMode(Bitmap::MODE_TRANS);
        /*
        int top = -save.getHeight() / 3;
        top = 30;
        */
        int lamp_height = save.getHeight() - top;

        /* y = tan(theta) * x */
        int lamp_top = (int)(((double)lower_width * 2.0 / (double)lamp_height) * (double)upper_width);

        // int top = 0;
        save.triangle(left, top, middle, top, left, save.getHeight(), black);
        save.triangle(right, top, middle, top, right, save.getHeight(), black);
        int nwidth = (int)((double) lamp_top / ((double) lamp_height / (double) lower_width));
        save.triangle(middle, top, middle - nwidth, top + lamp_top, middle + nwidth, top + lamp_top, black);

        save.rectangleFill(0, 0, right, top, black);
        Bitmap::drawingMode(Bitmap::MODE_SOLID);
        int xwidth = (int)((double) lamp_height / ((double)(save.getHeight() - top) / (double) lower_width));
        save.light(middle, top, xwidth, lamp_height, lamp_top, light_alpha, dark_alpha, light, black);
        lamp->draw(middle - 8, top, save);
    } else {
        lamp->draw(middle - 8, top, save);
    }
    save.draw(where_x, 0, *work);
}

int NightAtmosphere::getSkyColor() const {
    if (lightning){
        return Bitmap::makeColor(255, 255, 255);
    } else {
        return Bitmap::makeColor(0, 0, 0);
    }
}

int NightAtmosphere::getSkyDarkness() const {
    if (lightning){
        return lightningFade;
    } else {
        return 255 - darkness;
    }
}

void NightAtmosphere::drawFront(Bitmap * work, int x){
    Bitmap::transBlender(0, 0, 0, getSkyDarkness());
    work->applyTrans(getSkyColor());
}

void NightAtmosphere::drawBackground(Bitmap * work, int x){
}
	
void NightAtmosphere::drawScreen(Bitmap * work, int x){
}

void NightAtmosphere::addLight(const int x, const int y, const int lower_width, const int upper_width, const int color, const int alpha){
    lights.push_back(new Light(x, y, lower_width, upper_width, color, alpha));
}

void NightAtmosphere::drawForeground(Bitmap * work, int x){
    const int sky = getSkyColor();
    Bitmap save = Bitmap::temporaryBitmap(work->getWidth(), work->getHeight());
    work->Blit(save);
    Bitmap::transBlender(0, 0, 0, getSkyDarkness());
    work->applyTrans(sky);

    for (vector<Light*>::iterator it = lights.begin(); it != lights.end(); it++){
        Light * light = *it;
        int my_x = light->x - x;
        int my_y = light->y;
        drawLight(&save, work, my_x, my_y, light->lower_width, light->upper_width, sky, darkness, light->color, light->alpha, ! lightning);
    }
}

void NightAtmosphere::act(const Scene & level){
    if (lightning){
        if (lightningFade > 0){
            lightningFade *= 0.7;
        } else {
            lightning = false;
        }
    } else if (Util::rnd(300) == 0){
        lightning = true;
        lightningFade = 255;
    }
}

void NightAtmosphere::processLight(Token * token){
    int x;
    *token >> x;
    addLight(x, 30, 50, 30, Bitmap::makeColor(32,32,0), 0);
}
    
void NightAtmosphere::interpret(Token * message){
    Token * night = message->findToken("message/night");
    if (night != NULL){
        vector<Token*> lights = night->findTokens("night/lamp");
        for (vector<Token*>::iterator it = lights.begin(); it != lights.end(); it++){
            Token * token = *it;
            try{
                processLight(token);
            } catch (const TokenException & e){
                Global::debug(0) << "Could not add light to night atmosphere: " << e.getReason() << endl;
            }
        }
    }
}
