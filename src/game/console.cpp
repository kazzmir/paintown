#include "console.h"
#include "util/bitmap.h"
#include "util/font.h"
#include "util/funcs.h"
#include "util/file-system.h"
#include "input/input-manager.h"
#include "input/input-map.h"
#include "globals.h"
#include <string>
#include <sstream>

using namespace std;

namespace Console{

static int Toggle = 254;
static int Backspace = 9;
static int Esc = 8;
static int Control = 7;
static int Enter = 6;

ConsoleEnd Console::endl;
const std::string Console::DEFAULT_FONT = Global::DEFAULT_FONT;

Console::Console(const int maxHeight, const std::string & font):
state(Closed),
maxHeight(maxHeight),
height(0),
font(font),
textHeight(15),
textWidth(15),
offset(0){
    const int delay = 10;
    input.set(Keyboard::Key_TILDE, delay * 2, false, Toggle);
    input.set(Keyboard::Key_BACKSPACE, delay, false, Backspace);
    input.set(Keyboard::Key_ESC, delay, false, Esc);
    input.set(Keyboard::Key_LCONTROL, 0, false, Control);
    input.set(Keyboard::Key_ENTER, 0, false, Enter);
    
    /* ugh, do we really have to enumerate every key?? */
    input.set(Keyboard::Key_A, delay, false, 'a');
    input.set(Keyboard::Key_B, delay, false, 'b');
    input.set(Keyboard::Key_C, delay, false, 'c');
    input.set(Keyboard::Key_D, delay, false, 'd');
    input.set(Keyboard::Key_E, delay, false, 'e');
    input.set(Keyboard::Key_F, delay, false, 'f');
    input.set(Keyboard::Key_G, delay, false, 'g');
    input.set(Keyboard::Key_H, delay, false, 'h');
    input.set(Keyboard::Key_I, delay, false, 'i');
    input.set(Keyboard::Key_J, delay, false, 'j');
    input.set(Keyboard::Key_K, delay, false, 'k');
    input.set(Keyboard::Key_L, delay, false, 'l');
    input.set(Keyboard::Key_M, delay, false, 'm');
    input.set(Keyboard::Key_N, delay, false, 'n');
    input.set(Keyboard::Key_O, delay, false, 'o');
    input.set(Keyboard::Key_P, delay, false, 'p');
    input.set(Keyboard::Key_Q, delay, false, 'q');
    input.set(Keyboard::Key_R, delay, false, 'r');
    input.set(Keyboard::Key_S, delay, false, 's');
    input.set(Keyboard::Key_T, delay, false, 't');
    input.set(Keyboard::Key_U, delay, false, 'u');
    input.set(Keyboard::Key_V, delay, false, 'v');
    input.set(Keyboard::Key_W, delay, false, 'w');
    input.set(Keyboard::Key_X, delay, false, 'x');
    input.set(Keyboard::Key_Y, delay, false, 'y');
    input.set(Keyboard::Key_Z, delay, false, 'z');
    input.set(Keyboard::Key_SPACE, delay, false, ' ');
    input.set(Keyboard::Key_COMMA, delay, false, ',');
    input.set(Keyboard::Key_STOP, delay, false, '.');
}

Console::~Console(){
    for (map<string, Command*>::iterator it = commands.begin(); it != commands.end(); it++){
        delete (*it).second;
    }
}
    
void Console::addCommand(const std::string & name, Command * command){
    if (commands[name] != 0){
        Global::debug(0) << "Warning: duplicate console command for '" << name << "'" << std::endl;
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

static bool isChar(char c){
    const char * letters = "abcdefghijklmnopqrstuvwxyz ,.";
    return strchr(letters, c) != NULL;
}

/* console input */
bool Console::doInput() {
    InputMap<char>::Output inputState = InputManager::getMap(input);

    /* the order of reading input is arbitrary right now. I'm not
     * sure it matters what order things are done in, but probably
     * a few corner cases exist. When they come up please document them.
     */

    if (inputState[Toggle]){
        toggle();
        return false;
    }

    if (inputState[Enter]){
        if (currentCommand.str() != ""){
            process(currentCommand.str());
        }
        clearInput();
    }

    /* ctrl-X keys */
    if (inputState[Control]){
        /* standard linux console commands */
        if (inputState['u']){
            clearInput();
            /* ignore any other input */
            return true;
        }
        if (inputState['w']){
            deleteLastWord();
            return true;
        }
    }

    for (InputMap<char>::Output::iterator it = inputState.begin(); it != inputState.end(); it++){
        char c = (*it).first;
        bool pressed = (*it).second;
        if (pressed && isChar(c)){
            currentCommand << c;
        }
    }

    if (inputState[Esc]){
        throw ReturnException();
    }

    if (inputState[Backspace]){
        backspace();
    }

    return true;
}

void Console::draw(const Bitmap & work){
    /* if we can show something */
    if (height > 0){
        Bitmap::drawingMode(Bitmap::MODE_TRANS);
	Bitmap::transBlender(0, 0, 0, 160);
        work.rectangleFill(0, 0, work.getWidth(), height, Bitmap::makeColor(200,0,0));
        work.horizontalLine(0, height, work.getWidth(), Bitmap::makeColor(200, 200, 200));
        const Font & font = Font::getFont(Filesystem::find(getFont()), textWidth, textHeight);
        //font.printf(0, height - font.getHeight(), Bitmap::makeColor(255, 255, 255), work, "Console!", 0 );
        Bitmap::drawingMode(Bitmap::MODE_SOLID);
	// if (state == Open){
            if (!lines.empty()){
                int start = height - font.getHeight() * 2;
                for (std::vector<std::string>::reverse_iterator i = lines.rbegin(); i != lines.rend() && start > 0; ++i){
                    std::string str = *i;
                    font.printf(0, start, Bitmap::makeColor(255,255,255), work, str, 0);
                    start -= font.getHeight();
                }
            }
            font.printf(0, height - font.getHeight(), Bitmap::makeColor(255,255,255), work, "> " + currentCommand.str(), 0);
        // }
    }
}
    
void Console::toggle(){
    switch (state){
        case Open:
        case Opening: {
            state = Closing;
            break;
        }
        case Closed:
        case Closing: {
            state = Opening;
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

void Console::backspace(){
    string now = currentCommand.str();
    now = now.substr(0, now.size()-1);
    currentCommand.str(now);
    currentCommand.rdbuf()->pubseekoff(0, ios_base::end, ios_base::out);
    currentCommand.clear();
}

void Console::deleteLastWord(){
    string now = currentCommand.str();
    size_t get = now.rfind(" ");
    if (get != string::npos){
        now = now.substr(0, get + 1);
        currentCommand.str(now);
        currentCommand.rdbuf()->pubseekoff(0, ios_base::end, ios_base::out);
        currentCommand.clear();
    } else {
        clearInput();
    }
}

void Console::clearInput(){
    currentCommand.str(string());
    currentCommand.rdbuf()->pubseekoff(0, ios_base::end, ios_base::out);
    currentCommand.clear();
}
    
Console & Console::operator<<(const ConsoleEnd & e){
    checkStream();
    return *this;
}

void Console::clear(){
    lines.clear();
    textInput.str("");
    textInput.clear();
}

std::stringstream & Console::add(){
    checkStream();
    return textInput;
}

void Console::checkStream(){
    if (!textInput.str().empty()){
	lines.push_back(textInput.str());
	textInput.str("");
    }
}

}
