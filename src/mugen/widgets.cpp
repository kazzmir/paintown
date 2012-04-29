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
width(0),
cursorTime(0){
    input.enable();
}

InputBox::~InputBox(){
}

void InputBox::act(){
    if (input.doInput()){
    }
    cursorTime++;
    if (cursorTime > 60){
        cursorTime = 0;
    }
}

void InputBox::draw(int x, int y, const FontSystem::Font & font, const Graphics::Bitmap & work){
    const int height = font.getHeight()+2;
    drawBox(10, x, y, width, height, body, border, alpha, work);
    
    Graphics::Bitmap temp(work, x+5, y, width-10, height);
    if (font.getWidth(input.getText()) >= width-10){
        font.draw(width-10, height-1, -1, input.getText(), temp);
        renderCursor(width-11, height-1, font, temp);
    } else {
        font.draw(0, height-1, 1, input.getText(), temp);
        renderCursor(font.getWidth(input.getText())+1, height-1, font, temp);
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

void InputBox::renderCursor(int x, int y, const FontSystem::Font & font, const Graphics::Bitmap & work){
    if (cursorTime <= 30){
        font.draw(x, y, 0, "|", work);
    }
}

static void submit(void * panel){
    ChatPanel * chat = (ChatPanel *)panel;
    if (!chat->getInput().getText().empty()){
        const std::string & text = chat->getInput().getText();
        if (!chat->getClient().empty()){
            chat->addMessage(chat->getClient(), text);
        } else {
            chat->addMessage(text);
        }
        chat->notify(text);
        chat->getInput().clear();
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

ChatPanel::Event::Event(){
}
ChatPanel::Event::~Event(){
}

ChatPanel::~ChatPanel(){
}

void ChatPanel::act(){
    input.act();
}

void ChatPanel::draw(const Graphics::Bitmap & work){
    drawBox(10, x, y, width, height - font.getHeight(), Graphics::makeColor(0,0,60), Graphics::makeColor(0,0,20), 150, work);
    input.draw(x, y + (height - font.getHeight()), font, work);
    int mod = height-(font.getHeight()+2);
    
     Graphics::Bitmap temp(work, x+5, y, width-10, height-(font.getHeight()+2));
    for (std::deque<std::string>::iterator i = buffer.begin(); i != buffer.end(); ++i){
        const std::string & text = *i;
        font.draw(0, mod, 1, text, temp);
        mod-=(font.getHeight()+2);
    }
}

void ChatPanel::notify(const std::string & message){
    for (std::vector<EventCallback>::iterator i = callbacks.begin(); i != callbacks.end(); ++i){
        (*i)(message);
    }
    
    for (std::vector<Event *>::iterator i = subscribers.begin(); i != subscribers.end(); ++i){
        (*i)->addMessage(message);
    }
}

void ChatPanel::addMessage(const std::string & message){
    buffer.push_front(message);
    if ((buffer.size() * (font.getHeight()+2)) > height){
        buffer.pop_back();
    }
}

void ChatPanel::addMessage(const std::string & name, const std::string & message){
    addMessage("<"+name+"> " + message);
}

}
}
