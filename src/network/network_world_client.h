#ifndef _paintown_network_world_client_h
#define _paintown_network_world_client_h

#include <pthread.h>
#include "game/adventure_world.h"
#include "network.h"
#include "object/object.h"
#include "util/load_exception.h"
#include "input/input-map.h"
#include "input/text-input.h"
#include <vector>

class Bitmap;

class NetworkWorldClient: public AdventureWorld {
public:

	NetworkWorldClient( Network::Socket server, const std::vector< Object * > & players, const std::string & path, Object::networkid_t id, const std::map<Object::networkid_t, std::string> & clientNames, int screen_size = 320 ) throw ( LoadException );
	
	virtual void act();
	virtual void draw(Bitmap * work);

	void addIncomingMessage( const Network::Message & message );

	inline NLint getServer(){
		return server;
	}

	inline pthread_mutex_t * getLock(){
		return &message_mutex;
	}

        /* start thread to handle messages */
        void startMessageHandler();

        Object::networkid_t getId() const {
            return id;
        }
	
	virtual void doScene( int min_x, int max_x );
	virtual void addMessage( Network::Message m, Network::Socket from = 0, Network::Socket to = 0);

        virtual void changePause();

	virtual ~NetworkWorldClient();

	bool isRunning();
	void stopRunning();
	
	virtual bool finished() const;

protected:

	Object * findNetworkObject( Object::networkid_t id );
	void handleCreateCharacter( Network::Message & message );
	void handleCreateCat( Network::Message & message );
	void handleCreateBang( Network::Message & message );
	void handleCreateItem( Network::Message & message );
	Object * removeObject( Object::networkid_t id );
        void removePlayer(Object * obj);

	void sendMessage( const Network::Message & message, NLsocket socket );
        void sendMessages(const std::vector<Network::Message> & messages, Network::Socket socket);

	bool uniqueObject( Object::networkid_t id );
	void handleMessage( Network::Message & message );
        // std::vector< Network::Message > getIncomingMessages();
        /* avoid an unnecessary stack copy by passing in the vector directly */
        void getIncomingMessages(std::vector< Network::Message > & messages);
        Network::Message pingMessage(unsigned int id);
        void handlePing(Network::Message & message);

        enum Keys{
            Talk,
        };

public:
        void endChatLine();

private:
	NLsocket server;
	std::vector< Network::Message > incoming;
	std::vector< Network::Message > outgoing;
        std::deque<std::string> chatMessages;
        int removeChatTimer;
	pthread_mutex_t message_mutex;
	pthread_mutex_t running_mutex;
	pthread_t message_thread;

	bool world_finished;
        unsigned int secondCounter;
        Object::networkid_t id;

	bool running;

        std::map<unsigned int, uint64_t> pings;
        double currentPing;
        
        InputMap<Keys> input;
        bool enable_chat;
        TextInput chatInput;

        std::map<Object::networkid_t, std::string> clientNames;
        unsigned int pingCounter;
};

#endif
