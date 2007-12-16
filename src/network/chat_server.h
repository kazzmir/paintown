#ifndef _paintown_chat_server_h
#define _paintown_chat_server_h

#include "network.h"

class ChatServer{
public:
	ChatServer( Network::Socket socket );

	virtual void run();

	virtual ~ChatServer();

protected:
	Network::Socket socket;
};

#endif
