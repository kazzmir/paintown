#include <string>
#include <allegro.h>
#include <map>
#include "util/font.h"
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
	
		DATAFILE * obj = find_datafile_object( my_data, str.c_str() );
		if ( obj == NULL ){
			font_mapper[ str ] = new FreeTypeFont( str );
			font_mapper[ str ]->setSize( x, y );
		} else {
			font_mapper[ str ] = new AllegroFont( (FONT *)( obj->dat ) );
		}

		// font_mapper[ str ] = new AllegroFont( (FONT *)( obj->dat ) );
		// Font * f = new FreeTypeFont( str );
		// font_mapper[ str ] = f;
	}

	return font_mapper[ str ];
}

FontFactory::FontFactory(){
	my_data = load_datafile( "data/fonts.dat" );
	font_mapper[ "bios" ] = new AllegroFont( font );
}

FontFactory::~FontFactory(){
	for ( map<string,Font*>::iterator it = font_mapper.begin(); it != font_mapper.end(); it++ ){
		Font * s = (*it).second;
		delete s;
	}
	unload_datafile( my_data );
}
