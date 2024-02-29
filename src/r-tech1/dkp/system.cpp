#if defined(DEVKITPRO)

#include <string>
#include "r-tech1/system.h"
#include <stdint.h>
#include <stdio.h>
#include <fstream>
#include "r-tech1/debug.h"

#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/time.h>


#ifdef USE_SDL
#include <SDL/SDL.h>
#endif

#ifdef USE_SDL2
#include <SDL2/SDL.h>
#endif

/* GAMECUBE AND WII */
#if defined(GAMECUBE) || defined(WII)
#include <fat.h>
#endif

/* devkitpro doesn't have an implementation of access() yet. if it gets one this function
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

bool System::readableFile(const std::string & path){
    return !isDirectory(path) && readable(path);
}

/* GAMECUBE AND WII */
//#if defined(GAMECUBE) || defined(WII)
//#include <fat.h>

bool System::isDirectory(const std::string & path){
    DIR *pdir;
	struct dirent *pent = NULL;
	struct stat statbuf;
    pdir = opendir(path.c_str());
    if (!pdir){
        return false;
    }
    stat(pent->d_name, &statbuf);
    bool isDir = S_ISDIR(statbuf.st_mode);
    closedir(pdir);
    return isDir;
}
    
bool System::readable(const std::string & path){
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) == -1){
        return false;
    }
    return (S_ISREG(statbuf.st_mode) && access(path.c_str(), R_OK) == 0);
}

void System::makeDirectory(const std::string & path){    
    mkdir(path.c_str(), 0777);
}

uint64_t System::currentMilliseconds(){
#ifdef USE_SDL
    return SDL_GetTicks();
#elif USE_SDL2
    return SDL_GetTicks();
#else
    // FIXME
    return 0;
#endif
}
    
uint64_t System::getModificationTime(const std::string & path){
    // TODO
    return 0;
}

unsigned long System::memoryUsage(){
    // TODO ?
    return 0; 
}

/* call startMemoryUsage once at the very beginning of the program */
void System::startMemoryUsage(){
    /* FIXME */
}


#endif
