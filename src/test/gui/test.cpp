#include "../common/init.h"
#include "../common/timer.h"

#include "util/stretch-bitmap.h"
#include "util/input/input.h"
#include "util/input/input-manager.h"

#include "util/gui/animation.h"
#include "util/gui/box.h"
#include "util/gui/container.h"
#include "util/gui/context-box.h"
#include "util/gui/coordinate.h"
#include "util/gui/cutscene.h"
#include "util/gui/fadetool.h"
#include "util/gui/keys.h"
#include "util/gui/keyinput.h"
#include "util/gui/keyinput_manager.h"
#include "util/gui/lineedit.h"
#include "util/gui/rectarea.h"
#include "util/gui/popup-box.h"
#include "util/gui/scroll-list.h"
#include "util/gui/select-list.h"
#include "util/gui/tabbed-box.h"
#include "util/gui/timer.h"
#include "util/gui/widget.h"

using namespace std;

enum Keys{
    Up=0,
    Down,
    Left,
    Right,
    Esc
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
        vector<InputMap<Keys>::InputEvent> out = InputManager::getEvents(input, InputSource());
        for (vector<InputMap<Keys>::InputEvent>::iterator it = out.begin(); it != out.end(); it++){
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
            }
        }
    
    }

    double ticks(double system){
        return system;
    }
};

class Draw: public Util::Draw {
public:
    Draw(){
    }

    void draw(const Graphics::Bitmap & buffer){
        buffer.putPixel(rand() % 640, rand() % 480, Graphics::makeColor(rand() % 255, rand() % 255, rand() % 255));
        buffer.BlitToScreen();
    }
};

int main(int argc, char ** argv){
    Screen::realInit();
    Common::startTimers();
    
    InputManager manager;
    Graphics::Bitmap screen(Graphics::getScreenBuffer());
    Util::Parameter<Graphics::Bitmap*> use(Graphics::screenParameter, &screen);
    
    InputMap<Keys> input;
    input.set(Keyboard::Key_ESC, 0, true, Esc);
    
    Logic logic(input);
    Draw draw;

    Util::standardLoop(logic, draw);
    
    Screen::realFinish();
    return 0;
}
#ifdef USE_ALLEGRO
END_OF_MAIN()
#endif
