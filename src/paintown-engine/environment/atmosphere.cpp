#include "atmosphere.h"
#include "fog_atmosphere.h"
#include "globals.h"
#include "util/debug.h"
#include "paintown-engine/level/scene.h"
#include <math.h>
#include "night_atmosphere.h"
#include "rain_atmosphere.h"
#include "util/resource.h"
#include "snow_atmosphere.h"
#include "util/bitmap.h"
#include "util/trans-bitmap.h"
#include "util/file-system.h"
#include "util/funcs.h"
#include "util/sound.h"
#include "util/token.h"
#include "paintown-engine/object/object.h"
#include <vector>

using namespace std;

Atmosphere::Atmosphere(){
}
	
Atmosphere::~Atmosphere(){
}
	
Atmosphere * Atmosphere::createAtmosphere( const std::string & name ){
    if ( name == "rain" ){
        return new RainAtmosphere();
    }
    if ( name == "snow" ){
        return new SnowAtmosphere();
    }
    if ( name == "night" ){
        return new NightAtmosphere();
    }
    if ( name == "fog" ){
        return new FogAtmosphere();
    }

    Global::debug(0) << "Invalid atmosphere name " << name << std::endl;
    /* throw an exception? */
    return NULL;
}


static int screenX(){
    return 320;
}

static int screenY(){
    return 240;
}

FogAtmosphere::FogAtmosphere():
Atmosphere(){
    fog = new Graphics::Bitmap( 50, 50 );
    fog->fill(Graphics::MaskColor());
    fog->circleFill( 25, 25, 20, Graphics::makeColor( 0xbb, 0xbb, 0xcc ) );

    for ( int i = -20; i < screenX(); i += 20 ){
        fogs.push_back( new Fog( i + Util::rnd( 9 ) - 4, screenY() - 30, Util::rnd( 360 ) ) );
        fogs.push_back( new Fog( i + Util::rnd( 9 ) - 4, screenY() - 40, Util::rnd( 360 ) ) );
        fogs.push_back( new Fog( i + Util::rnd( 9 ) - 4, screenY() - 40, Util::rnd( 360 ) ) );
        fogs.push_back( new Fog( i + Util::rnd( 9 ) - 4, screenY() - 50, Util::rnd( 360 ) ) );
        fogs.push_back( new Fog( i + Util::rnd( 9 ) - 4, screenY() - 60, Util::rnd( 360 ) ) );
        /*
           for ( int q = 0; q < 3; q++ ){
           fogs.push_back( new Fog( i + Util::rnd( 9 ) - 4, screenY() - Util::rnd( 30 ) - 40, Util::rnd( 360 ) ) );
           }
           */
    }
}

FogAtmosphere::~FogAtmosphere(){
    delete fog;
    for ( vector< Fog * >::iterator it = fogs.begin(); it != fogs.end(); it++ ){
        delete *it;
    }
}

void FogAtmosphere::drawForeground(Graphics::Bitmap * work, int x){
}

void FogAtmosphere::drawBackground(Graphics::Bitmap * work, int x){
}

void FogAtmosphere::drawFront(Graphics::Bitmap * work, int x){
}

void FogAtmosphere::drawScreen(Graphics::Bitmap * work, int x){
    Graphics::Bitmap::transBlender( 0, 0, 0, 64 );
    for ( vector< Fog * >::iterator it = fogs.begin(); it != fogs.end(); it++ ){
        Fog * f = *it;
        int y = (int)(f->y + sin( f->ang * 3.14159 / 180.0 ) * 2);
        fog->translucent().draw( f->x, y, *work );
    }
    /*
       screenX();
       for ( int i = -10; i < screenX() + 10; i += 15 ){
       fog->drawTrans( i, screenY() - 50, *work );
       }
       */
}

void FogAtmosphere::act(const Scene & level, const vector<Paintown::Object*> * objects){
    for ( vector< Fog * >::iterator it = fogs.begin(); it != fogs.end(); it++ ){
        Fog * f = *it;
        f->ang += 1;
    }
}

void FogAtmosphere::interpret(const Token * message){
}

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
thunder(NULL),
lightning(false),
thunderPause(-1){

    lamp = new Graphics::Bitmap(Storage::instance().find(Filesystem::RelativePath("sprites/lamp.png")).path());
    thunder = Resource::getSound(Filesystem::RelativePath("sounds/thunder.wav"));
    /*
    addLight(500, 30, 50, 30, Graphics::Bitmap::makeColor(32,32,0), 0);
    addLight(300, 30, 70, 30, Graphics::Bitmap::makeColor(0,32,192), 128);
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
void NightAtmosphere::drawLight(Graphics::Bitmap * original, Graphics::Bitmap * work, const int x, const int y, const int lower_width, const int upper_width, const Graphics::Color black, const int dark_alpha, const int light, const int light_alpha, bool draw_light){
    int center_x = x;
    // int center_x = screenX();
    
    /*
    const int black = Graphics::Bitmap::makeColor(0,0,0);
    const int light = Graphics::Bitmap::makeColor(32,32,0);
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

    // Bitmap save(*original, where_x, 0, total, work->getWidth());
    // Bitmap save(*original, where_x, 0, total, original->getHeight());
    Graphics::Bitmap save(*original, where_x, 0, total, original->getHeight());
    
    int top = y;

    if (draw_light){
        Graphics::Bitmap::transBlender(0, 0, 0, dark_alpha);
        // Graphics::Bitmap::drawingMode(Graphics::Bitmap::MODE_TRANS);
        /*
        int top = -save.getHeight() / 3;
        top = 30;
        */
        int lamp_height = save.getHeight() - top;

        /* y = tan(theta) * x */
        int lamp_top = (int)(((double)lower_width * 2.0 / (double)lamp_height) * (double)upper_width);

        // int top = 0;
        save.translucent().triangle(left, top, middle, top, left, save.getHeight(), black);
        save.translucent().triangle(right, top, middle, top, right, save.getHeight(), black);
        int nwidth = (int)((double) lamp_top / ((double) lamp_height / (double) lower_width));
        save.translucent().triangle(middle, top, middle - nwidth, top + lamp_top, middle + nwidth, top + lamp_top, black);

        save.translucent().rectangleFill(0, 0, right, top, black);
        // Graphics::Bitmap::drawingMode(Graphics::Bitmap::MODE_SOLID);
        int xwidth = (int)((double) lamp_height / ((double)(save.getHeight() - top) / (double) lower_width));
        save.light(middle, top, xwidth, lamp_height, lamp_top, light_alpha, dark_alpha, light, black);
        lamp->draw(middle - 8, top, save);
    } else {
        lamp->draw(middle - 8, top, save);
    }
    save.draw(where_x, 0, *work);
}

Graphics::Color NightAtmosphere::getSkyColor() const {
    if (lightning){
        return Graphics::makeColor(255, 255, 255);
    } else {
        return Graphics::makeColor(0, 0, 0);
    }
}

int NightAtmosphere::getSkyDarkness() const {
    if (lightning){
        return lightningFade;
    } else {
        return 255 - darkness;
    }
}

void NightAtmosphere::drawFront(Graphics::Bitmap * work, int x){
    Graphics::Bitmap::transBlender(0, 0, 0, getSkyDarkness());
    work->applyTrans(getSkyColor());
}

void NightAtmosphere::drawBackground(Graphics::Bitmap * work, int x){
}
	
void NightAtmosphere::drawScreen(Graphics::Bitmap * work, int x){
}

void NightAtmosphere::addLight(const int x, const int y, const int lower_width, const int upper_width, const Graphics::Color color, const int alpha){
    lights.push_back(new Light(x, y, lower_width, upper_width, color, alpha));
}

void NightAtmosphere::drawForeground(Graphics::Bitmap * work, int x){
    const Graphics::Color sky = getSkyColor();
    Graphics::Bitmap::transBlender(0, 0, 0, getSkyDarkness());
    work->applyTrans(sky);

    /* FIXME: lights are broken in sdl, valgrind spits out some error:
     * == Invalid read of size 2
     * ==6497==    at 0x836E601: SPG_RectFilledBlend (SPG_primitives.c:2543)
     * ==6497==    by 0x8369D56: spg_linehblend (SPG_primitives.c:907)
     * ==6497==    by 0x8360DB4: SPG_TrigonFilledBlend (SPG_polygon.c:786)
     * ==6497==    by 0x8343265: Bitmap::triangle(int, int, int, int, int, int, int) const (bitmap.cpp:884)
     * ==6497==    by 0x815134B: NightAtmosphere::drawLight(Bitmap*, Bitmap*, int, int, int, int, int, int, int, int, bool) (night_atmosphere.cpp:96)
     *
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
    */
}

void NightAtmosphere::act(const Scene & level, const vector<Paintown::Object*> * objects){
    if (lightning){
        if (lightningFade > 0){
            lightningFade *= 0.7;
        } else {
            lightning = false;
        }
    } else if (Util::rnd(300) == 0){
        lightning = true;
        lightningFade = 255;
        thunderPause = 20 + Util::rnd(4);
        // thunder->play();
    }

    if (thunderPause == 0){
        thunder->play();
        thunderPause -= 1;
    } else if (thunderPause > 0){
        thunderPause -= 1;
    }
}

void NightAtmosphere::processLight(const Token * token){
    int x;
    token->view() >> x;
    addLight(x, 30, 50, 30, Graphics::makeColor(32,32,0), 0);
}
    
void NightAtmosphere::interpret(const Token * message){
    const Token * night = message->findToken("message/night");
    if (night != NULL){
        vector<const Token*> lights = night->findTokens("night/lamp");
        for (vector<const Token*>::iterator it = lights.begin(); it != lights.end(); it++){
            const Token * token = *it;
            try{
                processLight(token);
            } catch (const TokenException & e){
                Global::debug(0) << "Could not add light to night atmosphere: " << e.getTrace() << endl;
            }
        }
    }
}

RainAtmosphere::RainAtmosphere():
Atmosphere(),
playing( false ){

    rain_sound = Sound(Storage::instance().find(Filesystem::RelativePath("sounds/rain.wav")).path());

    Graphics::Color colors[ 2 ];
    colors[0] = Graphics::makeColor( 0x22, 0x66, 0x66 );
    colors[1] = Graphics::makeColor( 0x11, 0x44, 0x77 );
    for (int i = 0; i < 100; i++){
        Drop * d = new Drop(Util::rnd(screenX() * 2) - screenX() / 2, Util::rnd( screenY() ), Util::rnd(4) + 3, colors[Util::rnd(2)]);
        rain_drops.push_back(d);
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

void RainAtmosphere::drawBackground(Graphics::Bitmap * work, int x){
    const Graphics::Color bluish = Graphics::makeColor(106, 184, 225);
    Graphics::Bitmap::transBlender(0, 0, 0, 64);
    for (vector<Puddle*>::iterator it = puddles.begin(); it != puddles.end(); it++){
        Puddle * puddle = *it;
        if (puddle->x == -1000){
            puddle->x = x + Util::rnd(screenX() + 30) - 15;
        }
        // work->circle(puddle->x, puddle->y, (int)puddle->current, bluish);
        int rx = (int) puddle->current;
        int ry = (int)(puddle->current * 0.8);
        work->translucent().ellipse(puddle->x - x, puddle->y, rx, ry < 1 ? 1 : ry, bluish);
    }
}

void RainAtmosphere::drawForeground(Graphics::Bitmap * work, int x){
    const Graphics::Color bluish = Graphics::makeColor(106, 184, 225);
    Graphics::Bitmap::transBlender(0, 0, 0, 64);
    for (vector<Puddle*>::iterator it = objectPuddles.begin(); it != objectPuddles.end(); it++){
        Puddle * puddle = *it;
        int rx = (int) puddle->current;
        int ry = (int)(puddle->current * 0.8);
        work->translucent().ellipse(puddle->x - x, puddle->y, rx, ry < 1 ? 1 : ry, bluish);
    }
}

void RainAtmosphere::drawFront(Graphics::Bitmap * work, int x){
}

void RainAtmosphere::interpret(const Token * message){
}

void RainAtmosphere::drawScreen(Graphics::Bitmap * work, int x){
    for ( vector< Drop * >::iterator it = rain_drops.begin(); it != rain_drops.end(); it++ ){
        Drop * d = *it;
        work->line( d->x, d->y, d->x + d->length * 2 / 3, d->y + d->length, d->color );
    }
}

void RainAtmosphere::act(const Scene & level, const vector<Paintown::Object*> * objects){

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

    for (vector<Puddle*>::iterator it = objectPuddles.begin(); it != objectPuddles.end(); ){
        Puddle * puddle = *it;
        puddle->current += 0.3;
        if (puddle->current >= puddle->size){
            delete puddle;
            it = objectPuddles.erase(it);
        } else {
            it++;
        }
    }

    /* create droplets on characters */
    if (objects != NULL && objects->size() > 0){
        for (int i = objectPuddles.size(); i < 5; i++){
            Paintown::Object * who = (*objects)[Util::rnd(objects->size())];
            int x = who->getRX() + Util::rnd(-who->getWidth(), who->getWidth());
            int y = who->getRY() - Util::rnd(who->getHeight());
            if (who->touchPoint(x, y)){
                int size = Util::rnd(4) + 2;
                objectPuddles.push_back(new Puddle(x, y, size));
            }
        }
    }

    for (int i = puddles.size(); i < 20; i++){
        // int x = Util::rnd(screenX());
        int y = Util::rnd(level.getMinimumZ(), level.getMaximumZ());
        int size = Util::rnd(4) + 2;
        /* supreme hack! set x to nothing and later on in the draw routine
         * update x
         */
        puddles.push_back(new Puddle(-1000, y, size));
    }

    for (vector<Drop *>::iterator it = rain_drops.begin(); it != rain_drops.end(); it++){
        Drop * d = *it;
        d->y += 7;
        d->x += 3;
        if (d->y > screenY()){
            d->y = -Util::rnd(100) - 20;
        }
        if (d->x > screenX()){
            d->x -= screenX();
        }
    }
}

SnowAtmosphere::SnowAtmosphere():
Atmosphere(){
    for ( int i = 0; i < 150; i++ ){
        Flake * f = new Flake(Util::rnd(screenX() * 2 ) - screenX() / 2, Util::rnd( screenY() ), FlakeType(Util::rnd(2)), Util::rnd(360));
        flakes.push_back(f);
    }
}

SnowAtmosphere::~SnowAtmosphere(){
    for ( vector< Flake * >::iterator it = flakes.begin(); it != flakes.end(); it++ ){
        delete *it;
    }
}

static void drawFlakeSmall(Flake * flake, Graphics::Bitmap * work){
    int c = (int)(200 + 3 * log((double)(flake->y < 1 ? 1 : 2 * flake->y)));
    if (c > 255){
        c = 255;
    }
    Graphics::Color color = Graphics::makeColor(c, c, c);
    work->putPixel(flake->x, flake->y, color);
}

static void drawFlake0( Flake * f, Graphics::Bitmap * work ){
    int c = (int)(200 + 3 * log((double)(f->y < 1 ? 1 : 2 * f->y)));
    if (c > 255){
        c = 255;
    }
    Graphics::Color color = Graphics::makeColor(c, c, c);
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

void SnowAtmosphere::drawBackground(Graphics::Bitmap * work, int x){
}

void SnowAtmosphere::drawForeground(Graphics::Bitmap * work, int x){
}

void SnowAtmosphere::drawFront(Graphics::Bitmap * work, int x){
}

void SnowAtmosphere::drawScreen(Graphics::Bitmap * work, int x){
    for ( vector< Flake * >::iterator it = flakes.begin(); it != flakes.end(); it++ ){
        Flake * f = *it;
        switch (f->type){
            case Small: {
                drawFlakeSmall(f, work);
                break;
            }
            case Medium: {
                drawFlake0(f, work);
                break;
            }
        }
    }
}

void SnowAtmosphere::act(const Scene & level, const vector<Paintown::Object*> * objects){
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

void SnowAtmosphere::interpret(const Token * message){
}
