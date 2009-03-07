#ifndef _paintown_console_h
#define _paintown_console_h

#include <string>
#include <vector>
#include <sstream>

class Bitmap;

class ConsoleEnd{
private:
    friend class Console;
    ConsoleEnd(){
    }
};

class Console{
public:
    Console(const int maxHeight);
    virtual ~Console();

    virtual void act();
    virtual void draw(const Bitmap & work);
    virtual void toggle();
    
    virtual void clear();
    
    inline int getTextHeight(){ return textHeight; };
    inline int getTextWidth(){ return textWidth; };
    
    inline void setTextHeight(int h){ textHeight = h; };
    inline void setTextWidth(int w){ textWidth = w; };

    /* for arbitrary data */
    template<typename T> Console & operator<<(const T & x){
        textInput << x;
        return *this;
    }

    /* for end of line, always pass Console::endl */
    Console & operator<<(const ConsoleEnd & e);

    static ConsoleEnd endl;

protected:

    virtual std::stringstream & add();

    enum State{
        Closed,
        Open,
        Opening,
        Closing,
    } state;

    const int maxHeight;
    int height;
    
    // Text height
    int textHeight;
    // Text width
    int textWidth;
    std::vector<std::string>lines;
    // Our text inputer
    std::stringstream textInput;
    unsigned int offset;
    
    void checkStream();
};

#endif
