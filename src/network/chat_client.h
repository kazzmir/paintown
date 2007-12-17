#ifndef _paintown_chat_client_h
#define _paintown_chat_client_h

#include "network.h"
#include "messages.h"
#include <string>
#include <pthread.h>

class Keyboard;
class Bitmap;

enum Focus{
	INPUT_BOX,
	QUIT,
};

class ChatClient{
public:
	ChatClient( Network::Socket socket, const std::string & name );

	void run();

	virtual ~ChatClient();

	inline Network::Socket getSocket(){
		return socket;
	}
	
	void addMessage( const std::string & s, unsigned int id );

protected:
	void logic( Keyboard & keyboard );
	Focus nextFocus( Focus f );
	bool needToDraw();
	void draw( const Bitmap & bitmap );
	void needUpdate();
	void handleInput( Keyboard & keyboard );
	void drawInputBox( int x, int y, const Bitmap & work );
	void sendMessage( const std::string & message );

private:
	Bitmap * background;
	bool need_update;
	Messages messages;
	Network::Socket socket;
	std::string input;
	Focus focus;
	pthread_mutex_t lock;
	pthread_t inputThread;
};

#endif
