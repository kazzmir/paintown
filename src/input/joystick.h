#ifndef _paintown_joystick_h
#define _paintown_joystick_h

#include <vector>

struct JoystickInput{
    JoystickInput():
        left(false),
        right(false),
        up(false),
        down(false),
        button1(false),
        button2(false),
        button3(false),
        button4(false){
    }

    JoystickInput(const JoystickInput & copy):
        left(copy.left),
        right(copy.right),
        up(copy.up),
        down(copy.down),
        button1(copy.button1),
        button2(copy.button2),
        button3(copy.button3),
        button4(copy.button4){
    }

    bool left;
    bool right;
    bool up;
    bool down;
    bool button1;
    bool button2;
    bool button3;
    bool button4;
};

class Joystick{
public:
    virtual void poll() = 0;
    virtual JoystickInput readAll() = 0;
    virtual bool pressed() = 0;
    virtual ~Joystick();

    static Joystick * create();

    enum Key{
        Invalid = -1,
        Up = 0,
        Down,
        Left,
        Right,
        Button1,
        Button2,
        Button3,
        Button4,
    };
	
    static const char * keyToName(Key key);

protected:

    Joystick();
};

#endif
