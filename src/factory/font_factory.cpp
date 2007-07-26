#include <string>
#include <allegro.h>
#include <map>
#include "util/font.h"
#include "util/funcs.h"
#include "font_factory.h"

#include "fonts.h"

using namespace std;

FontFactory * FontFactory::my_factory = NULL;
Font * FontFactory::getFont( const string & str, const int x, const int y ){
	if ( my_factory == NULL ){
		my_factory = new FontFactory();
	}

	return my_factory->getRealFont( str, x, y );
}

void FontFactory::destroy(){
	if ( my_factory != NULL )
		delete my_factory;
}
	
Font * FontFactory::getRealFont( const string & str, const int x, const int y ){
	if ( font_mapper.find( str ) == font_mapper.end() ){
	
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

		// font_mapper[ str ] = new AllegroFont( (FONT *)( obj->dat ) );
		// Font * f = new FreeTypeFont( str );
		// font_mapper[ str ] = f;
	}

	Font * f = font_mapper[ str ];
	f->setSize( x, y );

	return f;
	// return font_mapper[ str ];
}

FontFactory::FontFactory(){
	my_data = load_datafile( (Util::getDataPath() + "/fonts.dat").c_str() );
	font_mapper[ "bios" ] = new AllegroFont( ::font );
}

FontFactory::~FontFactory(){
	for ( map<string,Font*>::iterator it = font_mapper.begin(); it != font_mapper.end(); it++ ){
		Font * s = (*it).second;
		delete s;
	}
	if ( my_data != NULL ){
		unload_datafile( my_data );
	}
}
