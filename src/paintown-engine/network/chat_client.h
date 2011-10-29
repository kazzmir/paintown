#ifndef _paintown_chat_client_h
#define _paintown_chat_client_h

#include "util/network/network.h"
#include "util/messages.h"
#include <string>
#include "util/thread.h"
#include <queue>

class Keyboard;
namespace Graphics{
class Bitmap;
}
namespace Gui{
    class LineEdit;
}
class keys;

enum Focus{
	INPUT_BOX,
	QUIT,
};

struct Buddy{
	std::string name;
	int id;
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
	void removeBuddy( int id );
	void changeName( int id, const std::string & s );
	void addBuddy( int id, const std::string & s );
	void finish();
	bool isFinished();

protected:
	void killInputThread();
	bool logic();
	Focus nextFocus( Focus f );
	bool needToDraw();
	void draw( const Graphics::Bitmap & bitmap );
	void needUpdate();
	void drawInputBox( int x, int y, const Graphics::Bitmap & work );
	void drawBuddies( const Graphics::Bitmap & area, int x, int y, const Font & f );
	bool sendMessage( const std::string & message );
	static void popup(const Graphics::Bitmap & work, const std::string & str );

        static void enter_pressed(void * self);
        static void next_focus(void * self);

private:
        Graphics::Bitmap * background;
	bool need_update;
	Messages messages;
	Network::Socket socket;
	std::string input;
	Focus focus;
        Util::Thread::Lock lock;
        Util::Thread::Id inputThread;
	std::vector< Buddy > buddies;
	bool finished;
        Gui::LineEdit * lineEdit;
	unsigned long long editCounter;
	bool enterPressed;
        std::queue<std::string> toSend;
};

#endif
