#ifdef WINDOWS

#include <sys/types.h>
#include <sys/stat.h>
#include <windows.h>
#include <psapi.h>
#include <fstream>
#include "util/system.h"
#include <dirent.h>

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

void makeDirectory(const std::string & path){
    mkdir(path.c_str());
}

uint64_t currentMilliseconds(){
    LARGE_INTEGER ticksPerSecond;
    LARGE_INTEGER tick;  
    QueryPerformanceFrequency(&ticksPerSecond);
    QueryPerformanceCounter(&tick);
    return (tick.QuadPart)/(ticksPerSecond.QuadPart/1000000)/1000;
}
    
uint64_t getModificationTime(const std::string & path){
    struct _stat info;
    if (_stat(path.c_str(), &info) == 0){
        return info.st_mtime;
    }
    return 0;
}

unsigned long memoryUsage(){
    /*
    HANDLE id = GetCurrentProcess(); PROCESS_MEMORY_COUNTERS info; BOOL okay =
    GetProcessMemoryInfo(id, &info, sizeof(info)); if (okay){ return
    info.WorkingSetSize; }
    */
    return 0; }

/* call startMemoryUsage once at the very beginning of the program */
void startMemoryUsage(){
    /* FIXME */
}


}

#endif
