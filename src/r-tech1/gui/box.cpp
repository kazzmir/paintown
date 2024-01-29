#include "r-tech1/graphics/bitmap.h"
#include "r-tech1/graphics/texture-cache.h"
#include "r-tech1/gui/box.h"
#include "r-tech1/font.h"
// #include "r-tech1/debug.h"

using namespace Gui;

Box::Box(){
    // Nothing yet
}

Box::Box( const Box & b ):
Widget(){
    this->location = b.location;
    this->transforms = b.transforms;
}

Box::~Box(){
    // Nothing yet
}

Box &Box::operator=( const Box &copy){
    location = copy.location;
    transforms = copy.transforms;

    return *this;
}

// Logic
void Box::act(const Font & font){
    // Nothing yet
}

// Render
void Box::render(const Graphics::Bitmap & work){
    Util::ReferenceCount<Graphics::Bitmap> workArea = checkWorkArea(work);
    if (workArea != NULL){
        // Graphics::Bitmap::transBlender(0, 0, 0, colors.bodyAlpha);
        // Check if we are using a rounded box
        if (transforms.getRadius() > 0){

            std::string key = std::string(__FILE__) + ":" + std::to_string(__LINE__) + ":" + std::to_string(workArea->getWidth()) + ":" + std::to_string(workArea->getHeight());

            // Graphics::Bitmap round = Graphics::makeRoundedRect(workArea->getWidth(), workArea->getHeight(), transforms.getRadius(), colors.body, colors.border);
            std::shared_ptr<Graphics::Bitmap> round = Graphics::TextureCache::cache.current()->getTexture(key, [&]{
                // DebugLog << "Generate box texture" << std::endl;
                return std::shared_ptr<Graphics::Bitmap>(new Graphics::Bitmap(Graphics::makeRoundedRect(workArea->getWidth(), workArea->getHeight(), transforms.getRadius(), colors.body, colors.border)));
            });

            round->translucent(colors.bodyAlpha).draw(0, 0, *workArea);

            /*
            area.roundRectFill((int)transforms.getRadius(), 0, 0, area.getWidth() - 1, area.getHeight()-1, colors.body);
            area.roundRect((int)transforms.getRadius(), 0, 0, area.getWidth()-1, area.getHeight()-1, colors.border);
            */
        } else {
            const Graphics::TranslucentBitmap area = workArea->translucent(colors.bodyAlpha);
            area.rectangleFill(0, 0, area.getWidth()-1, area.getHeight()-1, colors.body);
            area.rectangle(0, 0, area.getWidth()-1, location.getHeight()-1, colors.border);
        }
        // workArea->translucent().draw(location.getX(), location.getY(), work);
    }
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
