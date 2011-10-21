#include "../common/init.h"
#include "../common/timer.h"

#include <iostream>
#include <vector>

#include "character-select.h"
#include "util/debug.h"
#include "util/load_exception.h"
#include "util/token_exception.h"
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
    Logic(InputMap<Keys> & input, CharacterSelect & select):
    is_done(false),
    input(input),
    select(select){
    }

    bool is_done;
    InputMap<Keys> & input;
    CharacterSelect & select;
    
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
                /* NOTE Assumes only one cursor */
                if (event.out == Up){
                    select.getList()->up(0);
                }
                if (event.out == Down){
                    select.getList()->down(0);
                }
                if (event.out == Left){
                    select.getList()->left(0);
                }
                if (event.out == Right){
                    select.getList()->right(0);
                }
                if (event.out == Enter){
                }
            }
        }
        
        select.act();
    
    }

    double ticks(double system){
        return system;
    }
};

class Draw: public Util::Draw {
public:
    Draw(CharacterSelect & select):
    select(select){
    }
    
    CharacterSelect & select;

    void draw(const Graphics::Bitmap & buffer){
        buffer.clear();
        select.draw(buffer);
        buffer.BlitToScreen();
    }
};

int main(int argc, char ** argv){
    if (argc > 1){
        Screen::realInit();
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
            
            CharacterSelect select(file);
            
            Logic logic(input, select);
            Draw draw(select);

            Util::standardLoop(logic, draw);
            
        } catch (const LoadException & ex){
            Global::debug(0) << "Problem loading file [" << file << "]. Reason: " << ex.getTrace() << std::endl;
        } catch (const TokenException & ex){
            Global::debug(0) << "Problem parsing file [" << file << "]. Reason: " << ex.getTrace() << std::endl;
        }
        
        Screen::realFinish();
    } else {
        std::cout << "Usage: ./" << argv[0] << " select-screen.txt" << std::endl;
    }
    return 0;
}
#ifdef USE_ALLEGRO
END_OF_MAIN()
#endif