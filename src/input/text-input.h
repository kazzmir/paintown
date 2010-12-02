#ifndef _paintown_text_input_h
#define _paintown_text_input_h

#include "input-map.h"
#include <sstream>
#include <string>
#include <map>

typedef void (*callback)(void * arg);

struct Callback{
    Callback():
        function(0),
        data(0){
        }

    Callback(callback function, void * data):
        function(function),
        data(data){
        }

    callback function;
    void * data;
};

class TextInput: public InputMap<unsigned char> {
public:
    TextInput(const std::string & start = "");

    /* returns true if the text was modified */
    bool doInput();
    void enable();
    void disable();

    void addHandle(int key, int delay, callback function, void * data);
    void addBlockingHandle(int key, callback function, void * data);

    inline void setBlockingKeys(){
        blockingKeys = true;
    }

    std::string getText();
    void setText(const std::string & text);

    void clearInput();
    // void clear();

    bool isEnabled() const {
        return enabled;
    }

    void backspace();
    void deleteLastWord();
    
    virtual KeyState<unsigned char> * getState(int key){
        KeyState<unsigned char> * state = InputMap<unsigned char>::getState(key);
        if (state == NULL){
            if (blockingKeys){
                set(key, 0, true, key);
            } else {
                set(key, 10, false, key);
            }
            state = InputMap<unsigned char>::getState(key);
        }
        return state;
    }

    virtual ~TextInput();

protected:
    int nextHandle();

    std::ostringstream text;
    /* whether key repeat is on or off */
    bool blockingKeys;
    bool enabled;
    std::map<int, Callback> callbacks;
    int handle;
};

#endif
