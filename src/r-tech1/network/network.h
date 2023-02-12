#ifndef _rtech1_network_h
#define _rtech1_network_h

#include <stdint.h>
#ifdef HAVE_NETWORKING
#include "hawknl/hawknl/nl.h"
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

/*
template <class M>
int totalSize(const std::vector<M> & messages);

template <class M>
void dump(const std::vector<M> & messages, uint8_t * buffer );
*/

int8_t read8(Socket socket);
int16_t read16(Socket socket);
int32_t read32(Socket socket);
char * dump16(char * where, int16_t length);
char * dump32(char * where, uint32_t length);
void send16(Socket socket, int16_t length);

/* Reads a string by expecting the string to be terminated with a null byte */
std::string readStr(Socket socket, const uint16_t length);

/* This will send a string plus its null byte. If you just wanted to send the string
 * without a null byte then use sendBytes(socket, str.c_str(), str.size())
 */
void sendStr(Socket socket, const std::string & str );
void sendBytes(Socket socket, const uint8_t * data, int length);
void sendAllBytes(Socket socket, const uint8_t * data, int length);
void readBytes(Socket socket, uint8_t * data, int length);
/* reads as many bytes as possible in one go. useful for udp where partial packets
 * are discarded.
 */
int readUptoBytes(Socket socket, uint8_t * data, int length);

/* Copies the string plus its null byte to the `where' buffer.
 * Returns a pointer that is where + str.size() + 1
 */
char * dumpStr(char * where, const std::string & str);
char * parse16(char * where, uint16_t * out);
char * parse32(char * where, uint32_t * out);

/* Reads a string into 'out' from 'where' that is expected to be 'length' bytes.
 * Right now the function lies, it just does *out = string(where) so if the actual
 * string is longer than 'length' the returned pointer will be into the middle
 * of the 'where' buffer.
 */
char * parseString(char * where, std::string * out, uint16_t length);
void init();
void shutdown();
/* Whether or not blocking is enabled by default for new sockets */
void blocking(bool b);
/* Enable/disable blocking for a specific socket */
bool blocking(Socket s, bool b);
/* Enable/disable NODELAY -- the Nagle algorithm for TCP */
bool noDelay(Socket s, bool b);

/* Whether or not new sockets re-use ports */
void reuseSockets(bool what);

void listen(Socket s);
Socket accept(Socket s);

Socket openReliable(int port);
Socket openUnreliable(int port);
Socket connectReliable(std::string server, int port);
Socket connectUnreliable(std::string server, int port);
void close(Socket);
void closeAll();

static std::vector<Socket> open_sockets;

}

#endif
