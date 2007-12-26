#include "gui/box.h"
#include "util/bitmap.h"

Box::Box()
{
	// Nothing yet
}

Box::~Box()
{
	// Nothing yet
}


// Logic
void Box::logic()
{
	// Nothing yet
}

// Render
void Box::render(Bitmap *work)		
{
	// Check if we are using a rounded box
	if(position.radius>0)
	{
		if(position.bodyAlpha < 255)
		{
			Bitmap::transBlender( 0, 0, 0, position.bodyAlpha );
			work->drawingMode( Bitmap::MODE_TRANS );
			roundRectFill( work, position.radius, position.x, position.y, position.getX2(), position.getY2(), position.body );
		}
		else 
		{
			work->drawingMode( Bitmap::MODE_SOLID );
			roundRectFill( work, position.radius, position.x, position.y, position.getX2(), position.getY2(), position.body );
		}
		if(position.borderAlpha < 255)
		{
			Bitmap::transBlender( 0, 0, 0, position.borderAlpha );
			work->drawingMode( Bitmap::MODE_TRANS );
			roundRect( work, position.radius, position.x, position.y, position.getX2(), position.getY2(), position.border );
		}
		else 
		{
			work->drawingMode( Bitmap::MODE_SOLID );
			roundRect( work, position.radius, position.x, position.y, position.getX2(), position.getY2(), position.border );
		}
	}
	else
	{
		if(position.bodyAlpha < 255)
		{
			Bitmap::transBlender( 0, 0, 0, position.bodyAlpha );
			work->drawingMode( Bitmap::MODE_TRANS );
			work->rectangleFill( position.x, position.y, position.getX2(), position.getY2(), position.body );
		}
		else 
		{
			work->drawingMode( Bitmap::MODE_SOLID );
			work->rectangleFill( position.x, position.y, position.getX2(), position.getY2(), position.body );
		}
		if(position.borderAlpha < 255)
		{
			Bitmap::transBlender( 0, 0, 0, position.borderAlpha );
			work->drawingMode( Bitmap::MODE_TRANS );
			work->rectangle( position.x, position.y, position.getX2(), position.getY2(), position.border );
		}
		else 
		{
			work->drawingMode( Bitmap::MODE_SOLID );
			work->rectangle( position.x, position.y, position.getX2(), position.getY2(), position.border );
		}
	}
	work->drawingMode( Bitmap::MODE_SOLID );
}

