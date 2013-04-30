#include "../common/init.h"
#include "../common/timer.h"

#include <iostream>
#include <vector>
#include <stdexcept>

#include "util/graphics/bitmap.h"
#include "util/font.h"
#include "util/debug.h"
#include "util/exceptions/load_exception.h"
#include "util/token_exception.h"
#include "util/input/input.h"
#include "util/input/input-manager.h"
#include "util/sound/sound.h"
#include "util/exceptions/exception.h"
#include "util/network/network.h"
#include "util/network/irc.h"
#include "util/thread.h"
#include "util/pointer.h"
#include "util/system.h"

#include <queue>

static std::vector<std::string> split(std::string str, char splitter){
    std::vector<std::string> strings;
    size_t next = str.find(splitter);
    while (next != std::string::npos){
        strings.push_back(str.substr(0, next));
        str = str.substr(next+1);
        next = str.find(splitter);
    }
    if (str != ""){
        strings.push_back(str);
    }

    return strings;
}

static void setTrue(void * b){
    bool * what = (bool*) b;
    *what = true;
}

static void nextTab(void * i){
    ::Network::IRC::ChatInterface * interface = (::Network::IRC::ChatInterface *)i;
    interface->nextChannel();
}

static void previousTab(void * i){
    ::Network::IRC::ChatInterface * interface = (::Network::IRC::ChatInterface *)i;
    interface->previousChannel();
}

class InputLogicDraw: public Util::Logic, public Util::Draw {
public:
    InputLogicDraw(int port, const std::string & host = "127.0.0.1"):
    chatInterface(host, port),
    escaped(false){
        ircClient = Util::ReferenceCount< ::Network::IRC::Client >(new ::Network::IRC::Client(host, port));
        chatInterface.getInputBox().addHook(Keyboard::Key_ESC, setTrue, &escaped);
        chatInterface.getInputBox().addHook(Keyboard::Key_TAB, nextTab, &chatInterface);
    }
    
    ::Network::IRC::ChatInterface chatInterface;
    Util::ReferenceCount< ::Network::IRC::Client > ircClient;
    
    bool escaped;
    
    std::queue< ::Network::Chat::Message > sendable;
    std::queue< ::Network::Chat::Message > messages;
    
    ::Util::Thread::LockObject lock;
    ::Util::Thread::Id thread;
    
    // check ctcp reply
    std::map<std::string, uint64_t> pingReply;
    std::map< std::string, std::vector<std::string> > namesRequest;
    
    double ticks(double system){
        return system;
    }

    void run(){
        try {
            chatInterface.act();
        } catch (const Exception::Return & ex){
            escaped = true;
            throw ex;
        }
    }

    bool done(){
        return escaped;
    }
    
    void draw(const Graphics::Bitmap & screen){
        screen.fill(Graphics::makeColor(0,0,0));
        chatInterface.draw(screen);
        screen.BlitToScreen();
    }
};

static void doIrc(const std::string & host, int port){
    InputLogicDraw client(port, host);
    Util::standardLoop(client, client);
}

static void arguments(const std::string & application, int status){
    std::cout << "Usage: ./" << application << " port [host]" << std::endl;
    exit(status);
}

int main(int argc, char ** argv){
    if (argc >= 2){
        int port = atoi(argv[1]);
        std::string hostname = argv[2];
        
        Screen::realInit();
        atexit(Screen::realFinish);
        Common::startTimers();
        
        Sound::initialize();
        
        Global::setDebug(2);
        
        Graphics::Bitmap screen(*Graphics::getScreenBuffer());
        Util::Parameter<Graphics::Bitmap*> use(Graphics::screenParameter, &screen);
        Keyboard::pushRepeatState(true);
        
        InputManager manager;
        
        Util::Parameter<Util::ReferenceCount<Path::RelativePath> > 
            defaultFont(Font::defaultFont, Util::ReferenceCount<Path::RelativePath>(new Path::RelativePath("fonts/arial.ttf")));
        
        Network::init();
            
        try {
            doIrc(hostname, port);
        } catch (const Exception::Return & ex){
        } catch (const Network::NetworkException & ex){
            Global::debug(0) << "Network Exception: " << ex.getMessage() << std::endl;
        }
        Network::shutdown();
    } else {
        arguments(argv[0],0);
    }
    return 0;
}
#ifdef USE_ALLEGRO
END_OF_MAIN()
#endif
