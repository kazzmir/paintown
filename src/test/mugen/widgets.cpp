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
            player1Input = Mugen::getPlayer1Keys(20);
            player2Input = Mugen::getPlayer2Keys(20);
        }
        
        InputMap<Mugen::Keys> player1Input;
        InputMap<Mugen::Keys> player2Input;
        
        Mugen::Widgets::ChatPanel & panel;

        bool escaped;
        
        double ticks(double system){
            return system;
        }

        void run(){
            InputSource input1;
            InputSource input2;
            std::vector<InputMap<Mugen::Keys>::InputEvent> out1 = InputManager::getEvents(player1Input, input1);
            std::vector<InputMap<Mugen::Keys>::InputEvent> out2 = InputManager::getEvents(player2Input, input2);
            out1.insert(out1.end(), out2.begin(), out2.end());
            for (std::vector<InputMap<Mugen::Keys>::InputEvent>::iterator it = out1.begin(); it != out1.end(); it++){
                const InputMap<Mugen::Keys>::InputEvent & event = *it;
                if (!event.enabled){
                    continue;
                }


                if (event[Mugen::Esc]){
                    if (!escaped){
                        escaped = true;
                        InputManager::waitForRelease(player1Input, input1, Mugen::Esc);
                        InputManager::waitForRelease(player2Input, input2, Mugen::Esc);
                    }
                }
            }
            
            panel.act();
        }

        bool done(){
            return escaped;
        }
    };

    class Draw: public PaintownUtil::Draw {
    public:
        Draw(Mugen::Widgets::ChatPanel & panel):
        panel(panel),
        buffer(320,240){
        }

        Mugen::Widgets::ChatPanel & panel;
        Graphics::Bitmap buffer;
        
        void draw(const Graphics::Bitmap & screen){
            buffer.clear();
            panel.draw(buffer);
            buffer.drawStretched(screen);
            screen.BlitToScreen();
        }
    };
    
    try {
        Mugen::Widgets::ChatPanel chat(10, 20, 310, 220);
        std::vector< PaintownUtil::ReferenceCount<Gui::ScrollItem> > list;
        Mugen::OptionMenu menu(list);
        chat.setFont(menu.getFont());
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
