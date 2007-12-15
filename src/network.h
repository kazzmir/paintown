#ifndef _paintown_network_h
#define _paintown_network_h

#include <stdint.h>
#include "hawknl/nl.h"
#include <string>
#include <exception>

namespace Network{

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

struct Message{
	Message();
	Message( const Message & m );
	Message( NLsocket socket );

	uint16_t id;
	uint8_t data[ DATA_SIZE ];
	uint8_t * position;

	Message & operator<<( int x );
	Message & operator<<( std::string p );
	Message & operator>>( int & x );

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

void networkServer();

}

#endif
