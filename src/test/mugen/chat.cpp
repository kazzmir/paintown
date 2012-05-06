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
#include "util/network/chat.h"
#include "util/thread.h"
#include "util/pointer.h"

#include "mugen/util.h"
#include "mugen/search.h"
#include "mugen/exception.h"
#include "mugen/options.h"

#include "mugen/widgets.h"

#include <queue>

static bool isChatMessage(const ::Network::Chat::Message & message){
    if (message.getType() == ::Network::Chat::Message::Ping || 
        message.getType() == ::Network::Chat::Message::Unknown){
        return false;
    }
    return true;
}

class InputLogicDraw: public PaintownUtil::Logic, public PaintownUtil::Draw, public Mugen::Widgets::ChatPanel::Event{
public:
    InputLogicDraw(bool isServer, int port, const std::string & host = "127.0.0.1"):
    panel(10, 20, 300, 200),
    escaped(false){
        std::vector< PaintownUtil::ReferenceCount<Gui::ScrollItem> > list;
        Mugen::OptionMenu menu(list);
        panel.setFont(menu.getFont());
        panel.setClient("You");
        panel.subscribe(this);
        if (isServer){
            server = PaintownUtil::ReferenceCount< ::Network::Chat::Server >(new ::Network::Chat::Server(port));
            server->start();
        } else {
            Global::debug(0) << "Connecting to " << host << " on port " << port << std::endl;
            Network::Socket socket = Network::connect(host, port);
            Global::debug(0) << "Connected" << std::endl;
            client = PaintownUtil::ReferenceCount< ::Network::Chat::Client >(new ::Network::Chat::Client(0, socket));
            client->start();
        }
    }
    
    Mugen::Widgets::ChatPanel panel;

    bool escaped;
    
    Network::Socket socket;
    
    std::queue< ::Network::Chat::Message > sendable;
    std::queue< ::Network::Chat::Message > messages;
    
    PaintownUtil::ReferenceCount< ::Network::Chat::Client > client;
    PaintownUtil::ReferenceCount< ::Network::Chat::Server > server;
    
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
            if (server != NULL){
                server->cleanup();
            }
        } catch (const Exception::Return & ex){
            escaped = true;
            if (server != NULL){
                server->shutdown();
            } else {
                client->shutdown();
            }
            throw ex;
        }
    }

    bool done(){
        return escaped;
    }
    
    void sendMessages(){
        while (!sendable.empty()){
            ::Network::Chat::Message & next = sendable.front();
            if (isChatMessage(next)){
                if (server != NULL){
                    server->global(next);
                } else {
                    client->sendMessage(next);
                }
            }
            sendable.pop();
        }
    }
    
    void processMessages(){
        if (server != NULL){
            server->poll();
            while (server->hasMessages()){
                ::Network::Chat::Message message = server->nextMessage();
                if (isChatMessage(message)){
                    panel.addMessage(message.getName(), message.getMessage());
                }
            }
        } else {
            while (client->hasMessages()){
                ::Network::Chat::Message message = client->nextMessage();
                if (isChatMessage(message)){
                    panel.addMessage(message.getName(), message.getMessage());
                }
            }
        }
    }
    
    void addMessage(const std::string & message){
        ::Util::Thread::ScopedLock scope(lock);
        ::Network::Chat::Message ourMessage(::Network::Chat::Message::Chat, "remote", message);
        sendable.push(ourMessage);
    }
    
    void draw(const Graphics::Bitmap & screen){
        Graphics::StretchedBitmap stretch(320, 240, screen);
        stretch.start();
        stretch.fill(Graphics::makeColor(255,255,255));
        panel.draw(stretch);
        stretch.finish();
        screen.BlitToScreen();
    }
};

static void doServer(int port){
    InputLogicDraw server(true, port);
    PaintownUtil::standardLoop(server, server);
}


static void doClient(const std::string & host, int port){
    InputLogicDraw client(false, port, host);
    PaintownUtil::standardLoop(client, client);
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
