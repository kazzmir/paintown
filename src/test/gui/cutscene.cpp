#include "../common/init.h"
#include "../common/timer.h"

#include <iostream>
#include <vector>

#include "util/font.h"
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
    SpaceBar,
    S,
    P,
    R,
    Key1,
    Key2,
};

class CutSceneTool{
public:
    CutSceneTool(const std::string & file):
    path(file),
    state(Play),
    ticks(1){
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
        Stop=0,
        Play,
        Reverse,
    };
    
    void act(){
        switch (state){
            case Play:
                forward(ticks);
                break;
            case Reverse:
                reverse(ticks);
                break;
            case Stop:
            default:
                break;
        }
    }
    
    void render(const Graphics::Bitmap & work){
        if (scenes.empty()){
            return;
        }
        scenes[current]->render(work);
    }
    
    void forward(int tickCount = 1){
        if (scenes.empty()){
            return;
        }
        scenes[current]->forward(tickCount);
        if (scenes[current]->done()){
            scenes[current]->reset();
        }
    }
    
    void reverse(int tickCount = 1){
        if (scenes.empty()){
            return;
        }
        scenes[current]->reverse(tickCount);
        if (scenes[current]->done()){
            scenes[current]->setToEnd();
        }
    }
    
    void setState(const State & s){
        state = s;
    }
    
    const State & getState() const{
        return state;
    }
    
    int getTicks() const {
        return ticks;
    }
    
    void increaseTicks(){
        ticks++;
    }
    
    void decreaseTicks(){
        if (ticks > 0){
            ticks--;
        }
    }
    
private:
    const Filesystem::AbsolutePath path;
    CutScene cutscene;
    std::vector<Util::ReferenceCount<Scene> > scenes;
    State state;
    unsigned int current;
    int ticks;
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
                if (event.out == P){
                    cutscene.setState(CutSceneTool::Play);
                }
                if (event.out == R){
                    cutscene.setState(CutSceneTool::Reverse);
                }
                if (event.out == Left){
                    cutscene.setState(CutSceneTool::Stop);
                    cutscene.reverse(1);
                }
                if (event.out == Down){
                    cutscene.decreaseTicks();
                }
                if (event.out == Right){
                    cutscene.setState(CutSceneTool::Stop);
                    cutscene.forward(1);
                }
                if (event.out == Up){
                    cutscene.increaseTicks();
                }
                if (event.out == S || event.out == Enter || event.out == SpaceBar){
                    cutscene.setState(CutSceneTool::Stop);
                }
                if (event.out == Key1){
                    cutscene.reload();
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
        Font::getDefaultFont().printf(10, 385, Graphics::makeColor(0, 255, 0), buffer, "Current tick Speed: %d", 0, cutscene.getTicks());
        Font::getDefaultFont().printf(10, 400, Graphics::makeColor(255, 255, 255), buffer, "Controls:", 0);
        Font::getDefaultFont().printf(10, 415, Graphics::makeColor(255, 255, 255), buffer, "P - Play/Forward     |     R - Reverse     |     S/Space/Enter - Stop", 0);
        Font::getDefaultFont().printf(10, 430, Graphics::makeColor(255, 255, 255), buffer, "Up - Increase tick speed     |     Down - Decrease tick speed", 0);
        Font::getDefaultFont().printf(10, 445, Graphics::makeColor(255, 255, 255), buffer, "Left - Step through in reverse     |     Left - Step through forward", 0);
        Font::getDefaultFont().printf(10, 460, Graphics::makeColor(255, 0, 0), buffer, "1 - Reload file", 0);
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
        input.set(Keyboard::Key_S, 0, true, S);
        input.set(Keyboard::Key_P, 0, true, P);
        input.set(Keyboard::Key_R, 0, true, R);
        input.set(Keyboard::Key_SPACE, 0, true, SpaceBar);
        input.set(Keyboard::Key_1, 0, true, Key1);
        input.set(Keyboard::Key_2, 0, true, Key2);
       
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
