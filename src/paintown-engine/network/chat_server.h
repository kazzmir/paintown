#ifndef _paintown_chat_server_h
#define _paintown_chat_server_h

#include "../network/network.h"
#include "util/messages.h"
#include "util/thread.h"
#include "util/pointer.h"
#include <string>
#include <vector>

namespace Gui{
    class LineEdit;
}

namespace Graphics{
class Bitmap;
}
class Keyboard;

enum Focus{
    INPUT_BOX,
    START_GAME,
    QUIT
};

class keys;
class ChatServer;

class Client{
public:
	Client( Network::Socket socket, ChatServer * server, unsigned int id );

	inline unsigned int getId(){
		return id;
	}

	inline Network::Socket getSocket(){
		return socket;
	}
	
	inline ChatServer * getServer(){
		return server;
	}

	inline Util::Thread::Id getInputThread(){
		return inputThread;
	}

	inline Util::Thread::Id getOutputThread(){
		return outputThread;
	}

	inline void setOk(){
		ok = true;
	}

	inline bool isOk(){
		return ok;
	}

	void kill();
	bool isAlive();
	bool canKill();

	std::string getName();
	void setName( const std::string & s );
	
	bool getOutgoing( Network::Message & m );

	void addOutputMessage( const Network::Message & s );

	void startThreads();

	virtual ~Client();
private:

	Network::Socket socket;
	ChatServer * server;
	unsigned int id;
	bool alive;
	/* ok to use this client */
	bool ok;
	bool started;
	std::string name;
        Util::Thread::Id inputThread;
        Util::Thread::Id outputThread;
        Util::Thread::LockObject lock;
	std::vector< Network::Message > outgoing;
};

class ChatServer{
public:
	ChatServer( const std::string & name, Network::Socket socket );

	/* run the chat server - accept connections and handle input */
	virtual void run();

	/* add a connection from a client */
	virtual void addConnection( Network::Socket socket );

	Network::Socket getSocket(){
		return socket;
	}

	/* end a connection */
	void killClient( Client * c );

	/* add a message to the chat box and send it out */
	void addMessage( const std::string & s, unsigned int id );
	
	/* send a message from the guy who is identified by id */
	void sendMessage( const Network::Message & message, unsigned int id );

        /* send the message immediately */
        void sendMessageNow(const Network::Message & message, unsigned int id);
	
	/* need to update the screen because a message came through or
	 * the user typed something in the chat box
	 */
	void needUpdate();
		
	/* is accepting connections from clients */
	bool isAccepting();
        
        /* each new client starts an accepting thread */
        void addAccepter(Util::Thread::Id accepter);
        
        /* add a message to the chat box */
        void addLine(Gui::LineEdit & line);

	/* get the list of connected clients */
        std::vector<Client*> getConnectedClients();

	inline const std::string getName() const {
		return name;
	}

	virtual ~ChatServer();

protected:
	/* force kill all client threads */
	void killAllClients();

	/* notify clients that the game is about to start */
	void shutdownClientThreads();

	/* do chat logic */
	bool logic();

	/* true if a redraw is needed */
	bool needToDraw();

	/* draw things to work */
	void draw(const Graphics::Bitmap & work, Gui::LineEdit & lineEdit, Focus focus);
	
	/* draw the input box */
	// void drawInputBox( int x, int y, const Graphics::Bitmap & work );

	/* draw the buddy list */
	void drawBuddyList( int x, int y, const Graphics::Bitmap & work, const Font & font );

	/* get the next widget to be focused */
	Focus nextFocus( Focus f );

	/* get the color a focused widget should be drawn with */
	// int focusColor( Focus f );

	/* stop accepting new connections */
	void stopAccepting();

	/* used to start threads in the run() method instead of elsewhere */
	void startThreadsHack();

	/* return a new connection id */
	inline int clientId(){
		return client_id++;
	}

        // static void next_focus(void * self);
	
protected:
	bool need_update;
	Network::Socket socket;
        Util::ReferenceCount<Graphics::Bitmap> background;
	Messages messages;
	// std::string input;
	// Focus focus;
	unsigned int client_id;
        Util::Thread::LockObject lock;
        Util::Thread::Id acceptThread;
	std::vector< Client * > clients;
	std::string name;
	bool accepting;
	// unsigned long long editCounter;
	// bool enterPressed;
        std::vector<Util::Thread::Id> accepted;
        // Gui::LineEdit * lineEdit;
};

#endif
