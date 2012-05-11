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

static std::string join(const std::vector< std::string > & message, unsigned int start = 0){
    std::string all;
    for (unsigned int i = start; i < message.size(); ++i){
        all+=message.at(i) + (i < message.size()-1 ? " " : "");
    }
    return all;
}

class InputLogicDraw: public PaintownUtil::Logic, public PaintownUtil::Draw, public Mugen::Widgets::ChatPanel::Event{
public:
    InputLogicDraw(const ConnectionType & type, int port, const std::string & host = "127.0.0.1"):
    panel(10, 20, 250, 200),
    escaped(false){
        users.setWidth(45);
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
    Mugen::Widgets::ListBox users;

    bool escaped;
    
    Network::Socket socket;
    
    std::queue< ::Network::Chat::Message > sendable;
    std::queue< ::Network::Chat::Message > messages;
    
    PaintownUtil::ReferenceCount< ::Network::Chat::Client > client;
    PaintownUtil::ReferenceCount< ::Network::Chat::Server > server;
    PaintownUtil::ReferenceCount< ::Network::IRC::Client > ircClient;
    
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
            panel.act();
            if (ircClient == NULL){
                sendMessages();
            } else if (ircClient != NULL){
                // Check nick and fix
                if (ircClient->getName() != panel.getClient()){
                    panel.setClient(ircClient->getName());
                }
                // Update userlist
                users.replace(ircClient->getChannel().getUsers());
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
                    } else if (command.getType() == ::Network::IRC::Command::ReplyMOTD ||
                               command.getType() == ::Network::IRC::Command::ReplyMOTDStart ||
                               command.getType() == ::Network::IRC::Command::ReplyMOTDEndOf){
                        panel.addMessage("*** MOTD " + params.at(1));
                    } else if (command.getType() == ::Network::IRC::Command::Join){
                        panel.addMessage("*** You have joined the channel " + params.at(0) + ".");
                    } else if (command.getType() == ::Network::IRC::Command::ReplyNoTopic){
                        panel.addMessage("*** The channel " + params.at(1) + " has no topic set.");
                    } else if (command.getType() == ::Network::IRC::Command::ReplyTopic){
                        panel.addMessage("*** The channel topic for " + params.at(1) + " is: \"" + params.at(2) + "\".");
                    } else if (command.getType() == ::Network::IRC::Command::ReplyNames){
                        std::vector<std::string> names = split(params.at(2), ' ');
                        std::map<std::string, std::vector<std::string> >::iterator check = namesRequest.find(params.at(1));
                        if (check == namesRequest.end()){
                            namesRequest[params.at(1)] = std::vector<std::string>();
                            check = namesRequest.find(params.at(1));
                        }
                        check->second.insert(check->second.begin(), names.begin(), names.end());
                    } else if (command.getType() == ::Network::IRC::Command::ReplyNamesEndOf){
                        std::map<std::string, std::vector<std::string> >::iterator check = namesRequest.find(params.at(1));
                        if (check != namesRequest.end()){
                            panel.addMessage("*** Current users on " + params.at(1) + " \"" + join(check->second) + "\".");
                            namesRequest.erase(check);
                        }
                    } else if (command.getType() == ::Network::IRC::Command::ErrorNickInUse){
                        panel.addMessage("[Error] " + params.at(1) + ": Nick already in use.");
                    } else if (command.getType() == ::Network::IRC::Command::ErrorNoSuchNick){
                        panel.addMessage("[Error] " + params.at(1) + ": No such nick.");
                    } else if (command.getType() == ::Network::IRC::Command::ErrorNoSuchChannel){
                        panel.addMessage("[Error] " + params.at(1) + ": No such channel.");
                    } else if (command.getType() == ::Network::IRC::Command::Error){
                        Global::debug(0) << "Received Error: " << command.getSendable() << "... Aborting." << std::endl;
                        throw Exception::Return(__FILE__, __LINE__);
                    }
                } catch (const std::out_of_range & ex){
                }
                // Check if we got any CTCP delimited messages
                if (command.hasCtcp()){
                    std::vector<std::string> ctcp = command.getCtcp();
                    try {
                        if (ctcp.at(0) == "PING"){
                            // Lets check if there is an existing query otherwise send off request
                            if (command.getType() == ::Network::IRC::Command::Notice){
                                std::map<std::string, uint64_t>::iterator check = pingReply.find(command.getOwner());
                                if (check != pingReply.end()){
                                    // there is an existing entry lets display our ping
                                    std::ostringstream difference;
                                    difference << (double)((System::currentMicroseconds() - check->second)/1000000000);
                                    panel.addMessage("[CTCP] Received CTCP-PING reply from " + check->first + ": " + difference.str() + "second(s)" );
                                    pingReply.erase(check);
                                }
                            } else if (command.getType() == ::Network::IRC::Command::PrivateMessage){
                                ircClient->sendCommand(::Network::IRC::Command::Notice, command.getOwner(), ":\001PING " + ctcp.at(1) + "\001");
                                panel.addMessage("[CTCP] Received CTCP-PING request from " + command.getOwner() + ", sending answer.");
                            }
                        }
                    } catch (const std::out_of_range & ex){
                    }
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
        if (ircClient == NULL){
            try {
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
            } catch (const std::out_of_range & ex){
            }
        } else if (ircClient != NULL){
            if (command.at(0) == "help"){
                panel.addMessage("* commands: help, nick, whisper, ping, join, names, topic, quit");
            } else if (command.at(0) == "nick"){
                try {
                    const std::string & nick = command.at(1);
                    if (!nick.empty()){
                        ircClient->setName(nick);
                        panel.setClient(nick);
                        panel.addMessage("* nick changed to " + nick);
                    } 
                } catch (const std::out_of_range & ex){
                    panel.addMessage("* /nick [name]");
                }
            } else if (command.at(0) == "whisper"){
                try {
                    const std::string & who = command.at(1);
                    const std::string & message = join(command, 2);
                    if (!who.empty() && !message.empty()){
                        ircClient->sendCommand(::Network::IRC::Command::PrivateMessage, who, ":" + message);
                        panel.addMessage("-> " + who + " " + message); 
                    }
                } catch (const std::out_of_range & ex){
                    panel.addMessage("* /whisper [nick] [message]");
                }
            } else if (command.at(0) == "ping"){
                try {
                    const std::string & who = command.at(1);
                    if (!who.empty()){
                        std::ostringstream timestamp;
                        uint64_t time = System::currentMicroseconds();
                        timestamp << time;
                        ircClient->sendCommand(::Network::IRC::Command::PrivateMessage, who, ":\001PING " + timestamp.str() + "\001");
                        panel.addMessage("[CTCP] Sending CTCP-PING request to " + who);
                        // Log user
                        pingReply[who] = time;
                    }
                } catch (const std::out_of_range & ex){
                    panel.addMessage("* /ping [nick]");
                }
            } else if (command.at(0) == "join"){
                try {
                    const std::string & channel = command.at(1);
                    if (!channel.empty()){
                        ircClient->joinChannel(channel);
                    }
                } catch (const std::out_of_range & ex){
                    panel.addMessage("* /join [channel]");
                }
            } else if (command.at(0) == "names"){
                try {
                    const std::string & channel = command.at(1);
                    if (!channel.empty()){
                        ircClient->sendCommand(::Network::IRC::Command::Names, channel);
                        namesRequest[channel] = std::vector<std::string>();
                    }
                } catch (const std::out_of_range & ex){
                    panel.addMessage("* /names [channel]");
                }
            } else if (command.at(0) == "topic"){
                try {
                    const std::string & channel = command.at(1);
                    if (!channel.empty()){
                        ircClient->sendCommand(::Network::IRC::Command::Topic, channel);
                    }
                } catch (const std::out_of_range & ex){
                    ircClient->sendCommand(::Network::IRC::Command::Topic, ircClient->getChannel().getName());
                }
            } else if (command.at(0) == "quit"){
                const std::string & message = join(command, 1);
                if (!message.empty()){
                    ircClient->sendCommand(::Network::IRC::Command::Quit, ":" + message);
                    Global::debug(0) << "Quit (" + message + "). Waiting for server to close connection..." << std::endl;
                } else {
                    ircClient->sendCommand(::Network::IRC::Command::Quit);
                    Global::debug(0) << "Quit. Waiting for server to close connection..." << std::endl;
                }
            } else {
                panel.addMessage("* Uknown command.");
            }
        }
    
    }
    
    void draw(const Graphics::Bitmap & screen){
        Graphics::StretchedBitmap stretch(320, 240, screen);
        stretch.start();
        stretch.fill(Graphics::makeColor(0,0,0));
        if (ircClient != NULL){
            // display channel
            panel.getFont().draw(160, panel.getFont().getHeight()+10, 0, "(" + ircClient->getChannel().getName() + ")", stretch);
        }
        panel.draw(stretch);
        users.draw(265, 20, panel.getFont(), stretch);
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
