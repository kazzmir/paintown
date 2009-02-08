#ifndef _paintown_console_h
#define _paintown_console_h

class Bitmap;

class Console{
public:
    Console(const int maxHeight);
    virtual ~Console();

    virtual void act();
    virtual void draw(const Bitmap & work);
    virtual void toggle();

protected:
    enum State{
        Closed,
        Open,
        Opening,
        Closing,
    } state;

    const int maxHeight;
    int height;
};

#endif
