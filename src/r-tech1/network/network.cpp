#ifdef HAVE_NETWORKING
#include "libs/hawknl/hawknl/nl.h"
#endif
#include "r-tech1/network/network.h"
#include "r-tech1/debug.h"
#include <string>
#include <sstream>
#include <string.h>
#include "r-tech1/system.h"
#include "r-tech1/compress.h"
#include "r-tech1/thread.h"

#ifdef HAVE_NETWORKING
#ifdef DEVKITPRO
#include <network.h>
#elif defined(WINDOWS)
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif
#else
#ifndef htonl
#define htonl(x) x
#endif
#ifndef htons
#define htons(x) x
#endif
#ifndef ntohl
#define ntohl(x) x
#endif
#ifndef ntohs
#define ntohs(x) x
#endif
#endif

using namespace std;

/* TODO: Wrap open_sockets with a mutex */
namespace Network{

NetworkException::~NetworkException() throw (){
}
    
MessageEnd::MessageEnd(){
}
    
InvalidPortException::InvalidPortException( int port, const string message ):
NetworkException(""){
    ostringstream num;
    num << port;
    num << ". ";
    num << message;
    this->setMessage( "Invalid port " + num.str() );
}

/*
template <typename M>
int messageSize(const M& message);
*/

/*
template <>
int messageSize<Message>(Message const & message){
    return message.size();
}

template <>
int messageSize<Message*>(Message* const & message){
    return message->size();
}
*/
/*
template <class M>
uint8_t * messageDump(const M& message, uint8_t * buffer);

template <>
uint8_t * messageDump<Message>(const Message & message, uint8_t * buffer){
    return message.dump(buffer);
}

template <>
uint8_t * messageDump<Message*>(Message* const & message, uint8_t * buffer){
    return message->dump(buffer);
}
*/

#ifdef HAVE_NETWORKING

static string getHawkError(){
    return string(" HawkNL error: '") +
           string(nlGetErrorStr(nlGetError())) +
           string("' HawkNL system error: '") +
           string(nlGetSystemErrorStr(nlGetSystemError()));
}

template<typename X>
static X readX(Socket socket){
    X data;
    readBytes(socket, (uint8_t*) &data, sizeof(X)); 
    return data;
}

int8_t read8(Socket socket){
    return readX<uint8_t>(socket);
}

int16_t read16(Socket socket){
    return ntohs(readX<uint16_t>(socket));
}

int32_t read32(Socket socket){
    return ntohl(readX<uint32_t>(socket));
}

void send16(Socket socket, int16_t bytes){
    bytes = htons(bytes);
    sendBytes(socket, (uint8_t *) &bytes, sizeof(bytes));
}

char * dump16(char * where, int16_t bytes){
    bytes = htons(bytes);
    *(uint16_t*) where = bytes;
    return where + sizeof(uint16_t);
}

char * parse16(char * where, uint16_t * out){
    *out = ntohs(*(uint16_t*) where);
    return where + sizeof(uint16_t);
}

char * dump32(char * where, uint32_t bytes){
    bytes = htonl(bytes);
    *(uint32_t*) where = bytes;
    return where + sizeof(uint32_t);
}

char * parse32(char * where, uint32_t * out){
    *out = ntohl(*(uint32_t*) where);
    return where + sizeof(uint32_t);
}

char * parseString(char * where, string * out, uint16_t length){
    *out = string(where);
    return where + length;
}

char * dumpStr(char * where, const std::string & str){
    memcpy(where, str.c_str(), str.size() + 1);
    return where + str.size() + 1;
}

string readStr(Socket socket, const uint16_t length){
    char buffer[length + 1];
    NLint bytes = nlRead(socket, buffer, length);
    if (bytes == NL_INVALID){
        throw NetworkException(string("Could not read string.") + getHawkError());
    }
    buffer[length] = 0;
    bytes += 1;
    return string(buffer);
}

void sendStr(Socket socket, const string & str){
    if (nlWrite(socket, str.c_str(), str.length() + 1) != (signed)(str.length() + 1)){
        throw NetworkException( string("Could not write string.") + getHawkError() );
    }
}

void sendAllBytes(Socket socket, const uint8_t * data, int length){
    int sent = nlWrite(socket, data, length);
    if (sent != length){
        throw NetworkException(string("Could not send bytes.") + getHawkError());
    }
}

void sendBytes(Socket socket, const uint8_t * data, int length){
    const uint8_t * position = data;
    int written = 0;
    while ( written < length ){
        int bytes = nlWrite(socket, position, length - written);
        if (bytes == NL_INVALID){
            throw NetworkException(string("Could not send bytes.") + getHawkError());
        }
        written += bytes;
        position += bytes;
    }
}

int readUptoBytes(Socket socket, uint8_t * data, int length){
    return nlRead(socket, data, length);
}

void readBytes(Socket socket, uint8_t * data, int length){
    uint8_t * position = data;
    int read = 0;
    while (read < length){
        int bytes = nlRead(socket, position, length - read);
        if (bytes == NL_INVALID){
            switch (nlGetError()){
                case NL_MESSAGE_END : throw MessageEnd();
                default : throw NetworkException(string("Could not read bytes.") + getHawkError());
            }
        }
        read += bytes;
        position += bytes;
    }
}

Util::Thread::Lock socketsLock;

Socket openReliable(int port){
    // NLsocket server = nlOpen( port, NL_RELIABLE_PACKETS );
    Global::debug(1, "network") << "Attemping to open reliable port " << port << endl;
    Socket server = nlOpen(port, NL_RELIABLE);
    /* server will either be NL_INVALID (-1) or some low integer. hawknl
     * sockets are mapped internally to real sockets, so don't be surprised
     * if you get a socket back like 0.
     */
    if (server == NL_INVALID){
        throw InvalidPortException(port, nlGetSystemErrorStr(nlGetSystemError()));
    }
    Global::debug(1, "network") << "Successfully opened a socket: " << server << endl;
    Util::Thread::acquireLock(&socketsLock);
    open_sockets.push_back(server);
    Util::Thread::releaseLock(&socketsLock);
    return server;
}

Socket openUnreliable(int port){
    // NLsocket server = nlOpen( port, NL_RELIABLE_PACKETS );
    Global::debug(1, "network") << "Attemping to open unreliable port " << port << endl;
    Socket server = nlOpen(port, NL_UNRELIABLE);
    /* server will either be NL_INVALID (-1) or some low integer. hawknl
     * sockets are mapped internally to real sockets, so don't be surprised
     * if you get a socket back like 0.
     */
    if (server == NL_INVALID){
        throw InvalidPortException(port, nlGetSystemErrorStr(nlGetSystemError()));
    }
    Global::debug(1, "network") << "Successfully opened a socket: " << server << endl;
    Util::Thread::acquireLock(&socketsLock);
    open_sockets.push_back(server);
    Util::Thread::releaseLock(&socketsLock);
    return server;
}

Socket connectReliable(string server, int port){
    NLaddress address;
    nlGetAddrFromName(server.c_str(), &address);
    nlSetAddrPort(&address, port);
    /* The port that this socket has opened will be immediately rebound to some
     * other port by sock_connect, but we still need to call openReliable to get
     * an NL_RELIABLE socket.
     */
    Socket socket = openReliable(0);
    if (nlConnect(socket, &address) == NL_FALSE){
        close(socket);
        throw NetworkException("Could not connect");
    }
    return socket;
}

Socket connectUnreliable(string server, int port){
    NLaddress address;
    nlGetAddrFromName(server.c_str(), &address);
    nlSetAddrPort(&address, port);
    /* The port that this socket has opened will be immediately rebound to some
     * other port by sock_connect, but we still need to call openReliable to get
     * an NL_RELIABLE socket.
     */
    Socket socket = openUnreliable(0);
    if (nlConnect(socket, &address) == NL_FALSE){
        close(socket);
        throw NetworkException("Could not connect");
    }
    return socket;
}


void close(Socket s){
    Util::Thread::acquireLock(&socketsLock);
    for (vector< Socket >::iterator it = open_sockets.begin(); it != open_sockets.end(); ){
        if ( *it == s ){
            Global::debug(1, "network") << "Closing socket " << s << endl;
            nlClose(*it);
            Global::debug(1, "network") << "Closed" << endl;
            it = open_sockets.erase(it);
        } else {
            it++;
        }
    }
    Util::Thread::releaseLock(&socketsLock);
}

void closeAll(){
    Global::debug(1, "network") << "Closing all sockets" << std::endl;
    Util::Thread::acquireLock(&socketsLock);
    for (vector<Socket>::iterator it = open_sockets.begin(); it != open_sockets.end(); it++ ){
        nlClose(*it);
    }
    open_sockets.clear();
    Util::Thread::releaseLock(&socketsLock);
}

void init(){
    nlInit();
    nlSelectNetwork(NL_IP);
    nlEnable(NL_BLOCKING_IO);
    Util::Thread::initializeLock(&socketsLock);
    // nlDisable( NL_BLOCKING_IO );
}

void reuseSockets(bool what){
    if (what){
        nlEnable(NL_REUSE_ADDRESS);
    } else {
        nlDisable(NL_REUSE_ADDRESS);
    }
}

bool blocking(Socket s, bool b){
    return nlSetSocketOpt(s, NL_BLOCKING_IO, b) == NL_TRUE;
}

void blocking(bool b){
    if (b){
        nlEnable(NL_BLOCKING_IO);
    } else {
        nlDisable(NL_BLOCKING_IO);
    }
}

bool noDelay(Socket s, bool b){
    return nlSetSocketOpt(s, NL_TCP_NO_DELAY, b) == NL_TRUE;
}

void listen( Socket s ) throw( NetworkException ){
    if ( nlListen( s ) == NL_FALSE ){
        throw CannotListenException( string(nlGetSystemErrorStr( nlGetSystemError() )) );
    }
}

Socket accept( Socket s ) throw( NetworkException ){
    Socket connection = nlAcceptConnection(s);
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
    Util::Thread::acquireLock(&socketsLock);
    open_sockets.push_back(connection);
    Util::Thread::releaseLock(&socketsLock);
    return connection;
}

void shutdown(){
    nlShutdown();
}

#else

/* Dummy implementations */
char * dump16(char * where, int16_t length){
    return where;
}

int8_t read8(Socket socket){
    return 0;
}

int16_t read16(Socket socket){
    return 0;
}

char * dumpStr(char * where, const std::string & str){
    return where;
}

void readBytes(Socket socket, uint8_t * data, int length){
}

void sendBytes(Socket socket, const uint8_t * data, int length){
}

char * parseString(char * where, std::string * out, uint16_t length){
    return where;
}

char * parse16(char * where, uint16_t * out){
    return nullptr;
}

void close(Socket socket){
}

Socket openReliable(int port){
    return 0;
}

void listen(Socket s){
}

Socket connectReliable(std::string server, int port){
    return 0;
}

Socket accept(Socket s){
    return 0;
}

#endif


}
