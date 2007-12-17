#include "hawknl/nl.h"
#include "network.h"
#include "globals.h"
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
	
Message::Message( Socket socket ){
	position = data;
	id = read16( socket );
	readBytes( socket, data, DATA_SIZE );
	int str = read16( socket );
	if ( str != -1 ){
		char buf[ 1024 ];
		str = (signed)(sizeof( buf ) - 1) < str ? (signed)(sizeof(buf) - 1) : str;
		readBytes( socket, (uint8_t *) buf, str );
		buf[ str ] = 0;
		this->path = buf;
	}
}

void Message::send( Socket socket ){
	send16( socket, id );
	sendBytes( socket, data, DATA_SIZE );
	if ( path != "" ){
		send16( socket, path.length() + 1 );
		sendStr( socket, path );
	} else {
		send16( socket, -1 );
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

static string getHawkError(){
	return string(" HawkNL error: '") +
	       string( nlGetErrorStr( nlGetError() ) ) +
	       string( "' HawkNL system error: '" ) +
	       string( nlGetSystemErrorStr( nlGetSystemError() ) );
}

int16_t read16( NLsocket socket ){
	uint16_t b;
	int read = nlRead( socket, &b, sizeof(int16_t) );
	if ( read != sizeof(int16_t) ){
		throw NetworkException( string("Could not read 16 bits.") + getHawkError() );
	}
	return b;
}

void send16( NLsocket socket, int16_t length ){
	if ( nlWrite( socket, &length, sizeof(int16_t) ) != sizeof(int16_t) ){
		throw NetworkException( string("Could not send 16 bits.") + getHawkError() );
	}
}

string readStr( NLsocket socket, const uint16_t length ){

	char buffer[ length + 1 ];
	NLint bytes = nlRead( socket, buffer, length );
	if ( bytes == NL_INVALID ){
		throw NetworkException( string("Could not read string.") + getHawkError() );
	}
	buffer[ length ] = 0;
	bytes += 1;
	return string( buffer );

}

void sendStr( NLsocket socket, const string & str ){
	if ( nlWrite( socket, str.c_str(), str.length() + 1 ) != (signed)(str.length() + 1) ){
		throw NetworkException( string("Could not write string.") + getHawkError() );
	}
}

void sendBytes( NLsocket socket, const uint8_t * data, int length ){
	const uint8_t * position = data;
	int written = 0;
	while ( written < length ){
		int bytes = nlWrite( socket, position, length - written );
		if ( bytes == NL_INVALID ){
			throw NetworkException( string("Could not send bytes.") + getHawkError() );
		}
		written += bytes;
		position += bytes;
	}
}

void readBytes( NLsocket socket, uint8_t * data, int length ){
	uint8_t * position = data;
	int read = 0;
	while ( read < length ){
		int bytes = nlRead( socket, position, length - read );
		if ( bytes == NL_INVALID ){
			throw NetworkException( string("Could not read bytes.") + getHawkError() );
		}
		read += bytes;
		position += bytes;
	}
}

Socket open( int port ){
	// NLsocket server = nlOpen( port, NL_RELIABLE_PACKETS );
	NLsocket server = nlOpen( port, NL_RELIABLE );
	if ( server == NL_INVALID ){
		throw NetworkException();
	}
	open_sockets.push_back( server );
	return server;
}

Socket connect( string server, int port ) throw ( NetworkException ) {
	NLaddress address;
	nlGetAddrFromName( server.c_str(), &address );
	nlSetAddrPort( &address, port );
	Socket socket = open( 0 );
	if ( nlConnect( socket, &address ) == NL_FALSE ){
		close( socket );
		throw NetworkException( "Could not connect" );
	}
	return socket;
}

void close( Socket s ){
	for ( vector< Socket >::iterator it = open_sockets.begin(); it != open_sockets.end(); ){
		if ( *it == s ){
			Global::debug( 0 ) << "Closing socket " << s << endl;
			nlClose( *it );
			Global::debug( 0 ) << "Closed" << endl;
			it = open_sockets.erase( it );
		} else {
			it++;
		}
	}
}

void closeAll(){
	Global::debug( 0 ) << "Closing all sockets" << std::endl;
	for ( vector< Socket >::iterator it = open_sockets.begin(); it != open_sockets.end(); it++ ){
		nlClose( *it );
	}
	open_sockets.clear();
}

void init(){
	nlInit();
	nlSelectNetwork( NL_IP );
	nlEnable( NL_BLOCKING_IO );
}

void listen( Socket s ){
		  nlListen( s );
}

Socket accept( Socket s ) throw( NetworkException ){
		  Socket connection = nlAcceptConnection( s );
		  if ( connection == NL_INVALID ){
					 throw NetworkException("Could not accept connection");
		  }
		  return connection;
}

void shutdown(){
	nlShutdown();
}

}
