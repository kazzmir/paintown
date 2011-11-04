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

/* test the space api */
int main(int argc, char ** argv){
    Global::setDebug(0);   
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
    return 0;
}
#ifdef USE_ALLEGRO
END_OF_MAIN()
#endif

