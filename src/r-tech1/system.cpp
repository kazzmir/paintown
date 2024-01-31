#include <string>
#include "r-tech1/system.h"
#include <stdint.h>
#include <stdio.h>
#include <fstream>
#include "r-tech1/debug.h"

#ifdef USE_SDL
#include <SDL/SDL.h>
#endif

#ifdef USE_SDL2
#include <SDL2/SDL.h>
#endif

#ifndef WINDOWS
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#else
#include <chrono>
#include "libs/filesystem/fs-wrapper.h"
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

#else

// FIXME For the time being on cross-build, needs to be corrected


uint64_t System::getModificationTime(const std::string & path){
#ifndef WINDOWS
    // FIXME figure out how to convert to unit64_t and remove auto
    //auto p = fs::path(path);
    //auto time = fs::last_write_time(p);
    return 0;
#else
    HANDLE hFile = CreateFile(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

    if (hFile != INVALID_HANDLE_VALUE) {
        FILETIME modificationTime;
        if (GetFileTime(hFile, nullptr, nullptr, &modificationTime)) {
            CloseHandle(hFile);

            ULARGE_INTEGER uli;
            uli.LowPart = modificationTime.dwLowDateTime;
            uli.HighPart = modificationTime.dwHighDateTime;

            return uli.QuadPart;
        } else {
            DebugLog2 << "Error getting file modification time: " << GetLastError() << std::endl;
            CloseHandle(hFile);
            return 0; // Return 0 or handle error as appropriate for your use case
        }
    } else {
        DebugLog2 << "Error opening file: " << GetLastError() << std::endl;
        return 0; // Return 0 or handle error as appropriate for your use case
    }
#endif
}

void System::makeDirectory(const std::string & path){
#ifndef WINDOWS
    fs::path p = fs::path(path);
    fs::create_directory(p);
#else
    if (CreateDirectory(path.c_str(), nullptr) || ERROR_ALREADY_EXISTS == GetLastError()) {
        return;
    } else {
        DebugLog2 << "Error creating directory: " << GetLastError() << std::endl;
    }
#endif
}

bool System::isDirectory(const std::string & path){
#ifndef WINDOWS
    return fs::is_directory(fs::path(path));
#else
    DWORD attributes = GetFileAttributes(path.c_str());
    if (attributes != INVALID_FILE_ATTRIBUTES) {
        return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    }
    return false;
#endif
}

bool System::readable(const std::string & path){
#ifndef WINDOWS
    fs::path p = fs::path(path);
    fs::perms permissions = fs::status(p).permissions();

    return ((permissions & fs::perms::owner_read) != fs::perms::none &&
            (permissions & fs::perms::group_read) != fs::perms::none &&
            (permissions & fs::perms::others_read) != fs::perms::none);
#else
    DWORD attributes = GetFileAttributes(path.c_str());
    if (attributes != INVALID_FILE_ATTRIBUTES) {
        // Check if the file is readable
        if ((attributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
            HANDLE hFile = CreateFile(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (hFile != INVALID_HANDLE_VALUE) {
                CloseHandle(hFile);
                return true;
            } else {
                DebugLog2 << "Error opening file: " << GetLastError() << std::endl;
                return false;
            }
        }

        // For directories, assume readable
        return true;
    } 
    return false;
#endif
}

uint64_t System::currentMilliseconds(){
#ifdef USE_SDL
    return SDL_GetTicks();
#elif USE_SDL2
    return SDL_GetTicks();
#endif
}

unsigned long System::memoryUsage(){
    return 0;
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

