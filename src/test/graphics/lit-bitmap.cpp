#include <iostream>

#include "r-tech1/file-system.h"
#include "r-tech1/debug.h"
#include "r-tech1/init.h"
#include "r-tech1/events.h"
#include "r-tech1/input/input-source.h"
#include "r-tech1/input/input.h"
#include "r-tech1/input/input-manager.h"
#include "r-tech1/graphics/bitmap.h"

using namespace std;

enum Keys{
    Up=0,
    Down,
    Left,
    Right,
    Esc,
    Enter,
};

#ifndef WINDOWS

/* FIXME: dont put these methods in this test file */
Filesystem::AbsolutePath Filesystem::configFile(){
    std::ostringstream str;
    /* what if HOME isn't set? */
    str << getenv("HOME") << "/.paintownrc";
    return Filesystem::AbsolutePath(str.str());
}

Filesystem::AbsolutePath Filesystem::userDirectory(){
    std::ostringstream str;
    char * home = getenv("HOME");
    if (home == NULL){
        str << "/tmp/paintown";
    } else {
        str << home << "/.paintown/";
    }
    return Filesystem::AbsolutePath(str.str());
}
#endif

class Logic: public Util::Logic {
public:
    Logic(InputMap<Keys> & input):
    input(input){
    }

    bool is_done = false;
    InputMap<Keys> & input;
    
    bool done(){
        return is_done;
    }

    void run(){
        std::vector<InputMap<Keys>::InputEvent> out = InputManager::getEvents(input, InputSource(true));
        for (const InputMap<Keys>::InputEvent & event: out){
            if (event.enabled){
                if (event.out == Esc){
                    is_done = true;
                }
            }
        }
    }

    double ticks(double system){
        return system;
    }
};

class Draw: public Util::Draw {
public:
    Draw():
    object(100, 100){
        object.fill(Graphics::makeColor(255, 0, 0));
    }
    
    Graphics::Bitmap object;

    void draw(const Graphics::Bitmap & buffer){
        object.draw(100, 100, buffer);
    }
};


int test_main(int argc, char** argv){
    DebugLog << "Graphics test" << endl;

    Global::setDebug(1);

    Global::InitConditions conditions;
    // conditions.graphics = Global::InitConditions::Disabled;
    Global::init(conditions);

    InputManager manager;

    Graphics::Bitmap screen(*Graphics::getScreenBuffer());
    Util::Parameter<Graphics::Bitmap*> use(Graphics::screenParameter, &screen);

    InputMap<Keys> input;
    input.set(Keyboard::Key_ESC, 0, true, Esc);
    input.set(Keyboard::Key_ENTER, 0, true, Enter);
    input.set(Keyboard::Key_UP, 0, true, Up);
    input.set(Keyboard::Key_DOWN, 0, true, Down);
    input.set(Keyboard::Key_LEFT, 0, true, Left);
    input.set(Keyboard::Key_RIGHT, 0, true, Right);

    Logic logic(input);
    Draw draw;

    Util::standardLoop(logic, draw);

    return 0;
}

int main(int argc, char** argv){
    return test_main(argc, argv);
}
