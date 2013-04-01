#ifdef HAVE_NETWORKING

#include "util/graphics/bitmap.h"
#include "util/network/network.h"
#include "util/events.h"
#include "chat_server.h"
#include "chat.h"
#include "util/funcs.h"
#include "util/input/keyboard.h"
#include "util/font.h"
#include "util/file-system.h"
#include "util/exceptions/load_exception.h"
#include "util/exceptions/exception.h"
#include "globals.h"
#include "util/version.h"
#include "util/init.h"
#include "util/input/input-source.h"
#include "util/gui/lineedit.h"
#include "util/input/input-manager.h"
#include "util/menu/menu.h"
#include "util/timer.h"
#include <iostream>
#include <sstream>
#include "util/resource.h"
#include "util/sound/sound.h"

#include <signal.h>

using namespace std;

static std::ostream & debug( int level ){
    return Global::debug( level, "chat-server");
}

Client::Client( Network::Socket socket, ChatServer * server, unsigned int id ):
socket(socket),
server(server),
id(id),
alive(true),
started(false){
}
	
Client::~Client(){
}
	
string Client::getName(){
    string s;
    lock.acquire();
    s = name;
    lock.release();
    return s;
}

void Client::kill(){
    lock.acquire();
    alive = false;
    lock.release();
}

bool Client::isAlive(){
    bool b;
    lock.acquire();
    b = alive;
    lock.release();
    return b;
}

void Client::setName( const std::string & s ){
    lock.acquire();
    name = s;
    lock.release();
}

static void * clientInput( void * client_ ){
    Client * client = (Client *) client_;
    bool done = false;
    bool error = false;
    while ( ! done ){
        try{
            debug(1) << "Client " << client->getId() << " waiting for message" << endl;
            Network::Message message( client->getSocket() );
            debug(1) << client->getId() << " Got a message: '" << message.path << "'" << endl;
            int type;
            message >> type;
            switch (type){
                case CHANGE_NAME : {
                    message << client->getId();
                    client->getServer()->sendMessage( message, client->getId() );
                    client->setName( message.path );
                    client->getServer()->needUpdate();
                    break;
                }
                case ADD_MESSAGE : {
                    client->getServer()->addMessage( client->getName() + ":" + message.path, client->getId() );
                    break;
                }
                case OK_TO_START : {
                    client->setOk();
                    done = true;
                    break;
                }
            }
            // done = ! client->isAlive();
            Util::rest(1);
        } catch ( const Network::NetworkException & e ){
            debug( 0 ) << "Client input " << client->getId() << " died" << endl;
            done = true;
            error = true;
        }
    }

    debug(1) << client->getId() << " is done" << endl;

    // if ( client->canKill() ){
    // }
    if (error){
        debug(1) << "Input thread killing client" << endl;
        client->getServer()->killClient( client );
    }

    return NULL;
}

static void * clientOutput( void * client_ ){
    Client * client = (Client *) client_;
    bool done = false;
    bool error = false;
    while ( ! done ){
        Network::Message message;
        done = ! client->isAlive();
        if ( client->getOutgoing( message ) != false ){
            debug(1) << "Sending a message to " << client->getId() << endl;
            try{
                /*
                   Network::Message net;
                   net.path = message;
                   net.send( client->getSocket() );
                   */
                message.send( client->getSocket() );
                debug(1) << "Sent a message to " << client->getId() << endl;
            } catch ( const Network::NetworkException & e ){
                debug(0) << "Client output " << client->getId() << " died" << endl;
                done = true;
                error = true;
            }
        } else {
            Util::rest( 1 );
        }
    }

    debug(1) << "Output thread for client " << client->getId() << " is done" << endl;

    if (error){
        debug(1) << "Output thread killing client" << endl;
        client->getServer()->killClient( client );
    }

    return NULL;
}
	
bool Client::canKill(){
    bool f;
    lock.acquire();
    f = alive;
    /* why set alive to false here?
     * it was supposed to check if the client was already dead.
     */
    alive = false;
    lock.release();
    return f;
}

bool Client::getOutgoing(Network::Message & m){
    bool has;
    lock.acquire();
    has = ! outgoing.empty();
    if (has){
        m = outgoing.front();
        outgoing.erase(outgoing.begin());
    }
    lock.release();
    return has;
}

void Client::addOutputMessage(const Network::Message & s){
    lock.acquire();
    outgoing.push_back(s);
    lock.release();
}

void Client::startThreads(){
    lock.acquire();
    if (! started){
        Util::Thread::createThread(&inputThread, NULL, (Util::Thread::ThreadFunction) clientInput, this);
        Util::Thread::createThread(&outputThread, NULL, (Util::Thread::ThreadFunction) clientOutput, this);
        started = true;
    }
    lock.release();
}

struct do_add_stuff{
    do_add_stuff(ChatServer * server, Network::Socket socket):
    server(server), socket(socket){
    }

    ChatServer * server;
    Network::Socket socket;
};

static void kill_socket(void * arg){
    Network::Socket socket = *(Network::Socket *) arg;
    Network::close(socket);
}

static void * do_add(void * stuff_){
    do_add_stuff * stuff = (do_add_stuff *) stuff_;
    ChatServer * server = stuff->server;
    Network::Socket socket = stuff->socket;
    /* client has 5 seconds to perform the handshake */
    Util::Timer fail(5, &kill_socket, &socket);
    /* there is a very unlikely chance that addConnection will be processing
     * the client while the timer expires. A normal scenario is:
     *  1. client connects
     *  2. client sends HELLO
     *  3. server sends ADD_BUDDY messages to the clients (by putting the
     *    messages into queues)
     *  4. server stores the client in internal datastructures
     *
     * After the client sends the HELLO the other steps should be fast, like
     * a few microseconds at the most. So if the client takes 4.9999 seconds
     * to send HELLO then 5 seconds could expire during step 3 or something.
     *
     * This is so remote I don't care about fixing it at the moment. A possible
     * fix is to do everything after step 2 outside of the timer so that
     * if the client sends a HELLO then the server can take its time doing
     * the rest of the processing.
     */
    try{
        /* if the timer expires the socket will close and send a NetworkException */
        server->addConnection(socket);
        /* the timer could also expire right *here* and then things would break */
        fail.stop();
    } catch ( const Network::NetworkException & e ){
        Global::debug(0) << "Timer expired while processing client! Client dropped." << endl;
    }

    delete stuff;

    return NULL;
}

static void * acceptConnections( void * server_ ){
    bool done = false;
    ChatServer * server = (ChatServer *) server_;
    Network::Socket socket = server->getSocket();
    debug( 1 ) << "Accepting connections" << endl;
    while (! done){
        done = ! server->isAccepting();
        try{
            /* start the accepting handshake in a thread in case
             * the client dies or has an invalid version.
             */
            Network::Socket client = Network::accept(socket);
            Util::Thread::Id accepter;
            do_add_stuff * stuff = new do_add_stuff(server, client);
            Util::Thread::createThread(&accepter, NULL, (Util::Thread::ThreadFunction) do_add, stuff);
            server->addAccepter(accepter);
            // server->addConnection(client);
        } catch ( const Network::NoConnectionsPendingException & e ){
        } catch ( const Network::NetworkException & e ){
            debug( 0 ) << "Error accepting connections: " << e.getMessage() << endl;
            done = true;
        }
        Util::rest(1);
    }

#ifdef _WIN32
    Network::close( socket );
#endif

    debug( 1 )  << "Accept connection thread is done" << endl;

    return NULL;
}

/*
static void enter_pressed(void * self){
    ChatServer * chat = (ChatServer *) self;
    chat->addLine();
}
*/

static const char * welcomeMessage(){
    switch (Util::rnd(5)){
        case 0: return "Hi!";
        case 1: return "Hey!";
        case 2: return "What's up?";
        case 3: return "Cool!";
        case 4: return "Rockin!";
        default: return "Hi!";
    }
}

ChatServer::ChatServer(const string & name, Network::Socket socket):
need_update(true),
socket(socket),
messages(400, 350),
client_id(1),
name(name),
accepting(true){
    background = new Graphics::Bitmap(Global::titleScreen().path());

    debug(1) << "Start accepting connections" << endl;
    
    // editCounter = 0;

    /* listen() may throw an exception, so call it here so that the destructor
     * can properly delete the other objects that were created.
     */
    debug(1) << "Listen on socket" << endl;
    Network::listen(socket);
}

void ChatServer::addLine(Gui::LineEdit & line){
    if (line.getText().size() > 0){
        addMessage(name + ": " + line.getText(), 0);
        line.clearText();
        needUpdate();
    }
}
	
bool ChatServer::isAccepting(){
    bool f = false;
    lock.acquire();
    f = accepting;	
    lock.release();
    return f;
}
        
void ChatServer::addAccepter(Util::Thread::Id accepter){
    lock.acquire();
    accepted.push_back(accepter);
    lock.release();
}

void ChatServer::stopAccepting(){
    debug(1) << "Stop accepting" << endl;
    lock.acquire();
    accepting = false;
    lock.release();
    // #ifndef WINDOWS
    Network::close(socket);
    // #endif
    debug(1) << "Waiting for client accept threads to stop" << endl;
    lock.acquire();
    for (vector<Util::Thread::Id>::iterator it = accepted.begin(); it != accepted.end(); it++){
        Util::Thread::Id accept = *it;
        debug(2) << "Waiting for client accept thread " << endl;
        Util::Thread::joinThread(accept);
    }
    lock.release();
    debug(1) << "Waiting for accepting thread to stop" << endl;
    Util::Thread::joinThread(acceptThread);
    debug(1) << "Not accepting any connections" << endl;
}

static bool checkVersion(int version){
    if (version == Version::getVersion()){
        return true;
    }

    /* when an incompatible version is made, add a check here, like
     *  version < getVersion(3, 5)
     * would mean any client below version 3.5 is incompatible.
     *
     * assume versions of client's greater than ourself is compatible, but
     * this may not be true. There is no way to check this.
     */
    if (version < 0){
        return false;
    }

    return true;
}

void ChatServer::addConnection(Network::Socket s){
    Client * client = new Client(s, this, clientId());

    /* the client should send a hello message to us immediately */
    try{
        Network::Message hello(s);
        int type;
        hello >> type;
        if (type != HELLO){
            ostringstream out;
            out << "Client sent something other than a HELLO: " << type;
            throw LoadException(__FILE__, __LINE__, out.str());
        }
        unsigned int magic;
        hello >> magic;
        if (magic != Network::MagicId){
            ostringstream out;
            out << "Invalid magic id: " << magic;
            throw LoadException(__FILE__, __LINE__, out.str());
        }
        int version;
        hello >> version;
        if (! checkVersion(version)){
            ostringstream out;
            out << "Invalid version: " << version;
            throw LoadException(__FILE__, __LINE__, out.str());
        }
        string name;
        name = hello.path;
        client->setName(name);
    } catch (const Network::NetworkException & e){
        delete client;
        throw e;
    } catch (const LoadException & e){
        delete client;
        Global::debug(0) << "Dropping connection: " << e.getTrace() << endl;
        return;
    }

    /* send the server name to the just connected client */
    {
        Network::Message message;
        message << ADD_BUDDY;
        message << 0;
        message.path = getName();
        client->addOutputMessage(message);
    }

    /* send all the other client names to the just connected client */
    lock.acquire();
    for ( vector< Client * >::iterator it = clients.begin(); it != clients.end(); it++ ){
        Client * c = *it;
        Network::Message message;
        message << ADD_BUDDY;
        message << c->getId();
        message.path = c->getName();
        client->addOutputMessage( message );
    }
    lock.release();

    debug(1) << "Adding client " << client->getId() << endl;

    /* don't know the name of the client yet. the client will send
     * a CHANGE_NAME packet very soon.
     */
    addMessage(string("** Client ") + client->getName() + " joined", 0);

    Network::Message message;
    message << ADD_BUDDY;
    message << client->getId();
    message << client->getName();
    sendMessage(message, 0);

    lock.acquire();
    clients.push_back(client);
    lock.release();

    Resource::getSound(Filesystem::RelativePath("menu/sounds/chip-in.wav"))->play();
}

#if 0
static char lowerCase( const char * x ){
	if ( x[0] >= 'A' && x[0] <= 'Z' ){
		return x[0] - 'A' + 'a';
	}
	return x[0];
}
#endif

void ChatServer::sendMessageNow(const Network::Message & message, unsigned int id){
    lock.acquire();
    for (vector<Client *>::iterator it = clients.begin(); it != clients.end(); it++){
        Client * c = *it;
        if (c->getId() != id){
            message.send(c->getSocket());
        }
    }
    lock.release();
}

void ChatServer::sendMessage( const Network::Message & message, unsigned int id ){
    lock.acquire();
    for ( vector< Client * >::iterator it = clients.begin(); it != clients.end(); it++ ){
        Client * c = *it;
        if ( c->getId() != id ){
            c->addOutputMessage( message );
        }
    }
    lock.release();
}

void ChatServer::addMessage(const string & s, unsigned int id){
    Network::Message message;
    message << ADD_MESSAGE;
    message.path = s;
    lock.acquire();
    messages.addMessage(s);
    needUpdate();
    lock.release();

    sendMessage(message, id);
    /*
       for ( vector< Client * >::iterator it = clients.begin(); it != clients.end(); it++ ){
       Client * c = *it;
       if ( c->getId() != id ){
       c->addOutputMessage( message );
       }
       }
       */
}

void ChatServer::shutdownClientThreads(){
    debug(1) << "Shutting down client threads" << endl;

    /* end the output threads */
    lock.acquire();
    for ( vector< Client * >::iterator it = clients.begin(); it != clients.end(); it++ ){
        Client * c = *it;
        c->kill();
        Util::Thread::joinThread(c->getOutputThread());
    }
    lock.release();
    
    /* tell clients that the game is starting */
    Network::Message message;
    message << START_THE_GAME;
    sendMessageNow(message, 0);

    /* wait for clients to finish processing. clients should receive START_THE_GAME
     * then send back OK_TO_START and then finish.
     */
    for (vector< Client * >::iterator it = clients.begin(); it != clients.end(); it++){
        Client * c = *it;
        debug(1) << "Waiting for client " << c->getId() << " to finish input/output threads" << endl;
        Util::Thread::joinThread(c->getInputThread());
        debug(1) << "Input thread done for " << c->getId() << endl;
        // debug( 1 ) << "Output thread done for " << c->getId() << endl;
        debug(1) << "Client " << c->getId() << " is done" << endl;
    }

    debug(1) << "Shut down all clients" << endl;
}
	
vector<Client*> ChatServer::getConnectedClients(){
    vector<Client*> out;

    for (vector< Client * >::iterator it = clients.begin(); it != clients.end(); it++){
        Client * c = *it;
        if (c->isOk()){
            out.push_back(c);
        }
    }

    return out;
}

void ChatServer::killAllClients(){
    vector< Client * > all;
    lock.acquire();
    all = clients;
    lock.release();
    for ( vector< Client * >::iterator it = all.begin(); it != all.end(); it++ ){
        killClient( *it );
    }
}

void ChatServer::killClient(Client * c){
    int id = c->getId();
    string name = c->getName();
    lock.acquire();
    for ( vector< Client * >::iterator it = clients.begin(); it != clients.end(); ){
        Client * client = *it;
        if ( client == c ){
            debug(1) << "Killing client " << c->getId() << endl;
            c->kill();
            debug(1) << "Closing client socket " << c->getSocket() << endl;
            Network::close( c->getSocket() );
            /* It looks like the client that called killClient is waiting
             * for itself to exit but pthread_join won't block if the
             * argument is the same as the calling thread, so its ok.
             * TODO: ensure this semantics works with SDL threads too
             */
            debug(1) << "Waiting for input thread to die" << endl;
            Util::Thread::joinThread(c->getInputThread());
            debug(1) << "Waiting for output thread to die" << endl;
            Util::Thread::joinThread(c->getOutputThread());
            debug(1) << "Deleting client" << endl;
            /* delete can be moved to the input/output thread exit part
             * if need be.
             */
            delete client;
            it = clients.erase( it );
        } else {
            it++;
        }
    }
    needUpdate();
    lock.release();
    addMessage("** " + name + " quit", 0);
    Resource::getSound(Filesystem::RelativePath("menu/sounds/chip-out.wav"))->play();
    Network::Message remove;
    remove << REMOVE_BUDDY;
    remove << id;
    sendMessage(remove, 0);
}

bool ChatServer::logic(){
    /*
    const Font & font = Font::getDefaultFont(20, 20);
    lineEdit->act(font);
    if (lineEdit->didChanged(editCounter)){
        needUpdate();
    }
    */

#if 0
    if (keyboard[Keyboard::Key_TAB]){
        focus = nextFocus( focus );
        needUpdate();
    }

    keyInputManager::update();
    /*
       lineEdit->setFont(Menu::getFont()); lineEdit->logic();
       */

    lineEdit->setFocused(false);
    switch (focus){
        case INPUT_BOX : {
            lineEdit->setFocused(true);
            handleInput( keyboard );
            // handleInput( keyboard );
            lineEdit->colors.border = Bitmap::makeColor(255,255,0);
            break;
        }
        case START_GAME : {
            if ( keyboard[ Keyboard::Key_ENTER ] ){
                return true;
            }
            lineEdit->colors.border = Bitmap::makeColor(255,255,255);
            break;
        }
        case QUIT : {
            if ( keyboard[ Keyboard::Key_ENTER ] ){
                return true;
            }
            lineEdit->colors.border = Bitmap::makeColor(255,255,255);
            break;
        }
    }
#endif

    return false;
}
	
void ChatServer::needUpdate(){
    need_update = true;
}
	
Focus ChatServer::nextFocus(Focus f){
    switch (f){
        case INPUT_BOX: return START_GAME;
        case START_GAME: return QUIT;
        case QUIT: return INPUT_BOX;
        default : return INPUT_BOX;
    }
}

bool ChatServer::needToDraw(){
    return need_update;
}

/*
void ChatServer::drawInputBox(int x, int y, const Graphics::Bitmap & work){
    const Font & font = Font::getDefaultFont(20, 20 );

    // work.drawingMode( Bitmap::MODE_TRANS );
    Graphics::Bitmap::transBlender( 0, 0, 0, 128 );
    work.translucent().rectangleFill( x, y, x + messages.getWidth(), y + font.getHeight() + 1, Graphics::makeColor( 0, 0, 0 ) );
    // work.drawingMode( Bitmap::MODE_SOLID );
    int color = Graphics::makeColor(255, 255, 255);
    if (focus == INPUT_BOX){
        color = Graphics::makeColor(255, 255, 0);
    }
    work.rectangle(x, y, x + messages.getWidth(), y + font.getHeight(), color);
    Graphics::Bitmap input_box( work, x + 1, y, messages.getWidth(), font.getHeight() );
    // font.printf( x + 1, y, Bitmap::makeColor( 255, 255, 255 ), work, input, 0 );
    // font.printf( 0, 0, Bitmap::makeColor( 255, 255, 255 ), input_box, input, 0 );
}
*/

void ChatServer::drawBuddyList( int x, int y, const Graphics::Bitmap & work, const Font & font ){
    Graphics::Bitmap buddyList( work, x, y, work.getWidth() - x - 5, 200 );
    // buddyList.drawingMode( Bitmap::MODE_TRANS );
    Graphics::Bitmap::transBlender( 0, 0, 0, 128 );
    buddyList.translucent().rectangleFill( 0, 0, buddyList.getWidth(), buddyList.getHeight(), Graphics::makeColor( 0, 0, 0 ) );
    // buddyList.drawingMode( Bitmap::MODE_SOLID );
    buddyList.rectangle( 0, 0, buddyList.getWidth() -1, buddyList.getHeight() - 1, Graphics::makeColor( 255, 255, 255 ) );
    int fy = 1;
    for ( vector< Client * >::iterator it = clients.begin(); it != clients.end(); it++ ){
        Client * client = *it;
        const string & name = client->getName();
        font.printf( 1, fy, Graphics::makeColor( 255, 255, 255 ), buddyList, name, 0 );
        fy += font.getHeight();
    }
}
	
static Graphics::Color focusColor(Focus want, Focus now){
    if (want == now){
        return Graphics::makeColor(255, 255, 0);
    }
    return Graphics::makeColor(255, 255, 255);
}

void ChatServer::draw(const Graphics::Bitmap & work, Gui::LineEdit & lineEdit, Focus focus){
    int start_x = 20;
    int start_y = 20;
    const Font & font = Font::getDefaultFont(20, 20);
    background->Blit(work);
    messages.draw(start_x, start_y, work, font);

    // drawInputBox( start_x, start_y + messages.getHeight() + 5, work );

    drawBuddyList(start_x + messages.getWidth() + 10, start_y, work, font);

    font.printf(start_x, start_y + messages.getHeight() + 5 + font.getHeight() * 2 + 5, focusColor(START_GAME, focus), work, "Start the game", 0);
    font.printf(start_x + font.textLength("Start the game") + 20, start_y + messages.getHeight() + 5 + font.getHeight() * 2 + 5, focusColor(QUIT, focus), work, "Quit", 0);

    lineEdit.render(work);

    need_update = false;
}

void ChatServer::startThreadsHack(){
    lock.acquire();
    for (vector< Client * >::iterator it = clients.begin(); it != clients.end(); it++){
        Client * c = *it;
        c->startThreads();
    }
    lock.release();
}

static void set_to_true(void * b){
    bool * what = (bool *) b;
    *what = true;
}

/*
void ChatServer::next_focus(void * self){
    ChatServer * chat = (ChatServer*) self;
    chat->focus = chat->nextFocus(chat->focus);
    chat->lineEdit->setFocused(chat->focus == INPUT_BOX);
    if (chat->focus == INPUT_BOX){
        chat->lineEdit->colors.border = Graphics::makeColor(255,255,0);
    } else {
        chat->lineEdit->colors.border = Graphics::makeColor(255,255,255);
    }
    chat->needUpdate();
}
*/
	
void ChatServer::run(){
    // Global::speed_counter2 = 0;
    Util::Thread::createThread(&acceptThread, NULL, (Util::Thread::ThreadFunction) acceptConnections, this);

    Gui::LineEdit lineEdit;
    lineEdit.location.setPosition(Gui::AbsolutePoint(20, 20 + messages.getHeight() + 5));
    lineEdit.location.setDimensions(400, 30);
    lineEdit.transforms.setRadius(5);

    lineEdit.colors.body = Graphics::makeColor( 0, 0, 0 );
    lineEdit.colors.bodyAlpha = 128;
    lineEdit.colors.border = Graphics::makeColor( 255, 255, 0 );
    lineEdit.setHorizontalAlign(Gui::LineEdit::T_Left);
    lineEdit.setTextColor( Graphics::makeColor( 255, 255, 255 ) );

    lineEdit.setText(welcomeMessage());
    // lineEdit->setFont(Menu::getFont());
    lineEdit.setFont(& Font::getDefaultFont(20, 20));
    lineEdit.setFocused(true);

    class Logic: public Util::Logic {
    public:
        Logic(Gui::LineEdit & lineEdit, ChatServer & server, Focus & focus):
        editCounter(0),
        forceQuit(false),
        lineEdit(lineEdit),
        focus(focus),
        is_done(false),
        server(server){
            input.set(Keyboard::Key_TAB, 0, true, 0);
            input.set(Keyboard::Key_ENTER, 0, true, 1);
            input.set(Keyboard::Key_ESC, 0, true, 2);
            lineEdit.hookKey(Keyboard::Key_ENTER, enter_pressed, this);
            lineEdit.hookKey(Keyboard::Key_ESC, set_to_true, &forceQuit);
            lineEdit.hookKey(Keyboard::Key_TAB, next_focus, this);
        }

        InputMap<int> input;
        unsigned long long editCounter;
        bool forceQuit;
        Gui::LineEdit & lineEdit;
        Focus & focus;
        bool is_done;
        ChatServer & server;

        double ticks(double system){
            return system;
        }

        static void enter_pressed(void * self){
            Logic * logic = (Logic*) self;
            logic->server.addLine(logic->lineEdit);
        }

        static Focus nextFocus(Focus f){
            switch (f){
                case INPUT_BOX: return START_GAME;
                case START_GAME: return QUIT;
                case QUIT: return INPUT_BOX;
                default : return INPUT_BOX;
            }
        }

        static void next_focus(void * self){
            Logic * logic = (Logic*) self;
            logic->focus = nextFocus(logic->focus);
            logic->lineEdit.setFocused(logic->focus == INPUT_BOX);
            if (logic->focus == INPUT_BOX){
                logic->lineEdit.colors.border = Graphics::makeColor(255,255,0);
            } else {
                logic->lineEdit.colors.border = Graphics::makeColor(255,255,255);
            }
            logic->server.needUpdate();
        }

        void startGame(){
            server.stopAccepting();
            debug(1) << "Shut down client threads" << endl;
            server.shutdownClientThreads();
            debug(1) << "Finished shutting things down" << endl;
            is_done = true;
        }

        void doQuit(){
            server.addMessage("** Server quit", 0);
            server.stopAccepting();
            server.killAllClients();
            is_done = true;
            throw Exception::Return(__FILE__, __LINE__);
        }

        void handleInput(bool inputHadFocus){
            vector<InputMap<int>::InputEvent> events = InputManager::getEvents(input, InputSource(true));
            bool select = false;
            bool next = false;
            bool quit = false;
            for (vector<InputMap<int>::InputEvent>::iterator it = events.begin(); it != events.end(); it++){
                const InputMap<int>::InputEvent & event = *it;
                if (!event.enabled){
                    continue;
                }
                next = next || event[0];
                select = select || event[1];
                quit = quit || event[2];
            }

            if (forceQuit || (is_done && focus == QUIT) || quit){
                doQuit();
            }

            if (next && !inputHadFocus){
                next_focus(this);
            }

            if (select){
                switch (focus){
                    case START_GAME: {
                        startGame();
                        break;
                    }
                    case QUIT: {
                        doQuit();
                        break;
                    }
                    case INPUT_BOX: {
                        Global::debug(0) << "Shouldn't get here" << endl;
                        throw Exception::Return(__FILE__, __LINE__);
                    }
                }
            }
        }

        void run(){
            server.startThreadsHack();

            /* hack to know if the input box had the focus so we don't register
             * the TAB key too many times.
             * if the input box has focus and tab is pressed, the next_focus()
             * function will be executed twice, once due to the input box
             * and once due to the loop below.
             */
            bool inputHadFocus = focus == INPUT_BOX;

            const Font & font = Font::getDefaultFont(20, 20);
            lineEdit.act(font);
            if (lineEdit.didChanged(editCounter)){
                server.needUpdate();
            }

            handleInput(inputHadFocus);
        }

        bool done(){
            return is_done;
        }
    };

    class Draw: public Util::Draw {
    public:
        Draw(Gui::LineEdit & lineEdit, ChatServer & server, Focus & focus):
        lineEdit(lineEdit),
        server(server),
        focus(focus){
        }

        Gui::LineEdit & lineEdit;
        ChatServer & server;
        Focus & focus;

        void draw(const Graphics::Bitmap & buffer){
            if (server.needToDraw()){
                Graphics::StretchedBitmap work(640, 480, buffer);
                work.start();
                server.draw(work, lineEdit, focus);
                work.finish();
                buffer.BlitToScreen();
            }
        }
    };

    Focus focus(INPUT_BOX);
    Logic logic(lineEdit, *this, focus);
    Draw draw(lineEdit, *this, focus);
            
    needUpdate();
    Util::standardLoop(logic, draw);

#if 0
    class Release{
    public:
        Release(Gui::LineEdit * line):
        line(line){
        }

        Gui::LineEdit * line;

        ~Release(){
            line->setFocused(false);
        }
    };

    Release release(lineEdit);

    InputMap<int> input;
    input.set(Keyboard::Key_TAB, 0, true, 0);
    input.set(Keyboard::Key_ENTER, 0, true, 1);
    input.set(Keyboard::Key_ESC, 0, true, 2);
    bool forceQuit = false;
    lineEdit->hookKey(Keyboard::Key_ESC, set_to_true, &forceQuit);
    lineEdit->hookKey(Keyboard::Key_TAB, ChatServer::next_focus, this);
    bool done = false;
    while (! done){
        int think = Global::speed_counter2;
        Global::speed_counter2 = 0;
        while (think > 0){
            InputManager::poll();
            /* I forgot why this hack needs to be put inside the main loop.
             * When I remember please explain it here.
             * Possible reason: the client thread can't be started as soon
             * as the client connects (why?) but has to be started at some point.
             * Putting the hack here ensures that the client thread starts at some
             * point but after the server has initialized whatever it needs.
             */
            startThreadsHack();

            /* hack to know if the input box had the focus so we don't register
             * the TAB key too many times.
             * if the input box has focus and tab is pressed, the next_focus()
             * function will be executed twice, once due to the input box
             * and once due to the loop below.
             */
            bool inputHadFocus = focus == INPUT_BOX;

            done = logic();
            think -= 1;
            
            vector<InputMap<int>::InputEvent> events = InputManager::getEvents(input);
            bool select = false;
            bool next = false;
            bool quit = false;
            for (vector<InputMap<int>::InputEvent>::iterator it = events.begin(); it != events.end(); it++){
                const InputMap<int>::InputEvent & event = *it;
                if (!event.enabled){
                    continue;
                }
                next = next || event[0];
                select = select || event[1];
                quit = quit || event[2];
            }

            if (forceQuit || (done && focus == QUIT) || quit){
                addMessage( "** Server quit", 0 );
                stopAccepting();
                killAllClients();
                done = true;
                throw Exception::Return(__FILE__, __LINE__);
            }

            if (next && !inputHadFocus){
                next_focus(this);
            }

            if (select){
                switch (focus){
                    case START_GAME: {
                        stopAccepting();
                        debug(1) << "Shut down client threads" << endl;
                        shutdownClientThreads();
                        debug(1) << "Finished shutting things down. Done is " << done << endl;
                        done = true;
                        debug(1) << "Done is " << done << endl;
                        break;
                    }
                    case QUIT: {
                        addMessage( "** Server quit", 0 );
                        stopAccepting();
                        killAllClients();
                        done = true;
                        throw Exception::Return(__FILE__, __LINE__);
                    }
                    case INPUT_BOX: {
                        Global::debug(0) << "Shouldn't get here" << endl;
                        throw Exception::Return(__FILE__, __LINE__);
                    }
                }
            }
        }

        if (needToDraw()){
            draw(work);
            work.BlitToScreen();
            work.clear();
        }

        while (Global::speed_counter2 == 0){
            Util::rest(1);
        }
    }
#endif

    debug(1) << "Chat server done" << endl;
}
	
ChatServer::~ChatServer(){
    // delete background;
    for (vector<Client *>::iterator it = clients.begin(); it != clients.end(); it++){
        delete *it;
    }
    // delete lineEdit;
}

#endif
