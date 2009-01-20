#ifndef _paintown_chat_client_h
#define _paintown_chat_client_h

#include "network.h"
#include "messages.h"
#include "gui/sigslot.h"
#include <string>
#include <pthread.h>
#include <queue>

class Keyboard;
class Bitmap;
class LineEdit;
class keys;

enum Focus{
	INPUT_BOX,
	QUIT,
};

struct Buddy{
	std::string name;
	int id;
};

class ChatClient: public sigslot::has_slots<> {
public:
	ChatClient( Network::Socket socket, const std::string & name );

	void run();

	virtual ~ChatClient();

	inline Network::Socket getSocket(){
		return socket;
	}
	
	void addMessage( const std::string & s, unsigned int id );
	void removeBuddy( int id );
	void changeName( int id, const std::string & s );
	void addBuddy( int id, const std::string & s );
	void finish();
	bool isFinished();

protected:
	void killInputThread();
	bool logic( Keyboard & keyboard );
	Focus nextFocus( Focus f );
	bool needToDraw();
	void draw( const Bitmap & bitmap );
	void needUpdate();
	void handleInput( Keyboard & keyboard );
	void drawInputBox( int x, int y, const Bitmap & work );
	void drawBuddies( const Bitmap & area, int x, int y, const Font & f );
	bool sendMessage( const std::string & message );
	void popup( Bitmap & work, Keyboard & key, const std::string & str );

	sigslot::slot keyPress(const keys &k);
	sigslot::slot keyRelease(const keys &k);

private:
	Bitmap * background;
	bool need_update;
	Messages messages;
	Network::Socket socket;
	std::string input;
	Focus focus;
	pthread_mutex_t lock;
	pthread_t inputThread;
	std::vector< Buddy > buddies;
	bool finished;
	LineEdit * lineEdit;
	unsigned long long editCounter;
	bool enterPressed;
        std::queue<std::string> toSend;
};

#endif
