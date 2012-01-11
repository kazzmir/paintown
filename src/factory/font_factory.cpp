#include <string>
#ifdef USE_ALLEGRO
#include <allegro.h>
#endif
#include <map>
#include "util/font.h"
#include "util/funcs.h"
#include "util/file-system.h"
#include "util/thread.h"
#include "font_factory.h"
#include "globals.h"
#include "util/debug.h"
#include "util/exceptions/exception.h"
#include "util/load_exception.h"
#include "util/ftalleg.h"

// #include "fonts.h"

using namespace std;

FontFactory * FontFactory::my_factory = NULL;
Font * FontFactory::getFont(const Filesystem::RelativePath & path, const int x, const int y ){
    if ( my_factory == NULL ){
        my_factory = new FontFactory();
    }

    return my_factory->getRealFont(path, x, y );
}

Font * FontFactory::getFont(const Filesystem::AbsolutePath & path, const int x, const int y ){
    if ( my_factory == NULL ){
        my_factory = new FontFactory();
    }

    return my_factory->getRealFont(path, x, y );
}

void FontFactory::destroy(){
    if ( my_factory != NULL ){
        delete my_factory;
        my_factory = NULL;
    }
}
    
void FontFactory::clear(){
    if (my_factory != NULL){
        my_factory->_clear();
    }
}

Font * FontFactory::getRealFont(const Filesystem::AbsolutePath & path, int x, int y){
    Util::Thread::ScopedLock locked(lock);
    if (font_mapper.find(path.path()) == font_mapper.end()){
        try{
            FreeTypeFont * font = new FreeTypeFont(path);
            font_mapper[path.path()] = font;
        } catch (const ftalleg::Exception & failure){
            throw LoadException(__FILE__, __LINE__, failure.getReason());
        }
    }
    Font * font = font_mapper[path.path()];
    if (font == NULL){
        throw Exception::Base(__FILE__, __LINE__);
    }
    if (x < 1){
        x = 1;
    }
    if (y < 1){
        y = 1;
    }
    font->setSize(x, y);

    return font;
}

Font * FontFactory::getRealFont(const Filesystem::RelativePath & path, const int x, const int y ){
    Util::Thread::ScopedLock locked(lock);
    try{
        if (font_mapper.find(path.path()) == font_mapper.end()){
            font_mapper[path.path()] = new FreeTypeFont(Storage::instance().find(path));
        }

        Font * f = font_mapper[path.path()];
        if (f == NULL){
            throw Exception::Base(__FILE__, __LINE__);
        }
        f->setSize(x, y);

        return f;
    } catch (const Filesystem::NotFound & e){
        Global::debug(0) << "Warning: could not find font " << path.path() << ": " << e.getTrace() << endl;
        return &nullFont;
    }
    // return font_mapper[ str ];
}

void FontFactory::_clear(){
    for (map<string,Font*>::iterator it = font_mapper.begin(); it != font_mapper.end(); it++){
        Font * s = (*it).second;
        delete s;
    }
    font_mapper.clear();
#ifdef USE_ALLEGRO
    font_mapper["bios"] = new AllegroFont(::font);
#endif
}

FontFactory::FontFactory(){
    // my_data = load_datafile(Filesystem::find(Filesystem::RelativePath("fonts.dat")).path().c_str());
#ifdef USE_ALLEGRO
    font_mapper[ "bios" ] = new AllegroFont(::font);
#endif
}

FontFactory::~FontFactory(){
    for ( map<string,Font*>::iterator it = font_mapper.begin(); it != font_mapper.end(); it++ ){
        Font * s = (*it).second;
        delete s;
    }

        /*
	if ( my_data != NULL ){
		unload_datafile( my_data );
	}
        */
}
