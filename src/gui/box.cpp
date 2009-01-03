#include "util/bitmap.h"
#include "gui/box.h"

Box::Box()
{
	// Nothing yet
}

Box::Box( const Box & b ){
}

Box::~Box()
{
	// Nothing yet
}

Box &Box::operator=( const Box &copy){
	position = copy.position;
	workArea = copy.workArea;
	
	return *this;
}

// Logic
void Box::logic()
{
	// Nothing yet
}

// Render
void Box::render(const Bitmap *work)		
{
	checkWorkArea();
	// Check if we are using a rounded box
	if(position.radius>0)
	{
		roundRectFill( workArea, position.radius, 0, 0, position.width-1, position.height-1, position.body );
		roundRect( workArea, position.radius, 0, 0, position.width-1, position.height-1, position.border );
	}
	else
	{
		workArea->rectangleFill( 0, 0, position.width-1, position.height-1, position.body );
		workArea->rectangle( 0, 0, position.width-1, position.height-1, position.border );
	}
	Bitmap::transBlender( 0, 0, 0, position.bodyAlpha );
	workArea->drawingMode( Bitmap::MODE_TRANS );
	workArea->drawTrans(position.x,position.y,*work);
	work->drawingMode( Bitmap::MODE_SOLID );
}

