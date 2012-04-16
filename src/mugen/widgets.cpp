#include "widgets.h"

#include "util/trans-bitmap.h"

namespace Mugen{
namespace Widgets{

void drawBox(int radius, int x, int y, int width, int height, const Graphics::Color & body, const Graphics::Color & border, int alpha, const Graphics::Bitmap & where){
    if (radius > 0){
        if (alpha < 255){
            Graphics::Bitmap::transBlender(0,0,0,alpha);
            where.translucent().roundRectFill(radius, x, y, x+width, y+height, body);
            where.translucent().roundRect(radius, x, y, x+width, y+height, border);
        } else {
            where.roundRectFill(radius, x, y, x+width, y+height, body);
            where.roundRect(radius, x, y, x+width, y+height, border);
        }
    } else {
        if (alpha < 255){
            Graphics::Bitmap::transBlender(0,0,0,alpha);
            where.translucent().rectangleFill(x, y, x+width, y+height, body);
            where.translucent().rectangle(x, y, x+width, y+height, border);
        } else {
            where.rectangleFill(x, y, x+width, y+height, body);
            where.rectangle(x, y, x+width, y+height, border);
        }
    }
}

InputBox::InputBox():
body(Graphics::makeColor(0,0,60)),
border(Graphics::makeColor(0,0,20)),
alpha(255),
width(0){
    input.enable();
}

InputBox::~InputBox(){
}

void InputBox::act(){
    if (input.doInput()){
    }
}

void InputBox::draw(int x, int y, const FontSystem::Font & font, const Graphics::Bitmap & work){
    const int height = font.getHeight();
    drawBox(15, x, y, width, height, body, border, alpha, work);
    
    Graphics::Bitmap temp(work, x, y, width, height);
    if (font.getWidth(input.getText()) >= width){
        font.draw(width, height, -1, input.getText(), temp);
    } else {
        font.draw(0, height, 1, input.getText(), temp);
    }
}

void InputBox::toggleEnabled(){
    if (input.isEnabled()){
        input.disable();
    } else {
        input.enable();
    }
}

void InputBox::addHook(int key, void (*callback)(void *), void * arg){
    input.addBlockingHandle(key, callback, arg);
}

}
}