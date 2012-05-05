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
#include "util/pointer.h"

#include "mugen/util.h"
#include "mugen/search.h"
#include "mugen/exception.h"
#include "mugen/options.h"

#include "mugen/widgets.h"

#include <queue>

static void * do_client_thread(void * c);
static void * do_server_thread(void * s);

class Client{
public:
    Client(int id, Network::Socket socket):
    id(id),
    socket(socket),
    end(false),
    valid(true){
        ::Util::Thread::createThread(&thread, NULL, (::Util::Thread::ThreadFunction) do_client_thread, this);
    }
    ~Client();
    
    void run(){
        while (!end){
            try {
                std::string message = Mugen::NetworkUtil::readMessage(socket);
                lock.acquire();
                messages.push(message);
                lock.signal();
                lock.release();
            } catch (const Network::MessageEnd & ex){
                end = true;
            }
        }
        valid = false;
    }
    
    int getId() const {
        return id;
    }
    
    void sendMessage(const std::string & message){
        ::Util::Thread::ScopedLock scope(lock);
        Mugen::NetworkUtil::sendMessage(message, socket);
    }
    
    bool hasMessages() const{
        ::Util::Thread::ScopedLock scope(lock);
        return !messages.empty();
    }
    
    std::string nextMessage() const {
        ::Util::Thread::ScopedLock scope(lock);
        std::string message = messages.front();
        messages.pop();
        return message;
    }
    
    void shutdown(){
        ::Util::Thread::ScopedLock scope(lock);
        end = true;
        Network::close(socket);
        ::Util::Thread::joinThread(thread);
    }
    
    bool isValid() const {
        ::Util::Thread::ScopedLock scope(lock);
        return valid;
    }
private:
    int id;
    Network::Socket socket;
    ::Util::Thread::Id thread;
    ::Util::Thread::LockObject lock;
    bool end;
    mutable std::queue<std::string> messages;
    bool valid;
};

static void * do_client_thread(void * c){
    Client * client = (Client *)c;
    client->run();
    return NULL;
}

class Server{
public:
    Server(int port):
    end(false){
        remote = Network::open(port);
        Network::listen(remote);
        Global::debug(0) << "Waiting for a connection on port " << port << std::endl;
        ::Util::Thread::createThread(&remoteThread, NULL, (::Util::Thread::ThreadFunction) do_server_thread, this);
    }
    ~Server(){
    }
    
    void run(){
        int idList = 0;
        while (!end){
            clients.push_back(PaintownUtil::ReferenceCount<Client>(new Client(idList++, Network::accept(remote))));
            Global::debug(0) << "Got a connection" << std::endl;
        }
    }
    
    void poll(){
        for (std::vector< PaintownUtil::ReferenceCount<Client> >::iterator i = clients.begin(); i != clients.end(); ++i){
            PaintownUtil::ReferenceCount<Client> client = *i;
            while(client->hasMessages()){
                const std::string & message = client->nextMessage();
                relay(client->getId(), message);
                messages.push(message);
            }
        }
    }
    
    void cleanup(){
        for (std::vector< PaintownUtil::ReferenceCount<Client> >::iterator i = clients.begin(); i != clients.end(); ++i){
            PaintownUtil::ReferenceCount<Client> client = *i;
            while(!client->isValid()){
                client->shutdown();
                i = clients.erase(i);
            }
        }
    }
    
    bool hasMessages() const{
        return !messages.empty();
    }
    
    std::string nextMessage() const {
        std::string message = messages.front();
        messages.pop();
        return message;
    }
    
    void global(const std::string & message){
        for (std::vector< PaintownUtil::ReferenceCount<Client> >::iterator i = clients.begin(); i != clients.end(); ++i){
            PaintownUtil::ReferenceCount<Client> client = *i;
            client->sendMessage(message);
        }
    }
    
    void relay(int id, const std::string & message){
        for (std::vector< PaintownUtil::ReferenceCount<Client> >::iterator i = clients.begin(); i != clients.end(); ++i){
            PaintownUtil::ReferenceCount<Client> client = *i;
            if (client->getId() != id){
                client->sendMessage(message);
            }
        }
    }
    
    void shutdown(){
        ::Util::Thread::ScopedLock scope(lock);
        end = true;
        for (std::vector< PaintownUtil::ReferenceCount<Client> >::iterator i = clients.begin(); i != clients.end(); ++i){
            PaintownUtil::ReferenceCount<Client> client = *i;
            client->shutdown();
        }
        Network::close(remote);
        ::Util::Thread::joinThread(remoteThread);
    }
    
private:
    Network::Socket remote;
    std::vector< PaintownUtil::ReferenceCount<Client> > clients;
    
    mutable std::queue<std::string> messages;
    
    ::Util::Thread::Id remoteThread;
    
    ::Util::Thread::LockObject lock;
    
    bool end;
};

void * do_server_thread(void * s){
    Server * server = (Server *)s;
    server->run();
    return NULL;
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
            server = PaintownUtil::ReferenceCount<Server>(new Server(port));
        } else {
            Global::debug(0) << "Connecting to " << host << " on port " << port << std::endl;
            Network::Socket socket = Network::connect(host, port);
            Global::debug(0) << "Connected" << std::endl;
            client = PaintownUtil::ReferenceCount<Client>(new Client(0, socket));
        }
    }
    
    Mugen::Widgets::ChatPanel panel;

    bool escaped;
    
    Network::Socket socket;
    
    std::queue<std::string> sendable;
    std::queue<std::string> messages;
    
    PaintownUtil::ReferenceCount<Client> client;
    PaintownUtil::ReferenceCount<Server> server;
    
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
            const std::string & next = sendable.front();
            if (server != NULL){
                server->global(next);
            } else {
                client->sendMessage(next);
            }
            sendable.pop();
        }
    }
    
    void processMessages(){
        if (server != NULL){
            server->poll();
            while (server->hasMessages()){
                panel.addMessage("remote", server->nextMessage());
            }
        } else {
            while (client->hasMessages()){
                panel.addMessage("remote", client->nextMessage());
            }
        }
    }
    
    void pollMessages(){
        /*try{
            while (!escaped){
                std::string message = Mugen::NetworkUtil::readMessage(socket);
                lock.acquire();
                messages.push(message);
                lock.signal();
                lock.release();
            }
        } catch (const Network::NetworkException & fail){
        }*/
    }
    
    void addMessage(const std::string & message){
        ::Util::Thread::ScopedLock scope(lock);
        sendable.push(message);
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
/*
static void * do_thread(void * arg){
    Logic * logic = (Logic*) arg;
    logic->pollMessages();
    return NULL;
}*/

static void doServer(int port){
    /*
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
    */
    InputLogicDraw server(true, port);
    PaintownUtil::standardLoop(server, server);
}


static void doClient(const std::string & host, int port){
    /*
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
    */
    
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
