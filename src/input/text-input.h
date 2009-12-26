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

class TextInput: InputMap<unsigned char> {
public:
    TextInput();

    void doInput();
    void enable();
    void disable();

    void addHandle(int key, callback function, void * data);

    std::string getText();

    void clearInput();
    void clear();

    bool isEnabled() const {
        return enabled;
    }

    void backspace();
    void deleteLastWord();

    virtual ~TextInput();

protected:
    int nextHandle();

    std::ostringstream text;
    bool enabled;
    std::map<int, Callback> callbacks;
    int handle;
};

#endif
