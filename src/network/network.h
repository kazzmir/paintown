#ifndef _paintown_network_h
#define _paintown_network_h

#include <stdint.h>
#include "hawknl/nl.h"
#include <string>
#include <vector>
#include <exception>

namespace Network{

typedef NLsocket Socket;

const int DATA_SIZE = 16;

class NetworkException: public std::exception{
public:
	NetworkException( const std::string & message = "" ):std::exception(),message(message){}

	inline const std::string getMessage() const {
		return message;
	}

	std::string message;

	~NetworkException() throw();
};

class NoConnectionsPendingException: public NetworkException{
public:
	NoConnectionsPendingException( const std::string & message = "" ):
		NetworkException( message ){
	}
};

struct Message{
	Message();
	Message( const Message & m );
	Message( Socket socket );

	uint16_t id;
	uint8_t data[ DATA_SIZE ];
	uint8_t * position;

	Message & operator=( const Message & m );
	Message & operator<<( int x );
	Message & operator<<( std::string p );
	Message & operator>>( int & x );

	int size() const;
	void dump( uint8_t * buffer ) const;

	void send( Socket socket ) const;
	void reset();

	std::string path;
};

int16_t read16( NLsocket socket );
void send16( NLsocket socket, int16_t length );
std::string readStr( NLsocket socket, const uint16_t length );
void sendStr( NLsocket socket, const std::string & str );
void sendBytes( NLsocket socket, const uint8_t * data, int length );
void readBytes( NLsocket socket, uint8_t * data, int length );
void init();
void shutdown();

void listen( Socket s );
Socket accept( Socket s ) throw( NetworkException );

Socket open( int port );
Socket connect( std::string server, int port ) throw ( NetworkException );
void close( Socket );
void closeAll();

static std::vector< Socket > open_sockets;

}

#endif
