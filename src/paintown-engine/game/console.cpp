#include "console.h"
#include "util/bitmap.h"
#include "util/trans-bitmap.h"
#include "util/font.h"
#include "util/funcs.h"
#include "util/file-system.h"
#include "exceptions/exception.h"
#include "util/input/input-manager.h"
#include "util/input/input-map.h"
#include "globals.h"
#include "util/debug.h"
#include <string>
#include <sstream>
#include <string.h>

using namespace std;

namespace Console{

ConsoleEnd Console::endl;

static void doProcess(void * self){
    Console * console = (Console*) self;
    console->activate();
}

static void doToggle(void * self){
    Console * console = (Console*) self;
    console->toggle();
}

Console::Console(const int maxHeight, const Filesystem::RelativePath & font):
state(Closed),
maxHeight(maxHeight),
height(0),
font(font),
textHeight(15),
textWidth(15),
offset(0){
    textInput.addBlockingHandle(Keyboard::Key_TILDE, doToggle, this);
    textInput.addBlockingHandle(Keyboard::Key_ENTER, doProcess, this);
}

Console::~Console(){
    textInput.disable();
    // InputManager::releaseInput(input);
    for (map<string, Command*>::iterator it = commands.begin(); it != commands.end(); it++){
        delete (*it).second;
    }
}
    
void Console::addCommand(const std::string & name, Command * command){
    if (commands[name] != 0){ Global::debug(0) << "Warning: duplicate console command for '" << name << "'" << std::endl;
        delete commands[name];
    }
    commands[name] = command;
}
    
void Console::act(){
    double speed = 10.0;
    switch (state){
        case Closed : {
            break;
        }
        case Open : {
            break;
        }
        case Closing : {
            int distance = height;
            height -= (int)((double)distance / speed + 1.0);
            if (height <= 0){
                height = 0;
                state = Closed;
            }
            break;
        }
        case Opening : {
            int distance = maxHeight - height;
            height += (int)((double)distance / speed + 1.0);
            if (height >= maxHeight){
                height = maxHeight;
                state = Open;
            }
            break;
        }
    }
    checkStream();
}

/*
static bool isChar(char c){
    const char * letters = "abcdefghijklmnopqrstuvwxyz ,.";
    return strchr(letters, c) != NULL;
}
*/

void Console::activate(){
    if (textInput.getText() != ""){
        process(textInput.getText());
    }
    textInput.clearInput();
}

/* console input */
bool Console::doInput() {
    if (state != Open){
        return false;
    }

    textInput.doInput();

    return true;
}

void Console::draw(const Graphics::Bitmap & work){
    /* if we can show something */
    if (height > 0){
        // Graphics::Bitmap::drawingMode(Bitmap::MODE_TRANS);
        Graphics::Bitmap::transBlender(0, 0, 0, 160);
        work.translucent().rectangleFill(0, 0, work.getWidth(), height, Graphics::makeColor(200,0,0));
        work.translucent().horizontalLine(0, height, work.getWidth(), Graphics::makeColor(200, 200, 200));
        const Font & font = Font::getFont(getFont(), textWidth, textHeight);
        //font.printf(0, height - font.getHeight(), Bitmap::makeColor(255, 255, 255), work, "Console!", 0 );
        // Bitmap::drawingMode(Bitmap::MODE_SOLID);
	// if (state == Open){
            if (!lines.empty()){
                int start = height - font.getHeight() * 2;
                for (std::vector<std::string>::reverse_iterator i = lines.rbegin(); i != lines.rend() && start > 0; ++i){
                    std::string str = *i;
                    font.printf(0, start, Graphics::makeColor(255,255,255), work, str, 0);
                    start -= font.getHeight();
                }
            }
            font.printf(0, height - font.getHeight(), Graphics::makeColor(255,255,255), work, "> " + textInput.getText() + "|", 0);
        // }
    }
}
    
void Console::toggle(){
    switch (state){
        case Open:
        case Opening: {
            state = Closing;
            textInput.disable();
            break;
        }
        case Closed:
        case Closing: {
            state = Opening;
            textInput.enable();
            break;
        }
    }
}

static vector<string> split(string str, char splitter){
    vector<string> strings;
    size_t next = str.find(splitter);
    while (next != string::npos){
        strings.push_back(str.substr(0, next));
        str = str.substr(next+1);
        next = str.find(splitter);
    }
    if (str != ""){
        strings.push_back(str);
    }

    return strings;
}

/* do something with a command */
void Console::process(const string & command){
    if (commands[command] != 0){
        string out = commands[command]->act();
        vector<string> each = split(out, '\n');
        for (vector<string>::iterator it = each.begin(); it != each.end(); it++){
            lines.push_back(*it);
        }
    } else {
        lines.push_back("Unknown command '" + command + "'");
    }
}
    
Console & Console::operator<<(const ConsoleEnd & e){
    // checkStream();
    return *this;
}

void Console::checkStream(){
    /*
    if (!textInput.str().empty()){
	lines.push_back(textInput.str());
	textInput.str("");
    }
    */
}

void Console::clear(){
    lines.clear();
}

}
