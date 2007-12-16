#ifndef _paintown_chat_server_h
#define _paintown_chat_server_h

#include "network.h"
#include "messages.h"

#include <string>

class Bitmap;
class Keyboard;

enum Focus{
	INPUT_BOX,
	QUIT,
};

class ChatServer{
public:
	ChatServer( Network::Socket socket );

	virtual void run();

	virtual ~ChatServer();

protected:
	void logic( Keyboard & keyboard );
	bool needToDraw();
	void needUpdate();
	void draw( const Bitmap & work );
	
	void drawInputBox( int x, int y, const Bitmap & work );
	Focus nextFocus( Focus f );
	void handleInput( Keyboard & keyboard );
	void addMessage( const std::string & s );

protected:
	bool need_update;
	Network::Socket socket;
	Bitmap * background;
	Messages messages;
	std::string input;
	Focus focus;
};

#endif
