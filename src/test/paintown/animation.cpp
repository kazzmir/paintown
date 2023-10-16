#include <iostream>
#include <functional>

#include "r-tech1/init.h"
#include "r-tech1/configuration.h"
#include "r-tech1/font.h"
#include "r-tech1/message-queue.h"
#include "r-tech1/file-system.h"
#include "r-tech1/input/input-source.h"
#include "r-tech1/input/input.h"
#include "r-tech1/input/input-manager.h"
#include "r-tech1/timedifference.h"
#include "r-tech1/graphics/bitmap.h"
#include "r-tech1/sound/sound.h"
#include "r-tech1/funcs.h"
#include "paintown-engine/game/mod.h"
#include "paintown-engine/object/player.h"
#include "factory/collector.h"

/*
#include <sstream>
#include <unistd.h>
*/

using namespace std;

enum Keys{
    Up=0,
    Down,
    Left,
    Right,
    Esc,
    Enter,
};

/* FIXME: dont put these methods in this test file */
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

/*
static int getPid(){
    return getpid();
}

static void runPmap(int pid){
    ostringstream out;
    out << "pmap " << pid;
    system(out.str().c_str());
}

static void showMemory(){
    runPmap(getPid());
}
*/

typedef std::function<void(Paintown::Player&)> AfterLoad;

static int load(const char * path, AfterLoad then){
    // showMemory();
    for (int i = 0; i < 1; i++){
        try{
            TimeDifference diff;
            diff.startTime();
            Global::debug(0) << "Loading " << path << endl;
            Paintown::Player player(Storage::instance().find(Filesystem::RelativePath(path)), Util::ReferenceCount<InputSource>(new InputSource(true)));
            diff.endTime();
            Global::debug(0, "test") << diff.printTime("Success! Took") << endl;

            then(player);
        } catch (const Filesystem::NotFound & e){
            Global::debug(0, "test") << "Test failure! Couldn't find a file: " << e.getTrace() << endl;
            return 1;
        }
    }
    return 0;
    // showMemory();
}

struct Data{
    int drawX;
    int drawY;

    Data(int x, int y):
        drawX(x), drawY(y){
        }
};

class Logic: public Util::Logic {
public:
    Logic(InputMap<Keys> & input, Paintown::Player & player, Data & data):
    is_done(false),
    input(input),
    player(player),
    ticker(0),
    data(data){
    }

    bool is_done;
    InputMap<Keys> & input;
    Paintown::Player & player;
    int ticker;
    Data & data;
    
    bool done(){
        return is_done;
    }

    void run(){
        std::vector<InputMap<Keys>::InputEvent> out = InputManager::getEvents(input, InputSource(true));
        for (const InputMap<Keys>::InputEvent & event: out){
            if (event.enabled){
                if (event.out == Esc){
                    is_done = true;
                }
                /* NOTE Assumes only one cursor */
                bool moved = false;
                if (event.out == Up){
                    data.drawY -= 1;
                    moved = true;
                }
                if (event.out == Down){
                    data.drawY += 1;
                    moved = true;
                }
                if (event.out == Left){
                    data.drawX -= 1;
                    moved = true;
                }
                if (event.out == Right){
                    data.drawX += 1;
                    moved = true;
                }

                if (moved){
                    Global::debug(0) << "Moved to " << data.drawX << ", " << data.drawY << endl;
                }
                /*
                if (event.out == Enter){
                    select.nextMessages();
                }
                */
            }
        }

        if (player.testAnimation()){
            Global::debug(0) << "Animation is done" << endl;
            player.testReset();
        }
        
        /*
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
        */
    }

    double ticks(double system){
        // return system * Global::ticksPerSecond(90);
        return system / 4;
    }
};

class Draw: public Util::Draw {
public:
    Draw(Paintown::Player & player, Data & data):
    player(player),
    data(data){
    }
    
    Paintown::Player & player;
    Data & data;

    void draw(const Graphics::Bitmap & buffer){
        buffer.clear();
        Graphics::Bitmap work(320, 240);
        // work.start();
        // work.clear();
        player.draw(&work, data.drawX, data.drawY);
        // work.finish();
        work.draw(0, 0, buffer);
        buffer.BlitToScreen();
    }
};

void showAnimation(Paintown::Player & player){
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
        Data data(0, 0);
        Logic logic(input, player, data);
        Draw draw(player, data);

        Util::standardLoop(logic, draw);

        /*
    } catch (const LoadException & ex){
        Global::debug(0) << "Problem loading file [" << file << "]. Reason: " << ex.getTrace() << std::endl;
    } catch (const TokenException & ex){
        Global::debug(0) << "Problem parsing file [" << file << "]. Reason: " << ex.getTrace() << std::endl;
    }
    */
    } catch (...){
        Global::debug(0) << "Unknown exception" << endl;
    }
}

int test_main(int argc, char ** argv){
    Util::setDataPath("paintown-data");
    Global::InitConditions conditions;
    // conditions.graphics = Global::InitConditions::Disabled;
    Global::init(conditions);
    Collector janitor;
    Util::Thread::initializeLock(&MessageQueue::messageLock);
    InputManager manager;

    Util::Parameter<Util::ReferenceCount<Path::RelativePath> > defaultFont(Font::defaultFont, Util::ReferenceCount<Path::RelativePath>(new Path::RelativePath("fonts/LiberationSans-Regular.ttf")));
    Configuration::loadConfigurations();
    Paintown::Mod::loadDefaultMod();
    Global::setDebug(1);

    int die = 0;
    if (argc < 2){
        die = load("players/akuma/akuma.txt", showAnimation);
    } else {
        die = load(argv[1], showAnimation);
    }

    // for (int i = 0; i < 3; i++){
      // }
    return die;
}

int main(int argc, char ** argv){
    return test_main(argc, argv);
}
