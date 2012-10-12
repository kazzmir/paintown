#include "widgets.h"

#include "util/trans-bitmap.h"
#include "util/network/network.h"
#include "util/input/keyboard.h"
#include "util/exceptions/exception.h"

#include <stdexcept>

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

/* FIXME: move this to some utils module */
static std::vector<std::string> split(std::string str, char splitter){
    std::vector<std::string> strings;
    size_t next = str.find(splitter);
    while (next != std::string::npos){
        strings.push_back(str.substr(0, next));
        str = str.substr(next+1);
        next = str.find(splitter);
    }
    if (str != ""){
        strings.push_back(str);
    }

    return strings;
}

static void submit(void * panel){
    ChatPanel * chat = (ChatPanel *)panel;
    if (!chat->getInput().getText().empty()){
        const std::string & text = chat->getInput().getText();
        // check if it's a command
        if (PaintownUtil::matchRegex(text, PaintownUtil::Regex("^/.*"))){
            const std::vector<std::string> & command = split(text.substr(1), ' ');
            chat->handleCommand(command);
        } else {
            if (!chat->getClient().empty()){
                chat->addMessage(chat->getClient(), text);
            } else {
                chat->addMessage(text);
            }
            chat->notify(text);
        }
        chat->getInput().clear();
    }
}

ListBox::ListBox():
width(0){
}

ListBox::~ListBox(){
}

void ListBox::draw(int x, int y, const FontSystem::Font & font, const Graphics::Bitmap & work){
    int height = list.size() * (font.getHeight()+2);
    
    drawBox(10, x, y, width, height, Graphics::makeColor(0,0,60), Graphics::makeColor(0,0,20), 150, work);
    
    int mod = font.getHeight() + 2;
    
    Graphics::Bitmap temp(work, x+5, y, width-10, height);
    for (std::vector<std::string>::iterator i = list.begin(); i != list.end(); ++i){
        const std::string & text = *i;
        font.draw(0, mod, 1, text, temp);
        mod+=(font.getHeight()+2);
    }
}

void ListBox::add(const std::string & text){
    list.push_back(text);
}

void ListBox::add(const std::vector<std::string> & more){
    list.insert(list.end(),more.begin(),more.end());
}

void ListBox::remove(const std::string & text){
    for (std::vector<std::string>::iterator i = list.begin(); i != list.end(); ++i){
        const std::string & match = *i;
        if (text == match){
            list.erase(i);
            return;
        }
    }
}

void ListBox::replace(const std::vector<std::string> & replace){
    list = replace;
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
void ChatPanel::Event::handleCommand(const std::vector<std::string> & command){
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
        (*i)->sendMessage(message);
    }
}

void ChatPanel::addMessage(const std::string & message){
    // Check message if it exceeds the length of the box so we can split it
    if (font.getWidth(message) > width-15){
        unsigned int marker = 0;
        unsigned int length = 0;
        while ((marker+length) < message.size()){
            //Global::debug(0) << "Substring: " << message.substr(marker, length) << " Marker: " << marker << " and Current length: " << length << std::endl;
            if (font.getWidth(message.substr(marker, length)) < width-15){
                length++;
                continue;
            } else {
                if (message[marker+length] == ' '){
                    buffer.push_front(message.substr(marker, length));
                    marker += length+1;
                    length = 0;
                } else {
                    // Search for previous space
                    unsigned int cutoff = marker+length;
                    while ((marker+length) > marker){
                        if (message[marker+length] == ' '){
                            break;
                        }
                        length--;
                    }
                    if ((marker+length) > marker){
                        buffer.push_front(message.substr(marker, length));
                        marker += length+1;
                        length = 0;
                    } else {
                        buffer.push_front(message.substr(marker, cutoff));
                        marker = cutoff+1;
                        length = 0;
                    }
                }
            }
        }
        // Add last item
        if ((marker+length) > marker){
            buffer.push_front(message.substr(marker, length));
        }
    } else {
        buffer.push_front(message);
    }
    
    // Drop out of sight
    if ((buffer.size() * (font.getHeight()+2)) > height){
        buffer.pop_back();
    }
}

void ChatPanel::addMessage(const std::string & name, const std::string & message){
    addMessage("<"+name+"> " + message);
}

void ChatPanel::handleCommand(const std::vector<std::string> & command){
    for (std::vector<Event *>::iterator i = subscribers.begin(); i != subscribers.end(); ++i){
        (*i)->handleCommand(command);
    }
}

}
}
