#ifndef _paintown_network_client_h
#define _paintown_network_client_h

#include <string>

namespace Network{

void networkClient();
void runClient(const std::string & name, const std::string & host, const std::string & port);

extern const char * propertyLastClientName;
extern const char * propertyLastClientHost;
extern const char * propertyLastClientPort;

}

#endif
