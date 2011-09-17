#ifndef _paintown_network_world_client_h
#define _paintown_network_world_client_h

#ifdef HAVE_NETWORKING

#include "paintown-engine/game/adventure_world.h"
#include "util/network/network.h"
#include "paintown-engine/object/object.h"
#include "util/load_exception.h"
#include "util/file-system.h"
#include "util/input/input-map.h"
#include "util/input/text-input.h"
#include "chat-widget.h"
#include "util/thread.h"
#include <vector>

namespace Graphics{
class Bitmap;
}

class NetworkWorldClient: public AdventureWorld, public ChatWidget {
public:

	NetworkWorldClient( Network::Socket server, const std::vector< Paintown::Object * > & players, const Filesystem::AbsolutePath & path, Paintown::Object::networkid_t id, const std::map<Paintown::Object::networkid_t, std::string> & clientNames, int screen_size = 320 ) throw ( LoadException );
	
	virtual void act();
	virtual void draw(Graphics::Bitmap * work);

	void addIncomingMessage( const Network::Message & message );

	inline NLint getServer(){
		return server;
	}

	inline Util::Thread::Lock * getLock(){
		return &message_mutex;
	}

        /* start thread to handle messages */
        void startMessageHandler();

        Paintown::Object::networkid_t getId() const {
            return id;
        }
	
	virtual void doScene( int min_x, int max_x );
	virtual void addMessage( Network::Message m, Network::Socket from = 0, Network::Socket to = 0);
        virtual bool respawnPlayers(const std::vector<Paintown::Object*> & players);

        virtual void changePause();

	virtual ~NetworkWorldClient();

	bool isRunning();
	void stopRunning();
	
	virtual bool finished() const;

protected:

        Paintown::Object * findNetworkObject( Paintown::Object::networkid_t id );
	void handleCreateCharacter( Network::Message & message );
	void handleCreateCat( Network::Message & message );
	void handleCreateBang( Network::Message & message );
	void handleCreateItem( Network::Message & message );
        Paintown::Object * removeObject( Paintown::Object::networkid_t id );
        void removePlayer(Paintown::Object * obj);

	void sendMessage( const Network::Message & message, NLsocket socket );
        void sendMessages(const std::vector<Network::Message> & messages, Network::Socket socket);

	bool uniqueObject( Paintown::Object::networkid_t id );
	void handleMessage( Network::Message & message );
        // std::vector< Network::Message > getIncomingMessages();
        /* avoid an unnecessary stack copy by passing in the vector directly */
        void getIncomingMessages(std::vector< Network::Message > & messages);
        Network::Message pingMessage(unsigned int id);
        void handlePing(Network::Message & message);

private:
	NLsocket server;
	std::vector< Network::Message > incoming;
	std::vector< Network::Message > outgoing;
        Util::Thread::Lock message_mutex;
        Util::Thread::Lock running_mutex;
        Util::Thread::Id message_thread;

	bool world_finished;
        unsigned int secondCounter;
        Paintown::Object::networkid_t id;

	bool running;

        std::map<unsigned int, uint64_t> pings;
        double currentPing;
       
        std::map<Paintown::Object::networkid_t, std::string> clientNames;
        unsigned int pingCounter;
};

#endif

#endif
