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
    Key3,
    Key4,
    Key5,
    Key6,
};

class CutSceneTool{
public:
    CutSceneTool(const std::string & file):
    path(file),
    state(Play),
    ticks(1),
    currentId(0),
    displayAnimationId(true){
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
        if (scenes.empty()){
             throw LoadException(__FILE__, __LINE__, "No scenes found in cutscene context.");
        }
        current = currentId = 0;
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
        scenes[current]->render(work);
    }
    
    void forward(int tickCount = 1){
        scenes[current]->forward(tickCount);
        if (scenes[current]->done()){
            scenes[current]->reset();
        }
    }
    
    void reverse(int tickCount = 1){
        scenes[current]->reverse(tickCount);
        if (scenes[current]->done()){
            scenes[current]->setToEnd();
        }
    }
    
    void nextScene(){
        if (current < scenes.size()-1){
            current++;
            currentId = 0;
        } else {
            current = 0;
            currentId = 0;
        }
    }
    
    void previousScene(){
        if (current > 0){
            current--;
            currentId = 0;
        } else {
            current = scenes.size()-1;
            currentId = 0;
        }
    }
    
    void nextAnimationId(){
        if (currentId < scenes[current]->getManager().getIdList().size()-1){
            currentId++;
        } else {
            currentId = 0;
        }
    }
    
    void previousAnimationId(){
        if (currentId > 0){
            currentId--;
        } else {
            currentId = scenes[current]->getManager().getIdList().size()-1;
        }
    }
    
    void toggleDisplayAnimationId(){
        displayAnimationId = !displayAnimationId;
    }
     
    void setState(const State & s){
        state = s;
    }
    
    const State & getState() const{
        return state;
    }
    
    const std::string getInfo(){
        if (!displayAnimationId){
            return "";
        }
        return scenes[current]->getManager().getInfo(scenes[current]->getManager().getIdList()[currentId]);
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
    unsigned int currentId;
    bool displayAnimationId;
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
                    cutscene.toggleDisplayAnimationId();
                }
                if (event.out == Key2){
                    cutscene.reload();
                }
                if (event.out == Key3){
                    cutscene.previousScene();
                }
                if (event.out == Key4){
                    cutscene.nextScene();
                }
                if (event.out == Key5){
                    cutscene.previousAnimationId();
                }
                if (event.out == Key6){
                    cutscene.nextAnimationId();
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
    cutscene(cutscene),
    infoTextY(355){
    }
    
    CutSceneTool & cutscene;
    int infoTextY;

    void draw(const Graphics::Bitmap & buffer){
        buffer.clear();
        cutscene.render(buffer);
        Font::getDefaultFont(12,12).printfWrap(5, 5, Graphics::makeColor(255, 255, 255), buffer, 320, cutscene.getInfo(), 0);
        infoTextY = 325;
        Font::getDefaultFont().printf(10, nextHeight(), Graphics::makeColor(0, 255, 0), buffer, "Current tick Speed: %d", 0, cutscene.getTicks());
        Font::getDefaultFont().printf(10, nextHeight(), Graphics::makeColor(255, 255, 255), buffer, "Controls:", 0);
        Font::getDefaultFont().printf(10, nextHeight(), Graphics::makeColor(255, 255, 255), buffer, "P - Play/Forward     |     R - Reverse     |     S/Space/Enter - Stop", 0);
        Font::getDefaultFont().printf(10, nextHeight(), Graphics::makeColor(255, 255, 255), buffer, "Up - Increase tick speed     |     Down - Decrease tick speed", 0);
        Font::getDefaultFont().printf(10, nextHeight(), Graphics::makeColor(255, 255, 255), buffer, "Left - Step through in reverse     |     Left - Step through forward", 0);
        Font::getDefaultFont().printf(10, nextHeight(), Graphics::makeColor(255, 255, 255), buffer, "1 - Toggle Animation/Frame Display", 0);
        Font::getDefaultFont().printf(10, nextHeight(), Graphics::makeColor(255, 0, 0), buffer, "2 - Reload file", 0);
        Font::getDefaultFont().printf(10, nextHeight(), Graphics::makeColor(255, 255, 255), buffer, "3 - Previous Scene     |     4 - Next Scene", 0);
        Font::getDefaultFont().printf(10, nextHeight(), Graphics::makeColor(255, 255, 255), buffer, "5 - Previous Animation ID     |     6 - Next Animation ID", 0);
        buffer.BlitToScreen();
    }
    
    int nextHeight(){
        return infoTextY+=15;
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
