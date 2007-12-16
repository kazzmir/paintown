#include "util/bitmap.h"
#include "messages.h"
	
Messages::Messages( int width, int height ):
width( width ),
height( height ){
}
	
void Messages::draw( int x, int y, const Bitmap & work ){
	work.drawingMode( Bitmap::MODE_TRANS );
	Bitmap::transBlender( 0, 0, 0, 128 );
	work.rectangleFill( x, y, x + width, y + height, Bitmap::makeColor( 0, 0, 0 ) );
	work.drawingMode( Bitmap::MODE_SOLID );
}
	
Messages::~Messages(){
}
