#include "util/gui/coordinate.h"
#include "util/debug.h"

namespace Util{

double min(double a, double b){
    if (a < b){
        return a;
    }
    return b;
}

double max(double a, double b){
    if (a > b){
        return a;
    }
    return b;
}


double clamp(double value, double min, double max){
    return Util::min(Util::max(value, min), max);
}

}

/*
#include "../common/init.h"
#include "../common/timer.h"
#include "util/debug.h"

#include <stdlib.h>

#include "util/stretch-bitmap.h"
#include "util/input/input.h"
#include "util/input/input-manager.h"

enum Keys{
    Up=0,
    Down,
    Left,
    Right,
    Esc,
    Enter,
};

class Logic: public Util::Logic {
public:
    Logic(InputMap<Keys> & input):
    is_done(false),
    input(input){
    }

    bool is_done;
    InputMap<Keys> & input;
    
    bool done(){
        return is_done;
    }

    void run(){
        std::vector<InputMap<Keys>::InputEvent> out = InputManager::getEvents(input, InputSource());
        for (std::vector<InputMap<Keys>::InputEvent>::iterator it = out.begin(); it != out.end(); it++){
            const InputMap<Keys>::InputEvent & event = *it;
            if (event.enabled){
                if (event.out == Esc){
                    is_done = true;
                }
                if (event.out == Up){
                }
                if (event.out == Down){
                }
                if (event.out == Left){
                }
                if (event.out == Right){
                }
                if (event.out == Enter){
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
    bitmap(Graphics::Bitmap(320,240)){
        bitmap.clear();
    }
    
    Graphics::Bitmap bitmap;
    
    void draw(const Graphics::Bitmap & buffer){
        bitmap.putPixel(random()%320,random()%240,Graphics::makeColor(random()%255,random()%255,random()%255));
        bitmap.Stretch(buffer);
        buffer.BlitToScreen();
    }
};

static void test1(int argc, char ** argv){
    if (argc > 2){
        int x = atoi(argv[1]), y = atoi(argv[2]);
        Screen::realInit(x, y);
        atexit(Screen::realFinish);
        
        Common::startTimers();
        
        InputManager manager;
        
        Graphics::Bitmap screen(Graphics::getScreenBuffer());
        Util::Parameter<Graphics::Bitmap*> use(Graphics::screenParameter, &screen);
        Keyboard::pushRepeatState(true);
        
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
        
    } else {
        Global::debug(0) << "Usage: ./" << argv[0] << " screen-width screen-height" << std::endl;
    }
}
*/

static int test2(){
    Gui::Space::Space space1(10, 10, 100, 100);
    Gui::Space::Space space2(5, 5, 20, 20);

    /* two points exactly in the middle */
    Gui::Space::Point point1 = space1.fromPhysical(320, 240);
    Gui::Space::Point point2 = space2.fromPhysical(320, 240);
    if (point1 != point2){
        Global::debug(0) << "Points from different spaces are not equal!" << std::endl;
        return 1;
    }

    Gui::Space::Point point3 = space2.fromPhysical(1, 1);
    if (point1 == point3){
        Global::debug(0) << "Points should not be the same" << std::endl;
        return 1;
    }

    /* middle + middle = end */
    Gui::Space::Point point4 = space2.fromPhysical(640, 480);
    if (point1 + point2 != point4){
        Global::debug(0) << "Points should be support addition" << std::endl;
        return 1;
    }

    return 0;
}

/* test the space api */
int main(int argc, char ** argv){
    Global::setDebug(0);   
    return test2();
}
#ifdef USE_ALLEGRO
END_OF_MAIN()
#endif

