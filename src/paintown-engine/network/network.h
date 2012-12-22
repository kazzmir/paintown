#ifndef _paintown_engine_network_h
#define _paintown_engine_network_h

#include "util/network/network.h"

namespace Network{

extern const unsigned int MagicId;

struct Message{
    Message();
    Message( const Message & m );
    Message( Socket socket );

    uint32_t id;
    uint8_t data[ DATA_SIZE ];
    uint8_t * position;

    Message & operator=( const Message & m );
    Message & operator<<( int x );
    Message & operator<<( unsigned int x );
    Message & operator<<( std::string p );
    Message & operator>>( int & x );
    Message & operator>>( unsigned int & x );
    Message & operator>>(std::string & out);

    int size() const;
    uint8_t * dump( uint8_t * buffer ) const;

    void send( Socket socket ) const;
    void reset();

    std::string path;

    /* time in microseconds
     * a 64-bit variable can hold
     *                     us     ms     s    m    h    d
     *  years = 2**64.0 / (1000 * 1000 * 60 * 60 * 24 * 365)
     *  years = 584942.417355072
     * worth of microseconds
     */
    uint64_t timestamp;
    Socket readFrom;
};

void sendAllMessages(const std::vector<Message> & messages, Socket socket);
void sendAllMessages(const std::vector<Message*> & messages, Socket socket);

}

#endif
