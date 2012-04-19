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
static void sendMessages(const std::vector<std::string> & messages, Network::Socket socket){
    int total = sizeof(uint16_t);
    for (std::vector<std::string>::const_iterator it = messages.begin(); it != messages.end(); it++){
        const std::string & message = *it;
        total += sizeof(uint16_t);
        total += message.size() + 1;
    }
    char * buffer = new char[total + sizeof(uint16_t)];
    char * position = buffer;
    position = Network::dump16(position, total);
    position = Network::dump16(position, messages.size());
    for (std::vector<std::string>::const_iterator it = messages.begin(); it != messages.end(); it++){
        const std::string & message = *it;
        position = Network::dump16(position, message.size());
        position = Network::dumpStr(position, message);
    }
    Network::sendBytes(socket, (uint8_t*) buffer, total + sizeof(uint16_t));
    delete[] buffer;
}

static std::vector<std::string> readMessages(Network::Socket socket){
    std::vector<std::string> out;
    int16_t total = Network::read16(socket);
    char * buffer = new char[total];
    Network::readBytes(socket, (uint8_t*) buffer, total);
    char * position = buffer;
    uint16_t messages = 0;
    position = Network::parse16(position, &messages);
    for (int i = 0; i < messages; i++){
        uint16_t length = 0;
        position = Network::parse16(position, &length);
        std::string message;
        position = Network::parseString(position, &message, length + 1);
        out.push_back(message);
    }
    delete[] buffer;

    return out;
}

std::queue<std::string> sendable;

static void * listenMessage(const std::string & message){
    sendable.push(message);
    return NULL;
}

class Logic: public PaintownUtil::Logic {
public:
    Logic(Mugen::Widgets::ChatPanel & panel):
    panel(panel),
    escaped(false){
    }
    
    Mugen::Widgets::ChatPanel & panel;

    bool escaped;
    
    Network::Socket socket;
    
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
            ::Util::Thread::joinThread(thread);
            throw ex;
        }
    }

    bool done(){
        return escaped;
    }
    
    void sendMessages(){
        std::vector<std::string> messages;
        while (!sendable.empty()){
            messages.push_back(sendable.front());
            sendable.pop();
        }
        sendMessages(messages, socket);
    }
    
    void processMessages(){
        lock.acquire();
        while (!messages.empty()){
            panel.addMessage("remote", messages.front());
            messages.pop();
        }
        lock.release();
    }
    
    void pollMessages(){
        try{
            while (!escaped){
                std::vector<std::string> more = readMessages(socket);
                lock.acquire();
                for (std::vector<std::string>::iterator i = more.begin(); i != more.end(); ++i){
                    messages.push(*i);
                }
                lock.signal();
                lock.release();
            }
        } catch (const Network::NetworkException & fail){
        }
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
    chat.connectListener(listenMessage);
    Logic logic(chat);
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
    chat.connectListener(listenMessage);
    Logic logic(chat);
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
