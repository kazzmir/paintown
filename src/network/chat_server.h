#ifndef _paintown_chat_server_h
#define _paintown_chat_server_h

#include "network.h"
#include "messages.h"

class Bitmap;

class ChatServer{
public:
	ChatServer( Network::Socket socket );

	virtual void run();

	virtual ~ChatServer();

protected:
	void logic();
	bool needToDraw();
	void draw( const Bitmap & work );

protected:
	bool need_update;
	Network::Socket socket;
	Bitmap * background;
	Messages messages;
};

#endif
