#ifndef _rtech1_version_h
#define _rtech1_version_h

#include <string>

namespace Version{

void setVersion(int major, int minor, int micro);

/* The current version */
int getVersion();

/* The current version as a string */
std::string getVersionString();

/* Turn some major.minor.micro into a single version number for comparisons */
int getVersion(int major, int minor, int micro);

}

#endif
