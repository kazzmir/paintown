#ifndef _paintown_console_h
#define _paintown_console_h

#include <string>
#include <vector>
#include <sstream>
#include <map>
#include "util/input/input-map.h"
#include "util/input/text-input.h"
#include "util/file-system.h"

namespace Graphics{
class Bitmap;
}

namespace Console{

/* equivalent of std::endl */
class ConsoleEnd{
private:
    friend class Console;
    ConsoleEnd(){
    }
};

class Command{
public:
    Command(){
    }

    virtual std::string act() = 0;

    virtual ~Command(){
    }
};

/* starts disabled */
class Console{
public:
    Console(const int maxHeight, const Filesystem::RelativePath & font = Global::DEFAULT_FONT);
    virtual ~Console();

    /* handle input and whatnot */
    virtual void act();
    virtual void draw(const Graphics::Bitmap & work);
    /* enable / disable */
    virtual void toggle();

    /* removes all output from the console */
    virtual void clear();
    
    virtual bool doInput();
    
    inline int getTextHeight(){ return textHeight; };
    inline int getTextWidth(){ return textWidth; };
    
    inline void setTextHeight(int h){ textHeight = h; };
    inline void setTextWidth(int w){ textWidth = w; };

    inline const Filesystem::RelativePath & getFont() const {
        return font;
    }

    /* for arbitrary data */
    template<typename T> Console & operator<<(const T & x){
        // textInput << x;
        return *this;
    }

    void addCommand(const std::string & name, Command * command);

    /* for end of line, always pass Console::endl */
    Console & operator<<(const ConsoleEnd & e);

    static ConsoleEnd endl;

    void activate();

protected:

    void process(const std::string & command);

    enum State{
        Closed,
        Open,
        Opening,
        Closing,
    } state;

    const int maxHeight;
    int height;

    Filesystem::RelativePath font;
    
    // Text height
    int textHeight;
    // Text width
    int textWidth;
    std::vector<std::string> lines;
    // Our text inputer
    // std::stringstream textInput;
    // std::stringstream currentCommand;
    unsigned int offset;
    // InputMap<char> input;
    TextInput textInput;
    std::map<std::string, Command*> commands;
    
    void checkStream();
};

}

#endif
