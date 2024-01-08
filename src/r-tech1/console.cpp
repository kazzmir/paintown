#include "r-tech1/console.h"
#include "r-tech1/graphics/bitmap.h"
#include "r-tech1/font.h"
#include "r-tech1/funcs.h"
#include "r-tech1/file-system.h"
#include "r-tech1/exceptions/exception.h"
#include "r-tech1/input/input-manager.h"
#include "r-tech1/input/input-map.h"
#include "r-tech1/debug.h"
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

static void doPreviousHistory(void * self){
    Console * console = (Console*) self;
    console->previousHistory();
}

static void doNextHistory(void * self){
    Console * console = (Console*) self;
    console->nextHistory();
}

static void doTabComplete(void * self){
    Console * console = (Console*) self;
    console->tabComplete();
}

static void doPageUp(void * self){
    Console * console = (Console*) self;
    console->pageUp();
}

static void doPageDown(void * self){
    Console * console = (Console*) self;
    console->pageDown();
}

static void doFontIncrease(void * self){
    Console * console = (Console*) self;
    console->fontIncrease();
}

static void doFontDecrease(void * self){
    Console * console = (Console*) self;
    console->fontDecrease();
}

Console::Console(const int maxHeight, const Filesystem::RelativePath & font):
state(Closed),
maxHeight(maxHeight),
height(0),
font(font),
textHeight(15),
textWidth(15),
offset(0),
historyIndex(0),
pagePosition(-1){
    textInput.addBlockingHandle(Keyboard::Key_TILDE, doToggle, this);
    textInput.addBlockingHandle(Keyboard::Key_ENTER, doProcess, this);
    textInput.addBlockingHandle(Keyboard::Key_UP, doPreviousHistory, this);
    textInput.addBlockingHandle(Keyboard::Key_DOWN, doNextHistory, this);
    textInput.addBlockingHandle(Keyboard::Key_TAB, doTabComplete, this);
    textInput.addBlockingHandle(Keyboard::Key_PGUP, doPageUp, this);
    textInput.addBlockingHandle(Keyboard::Key_PGDN, doPageDown, this);
    textInput.addBlockingHandle(Keyboard::Key_EQUALS, doFontIncrease, this);
    textInput.addBlockingHandle(Keyboard::Key_MINUS, doFontDecrease, this);
}

Console::~Console(){
    textInput.disable();
}

/* attempt to complete the current text to a command */
void Console::tabComplete(){
    string text = textInput.getText();
    for (map<string, Util::ReferenceCount<Command> >::iterator it = commands.begin(); it != commands.end(); it++){
        string what = it->first;
        if (what.find(text) == 0){
            Global::debug(0) << "Auto complete '" << what << "'" << std::endl;
            textInput.setText(what);
            return;
        }
    }
}
    
void Console::addCommand(const std::string & name, const Util::ReferenceCount<Command> & command){
    if (command == NULL){
        return;
    }

    if (commands[name] != 0){
        Global::debug(0) << "Warning: duplicate console command for '" << name << "'" << std::endl;
    }
    commands[name] = command;
}
    
void Console::addAlias(const std::string & alias, const std::string & name){
    addCommand(alias, commands[name]);
}

void Console::fontIncrease(){
    textWidth += 1;
    textHeight += 1;
}

void Console::fontDecrease(){
    if (textWidth > 2){
        textWidth -= 1;
    }
    if (textHeight > 2){
        textHeight -= 1;
    }
}

void Console::previousHistory(){
    if (historyIndex < history.size()){
        textInput.setText(history[historyIndex]);
        historyIndex += 1;
    }
}

void Console::nextHistory(){
    if (historyIndex > 0){
        historyIndex -= 1;
        textInput.setText(history[historyIndex]);
    } else {
        textInput.setText(string());
    }
}

static const int PAGE_MOVEMENT = 3;
void Console::pageUp(){
    if (pagePosition == -1){
        pagePosition = lines.size();
    }
    pagePosition -= PAGE_MOVEMENT;
    if (pagePosition < PAGE_MOVEMENT){
        pagePosition = PAGE_MOVEMENT;
    }
}

void Console::pageDown(){
    if (pagePosition != -1){
        pagePosition += PAGE_MOVEMENT;
    }
    if (pagePosition >= (int) lines.size()){
        pagePosition = -1;
    }
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
    
bool Console::isActive() const {
    return state == Open || state == Opening;
}

/* console input */
bool Console::doInput() {
    if (state == Closed || state == Closing){
        return false;
    }

    textInput.doInput();

    return true;
}

void Console::draw(const Graphics::Bitmap & work){
    int x = 3;
    /* if we can show something */
    if (height > 0){
        // Graphics::Bitmap::transBlender(0, 0, 0, 160);
        work.translucent(160).rectangleFill(0, 0, work.getWidth(), height, Graphics::makeColor(200,0,0));
        work.translucent(160).horizontalLine(0, height, work.getWidth(), Graphics::makeColor(200, 200, 200));
        const Font & font = Font::getFont(getFont(), textWidth, textHeight);
        int start = height - font.getHeight() * 2;
        Graphics::Color white = Graphics::makeColor(255, 255, 255);

        int position = 0;
        if (pagePosition != -1){
            position = lines.size() - pagePosition;
        }
        for (std::vector<std::string>::reverse_iterator i = lines.rbegin() + position; i != lines.rend() && start > 0; ++i){
            std::string str = *i;
            font.printf(x, start, white, work, str, 0);
            start -= font.getHeight();
        }
        if (position != 0){
            for (int x0 = 0; x0 < 5; x0++){
                work.circleFill(x + x0 * 5, height - font.getHeight() - 2, 1, white);
            }
        }
        font.printf(x, height - font.getHeight(), white, work, "> " + textInput.getText() + "|", 0);
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
    /* reset history index */
    historyIndex = 0;
    /* don't duplicate history */
    if (history.size() == 0 || (history.size() > 0 && !(command == history[1]))){
        history.push_front(command);
    }
    string start = command;
    if (command.find(' ') != string::npos){
        start = command.substr(0, command.find(' '));
    }

    const map<string, Util::ReferenceCount<Command> > & commandSet = commands;
    if (commandSet.find(start) != commandSet.end()){
        map<string, Util::ReferenceCount<Command> >::const_iterator found = commandSet.find(start);
        addLine(found->second->act(command));
    } else {
        addLine("Unknown command '" + start + "'");
    }
}
    
void Console::addLine(const std::string & line){
    vector<string> each = split(line, '\n');
    for (vector<string>::iterator it = each.begin(); it != each.end(); it++){
        lines.push_back(*it);
    }
}

Console & Console::operator<<(const ConsoleEnd & e){
    return *this;
}

void Console::clear(){
    lines.clear();
}

vector<Util::ReferenceCount<Command> > Console::getCommands() const {
    vector<Util::ReferenceCount<Command> > out;
    for (map<std::string, Util::ReferenceCount<Command> >::const_iterator it = commands.begin(); it != commands.end(); it++){
        Util::ReferenceCount<Command> command = it->second;
        if (command != NULL){
            out.push_back(command);
        }
    }
    return out;
}

}
