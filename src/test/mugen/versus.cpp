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

#include "mugen/versus.h"

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
    Logic(InputMap<Keys> & input, Mugen::VersusMenu & versus):
    is_done(false),
    input(input),
    versus(versus){
    }

    bool deinit, is_done;
    InputMap<Keys> & input;
    Mugen::VersusMenu & versus;
    
    bool done(){
        return is_done;
    }

    void run(){
        std::vector<InputMap<Keys>::InputEvent> out = InputManager::getEvents(input, InputSource());
        for (std::vector<InputMap<Keys>::InputEvent>::iterator it = out.begin(); it != out.end(); it++){
            const InputMap<Keys>::InputEvent & event = *it;
            if (event.enabled){
                if (event.out == Esc || event.out == Enter){
                    versus.cancel();
                }
            }
        }
        versus.act();
        is_done = versus.isDone();
    }

    double ticks(double system){
        return system;
    }
};
class Draw: public Util::Draw {
public:
    Draw(Mugen::VersusMenu & versus):
    versus(versus){
    }
    
    Mugen::VersusMenu & versus;

    void draw(const Graphics::Bitmap & buffer){
        buffer.clear();
        Graphics::StretchedBitmap work(320, 240, buffer);
        work.start();
        versus.draw(work);
        work.finish();
        //Font::getDefaultFont(9,9).printfWrap(2, 440, Graphics::makeColor(255, 255, 255), buffer, 320, MENU, 0);
        buffer.BlitToScreen();
    }
};

int main(int argc, char ** argv){
    if (argc > 2){
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
            Mugen::VersusMenu versus(path);
            Mugen::ArcadeData::CharacterCollection player1(Mugen::ArcadeData::CharacterCollection::Single);
            Mugen::ArcadeData::CharacterCollection player2(Mugen::ArcadeData::CharacterCollection::Single);
            
            player1.setFirst(Mugen::ArcadeData::CharacterInfo(Mugen::Util::findCharacterDef(argv[2])));
            player2.setFirst(Mugen::ArcadeData::CharacterInfo(Mugen::Util::findCharacterDef(argv[3])));
            
            versus.init(player1, player2);
            Logic logic(input, versus);
            Draw draw(versus);

            Util::standardLoop(logic, draw);
            
        } catch (const MugenException & ex){
            Global::debug(0) << "Problem loading file [" << file << "]. Reason: " << ex.getTrace() << std::endl;
        } catch (const LoadException & ex){
            Global::debug(0) << "Problem loading file [" << file << "]. Reason: " << ex.getTrace() << std::endl;
        } catch (const TokenException & ex){
            Global::debug(0) << "Problem parsing file [" << file << "]. Reason: " << ex.getTrace() << std::endl;
        }
        
    } else {
        std::cout << "Usage: ./" << argv[0] << " select.def character1 character2" << std::endl;
    }
    return 0;
}
#ifdef USE_ALLEGRO
END_OF_MAIN()
#endif
