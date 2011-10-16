/* this test will ensure that a server and client can talk to each other
 * through network sockets
 */

#include <string>
#include "util/network/network.h"
#include "util/thread.h"

using std::string;

/* global shared port */
volatile int port = 8923;
volatile int testResult = 0;

const char * message = "f3;o41ijfio;13fj1i;oejf;iofj;aiosf;klasasf";

static void * serverWorker(void * arg){
    try{
        Network::Socket socket = Network::open(port);
        Network::listen(socket);
        Network::Socket client = Network::accept(socket);
        unsigned char length = 0;
        Network::readBytes(client, &length, 1);
        if (length != string(message).size()){
            testResult = 1;
            return NULL;
        }
        string get = Network::readStr(client, length);
        if (get != message){
            testResult = 1;
        }
        Network::close(socket);
    } catch (...){
        testResult = 1;
    }
    return NULL;
}

static void * clientWorker(void * arg){
    try{
        Network::Socket socket = Network::connect("127.0.0.1", port);
        unsigned char length = string(message).size();
        Network::sendBytes(socket, &length, 1);
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
}

int main(){
    Network::init();

    test();
    
    Network::closeAll();

    return testResult;
}
