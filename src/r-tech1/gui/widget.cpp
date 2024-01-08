#include "r-tech1/graphics/bitmap.h"
#include "r-tech1/funcs.h"
#include "r-tech1/gui/widget.h"
#include <math.h>
#include "r-tech1/token.h"
#include "r-tech1/gui/coordinate.h"
#include "r-tech1/exceptions/load_exception.h"
#include <sstream>

using namespace Gui;

static void round_double (double dbl_to_round , int &rounded_num) {
    rounded_num = static_cast<int>(dbl_to_round);
    if ((dbl_to_round - static_cast<double>(rounded_num)) >= 0.5) {rounded_num++;}
}

ColorInfo::ColorInfo():
body(Graphics::makeColor(0, 0, 0)),
/* alpha 0 is invisible, 255 is opaque. set something in the middle as default */
bodyAlpha(128),
border(Graphics::makeColor(0, 0, 0)),
borderAlpha(128){
}

Transformations::Transformations():
radius(0){
}

Transformations::Transformations(const Transformations & transforms):
radius(transforms.radius){
}

Transformations::~Transformations(){
}

Transformations & Transformations::operator=(const Transformations & transforms){
    this->radius = transforms.radius;
    return *this;
}

Widget::Widget(){
    // Nothing yet
}
        
Widget::Widget( const Widget & w ){
    this->location = w.location;
    this->transforms = w.transforms;
}

Widget::~Widget(){
}

// copy
Widget &Widget::operator=( const Widget &copy){
    this->location = copy.location;
    this->transforms = copy.transforms;

    return *this;
}

void Widget::setCoordinates(const Token * token){
    if ( *token == "position" ){
        int x, y, width, height;
        token->view() >> x >> y >> width >> height;
        AbsolutePoint pos(x, y);
        AbsolutePoint dimensions(x + width, y + height);
        location.setPosition(pos);
        location.setPosition2(dimensions);
    } else if ( *token == "relative-position" ){
        double x1, y1, x2, y2;
        token->view() >> x1 >> y1 >> x2 >> y2;
        RelativePoint pos(x1,y1);
        RelativePoint dimensions(x2,y2);
        location = Coordinate(pos, dimensions);
    } else if ( *token == "coordinate" ){
        TokenView view = token->view();
        while (view.hasMore()){
            const Token * coordToken;
            view >> coordToken;
            if (*coordToken == "absolute"){
                int x, y, width, height;
                coordToken->view() >> x >> y >> width >> height;
                AbsolutePoint pos(x, y);
                AbsolutePoint dimensions(x + width, y + height);
                location = Coordinate(pos, dimensions);
            } else if (*coordToken == "relative"){
                double x1, y1, x2, y2;
                coordToken->view() >> x1 >> y1 >> x2 >> y2;
                RelativePoint pos(x1,y1);
                RelativePoint dimensions(x2,y2);
                location = Coordinate(pos, dimensions);
            } else if (*coordToken == "z"){
                double z;
                coordToken->view() >> z;
                location.setZ(z);
            }
        }
    }

    if (location.getWidth() < 0 || location.getHeight() < 0){
        std::ostringstream out;
        out << "Invalid location dimension (cannot have a negative size). Width " << location.getWidth() << " Height " << location.getHeight() << ". From token: ";
        token->toString(out, "");
        throw LoadException(__FILE__, __LINE__, out.str());
    }
}

void Widget::setColors(const Token * token){
    if ( *token == "position-body" ) {
        // This handles the body color of the widget
        int r,g,b;
        token->view() >> r >> g >> b >> colors.bodyAlpha;
        colors.body = Graphics::makeColor(r,g,b);
    } else if ( *token == "position-border" ) {
        // This handles the border color of the widget
        int r,g,b;
        token->view() >> r >> g >> b >> colors.borderAlpha;
        colors.border = Graphics::makeColor(r,g,b);
    } 
}

void Widget::setTransforms(const Token * token){
    try{
        if ( *token != "transforms" ){
            throw LoadException(__FILE__, __LINE__, "Not a Widget Transforms.");
        }
        TokenView view = token->view();
        while (view.hasMore()){
            const Token * tok;
            view >> tok;
            double radius = 0;
            try{
                if (tok->match("radius", radius)){
                    transforms.setRadius(radius);
                } else {
                    Global::debug(0) << "Unknown Transforms property: " << tok->getName() << std::endl;
                }
            } catch (const TokenException & ex){
                throw LoadException(__FILE__, __LINE__, ex, "Transforms parse error");
            } catch ( const LoadException & ex ) {
                throw ex;
            }
        }
    } catch (const TokenException & e){
        throw LoadException(__FILE__, __LINE__, e, "Error reading transforms.");
    }
}
        
void Widget::render(const Graphics::Bitmap & bitmap, const Font & font){
    render(bitmap);
}

void Widget::drawBox(int radius, int x1, int y1, int x2, int y2, const Gui::ColorInfo & colors, const Graphics::Bitmap & work){
    // rounded body?
    if (radius > 0){
        if (colors.bodyAlpha < 255){
            // Graphics::Bitmap::transBlender(0,0,0,colors.bodyAlpha);
            work.translucent(colors.bodyAlpha).roundRectFill(radius, x1, y1, x2, y2, colors.body);
            // Graphics::Bitmap::transBlender(0,0,0,colors.borderAlpha);
            work.translucent(colors.borderAlpha).roundRect(radius, x1, y1, x2-1, y2-1, colors.border);
        } else {
            work.roundRectFill(radius, x1, y1, x2, y2, colors.body);
            work.roundRect(radius, x1, y1, x2-1, y2-1, colors.border);
        }
    } else {
        if (colors.bodyAlpha < 255){
            // Graphics::Bitmap::transBlender(0,0,0,colors.bodyAlpha);
            work.translucent(colors.bodyAlpha).rectangleFill(x1, y1, x2, y2, colors.body );
            // Graphics::Bitmap::transBlender(0,0,0,colors.borderAlpha);
            work.translucent(colors.borderAlpha).vLine(y1,x1,y2-1,colors.border);
            work.translucent(colors.borderAlpha).hLine(x1,y2-1,x2,colors.border);
            work.translucent(colors.borderAlpha).vLine(y1,x2-1,y2-1,colors.border);
        } else {
            work.rectangleFill(x1, y1, x2, y2, colors.body );
            work.vLine(y1,x1,y2-1,colors.border);
            work.hLine(x1,y2-1,x2,colors.border);
            work.vLine(y1,x2-1,y2-1,colors.border);
        }
    }
}

Util::ReferenceCount<Graphics::Bitmap> Widget::checkWorkArea(const Graphics::Bitmap & parent){
    if (location.getWidth() <= 0 || location.getHeight() <= 0){
        return Util::ReferenceCount<Graphics::Bitmap>(NULL);
    }

    return Util::ReferenceCount<Graphics::Bitmap>(new Graphics::Bitmap(parent, location.getX(), location.getY(), location.getWidth(), location.getHeight()));
}
