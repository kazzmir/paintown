#include "r-tech1/version.h"
#include <sstream>

using std::string;

namespace Version{

static int major_version = 1;
static int minor_version = 0;
static int micro_version = 0;

void setVersion(int major, int minor, int micro){
    major_version = major;
    minor_version = minor;
    micro_version = micro;
}

int getVersion(int major, int minor, int micro){
    return major * 1000 + minor * 100 + micro;
}

int getVersion(){
    return getVersion(major_version, minor_version, micro_version);
}

string getVersionString(){
    std::ostringstream str;
    str << major_version << "." << minor_version << "." << micro_version;
    return str.str();
}

}
