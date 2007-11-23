#include "network_world.h"

using namespace std;

NetworkWorld::NetworkWorld( const vector< NLsocket > & sockets, const vector< Object * > & players, const string & path, int screen_size ) throw ( LoadException ):
World( players, path, screen_size ),
sockets( sockets ){
}
