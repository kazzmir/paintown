#include "util/bitmap.h"
#include "menu/option_screen_size.h"
#include "util/token.h"
#include "menu/menu.h"
#include "configuration.h"
#include "globals.h"
#include "init.h"
#include <sstream>

OptionScreenSize::OptionScreenSize(Token *token) throw (LoadException):
MenuOption(token, adjustableOption),
name(""),
lblue(255),
lgreen(255),
rblue(255),
rgreen(255){
    setRunnable(false);

    if ( *token != "screen-size" ){
        throw LoadException("Not a screen-size");
    }

    while (token->hasTokens()){
        try{
            Token * tok;
            *token >> tok;
            if ( *tok == "name" ){
                *tok >> name;
            } else {
                Global::debug( 3 ) <<"Unhandled menu attribute: "<<endl;
                if (Global::getDebug() >= 3){
                    tok->print(" ");
                }
            }
        } catch ( const TokenException & ex ) {
            // delete current;
            string m( "Menu parse error: " );
            m += ex.getReason();
            throw LoadException( m );
        } catch ( const LoadException & ex ) {
            // delete current;
            throw ex;
        }
    }

    if (name.empty()){
        throw LoadException("No name set, this option should have a name!");
    }
}

OptionScreenSize::~OptionScreenSize(){
    // Nothing
}

void OptionScreenSize::logic(){
    ostringstream temp;
    temp << "Screen size: " << Configuration::getScreenWidth() << " x " << Configuration::getScreenHeight();
    setText(temp.str());

    if (lblue < 255){
        lblue += 5;
    }

    if (rblue < 255){
        rblue += 5;
    }

    if (lgreen < 255){
        lgreen += 5;
    }

    if (rgreen < 255){
        rgreen += 5;
    }

    setLeftAdjustColor(Bitmap::makeColor(255, lblue, lgreen));
    setRightAdjustColor(Bitmap::makeColor(255, rblue, rgreen));
}

void OptionScreenSize::draw(Bitmap *work){
}

void OptionScreenSize::run(bool &endGame){
}

void OptionScreenSize::setMode(int width, int height){
    if (width != Configuration::getScreenWidth() ||
        height != Configuration::getScreenHeight()){

        Configuration::setScreenWidth(width);
        Configuration::setScreenHeight(height);
        int gfx = Configuration::getFullscreen() ? Global::FULLSCREEN : Global::WINDOWED;
        Bitmap::setGraphicsMode(gfx, width, height);
    }
}

static int modes[][2] = {{640,480}, {800,600}, {1024,768}, {1280,1024}, {1600,1200}};
// static int max_modes = sizeof(modes) / sizeof(int[]);
static int max_modes = 5;

static int findMode(int width, int height){
    for (int mode = 0; mode < max_modes; mode++){
        if (modes[mode][0] == width && modes[mode][1] == height){
            return mode;
        }
    }
    return -1;
}

bool OptionScreenSize::leftKey(){
    int mode = findMode(Configuration::getScreenWidth(), Configuration::getScreenHeight());
    if (mode >= 1 && mode < max_modes){
        mode -= 1;
    } else {
        mode = 0;
    }

    setMode(modes[mode][0], modes[mode][1]);

    lblue = lgreen = 0;
    return true;
}

bool OptionScreenSize::rightKey(){
    int mode = findMode(Configuration::getScreenWidth(), Configuration::getScreenHeight());
    if (mode >= 0 && mode < max_modes - 1){
        mode += 1;
    } else {
        mode = 0;
    }

    setMode(modes[mode][0], modes[mode][1]);

    rblue = rgreen = 0;
    return true;
}
