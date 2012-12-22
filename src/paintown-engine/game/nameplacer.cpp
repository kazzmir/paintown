#include <map>

#include "nameplacer.h"
#include "util/font.h"
#include "util/funcs.h"
#include "util/file-system.h"
#include "globals.h"

using namespace std;

NamePlacer * NamePlacer::placer = NULL;
void NamePlacer::getPlacement( int & x, int & y, int & id ){

	if ( placer == NULL )
		placer = new NamePlacer();

	placer->doPlacement( x, y, id );
}

void NamePlacer::destroy(){
	if ( placer != NULL ){
		delete placer;
		placer = NULL;
	}
}
	
NamePlacer::NamePlacer(){
	cur_id = 0;
}

void NamePlacer::doPlacement( int & x, int & y, int & id ){

	if ( id == -1 || places[ id ] == 0 )
		id = cur_id++;

	for ( map< int, int >::iterator it = places.begin(); it != places.end(); it++ ){
		int & second = (*it).second;
		if ( second > 0 )
			second--;
	}

	int count = 0;
	for ( map<int,int>::iterator it = places.begin(); it != places.end() && it != places.find( id ); it++){
		if ( (*it).second > 0 )
			count++;
	}
	/* give the place some buffer room to go away */
	places[ id ] = 10;
	const Font & ft = Font::getDefaultFont();
	x = 1 + (count&1) * 170;
	y = 1 + (count/2) * ft.getHeight() / 2;
}
