#include "hawknl/nl.h"
#include "network.h"
#include <string>

using namespace std;

namespace Network{

NetworkException::~NetworkException() throw (){
}

Message::Message(){
	memset( data, 0, sizeof(uint8_t) * DATA_SIZE );
	position = data;
}
	
Message::Message( const Message & m ){
	memcpy( data, m.data, sizeof(uint8_t) * DATA_SIZE );
	position = data;
	position += m.position - m.data;
	path = m.path;
	id = m.id;
}
	
Message::Message( NLsocket socket ){
	position = data;
	id = read16( socket );
	readBytes( socket, data, DATA_SIZE );
	int str = read16( socket );
	if ( str != -1 ){
		char buf[ 1024 ];
		readBytes( socket, (uint8_t *) buf, str );
		buf[ str ] = 0;
		this->path = buf;
	}
}
	
void Message::reset(){
	position = data;
}

Message & Message::operator<<( int x ){

	*(int16_t *) position = x;
	position += sizeof( int16_t );

	return *this;
}
	
Message & Message::operator>>( int & x ){
	x = *(int16_t *) position;
	position += sizeof( int16_t );
	return *this;
}

Message & Message::operator<<( string p ){
	path = p;
	return *this;
}

int16_t read16( NLsocket socket ){
	uint16_t b;
	int read = nlRead( socket, &b, sizeof(int16_t) );
	if ( read != sizeof(int16_t) ){
		throw NetworkException( "Could not read 16 bits" );
	}
	return b;
}

void send16( NLsocket socket, int16_t length ){
	if ( nlWrite( socket, &length, sizeof(int16_t) ) != sizeof(int16_t) ){
		throw NetworkException( "Could not send 16 bits" );
	}
}

string readStr( NLsocket socket, const uint16_t length ){

	char buffer[ length + 1 ];
	NLint bytes = nlRead( socket, buffer, length );
	if ( bytes == NL_INVALID ){
		throw NetworkException( "Could not read string" );
	}
	buffer[ length ] = 0;
	bytes += 1;
	return string( buffer );

}

void sendStr( NLsocket socket, const string & str ){
	if ( nlWrite( socket, str.c_str(), str.length() + 1 ) != (signed)(str.length() + 1) ){
		throw NetworkException( "Could not write string" );
	}
}

void sendBytes( NLsocket socket, const uint8_t * data, int length ){
	if ( nlWrite( socket, data, length ) != length ){
		throw NetworkException( "Could not send bytes" );
	}
}

void readBytes( NLsocket socket, uint8_t * data, int length ){
	if ( nlRead( socket, data, length ) == NL_INVALID ){
		throw NetworkException( "Could not read bytes" );
	}
}

}
