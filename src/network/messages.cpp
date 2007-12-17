#include "util/bitmap.h"
#include "messages.h"
#include "util/font.h"

using namespace std;
	
Messages::Messages( int width, int height ):
width( width ),
height( height ){
}
	
void Messages::addMessage( const std::string & s ){
	messages.push_back( s );
}
	
void Messages::draw( int x, int y, const Bitmap & work, const Font & font ){
	work.drawingMode( Bitmap::MODE_TRANS );
	Bitmap::transBlender( 0, 0, 0, 128 );
	work.rectangleFill( x, y, x + width, y + height, Bitmap::makeColor( 0, 0, 0 ) );
	work.drawingMode( Bitmap::MODE_SOLID );
	work.rectangle( x, y, x + width, y + height, Bitmap::makeColor( 255, 255, 255 ) );

	Bitmap area( work, x, y, width, height );
	int current_y = height - font.getHeight() - 1;
	for ( vector< string >::reverse_iterator it = messages.rbegin(); it != messages.rend(); it++ ){
		const string & s = *it;
		font.printf( 0, current_y, Bitmap::makeColor( 255, 255, 255 ), area, s, 0 );
		current_y -= font.getHeight();
	}
}
	
Messages::~Messages(){
}
