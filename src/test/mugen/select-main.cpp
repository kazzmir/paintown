#include "../common/init.h"
#include "../common/timer.h"

#include <iostream>
#include <vector>

#include "util/font.h"
#include "util/debug.h"
#include "util/load_exception.h"
#include "util/token_exception.h"
#include "util/stretch-bitmap.h"
#include "util/input/input.h"
#include "util/input/input-manager.h"
#include "util/sound.h"

#include "mugen/util.h"
#include "mugen/exception.h"

#include "character-select.h"

enum Keys{
    Up=0,
    Down,
    Left,
    Right,
    Esc,
    Enter,
    SpaceBar,
    S,
    P,
    R,
    Key1,
    Key2,
    Key3,
    Key4,
    Key5,
    Key6,
    Key7,
    Key8,
    Key9,
};

class Logic: public Util::Logic {
public:
    Logic(InputMap<Keys> & input, Mugen::CharacterSelect & select):
    deinit(false),
    is_done(false),
    input(input),
    select(select),
    ticker(0){
    }

    bool deinit, is_done;
    InputMap<Keys> & input;
    Mugen::CharacterSelect & select;
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
                    if (!deinit){
                        deinit = true;
                        select.deinit();
                    }
                }
                if (event.out == P){
                }
                if (event.out == R){
                }
                if (event.out == Left){
                    select.left(0);
                }
                if (event.out == Down){
                    select.down(0);
                }
                if (event.out == Right){
                    select.right(0);
                }
                if (event.out == Up){
                    select.up(0);
                }
                if (event.out == S || event.out == Enter || event.out == SpaceBar){
                    select.select(0);
                }
                if (event.out == Key1){
                    select.setMode(Mugen::Arcade, Mugen::CharacterSelect::Player1);
                }
                if (event.out == Key2){
                    select.setMode(Mugen::Versus, Mugen::CharacterSelect::Both);
                }
                if (event.out == Key3){
                    select.setMode(Mugen::TeamArcade, Mugen::CharacterSelect::Player1);
                }
                if (event.out == Key4){
                    select.setMode(Mugen::TeamVersus, Mugen::CharacterSelect::Both);
                }
                if (event.out == Key5){
                    select.setMode(Mugen::TeamCoop, Mugen::CharacterSelect::Player1);
                }
                if (event.out == Key6){
                    select.setMode(Mugen::Survival, Mugen::CharacterSelect::Player1);
                }
                if (event.out == Key7){
                    select.setMode(Mugen::SurvivalCoop, Mugen::CharacterSelect::Player1);
                }
                if (event.out == Key8){
                    select.setMode(Mugen::Training, Mugen::CharacterSelect::Player1);
                }
                if (event.out == Key9){
                    select.setMode(Mugen::Watch, Mugen::CharacterSelect::Player1);
                }
            }
        }
        select.act();
        is_done = select.isDone();
    }

    double ticks(double system){
        return system;
    }
};

static const std::string MENU = "[1]-Arcade [2]-Versus [3]-TeamArcade [4]-TeamVersus\n[5]-TeamCoop [6]-Survival [7]-SurvivalCoop [8]-Training [9]-Watch";

class Draw: public Util::Draw {
public:
    Draw(Mugen::CharacterSelect & select):
    select(select){
    }
    
    Mugen::CharacterSelect & select;

    void draw(const Graphics::Bitmap & buffer){
        buffer.clear();
        Graphics::StretchedBitmap work(320, 240, buffer);
        work.start();
        select.draw(work);
        Font::getDefaultFont(9,9).printfWrap(2, 210, Graphics::makeColor(255, 255, 255), work, 320, MENU, 0);
        work.finish();
        buffer.BlitToScreen();
    }
};

int main(int argc, char ** argv){
    if (argc > 1){
        Screen::realInit();
        atexit(Screen::realFinish);
        Common::startTimers();
        
        Sound::initialize();
        
        Global::setDebug(2);
        std::string file = argv[1];
        
        InputManager manager;
        Graphics::Bitmap screen(*Graphics::getScreenBuffer());
        Util::Parameter<Graphics::Bitmap*> use(Graphics::screenParameter, &screen);
        Keyboard::pushRepeatState(true);
        
        InputMap<Keys> input;
        input.set(Keyboard::Key_ESC, 0, true, Esc);
        input.set(Keyboard::Key_ENTER, 0, true, Enter);
        input.set(Keyboard::Key_UP, 0, true, Up);
        input.set(Keyboard::Key_DOWN, 0, true, Down);
        input.set(Keyboard::Key_LEFT, 0, true, Left);
        input.set(Keyboard::Key_RIGHT, 0, true, Right);
        input.set(Keyboard::Key_S, 0, true, S);
        input.set(Keyboard::Key_P, 0, true, P);
        input.set(Keyboard::Key_R, 0, true, R);
        input.set(Keyboard::Key_SPACE, 0, true, SpaceBar);
        input.set(Keyboard::Key_1, 0, true, Key1);
        input.set(Keyboard::Key_2, 0, true, Key2);
        input.set(Keyboard::Key_3, 0, true, Key3);
        input.set(Keyboard::Key_4, 0, true, Key4);
        input.set(Keyboard::Key_5, 0, true, Key5);
        input.set(Keyboard::Key_6, 0, true, Key6);
        input.set(Keyboard::Key_7, 0, true, Key7);
        input.set(Keyboard::Key_8, 0, true, Key8);
        input.set(Keyboard::Key_9, 0, true, Key9);
       
        try {
            Filesystem::AbsolutePath path(file);
            Mugen::CharacterSelect select(path);
            select.init();
            if (argc > 2){
                const std::string & check = std::string(argv[2]);
                if (check == "a"){
                    select.setMode(Mugen::Arcade, Mugen::CharacterSelect::Player1);
                } else if (check == "b"){
                    select.setMode(Mugen::Versus, Mugen::CharacterSelect::Both);
                } else if (check == "c"){
                    select.setMode(Mugen::TeamArcade, Mugen::CharacterSelect::Player1);
                } else if (check == "d"){
                    select.setMode(Mugen::TeamVersus, Mugen::CharacterSelect::Both);
                } else if (check == "e"){
                    select.setMode(Mugen::TeamCoop, Mugen::CharacterSelect::Player1);
                } else if (check == "f"){
                    select.setMode(Mugen::Survival, Mugen::CharacterSelect::Player1);
                } else if (check == "g"){
                    select.setMode(Mugen::SurvivalCoop, Mugen::CharacterSelect::Player1);
                } else if (check == "h"){
                    select.setMode(Mugen::Training, Mugen::CharacterSelect::Player1);
                } else if (check == "i"){
                    select.setMode(Mugen::Watch, Mugen::CharacterSelect::Player1);
                } 
            }
            Logic logic(input, select);
            Draw draw(select);

            Util::standardLoop(logic, draw);
            
        } catch (const MugenException & ex){
            Global::debug(0) << "Problem loading file [" << file << "]. Reason: " << ex.getTrace() << std::endl;
        } catch (const LoadException & ex){
            Global::debug(0) << "Problem loading file [" << file << "]. Reason: " << ex.getTrace() << std::endl;
        } catch (const TokenException & ex){
            Global::debug(0) << "Problem parsing file [" << file << "]. Reason: " << ex.getTrace() << std::endl;
        }
        
    } else {
        std::cout << "Usage: ./" << argv[0] << " select.def [Mode]" << std::endl;
        std::cout << "Modes (defaults to none):" << std::endl;
        std::cout << "[a] - Arcade       | [b] - Versus" << std::endl;
        std::cout << "[c] - TeamArcade   | [d] - TeamVersus" << std::endl;
        std::cout << "[e] - TeamCoop     | [f] - Survival" << std::endl;
        std::cout << "[g] - SurvivalCoop | [h] - Training" << std::endl;
        std::cout << "[i] - Watch" << std::endl;
    }
    return 0;
}
#ifdef USE_ALLEGRO
END_OF_MAIN()
#endif
