#ifdef HAVE_NETWORKING

#include "util/bitmap.h"
#include "../game/adventure_world.h"
#include "network_world_client.h"
#include "util/network/network.h"
#include "../level/scene.h"
#include "globals.h"
#include "../script/script.h"
#include "util/init.h"
#include "util/font.h"
#include "factory/font_render.h"
#include "../level/blockobject.h"
#include "util/funcs.h"
#include "util/file-system.h"
#include "../level/cacher.h"
#include "../object/object.h"
#include "../object/player.h"
#include "../factory/object_factory.h"
#include <string.h>
#include "util/gradient.h"
#include "util/system.h"
#include "cacher.h"
#include "util/input/input-manager.h"
#include <sstream>

#include "../object/character.h"
#include "../object/cat.h"
#include "../object/item.h"

using namespace std;

/* use java-style OOP */
typedef AdventureWorld super;

static std::ostream & debug( int level ){
    return Global::debug(level, "network-world-client");
}

static void * handleMessages(void * arg){
    NetworkWorldClient * world = (NetworkWorldClient *) arg;
    Network::Socket socket = world->getServer();

    /* at 100 messages per second (which is more than normal)
     * this can count 1.36 years worth of messages.
     * a reasonable rate is probably 10 messages per second which
     * gives 13.6 years worth of messages.
     */
    unsigned int received = 0;

    try{
        while (world->isRunning()){
            received += 1;
            {
                ostringstream context;
                context << __FILE__ << " " << (System::currentMicroseconds() / 1000);
                Global::debug(2, context.str()) << "Receiving message " << received << endl;
            }
            Network::Message m(socket);
            world->addIncomingMessage(m);

            {
                ostringstream context;
                context << __FILE__ << " " << (System::currentMicroseconds() / 1000);
                Global::debug(2, context.str()) << "Received path '" << m.path << "'" << endl;
            }
        }
    } catch (const Network::MessageEnd & end){
        debug(1) << "Closed connection with socket " << socket << endl;
    } catch (const Network::NetworkException & n){
        debug(0) << "Network exception: " << n.getMessage() << endl;
    }

    debug(1) << "Client input stopped" << endl;

    return NULL;
}
	
NetworkWorldClient::NetworkWorldClient( Network::Socket server, const std::vector< Paintown::Object * > & players, const Filesystem::AbsolutePath & path, Paintown::Object::networkid_t id, const map<Paintown::Object::networkid_t, string> & clientNames, int screen_size ) throw ( LoadException ):
super( players, path, new NetworkCacher(), screen_size ),
ChatWidget(*this, id),
server( server ),
world_finished( false ),
secondCounter(Global::second_counter),
id(id),
running(true),
currentPing(0),
clientNames(clientNames),
pingCounter(0){
    objects.clear();
    Util::Thread::initializeLock(&message_mutex);
    Util::Thread::initializeLock(&running_mutex);
}

void NetworkWorldClient::startMessageHandler(){
    Util::Thread::createThread( &message_thread, NULL, (Util::Thread::ThreadFunction) handleMessages, this );
}
	
NetworkWorldClient::~NetworkWorldClient(){
    debug( 1 ) << "Destroy client world" << endl;
}
	
bool NetworkWorldClient::isRunning(){
    Util::Thread::acquireLock( &running_mutex );
    bool b = running;
    Util::Thread::releaseLock( &running_mutex );
    return b;
}

void NetworkWorldClient::stopRunning(){
    Util::Thread::acquireLock( &running_mutex );
    running = false;
    Util::Thread::releaseLock( &running_mutex );
    Network::Message finish;
    finish << World::FINISH;
    finish.id = 0;
    finish.send(getServer());
    debug(1) << "Sent finish, waiting for message thread to end." << endl;
    Util::Thread::joinThread(message_thread);
}
	
void NetworkWorldClient::addIncomingMessage( const Network::Message & message ){
    Util::Thread::acquireLock( &message_mutex );
    incoming.push_back( message );
    Util::Thread::releaseLock( &message_mutex );
}
	
void NetworkWorldClient::getIncomingMessages(vector<Network::Message> & messages){
    // vector< Network::Message > m;
    Util::Thread::acquireLock( &message_mutex );
    messages = incoming;
    incoming.clear();
    Util::Thread::releaseLock( &message_mutex );
    // return m;
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

void NetworkWorldClient::changePause(){
    super::changePause();
    if (isPaused()){
        addMessage(pausedMessage());
    } else {
        addMessage(unpausedMessage());
    }
}

bool NetworkWorldClient::uniqueObject( Paintown::Object::networkid_t id ){
    for ( vector< Paintown::Object * >::iterator it = objects.begin(); it != objects.end(); it++ ){
        Paintown::Object * o = *it;
        if ( o->getId() == id ){
            return false;
        }
    }
    return true;
}

void NetworkWorldClient::handleCreateCharacter( Network::Message & message ){
    int alliance = 0;
    Paintown::Object::networkid_t id = 0;
    int map = 0;
    Filesystem::AbsolutePath path = Storage::instance().find(Filesystem::RelativePath(message.path));
    message >> alliance >> id >> map;
    if (uniqueObject(id)){
        bool found = false;
        for (vector<PlayerTracker>::iterator it = players.begin(); it != players.end(); it++ ){
            Paintown::Character * character = (Paintown::Character *) it->player;
            if (character->getId() == id){
                character->deathReset();
                addObject(character);
                found = true;
                break;
            }

        }
        if ( ! found ){
            /* TODO: set the block id (different from network id) */
            BlockObject block;
            int isPlayer;
            message >> isPlayer;
            if (isPlayer == World::IS_PLAYER){
                block.setType(ObjectFactory::NetworkPlayerType);
            } else {
                block.setType(ObjectFactory::NetworkCharacterType);
            }
            block.setMap(map);
            block.setPath(path);
            Paintown::Character * character = (Paintown::Character *) ObjectFactory::createObject( &block );
            if (character == NULL){
                debug(0) << "Could not create character!" << endl;
                return;
            }
            debug(1) << "Create '" << path.path() << "' with id " << id << " alliance " << alliance << endl;
            /* this is the network id */
            character->setId(id);
            character->setAlliance(alliance);
            /* TODO: should these values be hard-coded?
             * for now it doesn't matter becuase the server is going to send
             * a move message immediately after the create message
             */
            character->setX(200);
            character->setY(0);
            character->setZ(150);
            addObject(character);
        }
    } else {
        debug(1) << id << " is not unique" << endl;
    }
}

void NetworkWorldClient::handleCreateCat( Network::Message & message ){
    Paintown::Object::networkid_t id;
    message >> id;
    if ( uniqueObject( id ) ){
        Filesystem::AbsolutePath path = Storage::instance().find(Filesystem::RelativePath(message.path));
        BlockObject block;
        block.setType( ObjectFactory::CatType );
        block.setPath( path );
        /* TODO: should these values be hard-coded? */
        block.setCoords( 200, 150 );
        Paintown::Cat * cat = (Paintown::Cat *) ObjectFactory::createObject( &block );
        if ( cat == NULL ){
            debug( 0 ) << "Could not create cat" << endl;
            return;
        }

        cat->setY( 0 );
        cat->setId( (unsigned int) -1 );
        addObject( cat );
    }
}
	
bool NetworkWorldClient::finished() const {
    return world_finished;
}

Paintown::Object * NetworkWorldClient::removeObject( Paintown::Object::networkid_t id ){
    for ( vector< Paintown::Object * >::iterator it = objects.begin(); it != objects.end(); ){
        Paintown::Object * o = *it;
        if ( o->getId() == id ){
            it = objects.erase( it );
            return o;
        } else {
            it++;
        }
    }
    return NULL;
}

void NetworkWorldClient::handleCreateItem( Network::Message & message ){
    int id;
    message >> id;
    if ( uniqueObject( id ) ){
        int x, z;
        int value;
        message >> x >> z >> value;
        Filesystem::AbsolutePath path = Storage::instance().find(Filesystem::RelativePath(message.path));
        BlockObject block;
        block.setType(ObjectFactory::ItemType);
        block.setPath(path);
        /* TODO: dont hard-code this */
        block.setStimulationType( "health" );
        block.setStimulationValue( value );
        block.setCoords( x, z );
        Paintown::Item * item = (Paintown::Item *) ObjectFactory::createObject( &block );
        if ( item == NULL ){
            debug( 0 ) << "Could not create item" << endl;
            return;
        }

        item->setY( 0 );
        item->setId( id );
        addObject(item);
    }
}

void NetworkWorldClient::handleCreateBang( Network::Message & message ){
    int x, y, z;
    message >> x >> y >> z;
    Paintown::Object * addx = bang->copy();
    addx->setX( x );
    addx->setY( 0 );
    addx->setZ( y+addx->getHeight()/2 );
    addx->setHealth( 1 );
    addx->setId( (unsigned int) -1 );
    addObject( addx );
}
        
bool NetworkWorldClient::respawnPlayers(const std::vector<Paintown::Object*> & players){
    return true;
}

Paintown::Object * NetworkWorldClient::findNetworkObject( Paintown::Object::networkid_t id ){
    for ( vector< Paintown::Object * >::iterator it = objects.begin(); it != objects.end(); it++ ){
        Paintown::Object * o = *it;
        if ( o->getId() == id ){
            return o;
        }
    }
    return NULL;
}

/* TODO: this code is duplicated in game/adventure_world.cpp and network_world.cpp.
 * try to abstract it out at some point.
 */
void NetworkWorldClient::removePlayer(Paintown::Object * player){
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
}

void NetworkWorldClient::handlePing(Network::Message & message){
    unsigned int id;
    message >> id;
    if (pings.find(id) != pings.end()){
        uint64_t drift = (message.timestamp - pings[id]);

        /* exponential moving average
         * http://en.wikipedia.org/wiki/Moving_average#Exponential_moving_average
         */
        /* tested values of alpha
         *   0.25 - ok
         *   0.3 - ok
         */
        double alpha = 0.3;
        /* divide by 2 because drift is the time from client->server->client.
         * we want just client->server.
         * this assumes the time between client->server and server->client
         * is the same, which is probably a reasonable assumption.
         */
        currentPing = alpha * drift / 2 + (1.0 - alpha) * currentPing;

        Global::debug(1) << "Ping " << id << ": " << drift << " average ping: " << currentPing << endl;

        /* dont fill up the ping table; save memory */
        pings.erase(id);
    } else {
        Global::debug(0) << "Unknown ping reply: " << id << endl;
    }
}
	
/* messages are defined in ../world.h */
void NetworkWorldClient::handleMessage( Network::Message & message ){
    Global::debug(2) << "Handle message for id " << message.id << endl;
    if ( message.id == 0 ){
        int type;
        message >> type;
        Global::debug(2) << "Message type " << type << endl;
        switch ( type ){
            case CREATE_CHARACTER : {
                handleCreateCharacter( message );
                break;
            }
            case CREATE_CAT : {
                handleCreateCat( message );	
                break;
            }
            case CREATE_BANG : {
                handleCreateBang( message );
                break;
            }
            case CREATE_ITEM : {
                handleCreateItem( message );
                break;
            }
            case NEXT_BLOCK : {
                int block;
                message >> block;
                scene->advanceBlocks( block );
                break;
            }
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
            case DELETE_OBJ : {
                Paintown::Object::networkid_t id;
                message >> id;
                Paintown::Object * o = removeObject( id );

                if (isPlayer(o)){
                    removePlayer(o);
                } else {
                    if ( o != NULL ){
                        delete o;
                    }
                }
                break;
            }
            case CHAT : {
                Paintown::Object::networkid_t him;
                message >> him;
                string name = clientNames[him];
                if (him == 0){
                    name = "Server";
                }
                ChatWidget::receiveMessage(name + ": " + message.path);
                break;
            }
            case IGNORE_MESSAGE : {
                break;
            }
            case REMOVE : {
                Paintown::Object::networkid_t id;
                message >> id;
                removeObject( id );
                break;
            }
            case FINISH : {

                debug( 1 ) << "Received finish message" << endl;
                world_finished = true;
                break;
            }
            case NOTHING : {
                debug( 0 ) << "Invalid message. Data dump" << endl;
                for ( int i = 0; i < Network::DATA_SIZE; i++ ){
                    debug( 0 ) << (int) message.data[ i ] << " ";
                }
                debug( 0 ) << endl;
                break;
            }
            case PING_REPLY : {
                handlePing(message);
                break;
            }
            case PAUSE : {
                this->pause();
                break;
            }
            case UNPAUSE : {
                this->unpause();
                break;
            }
        }
    } else {
        for ( vector< Paintown::Object * >::iterator it = objects.begin(); it != objects.end(); it++ ){
            Paintown::Object * o = *it;
            /* message.id is a uint16_t and getId() is an unsigned long */
            if ( o->getId() == message.id ){
                o->interpretMessage(this, message );
            }
        }
    }
}

void NetworkWorldClient::addMessage( Network::Message m, Network::Socket from, Network::Socket to ){
    if (m.id == (uint32_t) -1){
        ostringstream out;
        out << "Message not properly formed: ";
        for ( int i = 0; i < Network::DATA_SIZE; i++ ){
            out << (int) m.data[ i ] << " ";
        }
        throw Network::NetworkException(out.str());
    }
    /* clients should only send updates about their own character to the server.
     * all other movement (like from enemies) should be generated by the server.
     */
    if (m.id == id || m.id == 0){
        debug(2) << "Sending message to id " << m.id << endl;
        outgoing.push_back(m);
    }
}
	
void NetworkWorldClient::doScene( int min_x, int max_x ){
    scene->act(min_x, max_x, 0);
#if 0
    vector< Object * > objs;
    scene->act( min_x, max_x,  &objs );

    /* throw out everything the scene just made because the server is
     * going to tell us which objects/characters to make
     */
    for ( vector< Object * >::iterator it = objs.begin(); it != objs.end(); it++ ){
        delete *it;
    }
#endif
}

Network::Message NetworkWorldClient::pingMessage(unsigned int pingId){
    Network::Message message;
    message.id = 0;
    message << World::PING_REQUEST;
    message << pingId;
    pings[pingId] = System::currentMicroseconds();

    Global::debug(1) << "Sending ping request " << pingId << endl;

    /* pings won't fill up unless the server dies or something, so
     * as a fail-safe don't let the table get too big
     */
    if (pings.size() > 1000){
        Global::debug(0, __FILE__) << "Warning: ping table is full. Is the server or network dead?" << endl;
        pings.clear();
    }

    return message;
}

void NetworkWorldClient::sendMessage( const Network::Message & message, Network::Socket socket ){
	message.send( socket );
}

void NetworkWorldClient::sendMessages(const vector<Network::Message> & messages, Network::Socket socket){
    Network::sendAllMessages(messages, socket);
    /*
    int length = Network::totalSize(messages);
    uint8_t * data = new uint8_t[length];
    Network::dump(messages, data);
    Network::sendBytes(socket, data, length);
    delete[] data;
    */
}
	
void NetworkWorldClient::draw(Graphics::Bitmap * work){
    super::draw(work);
    const Font & font = Font::getFont(Global::DEFAULT_FONT, 15, 15);
    FontRender * render = FontRender::getInstance();

    /* the coordinates are only right becuase I know that the screen is twice as
     * large as `work'. FontRender should be changed so that we don't have to know
     * its coordinate system, something like opengl where -1 is the top and 1
     * is the bottom.
     */
    render->addMessage(font, 1, work->getHeight() * 2 - font.getHeight() - 1, Graphics::makeColor(255, 255, 255), Graphics::MaskColor(), "Ping %d", (int) (currentPing / 1000));
    // font.printf(1, work->getHeight() - 11, Bitmap::makeColor( 255, 255, 255 ), *work, "Ping %d", 0, (int) (currentPing / 1000));

    ChatWidget::drawChat(work, work->getHeight() * 2 - 1 - font.getHeight() - 1);
}

/* does NOT call AdventureWorld::act() */
void NetworkWorldClient::act(){
	
    /* these things are copy/pasted from AdventureWorld */
    if ( quake_time > 0 ){
        quake_time--;
    }

    if (descriptionTime > 0){
        descriptionTime -= 1;
        descriptionGradient->update();
    }
    /* end copy/paste */

    ChatWidget::act();

    vector< Paintown::Object * > added_effects;
    if (! isPaused()){

        for ( vector< Paintown::Object * >::iterator it = objects.begin(); it != objects.end(); it++ ){
            Paintown::Object * o = *it;
            o->act( &objects, this, &added_effects );
            if ( o->getZ() < getMinimumZ() ){
                o->setZ( getMinimumZ() );
            }
            if ( o->getZ() > getMaximumZ() ){
                o->setZ( getMaximumZ() );
            }
        }

        double lowest = 9999999;
        for ( vector< PlayerTracker >::iterator it = players.begin(); it != players.end(); it++ ){
            Paintown::Object * player = it->player;
            double mx = player->getX() - screen_size / 2;
            if ( it->min_x < mx ){
                it->min_x++;
            }

            if ( it->min_x + screen_size >= scene->getLimit() ){
                it->min_x = scene->getLimit() - screen_size;
            }

            if ( it->min_x < lowest ){
                lowest = it->min_x;
            }

            /*
               if ( player->getX() < it->min_x ){
               player->setX( it->min_x );
               }
               */
            if ( player->getX() < 0 ){
                player->setX( 0 );
            }

            if ( player->getX() > scene->getLimit() ){
                player->setX( scene->getLimit() );
            }
            if ( player->getZ() < getMinimumZ() ){
                player->setZ( getMinimumZ() );
            }
            if ( player->getZ() > getMaximumZ() ){
                player->setZ( getMaximumZ() );
            }
        }

        doScene( 0, 0 );
    }

    vector< Network::Message > messages;
    getIncomingMessages(messages);
    // vector< Network::Message > messages = getIncomingMessages();
    for ( vector< Network::Message >::iterator it = messages.begin(); it != messages.end(); it++ ){
        handleMessage( *it );
    }

    for ( vector< Paintown::Object * >::iterator it = objects.begin(); it != objects.end(); ){
        if ( (*it)->getHealth() <= 0 ){
            (*it)->died( added_effects );
            if ( ! isPlayer( *it ) ){
                delete *it;
            }
            it = objects.erase( it );
        } else ++it;
    }

    if (currentPing < 1 || abs((long)(Global::second_counter) - (long)secondCounter) > 2){
        pingCounter += 1;
        addMessage(pingMessage(pingCounter));
        secondCounter = Global::second_counter;
    }

    sendMessages(outgoing, getServer());
    outgoing.clear();

    for ( vector< Paintown::Object * >::iterator it = added_effects.begin(); it != added_effects.end(); ){
        Paintown::Object * o = *it;
        o->setId( (Paintown::Object::networkid_t) -1 );
        it++;
    }
    objects.insert( objects.end(), added_effects.begin(), added_effects.end() );
}

#endif
