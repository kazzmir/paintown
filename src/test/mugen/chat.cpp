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
#include "util/sound.h"
#include "util/exceptions/exception.h"
#include "util/network/network.h"
#include "util/thread.h"

#include "mugen/util.h"
#include "mugen/search.h"
#include "mugen/exception.h"
#include "mugen/options.h"

#include "mugen/widgets.h"

#include <queue>
static void sendMessage(std::string message, Network::Socket socket){
    int size = message.size()+1 + sizeof(uint16_t);
    char * buffer = new char[size];
    char * position = buffer;
    position = Network::dump16(position, size);
    position = Network::dumpStr(position, message);
    Network::sendBytes(socket, (uint8_t*) buffer, size + sizeof(uint16_t));
    delete[] buffer;
}

static std::string readMessage(Network::Socket socket){
    int16_t size = Network::read16(socket);
    char * buffer = new char[size];
    Network::readBytes(socket, (uint8_t*) buffer, size);
    char * position = buffer;
    std::string message;
    position = Network::parseString(position, &message, size + 1);
    
    delete[] buffer;

    return message;
}

class Logic: public PaintownUtil::Logic, public Mugen::Widgets::ChatPanel::Event{
public:
    Logic(Mugen::Widgets::ChatPanel & panel):
    panel(panel),
    escaped(false){
    }
    
    Mugen::Widgets::ChatPanel & panel;

    bool escaped;
    
    Network::Socket socket;
    

    std::queue<std::string> sendable;
    std::queue<std::string> messages;
    
    ::Util::Thread::LockObject lock;
    ::Util::Thread::Id thread;
    
    double ticks(double system){
        return system;
    }

    void run(){
        try {
            panel.act();
            sendMessages();
            processMessages();
        } catch (const Exception::Return & ex){
            escaped = true;
            Network::close(socket);
            ::Util::Thread::joinThread(thread);
            throw ex;
        }
    }

    bool done(){
        return escaped;
    }
    
    void sendMessages(){
        ::Util::Thread::ScopedLock scope(lock);
        while (!sendable.empty()){
            sendMessage(sendable.front(), socket);
            sendable.pop();
        }
    }
    
    void processMessages(){
        ::Util::Thread::ScopedLock scope(lock);
        while (!messages.empty()){
            panel.addMessage("remote", messages.front());
            messages.pop();
        }
    }
    
    void pollMessages(){
        try{
            while (!escaped){
                std::string message = readMessage(socket);
                lock.acquire();
                messages.push(message);
                lock.signal();
                lock.release();
            }
        } catch (const Network::NetworkException & fail){
        }
    }
    
    void addMessage(const std::string & message){
        ::Util::Thread::ScopedLock scope(lock);
        sendable.push(message);
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

static void * do_thread(void * arg){
    Logic * logic = (Logic*) arg;
    logic->pollMessages();
    return NULL;
}

static void doServer(int port){
    Mugen::Widgets::ChatPanel chat(10, 20, 300, 200);
    std::vector< PaintownUtil::ReferenceCount<Gui::ScrollItem> > list;
    Mugen::OptionMenu menu(list);
    chat.setFont(menu.getFont());
    chat.setClient("You");
    Logic logic(chat);
    chat.subscribe(&logic);
    Draw draw(chat);
    
    Network::Socket remote = Network::open(port);
    Network::listen(remote);
    Global::debug(0) << "Waiting for a connection on port " << port << std::endl;
    logic.socket = Network::accept(remote);
    Network::close(remote);
    Global::debug(0) << "Got a connection" << std::endl;
    
    ::Util::Thread::createThread(&logic.thread, NULL, (::Util::Thread::ThreadFunction) do_thread, &logic);
    
    PaintownUtil::standardLoop(logic, draw);
}


static void doClient(const std::string & host, int port){    
    Mugen::Widgets::ChatPanel chat(10, 20, 300, 200);
    std::vector< PaintownUtil::ReferenceCount<Gui::ScrollItem> > list;
    Mugen::OptionMenu menu(list);
    chat.setFont(menu.getFont());
    chat.setClient("You");
    Logic logic(chat);
    chat.subscribe(&logic);
    Draw draw(chat);
    
    Global::debug(0) << "Connecting to " << host << " on port " << port << std::endl;
    logic.socket = Network::connect(host, port); 
    Global::debug(0) << "Connected" << std::endl;
    
    ::Util::Thread::createThread(&logic.thread, NULL, (::Util::Thread::ThreadFunction) do_thread, &logic);
    
    PaintownUtil::standardLoop(logic, draw);
}

static void arguments(const std::string & application, int status){
    std::cout << "Usage: ./" << application << " server port" << std::endl;
    std::cout << "       ./" << application << " client port [host]" << std::endl;
    exit(status);
}

int main(int argc, char ** argv){
    if (argc > 2){
        bool server = (strcmp(argv[1],"server")==0) ? true : false;
        bool client = (strcmp(argv[1],"client")==0) ? true : false;
        if (!server && !client){
            arguments(argv[0], 1);
        }
        int port = atoi(argv[2]);
        std::string hostname = "127.0.0.1";
        if (client && argc == 4){
            hostname = argv[3];
        }
        Screen::realInit();
        atexit(Screen::realFinish);
        Common::startTimers();
        
        Sound::initialize();
        
        Global::setDebug(2);
        
        Graphics::Bitmap screen(*Graphics::getScreenBuffer());
        Util::Parameter<Graphics::Bitmap*> use(Graphics::screenParameter, &screen);
        Keyboard::pushRepeatState(true);
        
        InputManager manager;
        
        Network::init();
            
        try {
            if (server){
                doServer(port);
            } else {
                doClient(hostname, port);
            }
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
