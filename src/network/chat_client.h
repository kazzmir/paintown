#ifndef _paintown_chat_client_h
#define _paintown_chat_client_h

#include "network.h"

class ChatClient{
public:
	ChatClient( Network::Socket socket );

	void run();

	virtual ~ChatClient();

private:
	Network::Socket socket;
};

#endif
