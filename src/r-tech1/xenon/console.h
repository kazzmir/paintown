#ifndef _rtech1_console_h
#define _rtech1_console_h

#include <string>
#include <vector>
#include <deque>
#include <sstream>
#include <map>
#include "input/input-map.h"
#include "input/text-input.h"
#include "file-system.h"
#include "font.h"
#include "pointer.h"

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

    virtual std::string act(const std::string & line) = 0;
    virtual std::string getDescription() const = 0;

    virtual ~Command(){
    }
};

/* starts disabled */
class Console{
public:
    Console(const int maxHeight, const Filesystem::RelativePath & font = Font::getDefaultFontPath());
    virtual ~Console();

    /* handle input and whatnot */
    virtual void act();
    virtual void draw(const Graphics::Bitmap & work);
    /* enable / disable */
    virtual void toggle();

    /* removes all output from the console */
    virtual void clear();
    
    virtual bool doInput();

    /* scroll up and down */
    virtual void pageUp();
    virtual void pageDown();

    virtual bool isActive() const;

    /* add a line of text */
    virtual void addLine(const std::string & line);
    
    inline int getTextHeight(){ return textHeight; };
    inline int getTextWidth(){ return textWidth; };
    
    inline void setTextHeight(int h){ textHeight = h; };
    inline void setTextWidth(int w){ textWidth = w; };

    void fontIncrease();
    void fontDecrease();

    inline const Filesystem::RelativePath & getFont() const {
        return font;
    }

    /* for arbitrary data */
    template<typename T> Console & operator<<(const T & x){
        // textInput << x;
        return *this;
    }

    std::vector<Util::ReferenceCount<Command> > getCommands() const;

    void addCommand(const std::string & name, const Util::ReferenceCount<Command> & command);

    /* make 'alias' do the same thing as command 'name' */
    void addAlias(const std::string & alias, const std::string & name);

    /* for end of line, always pass Console::endl */
    Console & operator<<(const ConsoleEnd & e);

    static ConsoleEnd endl;

    void activate();

    void previousHistory();
    void nextHistory();

    void tabComplete();

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
    std::map<std::string, Util::ReferenceCount<Command> > commands;

    /* history of typed commands */
    std::deque<std::string> history;
    /* index into history. 0 is latest */
    unsigned int historyIndex;
    int pagePosition;
};

}

#endif
