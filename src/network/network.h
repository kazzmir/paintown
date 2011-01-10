#ifndef _paintown_network_h
#define _paintown_network_h

#include <stdint.h>
#ifdef HAVE_NETWORKING
#include "hawknl/nl.h"
#endif
#include <string>
#include <vector>
#include <exception>

namespace Network{

#ifdef HAVE_NETWORKING
typedef NLsocket Socket;
#else
typedef int Socket;
#endif

const int NO_CONNECTIONS_PENDING = 1;
const int NETWORK_ERROR = 2;

const int DATA_SIZE = 16;

class NetworkException: public std::exception{
public:
	NetworkException( const std::string message = "" ):std::exception(),message(message){}

	inline const std::string getMessage() const {
		return message;
	}
	
	~NetworkException() throw();

protected:
	inline void setMessage( const std::string & m ){
		this->message = m;
	}

private:
	std::string message;
};

class NoConnectionsPendingException: public NetworkException{
public:
	NoConnectionsPendingException(const std::string message = ""):
		NetworkException(message){
	}
};

class MessageEnd: public NetworkException {
public:
    MessageEnd();
};

class InvalidPortException: public NetworkException{
public:
	InvalidPortException( int port, const std::string message = "" );
};

class CannotListenException: public NetworkException{
public:
	CannotListenException( const std::string message = "" ):
		NetworkException( message ){
	}
};

struct Message{
	Message();
	Message( const Message & m );
	Message( Socket socket );

	uint32_t id;
	uint8_t data[ DATA_SIZE ];
	uint8_t * position;

	Message & operator=( const Message & m );
	Message & operator<<( int x );
	Message & operator<<( unsigned int x );
	Message & operator<<( std::string p );
	Message & operator>>( int & x );
	Message & operator>>( unsigned int & x );
        Message & operator>>(std::string & out);

	int size() const;
	uint8_t * dump( uint8_t * buffer ) const;

	void send( Socket socket ) const;
	void reset();

	std::string path;

        /* time in microseconds
         * a 64-bit variable can hold
         *                     us     ms     s    m    h    d
         *  years = 2**64.0 / (1000 * 1000 * 60 * 60 * 24 * 365)
         *  years = 584942.417355072
         * worth of microseconds
         */
        uint64_t timestamp;
        Socket readFrom;
};

/*
template <class M>
int totalSize(const std::vector<M> & messages);

template <class M>
void dump(const std::vector<M> & messages, uint8_t * buffer );
*/

#ifdef HAVE_NETWORKING
void sendAllMessages(const std::vector<Message> & messages, Socket socket);
void sendAllMessages(const std::vector<Message*> & messages, Socket socket);

int16_t read16( Socket socket );
int32_t read32( Socket socket );
void send16( Socket socket, int16_t length );
std::string readStr( Socket socket, const uint16_t length );
void sendStr( Socket socket, const std::string & str );
void sendBytes( Socket socket, const uint8_t * data, int length );
void readBytes( Socket socket, uint8_t * data, int length );
void init();
void shutdown();
void blocking( bool b );

void listen( Socket s ) throw( NetworkException );
Socket accept( Socket s ) throw( NetworkException );

Socket open( int port ) throw( InvalidPortException );
Socket connect( std::string server, int port ) throw ( NetworkException );
void close( Socket );
void closeAll();

static std::vector< Socket > open_sockets;
#endif

}

#endif
