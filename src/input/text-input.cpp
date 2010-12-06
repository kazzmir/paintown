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

TextInput::TextInput(const string & start):
InputMap<unsigned char>(),
blockingKeys(false),
enabled(false),
handle(201){
    const int delay = 100;
    /*
    set(Keyboard::Key_TILDE, delay * 2, false, Toggle);
    set(Keyboard::Key_ESC, delay, false, Esc);
    set(Keyboard::Key_ENTER, 0, false, Enter);
    */
    
    // set(Keyboard::Key_LCONTROL, 0, false, Control);
    set(Keyboard::Key_BACKSPACE, delay, false, Backspace);
    /*
    set(Keyboard::Key_LSHIFT, 0, false, Shift);
    set(Keyboard::Key_RSHIFT, 0, false, Shift);
    */

    text.str(start);
    text.rdbuf()->pubseekoff(0, ios_base::end, ios_base::out);
    text.clear();
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

bool TextInput::doInput(){
    bool modified = false;
    /* TODO: ensure these codes are consistent across platforms. So far
     * they seem to work in windows xp and linux (ubuntu)
     */
    const Keyboard::unicode_t control_u = 21;
    const Keyboard::unicode_t control_w = 23;

    if (enabled){
        vector<InputEvent> events = InputManager::getEvents(*this);

        /* the order of reading input is arbitrary right now. I'm not
         * sure it matters what order things are done in, but probably
         * a few corner cases exist. When they come up please document them.
         */

        for (vector<InputEvent>::iterator it = events.begin(); it != events.end(); it++){
            InputEvent event = *it;

            if (event.enabled){
                if (event.out == Backspace){
                    backspace();
                    modified = true;
                }

                if (callbacks.find(event.out) != callbacks.end()){
                    Callback & callback = callbacks[event.out];
                    callback.function(callback.data);
                }

                if (event.unicode == control_u){
                    clearInput();
                    modified = true;
                }
                if (event.unicode == control_w){
                    deleteLastWord();
                    modified = true;
                }

                /* FIXME: whats the maximum unicode value? */
                if (event.unicode >= 32 && event.unicode < 0xffffff){
                    this->text << (unsigned char) event.unicode;
                    modified = true;
                }
            }
        }
    }

    return modified;
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

/*
void TextInput::clear(){
    text.str("");
    text.clear();
}
*/
    
void TextInput::setText(const std::string & text){
    this->text.str(text);
    this->text.rdbuf()->pubseekoff(0, ios_base::end, ios_base::out);
    this->text.clear();
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
    disable();
}
