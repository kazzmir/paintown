#ifndef _paintown_chat_server_h
#define _paintown_chat_server_h

#include "network.h"
#include "messages.h"
#include <pthread.h>
#include <string>

class Bitmap;
class Keyboard;

enum Focus{
	INPUT_BOX,
	QUIT,
};

class ChatServer;

class Client{
public:
	Client( Network::Socket socket, ChatServer * server, unsigned int id );

	inline const unsigned int getId(){
		return id;
	}

	inline Network::Socket getSocket(){
		return socket;
	}
	
	inline ChatServer * getServer(){
		return server;
	}

	inline pthread_t getInputThread(){
		return inputThread;
	}

	inline pthread_t getOutputThread(){
		return outputThread;
	}

	void kill();
	bool isAlive();

	std::string getName();
	void setName( const std::string & s );
	
	bool getOutgoing( std::string & s );

	void addOutputMessage( const std::string & s );

	void startThreads();

	virtual ~Client();
private:
	Network::Socket socket;
	ChatServer * server;
	unsigned int id;
	bool alive;
	std::string name;
	pthread_t inputThread;
	pthread_t outputThread;
	pthread_mutex_t lock;
	std::vector< std::string > outgoing;
};

class ChatServer{
public:
	ChatServer( Network::Socket socket );

	virtual void run();

	virtual void addConnection( Network::Socket socket );

	Network::Socket getSocket(){
		return socket;
	}

	void killClient( Client * c );

	void addMessage( const std::string & s, unsigned int id );
	
	void needUpdate();

	virtual ~ChatServer();

protected:
	void logic( Keyboard & keyboard );
	bool needToDraw();
	void draw( const Bitmap & work );
	
	void drawInputBox( int x, int y, const Bitmap & work );
	void drawBuddyList( int x, int y, const Bitmap & work, const Font & font );
	Focus nextFocus( Focus f );
	void handleInput( Keyboard & keyboard );

	inline const int clientId(){
		return client_id++;
	}

protected:
	bool need_update;
	Network::Socket socket;
	Bitmap * background;
	Messages messages;
	std::string input;
	Focus focus;
	unsigned int client_id;
	pthread_mutex_t lock;
	pthread_t acceptThread;
	std::vector< Client * > clients;
};

#endif
