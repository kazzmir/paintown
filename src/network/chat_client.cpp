#include "network.h"
#include "chat_client.h"


ChatClient::ChatClient( Network::Socket socket ):
socket( socket ){
}

void ChatClient::run(){
}

ChatClient::~ChatClient(){
}

