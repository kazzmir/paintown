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

class CutSceneTool{
public:
    CutSceneTool(const std::string & file):
    path(file),
    state(Play){
        reload();
    }
    ~CutSceneTool(){
    }
    
    void reload(){
        scenes.clear();
        cutscene = CutScene(path);
        while (cutscene.hasMore()){
            scenes.push_back(cutscene.getCurrent());
            cutscene.next();
        }
        current = 0;
    }
    
    enum State {
        Stopped=0,
        Play,
        Reverse,
        Forward,
        FastForward,
        Rewind,
    };
    
    void act(){
        if (scenes.empty()){
            return;
        }
        switch (state){
            case Play:
                scenes[current]->act();
                if (scenes[current]->done()){
                    scenes[current]->reset();
                }
                break;
            case Reverse:
            case Forward:
            case FastForward:
            case Rewind:
            case Stopped:
            default:
                break;
        }
    }
    
    void render(const Graphics::Bitmap & work){
        if (scenes.empty()){
            return;
        }
        switch (state){
            case Play:
                scenes[current]->render(work);
                break;
            case Reverse:
            case Forward:
            case FastForward:
            case Rewind:
            case Stopped:
            default:
                break;
        }
    }
    
    void setState(const State & s){
        state = s;
    }
    
private:
    const Filesystem::AbsolutePath path;
    CutScene cutscene;
    std::vector<Util::ReferenceCount<Scene> > scenes;
    State state;
    unsigned int current;
};


class Logic: public Util::Logic {
public:
    Logic(InputMap<Keys> & input, CutSceneTool & cutscene):
    is_done(false),
    input(input),
    cutscene(cutscene),
    ticker(0){
    }

    bool is_done;
    InputMap<Keys> & input;
    CutSceneTool & cutscene;
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
        cutscene.act();
    }

    double ticks(double system){
        return system;
    }
};

class Draw: public Util::Draw {
public:
    Draw(CutSceneTool & cutscene):
    cutscene(cutscene){
    }
    
    CutSceneTool & cutscene;

    void draw(const Graphics::Bitmap & buffer){
        buffer.clear();
        cutscene.render(buffer);
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
            
            CutSceneTool cutscene(file);
            
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
