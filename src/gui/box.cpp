#include "util/bitmap.h"
#include "gui/box.h"

#include "menu/menu.h"

#include "util/font.h"

Box::Box(){
	// Nothing yet
}

Box::Box( const Box & b ){
    this->position = b.position;
    this->workArea = b.workArea;
}

Box::~Box(){
	// Nothing yet
}

Box &Box::operator=( const Box &copy){
    position = copy.position;
    workArea = copy.workArea;

    return *this;
}

// Logic
void Box::logic(){
	// Nothing yet
}

// Render
void Box::render(const Bitmap *work){
    checkWorkArea();
    // Check if we are using a rounded box
    if(position.radius>0){
        roundRectFill( workArea, position.radius, 0, 0, position.width-1, position.height-1, position.body );
        roundRect( workArea, position.radius, 0, 0, position.width-1, position.height-1, position.border );
    } else {
        workArea->rectangleFill( 0, 0, position.width-1, position.height-1, position.body );
        workArea->rectangle( 0, 0, position.width-1, position.height-1, position.border );
    }
    Bitmap::transBlender( 0, 0, 0, position.bodyAlpha );
    workArea->drawingMode( Bitmap::MODE_TRANS );
    workArea->drawTrans(position.x,position.y,*work);
    work->drawingMode( Bitmap::MODE_SOLID );
}

void Box::msgDialog(const Bitmap & bmp, const std::string & message, int radius){
    const Font &vFont = Font::getFont(Menu::getFont(),Menu::getFontWidth(),Menu::getFontHeight());
    const int width = vFont.textLength(message.c_str()) + 10;
    const int height = vFont.getHeight() + 10;
    const int x = (bmp.getWidth()/2) - (width/2);
    const int y = (bmp.getHeight()/2) - (height/2);
    Box dialog;
    dialog.position.width = width;
    dialog.position.height = height;
    dialog.position.radius = radius;
    dialog.position.body = Bitmap::makeColor(0,0,0);
    dialog.position.bodyAlpha = 200;
    dialog.position.border = Bitmap::makeColor(255,255,255);
    dialog.position.borderAlpha = 255;
    Bitmap temp = Bitmap::temporaryBitmap(width,height);
    dialog.render(&temp);
    vFont.printf( 5, 5, Bitmap::makeColor(255,255,255), temp, message, -1);
    temp.BlitToScreen(x,y);
}
