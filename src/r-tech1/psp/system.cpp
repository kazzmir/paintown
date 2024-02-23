#ifdef MINPSPW

#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include "r-tech1/system.h"

/* psp doesn't have an implementation of access()? if it gets one this function
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
    // TODO: FIXME
    return false;
}

void System::makeDirectory(const std::string & path){
    // TODO: FIXME
}

void makeAllDirectory(const std::string & path){
    // TODO: FIXME
}

bool System::readableFile(const std::string & path){
    // TODO: FIXME
    return false;
}

bool System::readable(const std::string & path){
    // TODO: FIXME
    return false;
}

uint64_t System::getModificationTime(const std::string & path){
    // TODO: FIXME
    return 0;
}

uint64_t System::currentMilliseconds(){
    // TODO: FIXME
    return 0;
}

unsigned long System::memoryUsage(){
    // TODO: FIXME
    return 0;
}

/* call startMemoryUsage once at the very beginning of the program */
void System::startMemoryUsage(){
    // TODO: FIXME
}

#endif
