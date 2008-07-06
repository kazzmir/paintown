#include "hawknl/nl.h"
#include "network.h"
#include "globals.h"
#include <string>
#include <sstream>

using namespace std;

/* TODO: Wrap open_sockets with a mutex */
namespace Network{

NetworkException::~NetworkException() throw (){
}
	
InvalidPortException::InvalidPortException( int port, const string & message ):
NetworkException(""){
	ostringstream num;
	num << port;
	num << ". ";
	num << message;
	this->setMessage( "Invalid port " + num.str() );
}

Message::Message(){
	memset( data, 0, sizeof(data) );
	position = data;
}
	
Message::Message( const Message & m ){
	memcpy( data, m.data, sizeof(data) );
	position = data;
	position += m.position - m.data;
	path = m.path;
	id = m.id;
}
	
Message & Message::operator=( const Message & m ){

	memcpy( data, m.data, sizeof(data) );
	position = data;
	position += m.position - m.data;
	path = m.path;
	id = m.id;

	return *this;
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

uint8_t * Message::dump( uint8_t * buffer ) const {
	*(uint16_t *) buffer = id;
	buffer += sizeof(uint16_t);
	memcpy( buffer, data, DATA_SIZE );
	buffer += DATA_SIZE;
	if ( path != "" ){
		*(uint16_t *) buffer = path.length() + 1;
		buffer += sizeof(uint16_t);
		memcpy( buffer, path.c_str(), path.length() + 1 );
		buffer += path.length() + 1;
	} else {
		*(uint16_t *) buffer = (uint16_t) -1;
		buffer += sizeof(uint16_t);
	}
	return buffer;
}

int totalSize( const std::vector< Message > & messages ){
	int totalSize = 0;
	for ( vector< Message >::const_iterator it = messages.begin(); it != messages.end(); it++ ){
		totalSize += (*it).size();
	}
	return totalSize;
}

void dump( const std::vector< Message > & messages, uint8_t * buffer ){
	for ( vector< Message >::const_iterator it = messages.begin(); it != messages.end(); it++ ){
		buffer = (*it).dump( buffer );
	}
}

void Message::send( Socket socket ) const {
	/*
	send16( socket, id );
	sendBytes( socket, data, DATA_SIZE );
	if ( path != "" ){
		send16( socket, path.length() + 1 );
		sendStr( socket, path );
	} else {
		send16( socket, -1 );
	}
	*/
	uint8_t * buffer = new uint8_t[ size() ];
	dump( buffer );
	sendBytes( socket, buffer, size() );
	delete[] buffer;
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
	
int Message::size() const {
	return sizeof( uint16_t ) + DATA_SIZE + 
		(path != "" ? sizeof( uint16_t ) + path.length() + 1 :
		sizeof( uint16_t ));
}

static string getHawkError(){
	return string(" HawkNL error: '") +
	       string( nlGetErrorStr( nlGetError() ) ) +
	       string( "' HawkNL system error: '" ) +
	       string( nlGetSystemErrorStr( nlGetSystemError() ) );
}

int16_t read16( Socket socket ){
	uint8_t data[ sizeof(uint16_t) ];
	readBytes( socket, data, sizeof(uint16_t) ); 
	return *(uint16_t *)data;
	/*
	uint16_t b;
	int read = nlRead( socket, &b, sizeof(int16_t) );
	if ( read != sizeof(int16_t) ){
		throw NetworkException( string("Could not read 16 bits.") + getHawkError() );
	}
	return b;
	*/
}

void send16( Socket socket, int16_t bytes ){
	/*
	if ( nlWrite( socket, &length, sizeof(int16_t) ) != sizeof(int16_t) ){
		throw NetworkException( string("Could not send 16 bits.") + getHawkError() );
	}
	*/
	sendBytes( socket, (uint8_t *) &bytes, sizeof(bytes) );
}

string readStr( Socket socket, const uint16_t length ){

	char buffer[ length + 1 ];
	NLint bytes = nlRead( socket, buffer, length );
	if ( bytes == NL_INVALID ){
		throw NetworkException( string("Could not read string.") + getHawkError() );
	}
	buffer[ length ] = 0;
	bytes += 1;
	return string( buffer );

}

void sendStr( Socket socket, const string & str ){
	if ( nlWrite( socket, str.c_str(), str.length() + 1 ) != (signed)(str.length() + 1) ){
		throw NetworkException( string("Could not write string.") + getHawkError() );
	}
}

void sendBytes( Socket socket, const uint8_t * data, int length ){
	const uint8_t * position = data;
	int written = 0;
	while ( written < length ){
		/* put htons here for endianess compatibility */
		int bytes = nlWrite( socket, position, length - written );
		if ( bytes == NL_INVALID ){
			throw NetworkException( string("Could not send bytes.") + getHawkError() );
		}
		written += bytes;
		position += bytes;
	}
}

void readBytes( Socket socket, uint8_t * data, int length ){
	uint8_t * position = data;
	int read = 0;
	while ( read < length ){
		/* put htons here for endianess compatibility */
		int bytes = nlRead( socket, position, length - read );
		if ( bytes == NL_INVALID ){
			throw NetworkException( string("Could not read bytes.") + getHawkError() );
		}
		read += bytes;
		position += bytes;
	}
}

Socket open( int port ) throw( InvalidPortException ){
	// NLsocket server = nlOpen( port, NL_RELIABLE_PACKETS );
	Global::debug( 1 ) << "Attemping to open port " << port << endl;
	Socket server = nlOpen( port, NL_RELIABLE );
	if ( server == NL_INVALID ){
		throw InvalidPortException(port, nlGetSystemErrorStr(nlGetSystemError()));
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
			Global::debug( 1 ) << "Closing socket " << s << endl;
			nlClose( *it );
			Global::debug( 1 ) << "Closed" << endl;
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
	// nlDisable( NL_BLOCKING_IO );
}

void blocking( bool b ){
	if ( b ){
		nlEnable( NL_BLOCKING_IO );
	} else {
		nlDisable( NL_BLOCKING_IO );
	}
}

void listen( Socket s ) throw( NetworkException ){
	if ( nlListen( s ) == NL_FALSE ){
		throw CannotListenException( string(nlGetSystemErrorStr( nlGetSystemError() )) );
	}
}

Socket accept( Socket s ) throw( NetworkException ){
	Socket connection = nlAcceptConnection( s );
	if ( connection == NL_INVALID ){
		/*
		if ( nlGetError() == NL_NO_PENDING ){
			error = NO_CONNECTIONS_PENDING;
		} else {
			error = NETWORK_ERROR;
		}
		return s;
		*/
		if ( nlGetError() == NL_NO_PENDING ){
			throw NoConnectionsPendingException();
		}
		throw NetworkException("Could not accept connection");
	}
	open_sockets.push_back( connection );
	return connection;
}

void shutdown(){
	nlShutdown();
}

}
