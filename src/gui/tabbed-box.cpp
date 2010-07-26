#include "util/bitmap.h"
#include "tabbed-box.h"

#include "menu/menu.h"

#include "util/font.h"

#include "gui/context-box.h"

using namespace Gui;

class Tab{
    public:
        Tab():
        context(new ContextBox()){
        }
        ~Tab(){
            delete context;
        }
        
        std::string name;
        Gui::ContextBox * context;
};

TabbedBox::TabbedBox(){
	// Nothing yet
}

TabbedBox::TabbedBox( const TabbedBox & b ){
    this->location = b.location;
    this->workArea = b.workArea;
}

TabbedBox::~TabbedBox(){
	// Nothing yet
}

TabbedBox &TabbedBox::operator=( const TabbedBox &copy){
    location = copy.location;
    workArea = copy.workArea;

    return *this;
}

// Logic
void TabbedBox::act(){
	// Nothing yet
}

// Render
void TabbedBox::render(const Bitmap & work){
    checkWorkArea();
    // Check if we are using a rounded box
    if (location.getRadius() > 0){
        //roundRectFill( *workArea, (int)location.getRadius(), 0, 0, location.getWidth()-1, location.getHeight()-1, colors.body );
        //roundRect( *workArea, (int)location.getRadius(), 0, 0, location.getWidth()-1, location.getHeight()-1, colors.border );
    } else {
        workArea->rectangleFill( 0, 0, location.getWidth()-1, location.getHeight()-1, colors.body );
        workArea->rectangle( 0, fontHeight, location.getWidth()-1, location.getHeight()-1, colors.border );
    }
    Bitmap::transBlender( 0, 0, 0, colors.bodyAlpha );
    // workArea->drawingMode( Bitmap::MODE_TRANS );
    workArea->drawTrans(location.getX(), location.getY(), work);
    // work.drawingMode( Bitmap::MODE_SOLID );
}

