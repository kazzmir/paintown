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
#include "util/timedifference.h"
#include "util/configuration.h"

#include <queue>
#include <map>

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

class Game{
public:
    Game(const std::string & name, bool host = true):
    name(name),
    host(host){
    }
    ~Game(){
    }
    
    void addClient(const std::string & name, const std::string & ip){
        clients[name] = ip;
    }
    
    std::string clientsAsString(){
        std::string clientlist;
        for (std::map<std::string, std::string>::iterator i = clients.begin(); i != clients.end(); i++){
            clientlist += (*i).first + ", ";
        }
        return clientlist.substr(0, clientlist.size() - 2);
    }
    
    void start(const std::string & hostip) {
        Global::debug(0) << "Game " << name << " is starting." << std::endl;
        Global::debug(0) << "Connecting to " << hostip << "...." << std::endl;
    }
    
    std::map<std::string, std::string> & getClients(){
        return clients;
    }
    
    const std::string & getName() const {
        return name;
    }
    
    bool isHost() const {
        return host;
    }
    
private:
    std::string name;
    bool host;
    std::map<std::string, std::string> clients;
};

class InputLogicDraw: public Util::Logic, public Util::Draw, public ::Network::IRC::Message::EventInterface {
public:
    InputLogicDraw(int port, const std::string & host, const std::string & username):
    chatInterface(host, port, username),
    escaped(false){
        ircClient = Util::ReferenceCount< ::Network::IRC::Client >(new ::Network::IRC::Client(host, port));
        chatInterface.getInputBox().addHook(Keyboard::Key_ESC, setTrue, &escaped);
        chatInterface.getInputBox().addHook(Keyboard::Key_TAB, nextTab, &chatInterface);
        chatInterface.subscribe(this);
        // Get IP
        chatInterface.getClient()->sendCommand(::Network::IRC::Command::Userhost, chatInterface.getClient()->getName());
    }
    
    ::Network::IRC::ChatInterface chatInterface;
    Util::ReferenceCount< ::Network::IRC::Client > ircClient;
    
    bool escaped;
    
    std::string ipAddress;
    Util::ReferenceCount<Game> game;
    std::map<std::string, std::string> hosts;
    
    void remoteCommand(const ::Network::IRC::Command & command){
        if (command.getType() == ::Network::IRC::Command::ReplyUserhost){
            const std::vector<std::string> & params = command.getParameters();
            const std::string & extract = params.at(1);
            for (unsigned int i = 0; i < extract.size(); i++){
                if (extract[i] == '@'){
                    ipAddress = extract.substr(++i, extract.size()-1);
                    Global::debug(0) << "Your IP Address is: " << ipAddress << std::endl;
                }
            }
        }
        if (command.hasCtcp()){
            const std::vector<std::string> & ctcp = command.getCtcp();
            if (command.getType() == ::Network::IRC::Command::PrivateMessage){
                try {
                    if (ctcp.at(0) == "game"){
                        const std::string & gameCommand = ctcp.at(1);
                        if (gameCommand == "invite"){
                            const std::string & gameName = ctcp.at(2);
                            const std::string & hostName = ctcp.at(3);
                            chatInterface.addMessageToTab("* " + hostName + " has invited you to join the game [" + gameName + "]. Type \"/game join " + gameName + "\".");
                            hosts[gameName] = hostName;
                        } else if (gameCommand == "join"){
                            if (game != NULL && game->isHost()){                                
                                const std::string & gameName = ctcp.at(2);
                                const std::string & clientName = ctcp.at(3);
                                const std::string & ip = ctcp.at(4);
                                chatInterface.addMessageToTab("* " + clientName + " has joined " + gameName + ".");
                                game->addClient(clientName, ip);
                            } else {
                                chatInterface.addMessageToTab("* received a join request with no game pending.");
                            }
                        } else if (gameCommand == "start"){
                            if (game != NULL && !game->isHost()){
                                const std::string & gameName = ctcp.at(2);
                                const std::string & serverIp = ctcp.at(3);
                                chatInterface.addMessageToTab("* Host has started game " + gameName);
                                game->start(serverIp);
                            }
                        }
                    } else {
                        Global::debug(0) << "Got ctcp: " << ctcp.at(0) << std::endl;
                    }
                } catch (const std::out_of_range & ex){
                }
            }
        }
    }
    
    void localCommand(const std::vector<std::string> & command){
        if (command.at(0) == "help"){
            chatInterface.addMessageToTab("* commands: lol, game");
        } else if (command.at(0) == "lol"){
            chatInterface.addMessageToTab("* You LOLOLOLOLLOLOLOL yourself.");
        } else if (command.at(0) == "game"){
            // Game
            if (command.size() > 1){
                if (command.at(1) == "help"){
                    chatInterface.addMessageToTab("* game commands: help, new, start, list-users, list-hosts, invite, join.");
                } else if (command.at(1) == "new"){
                    try{
                        game = Util::ReferenceCount<Game>(new Game(command.at(2)));
                        chatInterface.addMessageToTab("* game \"" + command.at(2) + "\" created.");
                    } catch (const std::out_of_range & ex){
                        chatInterface.addMessageToTab("* /game new [name]");
                    }
                } else if (command.at(1) == "start"){
                    if (game != NULL && game->isHost()){
                        chatInterface.addMessageToTab("* Starting game " + game->getName());
                        for (std::map<std::string, std::string>::iterator i = game->getClients().begin(); i != game->getClients().end(); i++){
                            chatInterface.getClient()->sendCommand(::Network::IRC::Command::PrivateMessage,
                                                                    (*i).first,
                                                                    ":\001game start " + game->getName() + " " + ipAddress + "\001");
                        }
                    } else {
                        chatInterface.addMessageToTab("* Create a game first...");
                    }
                } else if (command.at(1) == "list-users"){
                    if (game != NULL){
                        chatInterface.addMessageToTab("* Users who accepted game request: " + game->clientsAsString());
                    } else {
                        chatInterface.addMessageToTab("* Create a game first...");
                    }
                } else if (command.at(1) == "list-hosts"){
                    if (!hosts.empty()){
                        std::string hostlist;
                        for (std::map<std::string, std::string>::iterator i = hosts.begin(); i != hosts.end(); i++){
                            hostlist += (*i).first + +" [" + (*i).second + "], ";
                        }
                        chatInterface.addMessageToTab("* Hosts and games you've been invited to: " + hostlist.substr(0, hostlist.size() - 2));
                    } else {
                        chatInterface.addMessageToTab("* You have no invites.");
                    }
                } else if (command.at(1) == "invite"){
                    if (game != NULL){
                        try {
                            chatInterface.addMessageToTab("* Sending request to " + command.at(2));
                            chatInterface.getClient()->sendCommand(::Network::IRC::Command::PrivateMessage,
                                                            command.at(2),
                                                            ":\001game invite " + game->getName() + " " + chatInterface.getClient()->getName() + "\001");
                        } catch (const std::out_of_range & ex){
                            chatInterface.addMessageToTab("* /game invite [username]");
                        }
                    } else {
                        chatInterface.addMessageToTab("* Create a game first...");
                    }
                } else if (command.at(1) == "join"){
                    if (!hosts.empty()){
                        try {
                            const std::string & gameName = command.at(2);
                            const std::string & hostName = hosts[gameName];
                            chatInterface.addMessageToTab("* Joining game at " + gameName);
                            chatInterface.getClient()->sendCommand(::Network::IRC::Command::PrivateMessage,
                                                            hostName,
                                                            ":\001game join " + gameName + " " + chatInterface.getClient()->getName() + " " + ipAddress + "\001");
                            game = Util::ReferenceCount<Game>(new Game(gameName, false));
                        } catch (const std::out_of_range & ex){
                            chatInterface.addMessageToTab("* /game join [name]");
                        }
                    } else {
                        chatInterface.addMessageToTab("* You must be invited to a game first...");
                    }
                }
            }
        } else {
            //chatInterface.addMessageToTab("* Uknown command.");
        }
    }
    
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
        chatInterface.draw(screen);
    }
};

static void doIrc(const std::string & host, int port, const std::string & username){
    InputLogicDraw client(port, host, username);
    Util::standardLoop(client, client);
}

static void arguments(const std::string & application, int status){
    std::cout << "Usage: ./" << application << " port host [username]" << std::endl;
    exit(status);
}

int main(int argc, char ** argv){
    if (argc >= 2){
        int port = atoi(argv[1]);
        std::string hostname = argv[2];
        std::string username = "paintown-test";
        if (argc > 3){
            username = argv[3];
        }
        
        Screen::realInit(Configuration::getScreenWidth(), Configuration::getScreenHeight());
        atexit(Screen::realFinish);
        Common::startTimers();
        
        Sound::initialize();
        
        Global::setDebug(2);
        
        Util::Parameter<Graphics::Bitmap*> use(Graphics::screenParameter, Graphics::getScreenBuffer());
        Keyboard::pushRepeatState(true);
        
        InputManager manager;
        
        Util::Parameter<Util::ReferenceCount<Path::RelativePath> > 
            defaultFont(Font::defaultFont, Util::ReferenceCount<Path::RelativePath>(new Path::RelativePath("fonts/arial.ttf")));
        
        Network::init();
            
        try {
            doIrc(hostname, port, username);
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
