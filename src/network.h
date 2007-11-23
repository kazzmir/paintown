#ifndef _paintown_network_h
#define _paintown_network_h

#include <stdint.h>
#include "hawknl/nl.h"
#include <string>

namespace Network{

enum{
	CREATE_CHARACTER,
	CREATE_CAT,
};

const int DATA_SIZE = 16;

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

	std::string path;
};

int16_t read16( NLsocket socket );
void send16( NLsocket socket, int16_t length );
std::string readStr( NLsocket socket, const uint16_t length );
void sendStr( NLsocket socket, const std::string & str );
void sendBytes( NLsocket socket, const uint8_t * data, int length );
void readBytes( NLsocket socket, uint8_t * data, int length );

}

#endif
