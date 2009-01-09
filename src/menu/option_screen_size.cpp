#include "util/bitmap.h"
#include "menu/option_screen_size.h"
#include "util/token.h"
#include "menu/menu.h"
#include "configuration.h"
#include "globals.h"
#include "init.h"
#include <sstream>

#ifdef WINDOWS
#include <windows.h>
#include <stdio.h>

/* contributed by Roy Underthump from allegro.cc */
void getScreenResolutions(){
    HWND hwnd;
    HDC  hdc;

    int i;

    int iPixelFormat;

    int descerr;

    int retval;

    DEVMODE d;

    PIXELFORMATDESCRIPTOR pfd;

    hwnd = GetDesktopWindow();
    hdc  = GetDC(hwnd);

    for(i=0;;i++){
        retval = EnumDisplaySettings(0,i,&d);
        if(!retval) break;

        descerr = DescribePixelFormat(hdc, i+1, sizeof(pfd), &pfd);
        if(!descerr){
            return -1;
        }

        printf("\n#%d bpp %d width %d height %d colorbits %d fps %d",i,d.dmBitsPerPel,
                d.dmPelsWidth, d.dmPelsHeight,pfd.cColorBits,d.dmDisplayFrequency);

        if(pfd.dwFlags & PFD_SUPPORT_OPENGL)printf(" OGL OK");
    }
}
#endif

OptionScreenSize::OptionScreenSize(Token *token) throw (LoadException):
MenuOption(token, AdjustableOption),
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

void OptionScreenSize::run(bool &endGame){
}

void OptionScreenSize::setMode(int width, int height){
    if (width != Configuration::getScreenWidth() ||
        height != Configuration::getScreenHeight()){

        int gfx = Configuration::getFullscreen() ? Global::FULLSCREEN : Global::WINDOWED;
        int ok = Bitmap::setGraphicsMode(gfx, width, height);
        if (ok == 0){
            Configuration::setScreenWidth(width);
            Configuration::setScreenHeight(height);
        }
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
