#ifdef HAVE_NETWORKING

#include "util/bitmap.h"
#include "../object/object.h"
#include "../object/character.h"
#include "../game/adventure_world.h"
#include "network_world.h"
#include "util/network/network.h"
#include "util/funcs.h"
#include "util/system.h"
#include "../level/scene.h"
#include "../level/cacher.h"
#include "../script/script.h"
#include "util/file-system.h"
#include "util/input/input-manager.h"
#include "util/font.h"
#include "factory/font_render.h"
#include "globals.h"
#include <sstream>

using namespace std;

typedef AdventureWorld super;

struct Stuff{
    Network::Socket socket;
    NetworkWorld * world;
};

static std::ostream & debug( int level ){
    return Global::debug(level, "network-world");
}

static void * handleMessages( void * arg ){
    Stuff * s = (Stuff *) arg;
    Network::Socket socket = s->socket;
    NetworkWorld * world = s->world;

    unsigned int id = 1;
    try{
        while (world->isRunning()){
            Network::Message m(socket);
            ostringstream context;
            context << __FILE__ << " " << System::currentMilliseconds();
            Global::debug(2, context.str()) << "Received message " << id << " with path '" << m.path << "'" << endl;
            id += 1;
            world->addIncomingMessage(m, socket);
            // debug(2, __FILE__) << "Received path '" << m.path << "'" << endl;
        }
    } catch (const Network::MessageEnd & end){
        debug(1) << "Closed connection with socket " << socket << endl;
    } catch ( const Network::NetworkException & ne ){
        debug(0) << "Network exception: " << ne.getMessage() << endl;
    }

    delete s;

    return NULL;
}

NetworkWorld::NetworkWorld(vector< Network::Socket > & sockets, const vector< Paintown::Object * > & players, const map<Paintown::Object*, Network::Socket> & characterToClient, const Filesystem::AbsolutePath & path, const map<Paintown::Object::networkid_t, string> & clientNames, int screen_size ) throw ( LoadException ):
AdventureWorld( players, path, new Level::DefaultCacher(), screen_size ),
ChatWidget(*this, 0),
sockets(sockets),
clientNames(clientNames),
characterToClient(characterToClient),
sent_messages( 0 ),
running(true){
    Paintown::Object::networkid_t max_id = 0;

    Util::Thread::initializeLock(&message_mutex);
    Util::Thread::initializeLock(&running_mutex);

    for ( vector< PlayerTracker >::iterator it = this->players.begin(); it != this->players.end(); it++ ){
        Paintown::Object * object = it->player;
        addMessage( object->getCreateMessage() );
        if ( object->getId() > max_id ){
            max_id = object->getId();
        }
    }
    this->id = max_id + 1;
}

void NetworkWorld::startMessageHandlers(){
    for ( vector<Network::Socket>::const_iterator it = sockets.begin(); it != sockets.end(); it++ ){
        Stuff * s = new Stuff;
        s->socket = *it;
        s->world = this;
        Util::Thread::Id thread;
        Util::Thread::createThread( &thread, NULL, (Util::Thread::ThreadFunction) handleMessages, s );
        threads.push_back( thread );
    }
}

void NetworkWorld::waitForHandlers(){
    for (vector<Util::Thread::Id>::iterator it = threads.begin(); it != threads.end(); it++){
        Util::Thread::Id & thread = *it;
        Util::Thread::joinThread(thread);
    }
}

NetworkWorld::~NetworkWorld(){
    stopRunning();
}
	
void NetworkWorld::addObject( Paintown::Object * o ){
    if ( o->getId() != (Paintown::Object::networkid_t) -1 ){
        for ( vector< Paintown::Object * >::iterator it = objects.begin(); it != objects.end(); it++ ){
            Paintown::Object * obj = *it;
            if ( obj->getId() == o->getId() ){
                return;
            }
        }
    }

    AdventureWorld::addObject( o );
}
	
void NetworkWorld::addMessage( Network::Message m, Network::Socket from, Network::Socket to){
    if (m.id == (uint32_t) -1){
        ostringstream out;
        out << "Message not properly formed: ";
        for ( int i = 0; i < Network::DATA_SIZE; i++ ){
            out << (int) m.data[ i ] << " ";
        }
        throw Network::NetworkException(out.str());
    }
    Util::Thread::acquireLock( &message_mutex );
    /* if the server generates a message `from' will probably be 0.
     * if the message should go to a specific client `to' will be non-zero.
     */
    Packet p(m, from, to);
    outgoing.push_back( p );
    Util::Thread::releaseLock( &message_mutex );
}

void NetworkWorld::addIncomingMessage( const Network::Message & message, Network::Socket from ){
    Util::Thread::acquireLock( &message_mutex );
    incoming.push_back(message);
    Util::Thread::releaseLock( &message_mutex );

    /* by default all messages get relayed to all clients, but a client
     * shouldn't send its own message back to itself so it populates the
     * `from' field.
     */
    addMessage( message, from );
}
	
void NetworkWorld::stopRunning(){
    Util::Thread::acquireLock( &running_mutex );
    running = false;
    Util::Thread::releaseLock( &running_mutex );
}

bool NetworkWorld::isRunning(){
    Util::Thread::acquireLock( &running_mutex );
    bool b = running;
    Util::Thread::releaseLock( &running_mutex );
    return b;
}

/* what is this method for? */
void NetworkWorld::sendMessage( const Network::Message & message, Network::Socket socket ){
	// message.send( socket );
}
	
Network::Message NetworkWorld::finishMessage(){
    Network::Message message;
    message.id = 0;
    message << FINISH;
    return message;
}

Network::Message NetworkWorld::nextBlockMessage( int block ){
    Network::Message message;

    message.id = 0;
    message << NEXT_BLOCK;
    message << block;

    return message;
}

static Network::Message pausedMessage(){
    Network::Message message;
    message.id = 0;
    message << World::PAUSE;

    return message;
}

static Network::Message unpausedMessage(){
    Network::Message message;
    message.id = 0;
    message << World::UNPAUSE;

    return message;
}

void NetworkWorld::changePause(){
    AdventureWorld::changePause();
    if (isPaused()){
        addMessage(pausedMessage());
    } else {
        addMessage(unpausedMessage());
    }
}

void NetworkWorld::doScene( int min_x, int max_x ){
    vector< Paintown::Object * > obj;
    int block = scene->getBlock();
    scene->act(min_x, max_x, &obj);
    if (scene->getBlock() != block){
        addMessage(nextBlockMessage(scene->getBlock()));
    }

    for (vector<Paintown::Object *>::iterator it = obj.begin(); it != obj.end(); it++){
        Paintown::Object * m = *it;
        m->setId(nextId());
        addMessage(m->getCreateMessage());
        addMessage(m->movedMessage());
    }

    objects.insert(objects.end(), obj.begin(), obj.end());
}

Paintown::Object * NetworkWorld::findNetworkObject( Paintown::Object::networkid_t id ){
    for ( vector< Paintown::Object * >::iterator it = objects.begin(); it != objects.end(); it++ ){
        Paintown::Object * o = *it;
        if ( o->getId() == id ){
            return o;
        }
    }
    return NULL;
}

void NetworkWorld::handlePing(Network::Message & message){
    Network::Message out;
    unsigned int ping_id;
    message >> ping_id;
    out.id = 0;
    out << World::PING_REPLY;
    out << ping_id;
    Global::debug(1) << "Replying to ping " << ping_id << " from " << message.readFrom << endl;
    /* only send ping reply to the client that asked for it */
    addMessage(out, 0, message.readFrom);
}

void NetworkWorld::removePlayer(Paintown::Object * player){
    for ( vector< PlayerTracker >::iterator it = players.begin(); it != players.end(); ){
        PlayerTracker & tracker = *it;
        if (tracker.player == player){
            void * handle = tracker.script;
            if (handle != NULL){
                Script::Engine::getEngine()->destroyObject(handle);
            }
            tracker.player->setScriptObject(NULL);
            
            it = players.erase(it);
        } else {
            it++;
        }
    }

    for ( vector< Paintown::Object * >::iterator it = objects.begin(); it != objects.end(); ){
        Paintown::Object * o = *it;
        if (o->getId() == player->getId()){
            it = objects.erase(it);
        } else {
            it++;
        }
    }
}

void NetworkWorld::removeSocket(Network::Socket socket){
    for (vector<Network::Socket>::iterator it = sockets.begin(); it != sockets.end(); ){
        if (socket == *it){
            it = sockets.erase(it);
        } else {
            it++;
        }
    }
}

Paintown::Object * NetworkWorld::findPlayerFromSocket(Network::Socket socket){
    for (map<Paintown::Object*, Network::Socket>::iterator it = characterToClient.begin(); it != characterToClient.end(); it++){
        Paintown::Object * who = (*it).first;
        Network::Socket search = (*it).second;
        if (search == socket){
            return who;
        }
    }
    return NULL;
}
	
void NetworkWorld::handleMessage( Network::Message & message ){
    if ( message.id == 0 ){
        int type;
        message >> type;
        switch (type){
            case GRAB : {
                Paintown::Object::networkid_t grabbing;
                Paintown::Object::networkid_t grabbed;
                message >> grabbing;
                message >> grabbed;
                Paintown::Character * c_grabbing = (Paintown::Character *) findNetworkObject( grabbing );
                Paintown::Character * c_grabbed = (Paintown::Character *) findNetworkObject( grabbed );
                if ( c_grabbing != NULL && c_grabbed != NULL ){
                    c_grabbed->grabbed( c_grabbing );
                    c_grabbing->setLink( c_grabbed );
                }
                break;
            }
            case CHAT : {
                Paintown::Object::networkid_t him;
                message >> him;
                string name = clientNames[him];
                ChatWidget::receiveMessage(name + ": " + message.path);
                break;
            }
            case THROWN : {
                Paintown::Object::networkid_t grabbing;
                Paintown::Object::networkid_t grabbed;
                message >> grabbing;
                message >> grabbed;
                Paintown::Character * c_grabbing = (Paintown::Character *) findNetworkObject( grabbing );
                Paintown::Character * c_grabbed = (Paintown::Character *) findNetworkObject( grabbed );
                if ( c_grabbed != NULL && c_grabbing != NULL ){
                    debug( 2 ) << grabbing << " threw " << grabbed << endl;
                    c_grabbed->setFacing( c_grabbing->getOppositeFacing() );
                    c_grabbed->thrown();
                    addMessage( c_grabbed->movedMessage() );
                    /* don't hardcode the fall numbers */
                    addMessage( c_grabbed->fallMessage( 3.2, 5.0 ) );
                    c_grabbed->fall( 3.2, 5.0 );
                    c_grabbing->setStatus( Paintown::Status_Ground );
                    debug( 2 ) << grabbed << " status is " << c_grabbed->getStatus() << endl;
                }
                break;
            }
            case QUIT : {
                /* when a client quits we have to
                 * 1. delete that player from existence, send DELETE_OBJ
                 * 2. stop the socket input threads 
                 * 3. remove the socket from our list of clients
                 * The thread will die on its own so we don't need to kill it.
                 */
                Paintown::Object * player = findPlayerFromSocket(message.readFrom);
                addMessage(deleteMessage(player->getId()));
                removePlayer(player);
                Network::close(message.readFrom);
                removeSocket(message.readFrom);

                /* TODO: add a warning message to the user
                 * that a client just quit.
                 */
                break;
            }
            case PING_REQUEST : {
                handlePing(message);
                break;
            }
            case PAUSE : {
                this->pause();

                /* I dont think I need to send another pause message
                 * because all client messages get sent to each other
                 * anyway.
                 */
                addMessage(pausedMessage());
                break;
            }
            case UNPAUSE : {
                this->unpause();
                addMessage(unpausedMessage());
                break;
            }
            case RequestName: {
                Paintown::Object::networkid_t id;
                message >> id;
                Paintown::Character * who = (Paintown::Character *) findNetworkObject(id);
                if (who != NULL){
                    addMessage(who->nameMessage(), 0, message.readFrom);
                }
                break;
            }
        }
    } else {
        Paintown::Object * o = findNetworkObject(message.id);
        if (o != NULL){
            o->interpretMessage(this, message);
        } else {
            Global::debug(0) << "Ignoring message sent to id " << message.id << endl;
        }
    }
}

vector< Network::Message > NetworkWorld::getIncomingMessages(){
    vector< Network::Message > m;
    Util::Thread::acquireLock(&message_mutex);
    m = incoming;
    incoming.clear();
    Util::Thread::releaseLock(&message_mutex);
    return m;
}

void NetworkWorld::flushOutgoing(){
    vector< Packet > packets;
    Util::Thread::acquireLock(&message_mutex);
    packets = outgoing;
    outgoing.clear();
    Util::Thread::releaseLock(&message_mutex);

    for (vector<Network::Socket>::iterator socket = sockets.begin(); socket != sockets.end(); socket++){
        vector<Network::Message*> messages;
        for ( vector< Packet >::iterator it = packets.begin(); it != packets.end(); it++ ){
            Network::Message & message = (*it).message;
            Network::Socket from = (*it).socket;
            Network::Socket to = (*it).to;

            /* send the socket if
             * 1. the sender is not the same as the receiver and
             * 2. a) either the receiver is not specified or
             *    b) the receiver is the right one
             */
            if (from != *socket && (to == 0 || to == *socket)){
                messages.push_back(&message);
            }
        }

        Network::sendAllMessages(messages, *socket);
    }
}
	
void NetworkWorld::draw(Graphics::Bitmap * work){
    super::draw(work);
    ChatWidget::drawChat(work, work->getHeight() * 2 - 1);
}
	
void NetworkWorld::act(){
    AdventureWorld::act();
    ChatWidget::act();

    vector<Network::Message> messages = getIncomingMessages();
    for (vector< Network::Message >::iterator it = messages.begin(); it != messages.end(); it++){
        handleMessage(*it);
    }

    flushOutgoing();
}

#endif
