
#include <string>
#include "r-tech1/system.h"
#include <stdint.h>
#include <stdio.h>
#include <fstream>
#include "r-tech1/debug.h"

#include <unistd.h>

// If not a cross build we should be good
#ifndef CROSS_BUILD

#include <sys/stat.h>
#include <sys/time.h>

#include <chrono>
#include "libs/filesystem/fs-wrapper.h"

#ifdef USE_SDL
#include <SDL/SDL.h>
#endif

#ifdef USE_SDL2
#include <SDL2/SDL.h>
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

#if 0
    // Use later?
    return fs::is_directory(fs::path(path));
#endif
}

bool System::readableFile(const std::string & path){
    return isReadable(path) && ! isDirectory(path);
}

bool System::readable(const std::string & path){
    return isReadable(path);

#if 0
    // Use later?
    fs::path p = fs::path(path);
    fs::perms permissions = fs::status(p).permissions();

    return ((permissions & fs::perms::owner_read) != fs::perms::none &&
            (permissions & fs::perms::group_read) != fs::perms::none &&
            (permissions & fs::perms::others_read) != fs::perms::none);
#endif
}
    
void System::makeDirectory(const std::string & path){
    mkdir(path.c_str(), 0777);
#if 0
    // Replace later?
    fs::path p = fs::path(path);
    fs::create_directory(p);
#endif

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
#elif USE_SDL2
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
#if 0
    // Use later?
    fs::file_time_type lastWriteTime = fs::last_write_time(path);
    std::chrono::time_point<fs::file_clock> timePoint = time_point_cast<milliseconds>(lastWriteTime);
    return static_cast<uint64_t>(timePoint.time_since_epoch().count());
#endif
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

#endif // CROSS_BUILD
    
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