#ifndef _paintown_network_h
#define _paintown_network_h

#include <stdint.h>
#include "hawknl/nl.h"
#include <string>

namespace Network{

enum{
	CREATE_OBJECT,
};

const int DATA_SIZE = 16;

struct Message{
	Message();
	uint8_t id;
	uint8_t data[ DATA_SIZE ];
	uint8_t * position;

	Message & operator<<( int x );
	Message & operator<<( const char * p );

	const char * path;
};

uint16_t read16( NLsocket socket );
void send16( NLsocket socket, uint16_t length );
std::string readStr( NLsocket socket, uint16_t length );
void sendStr( NLsocket socket, const std::string & str );

}

#endif
