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
    work(320, 240){
    }
    
    Graphics::Bitmap work;

    void draw(const Graphics::Bitmap & buffer){

        buffer.fill(Graphics::makeColor(0, 0, 255));

        /* draw one bitmap onto another */
        work.fill(Graphics::makeColor(255, 0, 0));
        work.draw(100, 100, buffer);

        /* sub bitmap */
        Graphics::Bitmap sub(buffer, 350, 300, 150, 150);
        sub.fill(Graphics::makeColor(0, 255, 0));

        /* sub bitmap inside another sub bitmap */
        Graphics::Bitmap sub2(sub, 10, 10, 50, 50);
        sub2.clear();

        Graphics::Bitmap box(50, 50);
        box.fill(Graphics::makeColor(255, 255, 0));

        box.draw(50, 50, sub);

        sub.hLine(0, 50, 50, Graphics::makeColor(255, 255, 255));
        sub.hLine(0, 100, 50, Graphics::makeColor(255, 255, 255));

        Graphics::Bitmap sub3(buffer, 50, 350, 250, 300);
        sub3.fill(Graphics::makeColor(128, 192, 0));

        Graphics::StretchedBitmap stuff(50, 50, sub3);
        stuff.start();
        stuff.hLine(10, 25, 30, Graphics::makeColor(255, 0, 255));
        stuff.finish();

        /*
        buffer.clear();
        work.activate();
        work.clear();
        // work.rectangle(10, 10, 30, 30, Graphics::makeColor(255, 255, 255));
        // work.start();
        // work.clear();
        player.draw(&work, 0, 0);
        // work.finish();
        // work.fill(Graphics::makeColor(0, 255, 0));
        work.draw(0, 0, buffer);
        // buffer.rectangle(20, 20, 40, 40, Graphics::makeColor(255, 0, 0));
        buffer.BlitToScreen();
        */
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
