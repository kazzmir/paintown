#include "network.h"
#include "util/system.h"
#include <vector>
#include <string>
#include <string.h>

/* FIXME: these things should come from util/network/network.h */
#ifdef HAVE_NETWORKING
#ifdef WII
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

using std::string;
using std::vector;

namespace Network{

static int messageSize(Message const & message){
    return message.size();
}

static int messageSize(Message* const & message){
    return message->size();
}

static uint8_t * messageDump(const Message & message, uint8_t * buffer){
    return message.dump(buffer);
}

static uint8_t * messageDump(Message* const & message, uint8_t * buffer){
    return message->dump(buffer);
}

template <typename M>
static int totalSize(const vector<M> & messages){
    int size = 0;
    for (typename vector<M>::const_iterator it = messages.begin(); it != messages.end(); it++){
        // size += messageSize<M>(*it);
        size += messageSize(*it);
    }
    return size;
}

template <class M>
static void dump(const std::vector<M> & messages, uint8_t * buffer ){
    for (typename vector<M>::const_iterator it = messages.begin(); it != messages.end(); it++ ){
        buffer = messageDump(*it, buffer);
    }
}

template <class M>
static void doSendAllMessages(const vector<M> & messages, Socket socket){
#ifdef HAVE_NETWORKING
    int length = totalSize<M>(messages);
    uint8_t * data = new uint8_t[length];
    dump<M>(messages, data);
    // Compress::testCompression((unsigned char *) data, length);
    sendBytes(socket, data, length);
    delete[] data;
#endif
}

void sendAllMessages(const vector<Message> & messages, Socket socket){
    doSendAllMessages<Message>(messages, socket);
}

void sendAllMessages(const vector<Message*> & messages, Socket socket){
    doSendAllMessages<Message*>(messages, socket);
}

void Message::send(Socket socket) const {
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
#ifdef HAVE_NETWORKING
    uint8_t * buffer = new uint8_t[size()];
    dump(buffer);
    sendBytes(socket, buffer, size());
    delete[] buffer;
#endif
}

Message::Message():
id((uint32_t) -1),
timestamp(0),
readFrom(0){
	memset(data, 0, sizeof(data));
	position = data;
}
	
Message::Message(const Message & m):
timestamp(m.timestamp),
readFrom(m.readFrom){
    memcpy( data, m.data, sizeof(data) );
    position = data;
    position += m.position - m.data;
    path = m.path;
    id = m.id;
}
	
Message & Message::operator=( const Message & m ){
    memcpy(data, m.data, sizeof(data));
    position = data;
    position += m.position - m.data;
    path = m.path;
    id = m.id;
    timestamp = m.timestamp;
    readFrom = m.readFrom;
    return *this;
}

Message::Message(Socket socket){
#ifdef HAVE_NETWORKING
    position = data;
    id = read32(socket);
    readBytes(socket, data, DATA_SIZE);
    int str = read16(socket);
    if (str != -1){
        /* cap strings at 1024 bytes */
        char buf[1024];
        str = (signed)(sizeof(buf) - 1) < str ? (signed)(sizeof(buf) - 1) : str;
        readBytes(socket, (uint8_t *) buf, str);
        buf[str] = 0;
        /* this is a string copy, not an assignment to a temporary pointer */
        this->path = buf;
    }
    timestamp = System::currentMicroseconds();
    readFrom = socket;
#endif
}

uint8_t * Message::dump(uint8_t * buffer) const {
    *(uint32_t *) buffer = htonl(id);
    // Global::debug(1, "network") << "Dumped id " << id << " as " << htonl(id) << std::endl;
    buffer += sizeof(id);
    memcpy(buffer, data, DATA_SIZE);
    buffer += DATA_SIZE;
    if (path != ""){
        *(uint16_t *) buffer = htons(path.length() + 1);
        buffer += sizeof(uint16_t);
        memcpy(buffer, path.c_str(), path.length() + 1);
        buffer += path.length() + 1;
    } else {
        *(uint16_t *) buffer = htons((uint16_t) -1);
        buffer += sizeof(uint16_t);
    }
    return buffer;
}


	
void Message::reset(){
    position = data;
}

Message & Message::operator<<(int x){
    if (position > data + DATA_SIZE - sizeof(uint16_t)){
        throw NetworkException("Tried to set too much data");
    }

    *(int16_t *) position = htons(x);
    position += sizeof(int16_t);

    return *this;
}

Message & Message::operator<<(unsigned int x){
    if (position > data + DATA_SIZE - sizeof(uint32_t)){
        throw NetworkException("Tried to set too much data");
    }

    *(int32_t *) position = htonl(x);
    position += sizeof(int32_t);
    return *this;
}
	
Message & Message::operator>>(int & x){
    if (position > data + DATA_SIZE - sizeof(uint16_t)){
        throw NetworkException("Tried to read too much data");
    }

    x = ntohs(*(int16_t *) position);
    position += sizeof(int16_t);
    return *this;
}

Message & Message::operator>>(unsigned int & x){
    if (position > data + DATA_SIZE - sizeof(uint32_t)){
        throw NetworkException("Tried to read too much data");
    }

    x = htonl(*(int32_t *) position);
    position += sizeof(int32_t);
    return *this;
}
        
Message & Message::operator>>(std::string & out){
    out = path;
    return *this;
}

Message & Message::operator<<(string p){
    path = p;
    return *this;
}
	
int Message::size() const {
    return sizeof(id) + DATA_SIZE + 
           (path != "" ? sizeof(uint16_t) + path.length() + 1 : sizeof(uint16_t));
}

}
