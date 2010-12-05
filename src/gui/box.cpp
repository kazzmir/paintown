#include "util/bitmap.h"
#include "box.h"

#include "menu/menu.h"

#include "util/font.h"

using namespace Gui;

Box::Box(){
	// Nothing yet
}

Box::Box( const Box & b ){
    this->location = b.location;
    this->workArea = b.workArea;
}

Box::~Box(){
	// Nothing yet
}

Box &Box::operator=( const Box &copy){
    location = copy.location;
    workArea = copy.workArea;

    return *this;
}

// Logic
void Box::act(const Font & font){
	// Nothing yet
}

// Render
void Box::render(const Bitmap & work){
    checkWorkArea();
    // Check if we are using a rounded box
    if (location.getRadius() > 0){
        roundRectFill(*workArea, (int)location.getRadius(), 0, 0, location.getWidth()-1, location.getHeight()-1, colors.body);
        roundRect(*workArea, (int)location.getRadius(), 0, 0, location.getWidth()-1, location.getHeight()-1, colors.border);
    } else {
        workArea->rectangleFill(0, 0, location.getWidth()-1, location.getHeight()-1, colors.body );
        workArea->rectangle(0, 0, location.getWidth()-1, location.getHeight()-1, colors.border );
    }
    Bitmap::transBlender( 0, 0, 0, colors.bodyAlpha );
    // workArea->drawingMode( Bitmap::MODE_TRANS );
    workArea->drawTrans(location.getX(), location.getY(), work);
    // work.drawingMode( Bitmap::MODE_SOLID );
}

void Box::messageDialog(int centerWidth, int centerHeight, const std::string & message, int radius){
    /* FIXME Get rid of this */
    #if 0
    const Font &vFont = Font::getFont(OldMenu::Menu::getFont(),OldMenu::Menu::getFontWidth(),OldMenu::Menu::getFontHeight());
    const int width = vFont.textLength(message.c_str()) + 10;
    const int height = vFont.getHeight() + 10;
    const int x = (centerWidth/2) - (width/2);
    const int y = (centerHeight/2) - (height/2);
    Box dialog;
    dialog.location.setDimensions(width, height);
    dialog.location.setRadius(radius);
    dialog.colors.body = Bitmap::makeColor(0,0,0);
    dialog.colors.bodyAlpha = 200;
    dialog.colors.border = Bitmap::makeColor(255,255,255);
    dialog.colors.borderAlpha = 255;
    Bitmap temp = Bitmap::temporaryBitmap(width,height);
    dialog.render(temp);
    vFont.printf( 5, 5, Bitmap::makeColor(255,255,255), temp, message, -1);
    temp.BlitToScreen(x,y);
    #endif
}
