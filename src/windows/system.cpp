#ifdef WINDOWS

#include <windows.h>
#include <fstream>
#include "util/system.h"

namespace System{

bool isDirectory(const std::string & path){
    unsigned int f = GetFileAttributes(path.c_str());
    if (f == INVALID_FILE_ATTRIBUTES){
        return false;
    }
    return f & FILE_ATTRIBUTE_DIRECTORY;
}

bool readableFile(const std::string & path){
    return !isDirectory(path) && readable(path);
}
    
bool readable(const std::string & path){
    if (isDirectory(path)){
        return true;
    }

	std::ifstream stream(path.c_str());
    bool ok = stream.good();
    if (stream.is_open()){
        stream.close();
    }
    return ok;
}

uint64_t currentMicroseconds(){
    LARGE_INTEGER ticksPerSecond;
    LARGE_INTEGER tick;  
    QueryPerformanceFrequency(&ticksPerSecond);
    QueryPerformanceCounter(&tick);
    return (tick.QuadPart)/(ticksPerSecond.QuadPart/1000000);
}

}

#endif
