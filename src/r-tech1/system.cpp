#include <string>
#include "r-tech1/system.h"
#include <stdint.h>
#include <stdio.h>
#include <fstream>

#ifdef USE_SDL
#include <SDL/SDL.h>
#endif

#ifndef WINDOWS
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#endif

#ifndef WINDOWS

/* devkitpro doesn't have an implementation of access() yet. if it gets one this function
 * can be removed.
 */
#if defined(WII) || defined(PS3)
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
#endif

static bool isReadable(const std::string & path){
    return access(path.c_str(), R_OK) == 0;
}

bool System::isDirectory(const std::string & path){
    struct stat info;
    if (stat(path.c_str(), &info) == 0){
        if (S_ISDIR(info.st_mode) == 1){
            return true;
        } else {
            return false;
        }
    }
    return false;
}

bool System::readableFile(const std::string & path){
    return isReadable(path) && ! isDirectory(path);
}

bool System::readable(const std::string & path){
    return isReadable(path);
}
    
void System::makeDirectory(const std::string & path){
    mkdir(path.c_str(), 0777);
}

/*
uint64_t System::currentMicroseconds(){
    struct timeval hold;
    gettimeofday(&hold, NULL);
    return hold.tv_sec * 1000 * 1000 + hold.tv_usec;
}
*/
    
uint64_t System::currentMilliseconds(){
#ifdef USE_SDL
    return SDL_GetTicks();
#else
    struct timeval hold;
    gettimeofday(&hold, NULL);
    return (hold.tv_sec * 1000 * 1000 + hold.tv_usec) / 1000;
#endif
}

uint64_t System::getModificationTime(const std::string & path){
    struct stat data;
    if (stat(path.c_str(), &data) == 0){
        return data.st_mtime;
    }
    return 0;
}

static void * start_memory = 0;
unsigned long System::memoryUsage(){
    void * here = sbrk(0);
    /* hopefully the heap is growing up */
    return (char*) here - (char*) start_memory;
}

void System::startMemoryUsage(){
    start_memory = sbrk(0);
}

#endif
    
void System::makeAllDirectory(const std::string & path){
    size_t last = path.find('/');
    while (last != std::string::npos){
        std::string sofar = path.substr(0, last);
        if (sofar != ""){
            makeDirectory(sofar);
        }
        last = path.find('/', last + 1);
    }
    makeDirectory(path);
}
 
uint64_t System::currentSeconds(){
    return currentMilliseconds() / 1000;
}

