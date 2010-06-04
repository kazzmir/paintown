#include "util/bitmap.h"
#include "util/trans-bitmap.h"
#include "messages.h"
#include "util/font.h"
#include "globals.h"

using namespace std;
	
Messages::Messages( int width, int height, int opaque ):
width(width),
height(height),
opaque(opaque),
borderColor(Bitmap::makeColor(200,200,200)){
}

static vector< string > wrapStrings( const string & left, const string & right, const Font & font, int max, vector< string > accum ){
	if ( left == "" ){
		return accum;
	}

	int length = font.textLength( left.c_str() );

	if ( length >= max ){
		return wrapStrings( left.substr( 0, left.length() / 2 ), left.substr( left.length() / 2 ) + right, font, max, accum );
	} else if ( length >= max - font.textLength( "E" ) || right == "" ){
		accum.push_back( left );
		return wrapStrings( right, "", font, max, accum );
	} else {
		return wrapStrings( left + right.substr( 0, 1 ), right.substr( 1 ), font, max, accum );
	}
}

/*
static int wrapPrint( const Bitmap & area, int y, int top, const Font & font, const string & left, const string & right ){
	if ( left == "" ){
		return top;
	}

	int length = font.textLength( left.c_str() );
	int max = area.getWidth();

	if ( length >= max ){
		return wrapPrint( area, y - font.getHeight(), top - font.getHeight(), font, left.substr( 0, left.length() / 2 ), left.substr( left.length() / 2 ) + right );
	} else if ( length >= max - font.textLength( "E" ) || right == "" ){
		font.printf( 0, y, Bitmap::makeColor( 255, 255, 255 ), area, left, 0 );
		return wrapPrint( area, y + font.getHeight(), top, font, right, "" );
	} else {
		return wrapPrint( area, y, top, font, left + right.substr( 0, 1 ), right.substr( 1 ) );
	}
}
*/

void Messages::addMessage( const std::string & s ){
	messages.push_back( s );
}
	
void Messages::draw( int x, int y, const Bitmap & work, const Font & font ){
    // work.drawingMode( Bitmap::MODE_TRANS );
    Bitmap::transBlender(0, 0, 0, this->opaque);
    work.translucent().rectangleFill( x, y, x + width, y + height, Bitmap::makeColor( 0, 0, 0 ) );
    // work.drawingMode( Bitmap::MODE_SOLID );
    // work.rectangle( x, y, x + width-1, y + height-1, Bitmap::makeColor( 255, 255, 255 ) );
    work.border(0, 1, borderColor);

    /* fast because its a sub-bitmap */
    Bitmap area(work, x, y, width, height);

    int current_y = height - font.getHeight() - 1;
    // int max_length = area.getWidth() / font.textLength( "E" );
    for ( vector< string >::reverse_iterator it = messages.rbegin(); it != messages.rend(); it++ ){
        if ( current_y < 0 - font.getHeight() ){
            break;
        }
        const string & s = *it;
        vector< string > all;
        all = wrapStrings(s, "", font, area.getWidth() - 3, all);
        current_y -= font.getHeight() * (all.size() - 1);
        int my = current_y;
        for ( vector< string >::iterator str = all.begin(); str != all.end(); str++ ){
            font.printf(2, my, Bitmap::makeColor( 255, 255, 255 ), area, *str, 0);
            my += font.getHeight();

        }
        // current_y = wrapPrint( area, current_y, current_y, font, s, "" );
        /*
           int lines = font.textLength( s.c_str() ) / area.getWidth();
           current_y -= font.getHeight() * lines;
           int my = current_y;
           while ( s != "" ){
           string sub = s.substr( 0, max_length );
           s.erase( 0, max_length );
           font.printf( 0, my, Bitmap::makeColor( 255, 255, 255 ), area, sub, 0 );
           my += font.getHeight();
           }
           */
        current_y -= font.getHeight();
    }
}
	
Messages::~Messages(){
}
