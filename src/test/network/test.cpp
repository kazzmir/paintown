/* this test will ensure that a server and client can talk to each other
 * through network sockets
 */

#include <string>
#include <iostream>
#include "util/network/network.h"
#include "util/file-system.h"
#include "util/thread.h"
#include "globals.h"

using std::string;

#if 0
namespace Util{
/* sleep for `x' milliseconds */
void rest(int x){
#ifdef USE_ALLEGRO
    ::rest(x);
#endif
#ifdef USE_SDL
    SDL_Delay(x);
#endif
#ifdef USE_ALLEGRO5
    al_rest((double) x / 1000.0);
#endif
}

}
#endif

namespace Global{
    int getVersion(){
        return 0;
    }

    const Filesystem::RelativePath DEFAULT_FONT = Filesystem::RelativePath("fonts/arial.ttf");
}

/* global shared port */
volatile int port = 8923;
volatile int testResult = 0;

const char * message = "f3;o41ijfio;13fj1i;oejf;iofj;aiosf;klasasf";

static void * serverWorker(void * arg){
    try{
        Network::Socket socket = Network::openReliable(port);
        Network::listen(socket);
        Network::Socket client = Network::accept(socket);
        short length = Network::read16(client);
        if (length != string(message).size()){
            testResult = 1;
            return NULL;
        }
        string get = Network::readStr(client, length);
        if (get != message){
            testResult = 1;
        }
        std::cout << "Message received" << std::endl;
        Network::close(socket);
        Network::close(client);
    } catch (...){
        testResult = 1;
    }
    return NULL;
}

static void * clientWorker(void * arg){
    try{
        Network::Socket socket = Network::connectReliable("127.0.0.1", port);
        short length = string(message).size();
        Network::send16(socket, length);
        Network::sendStr(socket, message);
        Network::close(socket);
    } catch (...){
        testResult = 1;
    }
    return NULL;
}

static void test(){
    Util::Thread::ThreadObject server(NULL, serverWorker);
    Util::Thread::ThreadObject client(NULL, clientWorker);

    server.start();
    client.start();
}

int main(){
    Network::init();

    test();
    
    Network::closeAll();

    return testResult;
}
