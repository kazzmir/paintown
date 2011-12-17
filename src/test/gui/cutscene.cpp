#include "../common/init.h"
#include "../common/timer.h"

#include <iostream>
#include <vector>

#include "util/gui/cutscene.h"
#include "util/debug.h"
#include "util/load_exception.h"
#include "util/token_exception.h"
#include "util/stretch-bitmap.h"
#include "util/input/input.h"
#include "util/input/input-manager.h"

using namespace Gui;

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
    Logic(InputMap<Keys> & input, CutScene & cutscene):
    is_done(false),
    input(input),
    cutscene(cutscene),
    ticker(0){
    }

    bool is_done;
    InputMap<Keys> & input;
    CutScene & cutscene;
    int ticker;
    
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
    Draw(CutScene & cutscene):
    cutscene(cutscene){
    }
    
    CutScene & cutscene;

    void draw(const Graphics::Bitmap & buffer){
        buffer.clear();
        //select.draw(buffer);
        buffer.BlitToScreen();
    }
};

int main(int argc, char ** argv){
    if (argc > 1){
        Screen::realInit();
        atexit(Screen::realFinish);
        Common::startTimers();
        
        Global::setDebug(0);
        std::string file = argv[1];
        
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
       
        try {
            
            Filesystem::AbsolutePath path(file);
            CutScene cutscene(path);
            /*while (cutscene.hasMore()){
                cutscene.playScene();
                cutscene.next();
            }*/
            
            Logic logic(input, cutscene);
            Draw draw(cutscene);

            Util::standardLoop(logic, draw);
            
        } catch (const LoadException & ex){
            Global::debug(0) << "Problem loading file [" << file << "]. Reason: " << ex.getTrace() << std::endl;
        } catch (const TokenException & ex){
            Global::debug(0) << "Problem parsing file [" << file << "]. Reason: " << ex.getTrace() << std::endl;
        }
        
    } else {
        std::cout << "Usage: ./" << argv[0] << " cutscene.txt" << std::endl;
    }
    return 0;
}
#ifdef USE_ALLEGRO
END_OF_MAIN()
#endif
