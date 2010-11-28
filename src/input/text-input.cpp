#include "input-map.h"
#include "text-input.h"
#include "input-manager.h"
#include <string.h>
#include <sstream>
#include <string>

using namespace std;

static const int Shift = 198;
static const int Control = 199;
static const int Backspace = 200;

TextInput::TextInput():
InputMap<unsigned char>(),
enabled(false),
handle(201){
    const int delay = 100;
    /*
    set(Keyboard::Key_TILDE, delay * 2, false, Toggle);
    set(Keyboard::Key_ESC, delay, false, Esc);
    set(Keyboard::Key_ENTER, 0, false, Enter);
    */
    
    set(Keyboard::Key_LCONTROL, 0, false, Control);
    set(Keyboard::Key_BACKSPACE, delay, false, Backspace);
    set(Keyboard::Key_LSHIFT, 0, false, Shift);
    set(Keyboard::Key_RSHIFT, 0, false, Shift);
    
    /* ugh, do we really have to enumerate every key?? */
    /*
    set(Keyboard::Key_A, delay, false, 'a');
    set(Keyboard::Key_B, delay, false, 'b');
    set(Keyboard::Key_C, delay, false, 'c');
    set(Keyboard::Key_D, delay, false, 'd');
    set(Keyboard::Key_E, delay, false, 'e');
    set(Keyboard::Key_F, delay, false, 'f');
    set(Keyboard::Key_G, delay, false, 'g');
    set(Keyboard::Key_H, delay, false, 'h');
    set(Keyboard::Key_I, delay, false, 'i');
    set(Keyboard::Key_J, delay, false, 'j');
    set(Keyboard::Key_K, delay, false, 'k');
    set(Keyboard::Key_L, delay, false, 'l');
    set(Keyboard::Key_M, delay, false, 'm');
    set(Keyboard::Key_N, delay, false, 'n');
    set(Keyboard::Key_O, delay, false, 'o');
    set(Keyboard::Key_P, delay, false, 'p');
    set(Keyboard::Key_Q, delay, false, 'q');
    set(Keyboard::Key_R, delay, false, 'r');
    set(Keyboard::Key_S, delay, false, 's');
    set(Keyboard::Key_T, delay, false, 't');
    set(Keyboard::Key_U, delay, false, 'u');
    set(Keyboard::Key_V, delay, false, 'v');
    set(Keyboard::Key_W, delay, false, 'w');
    set(Keyboard::Key_X, delay, false, 'x');
    set(Keyboard::Key_Y, delay, false, 'y');
    set(Keyboard::Key_Z, delay, false, 'z');

    set(Keyboard::Key_0, delay, false, '0');
    set(Keyboard::Key_1, delay, false, '1');
    set(Keyboard::Key_2, delay, false, '2');
    set(Keyboard::Key_3, delay, false, '3');
    set(Keyboard::Key_4, delay, false, '4');
    set(Keyboard::Key_5, delay, false, '5');
    set(Keyboard::Key_6, delay, false, '6');
    set(Keyboard::Key_7, delay, false, '7');
    set(Keyboard::Key_8, delay, false, '8');
    set(Keyboard::Key_9, delay, false, '9');

    set(Keyboard::Key_SLASH, delay, false, '/');
    set(Keyboard::Key_SPACE, delay, false, ' ');
    set(Keyboard::Key_COMMA, delay, false, ',');
    set(Keyboard::Key_STOP, delay, false, '.');
    */
}

int TextInput::nextHandle(){
    int i = handle;
    handle += 1;
    return i;
}

void TextInput::addBlockingHandle(int key, callback function, void * data){
    int handle = nextHandle();
    set(key, 1, true, handle);
    callbacks[handle] = Callback(function, data);
}
    
void TextInput::addHandle(int key, int delay, callback function, void * data){
    int handle = nextHandle();
    set(key, delay, false, handle);
    callbacks[handle] = Callback(function, data);
}

static bool isChar(char c){
    const char * letters = "abcdefghijklmnopqrstuvwxyz ,.0123456789/";
    return strchr(letters, c) != NULL;
}

static unsigned char doShift(unsigned char letter, bool shift){
    if (shift){
        if (letter >= 'a' && letter <= 'z'){
            return toupper(letter);
        }
        switch (letter){
            case '/' : return '?';
            case '0' : return ')';
            case '1' : return '!';
            case '2' : return '@';
            case '3' : return '#';
            case '4' : return '$';
            case '5' : return '%';
            case '6' : return '^';
            case '7' : return '&';
            case '8' : return '*';
            case '9' : return '(';
            default : return letter;
        }
    } else {
        return letter;
    }
}

void TextInput::doInput(){
    if (enabled){
        InputMap<unsigned char>::Output inputState = InputManager::getMap(*this);

        /* the order of reading input is arbitrary right now. I'm not
         * sure it matters what order things are done in, but probably
         * a few corner cases exist. When they come up please document them.
         */

        /*
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
        */

        /* ctrl-X keys */
        if (inputState[Control]){
            /* standard linux console commands */
            if (inputState['u']){
                clearInput();
                return;
            }
            if (inputState['w']){
                deleteLastWord();
                return;
            }
        }

        for (map<int, Callback>::iterator it = callbacks.begin(); it != callbacks.end(); it++){
            int handle = it->first;
            if (inputState[handle]){
                Callback callback = it->second;
                callback.function(callback.data);
            }
        }

        vector<Keyboard::unicode_t> text = InputManager::readText(*this, inputState);
        for (vector<Keyboard::unicode_t>::iterator it = text.begin(); it != text.end(); it++){
            Keyboard::unicode_t letter = *it;
            // Global::debug(0) << "Unicode: " << letter << endl;
            /* letters below 32 are mostly bogus */
            /* FIXME: deal with unicodeness */
            if (letter >= 32){
                this->text << (unsigned char) letter;
            }
        }

        /*
        for (InputMap<unsigned char>::Output::iterator it = inputState.begin(); it != inputState.end(); it++){
            unsigned char c = (*it).first;
            bool pressed = (*it).second;
            if (pressed){
                if (isChar(c)){
                    text << doShift(c, inputState[Shift]);
                } else {
                    if (callbacks.find(c) != callbacks.end()){
                        Callback back = callbacks[c];
                        back.function(back.data);
                    }
                }
            }
        }
        */

            /*
        if (inputState[Esc]){
            InputManager::releaseInput(input);
            throw ReturnException();
        }
        */

        if (inputState[Backspace]){
            backspace();
        }
    }
}

void TextInput::enable(){
    InputManager::captureInput(*this);
    enabled = true;
}

void TextInput::disable(){
    InputManager::releaseInput(*this);
    enabled = false;
}

string TextInput::getText(){
    return text.str();
}

void TextInput::clear(){
    text.str("");
    text.clear();
}

void TextInput::clearInput(){
    text.str(string());
    text.rdbuf()->pubseekoff(0, ios_base::end, ios_base::out);
    text.clear();
}

void TextInput::backspace(){
    string now = text.str();
    now = now.substr(0, now.size()-1);
    text.str(now);
    text.rdbuf()->pubseekoff(0, ios_base::end, ios_base::out);
    text.clear();
}

void TextInput::deleteLastWord(){
    string now = text.str();
    // Global::debug(0) << "Delete last word '" << now << "'" << endl;

    if (now.size() > 0){
        if (now.at(now.size() - 1) != ' '){
            int here = now.size() - 1;
            while (here > 0 && now.at(here) != ' '){
                here -= 1;
            }
            if (now.at(here) == ' '){
                here += 1;
            }
            now.erase(here);
        } else {
            int here = now.size() - 1;
            while (here > 0 && now.at(here) == ' '){
                here -= 1;
            }
            if (here > 0){
                while (here > 0 && now.at(here) != ' '){
                    here -= 1;
                }
                if (now.at(here) == ' '){
                    here += 1;
                }
            }
            now.erase(here);
        }
        text.str(now);
        text.rdbuf()->pubseekoff(0, ios_base::end, ios_base::out);
        text.clear();
    }

    /*
    size_t get = now.rfind(" ");
    if (get != string::npos){
        Global::debug(0) << "get " << get << " size " << now.size() << endl;
        if (get == now.size() - 1){
            get = now.find_last_not_of(' ');
            get = now.rfind(' ', get);
            if (get != string::npos){
                get = 0;
            }
            now.erase(get + 1);
        } else {
            now = now.substr(0, get + 1);
        }
        text.str(now);
        text.rdbuf()->pubseekoff(0, ios_base::end, ios_base::out);
        text.clear();
    } else {
        clearInput();
    }
    */
}
    
TextInput::~TextInput(){
}
