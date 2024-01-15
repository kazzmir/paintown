#include <iostream>
#include <vector>

#include "paintown-engine/game/character-select.h"
#include "paintown-engine/game/mod.h"

#include "r-tech1/init.h"
#include "r-tech1/debug.h"
#include "r-tech1/exceptions/load_exception.h"
#include "r-tech1/token_exception.h"
#include "r-tech1/graphics/bitmap.h"
#include "r-tech1/input/input.h"
#include "r-tech1/input/input-manager.h"
#include "r-tech1/font.h"

#include "factory/collector.h"

#include "r-tech1/libs/filesystem/fs-wrapper.h"

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
    select(select),
    ticker(0){
    }

    bool is_done;
    InputMap<Keys> & input;
    CharacterSelect & select;
    int ticker;
    
    bool done(){
        return is_done;
    }

    void run(){
        std::vector<InputMap<Keys>::InputEvent> out = InputManager::getEvents(input, InputSource(true));
        for (std::vector<InputMap<Keys>::InputEvent>::iterator it = out.begin(); it != out.end(); it++){
            const InputMap<Keys>::InputEvent & event = *it;
            if (event.enabled){
                if (event.out == Esc){
                    is_done = true;
                }
                /* NOTE Assumes only one cursor */
                if (event.out == Up){
                    select.moveUp(0);
                }
                if (event.out == Down){
                    select.moveDown(0);
                }
                if (event.out == Left){
                    select.moveLeft(0);
                }
                if (event.out == Right){
                    select.moveRight(0);
                }
                if (event.out == Enter){
                    select.nextMessages();
                }
            }
        }
        
        select.act();
       
        //! Update a message in a collection programmatically
        if (ticker++ >= 50){
            Util::ReferenceCount<MessageCollection> message = select.getMessages("player1");
            if (message != NULL){
                std::ostringstream number;
                number << random() % 9999999999;
                message->setReplaceMessage("number", number.str());
            }
            ticker = 0;
        }
    
    }

    double ticks(double system){
        return system / 4;
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

Filesystem::AbsolutePath Filesystem::configFile(){
    std::ostringstream str;
    /* what if HOME isn't set? */
    str << getenv("HOME") << "/.paintownrc";
    return Filesystem::AbsolutePath(str.str());
}

Filesystem::AbsolutePath Filesystem::userDirectory(){
    std::ostringstream str;
    char * home = getenv("HOME");
    if (home == NULL){
        str << "/tmp/paintown";
    } else {
        str << home << "/.paintown/";
    }
    return Filesystem::AbsolutePath(str.str());
}

// Use other directories
std::string getDataPath(){
    std::vector<std::string> locations = { "paintown-data", "data-new", "data-other" };
    for (std::vector<std::string>::iterator it = locations.begin(); it != locations.end(); it++){
        if (fs::is_directory(*it)){
            return *it;
        }
    }
    return "";
}

int test_main(int argc, char** argv){
    if (argc > 1){
        Collector janitor;
        Util::setDataPath(getDataPath());

        Util::Parameter<Util::ReferenceCount<Path::RelativePath> > defaultFont(Font::defaultFont, Util::ReferenceCount<Path::RelativePath>(new Path::RelativePath("fonts/LiberationSans-Regular.ttf")));

        Global::InitConditions conditions;
        Global::init(conditions);
        
        Global::setDebug(3);
        std::string file = argv[1];

        Paintown::Mod::loadDefaultMod();
        
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
       
        try {
            
            Filesystem::AbsolutePath path(file);
            CharacterSelect select(path);
            
            Logic logic(input, select);
            Draw draw(select);

            Util::standardLoop(logic, draw);
            
        } catch (const LoadException & ex){
            Global::debug(0) << "Problem loading file [" << file << "]. Reason: " << ex.getTrace() << std::endl;
        } catch (const TokenException & ex){
            Global::debug(0) << "Problem parsing file [" << file << "]. Reason: " << ex.getTrace() << std::endl;
        }
        
    } else {
        std::cout << "Usage: ./" << argv[0] << " select-screen.txt" << std::endl;
    }
    return 0;
}

int main(int argc, char ** argv){
    test_main(argc, argv);
}
