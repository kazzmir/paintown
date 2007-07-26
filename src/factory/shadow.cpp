#include "shadow.h"
#include "util/bitmap.h"
#include "util/funcs.h"
#include <vector>

using namespace std;

Shadow * Shadow::my_shadow = NULL;
Bitmap const * Shadow::getShadow( unsigned int y ){
	if ( my_shadow == NULL ){
		my_shadow = new Shadow();
	}

	/*
	int foo = (100 - y) / 8;
	if ( foo < 0 )
		foo = 0;
	if ( foo >= my_shadow->shadows.size() ){
		foo = my_shadow->shadows.size() - 1;
	}
	*/
	if ( y < 0 )
		y = 0;
	if ( y >= my_shadow->shadows.size() ){
		y = my_shadow->shadows.size() - 1;
	}

	return my_shadow->shadows[ y ];
}

void Shadow::destroy(){
	if ( my_shadow != NULL ){
		delete my_shadow;
		my_shadow = NULL;
	}
}

Shadow::Shadow(){
	for ( int x = 1; x <= 6; x++ ){
		string s = Util::getDataPath() + "/sprites/shadow/shadow";
		s += (char)(x+'0');
		s += ".png";
		shadows.push_back( new Bitmap( s ) );
	}
}

Shadow::~Shadow(){
	for ( vector< Bitmap * >::iterator it = shadows.begin(); it != shadows.end(); it++ ){
		delete *it;
	}
}
