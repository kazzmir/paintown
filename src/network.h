#ifndef _paintown_network_h
#define _paintown_network_h

#include <stdint.h>
#include "hawknl/nl.h"

struct Message{
	uint8_t id;
	uint8_t data[ 16 ];
};

namespace Network{

static uint16_t read16( NLsocket socket ){
	char buf[ 10 ];
	nlRead( socket, buf, 10 );
	return *(uint16_t *) buf;
}

static void send16( NLsocket socket, uint16_t length ){
	char buf[ 4 ];
	*(uint16_t *) buf = length;
	nlWrite( socket, buf, sizeof(uint16_t) );
}

static string readStr( NLsocket socket, uint16_t length ){

	char buffer[ length + 1 ];
	NLint bytes = nlRead( socket, buffer, length );
	buffer[ length ] = 0;
	bytes += 1;
	return string( buffer );

}

static void sendStr( NLsocket socket, const string & str ){
	nlWrite( socket, str.c_str(), str.length() + 1 );
}

}

#endif
