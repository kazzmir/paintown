/*
--------------
About
--------------
A Freetype wrapper for use with allegro
Feel free to do whatever you like with this, by all means enjoy!
Just add it to your project

--------------
Linking
--------------
on linux:
g++ `freetype-config --cflags` ftalleg.cpp myfiles.cpp `freetype-config --libs` `allegro-config --libs`

on windows (you may need to include the freetype dir location, ie -Ic:/mingw/include):
g++ ftalleg.cpp myfiles.cpp -lfreetype -lalleg

--------------
Disclaimer
--------------
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE 
SOFTWARE BE LIABLE FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE, 
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef FT_FONT_CPP
#define FT_FONT_CPP

#include "r-tech1/graphics/bitmap.h"

/*
#include <allegro.h>
#ifdef _WIN32
#include <winalleg.h>
#endif
*/

#include "r-tech1/ftalleg.h"
#include "r-tech1/utf.h"
#include <iostream>
#include <sstream>
#include <cassert>
#include <exception>
#ifdef USE_ALLEGRO5
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#endif

namespace ftalleg{

    Exception::Exception():
    std::exception(){
    }

    Exception::Exception(const std::string reason):
    std::exception(),
    reason(reason){
    }

    Exception::~Exception() throw(){
    }

    // typedef void (*pixeler)( BITMAP * bitmap, int x, int y, int color );

    static Graphics::Color fixColor(const unsigned char c, short grays){
        // Safety checks
        // assert(c != 0);
        assert(grays != 1);

        // invariant: c can be dereferenced safely
        // invariant: (grays - 1) != 0, thus it can be used as divisor.

        int red = c * 255 / (grays - 1);
        int green = c * 255 / (grays - 1);
        int blue = c * 255 / (grays - 1);
        //alpha = *c * 255 / (grays - 1);
        return Graphics::makeColor(red,green,blue);
    }
    // Static count of instances of fonts to track library
    static int instances = 0;
    // static FT_Library ftLibrary = 0;

    character::character() {
    }

        character::~character() {
            if (line){
                delete [] line;
            }
        }

    fontSize::fontSize() {
            width = height = italics = angle = 0;
    }
            
        fontSize::fontSize(int width, int height):
            width(width),
            height(height),
            italics(0),
            angle(0){
            }

    fontSize::~fontSize() {
    }

    bool fontSize::operator<(const fontSize &fs) const {
            return (width<fs.width || height<fs.height || italics<fs.italics);
    }

        /* im not sure this is a very unique key.. */
    int fontSize::createKey() const {
            return ((width+10) * (height+20) * (italics+250));
    }
}

#endif /* FONT_BASE_CPP */
