#include "../common/init.h"
#include "../common/timer.h"

#include <iostream>
#include <vector>

#include "util/stretch-bitmap.h"
#include "util/font.h"
#include "util/debug.h"
#include "util/load_exception.h"
#include "util/token_exception.h"
#include "util/stretch-bitmap.h"
#include "util/input/input.h"
#include "util/input/input-manager.h"
#include "util/sound/sound.h"
#include "util/exceptions/exception.h"

#include "mugen/util.h"
#include "mugen/search.h"
#include "mugen/exception.h"
#include "mugen/options.h"

#include "mugen/widgets.h"

static void chatWidget(){
    class Logic: public PaintownUtil::Logic {
    public:
        Logic(Mugen::Widgets::ChatPanel & panel):
        panel(panel),
        escaped(false){
        }
        
        Mugen::Widgets::ChatPanel & panel;

        bool escaped;
        
        double ticks(double system){
            return system;
        }

        void run(){
            panel.act();
        }

        bool done(){
            return escaped;
        }
    };

    class Draw: public PaintownUtil::Draw {
    public:
        Draw(Mugen::Widgets::ChatPanel & panel):
        panel(panel){
        }

        Mugen::Widgets::ChatPanel & panel;
        
        void draw(const Graphics::Bitmap & screen){
            Graphics::StretchedBitmap stretch(320, 240, screen);
            stretch.start();
            stretch.fill(Graphics::makeColor(255,255,255));
            panel.draw(stretch);
            stretch.finish();
            screen.BlitToScreen();
        }
    };
    
    try {
        Mugen::Widgets::ChatPanel chat(10, 20, 300, 200);
        std::vector< PaintownUtil::ReferenceCount<Gui::ScrollItem> > list;
        Mugen::OptionMenu menu(list);
        chat.setFont(menu.getFont());
        chat.addMessage("<burgers> fckin n000b.");
        chat.addMessage("<turkey> I agree it's what I would have said to ur bag.");
        chat.setClient("err");
        Logic logic(chat);
        Draw draw(chat);
        PaintownUtil::standardLoop(logic, draw);
    } catch (const Exception::Return & ex){
        throw ex;
    }
}

int main(int argc, char ** argv){
    
    Screen::realInit();
    atexit(Screen::realFinish);
    Common::startTimers();
    
    Sound::initialize();
    
    Global::setDebug(2);
    
    Graphics::Bitmap screen(*Graphics::getScreenBuffer());
    Util::Parameter<Graphics::Bitmap*> use(Graphics::screenParameter, &screen);
    Keyboard::pushRepeatState(true);
    
    InputManager manager;
        
    try {
        chatWidget();    
    } catch (const Exception::Return & ex){
    } 
    return 0;
}
#ifdef USE_ALLEGRO
END_OF_MAIN()
#endif
