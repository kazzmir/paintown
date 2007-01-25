#include <string>
#include <allegro.h>
#include <map>
#include "util/font.h"
#include "font_factory.h"

#include "fonts.h"

using namespace std;

FontFactory * FontFactory::my_factory = NULL;
Font * FontFactory::getFont( const string & str ){
	if ( my_factory == NULL ){
		my_factory = new FontFactory();
	}

	return my_factory->getRealFont( str );
}

void FontFactory::destroy(){
	if ( my_factory != NULL )
		delete my_factory;
}
	
Font * FontFactory::getRealFont( const string & str ){
	if ( font_mapper.find( str ) == font_mapper.end() ){
	
		DATAFILE * obj = find_datafile_object( my_data, str.c_str() );
		if ( obj == NULL )
			return NULL;

		font_mapper[ str ] = new Font( (FONT *)( obj->dat ) );
	}

	return font_mapper[ str ];
}

FontFactory::FontFactory(){
	my_data = load_datafile( "data/fonts.dat" );
	font_mapper[ "bios" ] = new Font( font );
}

FontFactory::~FontFactory(){
	for ( map<string,Font*>::iterator it = font_mapper.begin(); it != font_mapper.end(); it++ ){
		Font * s = (*it).second;
		delete s;
	}
	unload_datafile( my_data );
}
