#include <string>
#ifdef USE_ALLEGRO
#include <allegro.h>
#endif
#include <map>
#include "util/font.h"
#include "util/funcs.h"
#include "util/file-system.h"
#include "font_factory.h"
#include "globals.h"

// #include "fonts.h"

using namespace std;

FontFactory * FontFactory::my_factory = NULL;
Font * FontFactory::getFont(const Filesystem::RelativePath & path, const int x, const int y ){
    if ( my_factory == NULL ){
        my_factory = new FontFactory();
    }

    return my_factory->getRealFont(path, x, y );
}

void FontFactory::destroy(){
    if ( my_factory != NULL ){
        delete my_factory;
    }
}
	
Font * FontFactory::getRealFont(const Filesystem::RelativePath & path, const int x, const int y ){
    try{
    if (font_mapper.find(path.path()) == font_mapper.end()){
        font_mapper[path.path()] = new FreeTypeFont(Filesystem::find(path));
        /*
        if ( my_data != NULL ){
            DATAFILE * obj = find_datafile_object( my_data, str.c_str() );
            if ( obj == NULL ){
                font_mapper[ str ] = new FreeTypeFont( str );
            } else {
                font_mapper[ str ] = new AllegroFont( (FONT *)( obj->dat ) );
            }
        } else {
            font_mapper[ str ] = new AllegroFont( ::font );
        }
        */

        // font_mapper[ str ] = new AllegroFont( (FONT *)( obj->dat ) );
        // Font * f = new FreeTypeFont( str );
        // font_mapper[ str ] = f;
    }

    Font * f = font_mapper[path.path()];
    f->setSize(x, y);

    return f;
    } catch (const Filesystem::NotFound & e){
        Global::debug(0) << "Warning: could not find font " << path.path() << ": " << e.getReason() << endl;
        return &nullFont;
    }
    // return font_mapper[ str ];
}

FontFactory::FontFactory(){
    // my_data = load_datafile(Filesystem::find(Filesystem::RelativePath("fonts.dat")).path().c_str());
#ifdef USE_ALLEGRO
    font_mapper[ "bios" ] = new AllegroFont( ::font );
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
