/* this test will ensure that a server and client can talk to each other
 * through network sockets
 */

#include "util/network/network.h"

int main(){
    Network::init();
    Network::closeAll();
}
