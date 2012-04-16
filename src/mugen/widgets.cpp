#include "widgets.h"

#include "util/trans-bitmap.h"
#include "util/network/network.h"
#include "util/input/keyboard.h"
#include "util/exceptions/exception.h"

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
alpha(150),
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

static void submit(void * panel){
    ChatPanel * chat = (ChatPanel *)panel;
    if (!chat->getInput().getText().empty()){
        chat->addMessage(chat->getInput().getText());
    }
}

static void escape(void * panel){
    throw Exception::Return(__FILE__, __LINE__);
}

ChatPanel::ChatPanel(int x, int y, int width, int height):
x(x),
y(y),
width(width),
height(height){
    input.setWidth(width);
    input.addHook(Keyboard::Key_ENTER, submit, this);
    input.addHook(Keyboard::Key_ESC, escape, this);
}

ChatPanel::~ChatPanel(){
}

void ChatPanel::act(){
    input.act();
}

void ChatPanel::draw(const Graphics::Bitmap & work){
    drawBox(15, x, y, width, height - font.getHeight(), Graphics::makeColor(0,0,60), Graphics::makeColor(0,0,20), 150, work);
    input.draw(x, y + (height - font.getHeight()), font, work); 
}

void ChatPanel::addMessage(const std::string & message){
    buffer.push_back(message);
    if ((buffer.size() * font.getHeight()) > height){
        buffer.pop_front();
    }
}

}
}