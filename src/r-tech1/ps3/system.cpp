#ifdef PS3


#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include "r-tech1/system.h"
#include "r-tech1/funcs.h"

#ifdef USE_SDL
#include <SDL/SDL.h>
#endif

#ifdef USE_SDL2
#include <SDL2/SDL.h>
#endif

#include <cstdlib>

#include <errno.h>    // For errno
#include <cstring>    // For strerror()
#include <fcntl.h>
#include <ctime>
#include <iostream>


/* ps3 doesn't have an implementation of access() yet. if it gets one this function
 * can be removed.
 */
int access(const char * path, int mode){
    struct stat information;
    int ok = stat(path, &information);
    // printf("stat of '%s' is %d\n", path.c_str(), ok);
    if (ok == 0){
        if (mode == R_OK){
            if (((information.st_mode & S_IRUSR) == S_IRUSR) ||
                ((information.st_mode & S_IRGRP) == S_IRGRP) ||
                ((information.st_mode & S_IROTH) == S_IROTH)){
                return 0;
            } else {
            /* handle other modes if they become useful to us */
                return -1;
            }
       } else {
           return -1;
       }
    } else {
        // perror("stat");
        return -1;
    }
}

bool System::isDirectory(const std::string & path){
    struct ::stat fileStat; // Use :: to refer to the global stat function
    int result = ::stat(path.c_str(), &fileStat); // Use :: to refer to the global stat function
    if (result != 0) {
        DebugLog2 << "Error getting file stat: " <<  strerror(errno) << std::endl;
        return false; // Error getting file stat
    }

    return S_ISDIR(fileStat.st_mode);
}

void System::makeDirectory(const std::string & path){
    int result = mkdir(path.c_str(), 0777); // Create directory with full permissions
    if (result != 0) {
        // Handle error if mkdir fails
        DebugLog2 << "Error creating directory: " << strerror(errno) << std::endl;
    }
}

bool System::readableFile(const std::string & path){
    return !isDirectory(path) && readable(path);
}

bool System::readable(const std::string & path){
    int fd = open(path.c_str(), O_RDONLY);
    if (fd >= 0) {
        close(fd);
        return true;
    }
    return false;
}

uint64_t System::getModificationTime(const std::string & path){
    struct ::stat fileStat; // Use :: to refer to the global stat function
    int result = ::stat(path.c_str(), &fileStat); // Use :: to refer to the global stat function
    if (result != 0) {
        DebugLog2 << "Error getting file stat: " << strerror(errno) << std::endl;
        return 0; // Return 0 if there's an error
    }

    return static_cast<uint64_t>(fileStat.st_mtime);
}

uint64_t System::currentMilliseconds(){
#ifdef USE_SDL
    return SDL_GetTicks();
#elif USE_SDL2
    return SDL_GetTicks();
#else
    // TODO - FIXME
    return 0;
#endif
}

static void * start_memory = 0;
unsigned long System::memoryUsage(){
    // TODO - FIXME
    return 0;
 }

/* call startMemoryUsage once at the very beginning of the program */
void System::startMemoryUsage(){
    // TODO - FIXME
}

#endif