#ifndef _paintown_network_world_h
#define _paintown_network_world_h

#include "util/network/network.h"
#include "util/file-system.h"
#include "../object/object.h"
#include "../game/adventure_world.h"
#include "util/input/text-input.h"
#include "chat-widget.h"
#include "util/thread.h"
#include <vector>
#include <string>
#include <deque>
#include <sstream>
#include <map>

struct Packet{
	Packet(const Network::Message & m, Network::Socket s, Network::Socket to = 0):
            message(m),
            socket(s),
            to(to){}

	Network::Message message;
	Network::Socket socket;
        /* if you want to specify a receipient, make `to' non-null */
	Network::Socket to;
};

class NetworkWorld: public AdventureWorld, public ChatWidget {
public:
	NetworkWorld( std::vector<Network::Socket> & sockets, const std::vector< Paintown::Object * > & players, const std::map<Paintown::Object*, Network::Socket> & characterToClient, const Filesystem::AbsolutePath & path, const std::map<Paintown::Object::networkid_t, std::string> & clientNames, int screen_size = 320 );
	
	virtual void addMessage( Network::Message m, Network::Socket from = 0, Network::Socket to = 0);
	virtual void act();
        virtual void draw(Graphics::Bitmap * work);
	virtual void doScene( int min_x, int max_x );
        
        virtual void changePause();
	
	void addIncomingMessage( const Network::Message & message, Network::Socket from );

	Network::Message finishMessage();

        /* starts the threads for each client */
        void startMessageHandlers();
        void waitForHandlers();

	bool isRunning();

	void flushOutgoing();
	
	void stopRunning();
	
	virtual void addObject( Paintown::Object * o );

	virtual ~NetworkWorld();

protected:
        Paintown::Object * findNetworkObject( Paintown::Object::networkid_t id );
	void sendMessage( const Network::Message & message, Network::Socket socket );
        std::vector< Network::Message > getIncomingMessages();
	void handleMessage( Network::Message & message );
        void handlePing(Network::Message & message);

	Network::Message nextBlockMessage( int block );

        void removePlayer(Paintown::Object * player);
        void removeSocket(Network::Socket socket);
        Paintown::Object * findPlayerFromSocket(Network::Socket socket);

	inline unsigned int nextId(){
            unsigned int i = id;
            id += 1;
            return i;
	}

private:
	std::vector<Network::Socket> & sockets;
	std::vector<Packet> outgoing;
	std::vector<Network::Message> incoming;
	std::vector<Util::Thread::Id> threads;
        std::map<Paintown::Object::networkid_t, std::string> clientNames;
	std::map<Paintown::Object*, Network::Socket> characterToClient;
        Paintown::Object::networkid_t id;

	unsigned int sent_messages;

        Util::Thread::Lock message_mutex;
        Util::Thread::Lock running_mutex;
	bool running;
};

#endif
