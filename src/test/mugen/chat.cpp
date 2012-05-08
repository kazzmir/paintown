#include "../common/init.h"
#include "../common/timer.h"

#include <iostream>
#include <vector>
#include <stdexcept>

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
#include "util/network/irc.h"
#include "util/thread.h"
#include "util/pointer.h"
#include "util/system.h"

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

enum ConnectionType{
    Server,
    Client,
    IrcClient,
};

static std::string unsplit(const std::vector< std::string > & message){
    std::string all;
    for (unsigned int i = 0; i < message.size(); ++i){
        all+=message[i] + (i < message.size()-1 ? " " : "");
    }
    return all;
}

class InputLogicDraw: public PaintownUtil::Logic, public PaintownUtil::Draw, public Mugen::Widgets::ChatPanel::Event{
public:
    InputLogicDraw(const ConnectionType & type, int port, const std::string & host = "127.0.0.1"):
    panel(10, 20, 300, 200),
    escaped(false){
        std::vector< PaintownUtil::ReferenceCount<Gui::ScrollItem> > list;
        Mugen::OptionMenu menu(list);
        panel.setFont(menu.getFont());
        panel.setClient("You");
        panel.subscribe(this);
        if (type == Server){
            server = PaintownUtil::ReferenceCount< ::Network::Chat::Server >(new ::Network::Chat::Server(port));
            server->start();
        } else if (type == Client){
            Global::debug(0) << "Connecting to " << host << " on port " << port << std::endl;
            Network::Socket socket = Network::connect(host, port);
            Global::debug(0) << "Connected" << std::endl;
            client = PaintownUtil::ReferenceCount< ::Network::Chat::Client >(new ::Network::Chat::Client(0, socket));
            client->start();
        } else if (type == IrcClient){
            ircClient = PaintownUtil::ReferenceCount< ::Network::IRC::Client >(new ::Network::IRC::Client(host, port));
            ircClient->connect();
            panel.setClient(ircClient->getName());
        }
    }
    
    Mugen::Widgets::ChatPanel panel;

    bool escaped;
    
    Network::Socket socket;
    
    std::queue< ::Network::Chat::Message > sendable;
    std::queue< ::Network::Chat::Message > messages;
    
    PaintownUtil::ReferenceCount< ::Network::Chat::Client > client;
    PaintownUtil::ReferenceCount< ::Network::Chat::Server > server;
    PaintownUtil::ReferenceCount< ::Network::IRC::Client > ircClient;
    
    ::Util::Thread::LockObject lock;
    ::Util::Thread::Id thread;
    
    double ticks(double system){
        return system;
    }

    void run(){
        try {
            panel.act();
            if (ircClient == NULL){
                sendMessages();
            }
            processMessages();
            if (server != NULL){
                server->cleanup();
            }
        } catch (const Exception::Return & ex){
            escaped = true;
            if (server != NULL){
                server->shutdown();
            } else if (client != NULL){
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
                } else if (client != NULL){
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
        } else if (client != NULL) {
            while (client->hasMessages()){
                ::Network::Chat::Message message = client->nextMessage();
                if (isChatMessage(message)){
                    panel.addMessage(message.getName(), message.getMessage());
                }
            }
        } else if (ircClient != NULL) {
            while (ircClient->hasCommands()){
                ::Network::IRC::Command command = ircClient->nextCommand();
                std::vector<std::string> params = command.getParameters();
                //Global::debug(0) << "Got message: " << command.getSendable() << std::endl;
                try {
                    if (command.getType() == ::Network::IRC::Command::Ping){
                        ircClient->sendPong(command);
                        panel.addMessage(command.getOwner(), "*** Ping!");
                    } else if (command.getType() == ::Network::IRC::Command::PrivateMessage || 
                              command.getType() == ::Network::IRC::Command::Notice){
                        // Username and message 
                        panel.addMessage(command.getOwner(), params.at(1));
                    } else if (command.getType() == ::Network::IRC::Command::ReplyMOTD){
                        panel.addMessage("*** MOTD - " + params.at(1));
                    } else if (command.getType() == ::Network::IRC::Command::Join){
                        panel.addMessage("*** You have joined the channel " + params.at(0) + ".");
                    } else if (command.getType() == ::Network::IRC::Command::ReplyTopic){
                        panel.addMessage("*** The channel topic is \"" + params.at(2) + "\".");
                    } else if (command.getType() == ::Network::IRC::Command::ReplyNames){
                        panel.addMessage("*** Current users on " + params.at(1) + " \"" + params.at(2) + "\".");
                    } else if (command.getType() == ::Network::IRC::Command::Error){
                        Global::debug(0) << "Received Error: " << command.getSendable() << "... Aborting." << std::endl;
                        throw Exception::Return(__FILE__, __LINE__);
                    }
                } catch (const std::out_of_range & ex){
                }
            }
        }
    }
    
    void sendMessage(const std::string & message){
        ::Util::Thread::ScopedLock scope(lock);
        if (ircClient == NULL){
            if (panel.getClient() == "You"){
                ::Network::Chat::Message ourMessage(::Network::Chat::Message::Chat, "remote", message);
                sendable.push(ourMessage);
            } else {
                ::Network::Chat::Message ourMessage(::Network::Chat::Message::Chat, panel.getClient(), message);
                sendable.push(ourMessage);
            }
        } else if (ircClient != NULL){
            ircClient->sendMessage(message);
        }
    }
    
    void handleCommand(const std::vector<std::string> & command){
        try{
            if (ircClient == NULL){
                if (command.at(0) == "help"){
                    panel.addMessage("* commands: help, nick");
                } else if (command.at(0) == "nick"){
                    const std::string & nick = command.at(1);
                    if (!nick.empty()){
                        panel.setClient(nick);
                        panel.addMessage("* nick changed to " + nick);
                    }
                } else {
                    panel.addMessage("* Uknown command.");
                }
            } else if (ircClient != NULL){
                if (command.at(0) == "help"){
                    panel.addMessage("* commands: help, nick, whisper, ping, join, quit");
                } else if (command.at(0) == "nick"){
                    const std::string & nick = command.at(1);
                    if (!nick.empty()){
                        ircClient->setName(nick);
                        panel.setClient(nick);
                        panel.addMessage("* nick changed to " + nick);
                    }
                } else if (command.at(0) == "whisper"){
                    const std::string & who = command.at(1);
                    const std::string & message = command.at(2);
                    if (!who.empty() && !message.empty()){
                        ircClient->sendCommand(::Network::IRC::Command::PrivateMessage, who, ":" + message);
                        panel.addMessage("-> " + who + " " + message);
                    }
                } else if (command.at(0) == "ping"){
                    const std::string & who = command.at(1);
                    if (!who.empty()){
                        std::ostringstream timestamp;
                        timestamp << System::currentMicroseconds();
                        ircClient->sendCommand(::Network::IRC::Command::PrivateMessage, who, "\001PING " + timestamp.str() + "\001");
                        panel.addMessage("[CTCP] Sending CTCP-PING request to " + who);
                    }
                } else if (command.at(0) == "join"){
                    const std::string & channel = command.at(1);
                    if (!channel.empty()){
                        ircClient->joinChannel(channel);
                    }
                } else if (command.at(0) == "quit"){
                    const std::string & message = command.at(1);
                    if (!message.empty()){
                        ircClient->sendCommand(::Network::IRC::Command::Quit, ":"+message);
                        Global::debug(0) << "Quit (" + message + "). Shutting down..." << std::endl;
                        throw Exception::Return(__FILE__, __LINE__);
                    }
                } else {
                    panel.addMessage("* Uknown command.");
                }
            }
        } catch (const std::out_of_range & ex){
        }
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
    InputLogicDraw server(Server, port);
    PaintownUtil::standardLoop(server, server);
}


static void doClient(const std::string & host, int port){
    InputLogicDraw client(Client, port, host);
    PaintownUtil::standardLoop(client, client);
}

static void doIrc(const std::string & host, int port){
    InputLogicDraw client(IrcClient, port, host);
    PaintownUtil::standardLoop(client, client);
}

static void arguments(const std::string & application, int status){
    std::cout << "Usage: ./" << application << " server port" << std::endl;
    std::cout << "       ./" << application << " client port [host]" << std::endl;
    std::cout << "       ./" << application << " irc port [host]" << std::endl;
    exit(status);
}

int main(int argc, char ** argv){
    if (argc > 2){
        bool server = (strcmp(argv[1],"server")==0) ? true : false;
        bool client = (strcmp(argv[1],"client")==0) ? true : false;
        bool irc = (strcmp(argv[1],"irc")==0) ? true : false;
        if (!server && !client && !irc){
            arguments(argv[0], 1);
        }
        int port = atoi(argv[2]);
        std::string hostname = "127.0.0.1";
        if ((client || irc) && argc == 4){
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
            } else if (client){
                doClient(hostname, port);
            } else if (irc){
                doIrc(hostname, port);
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
