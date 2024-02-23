#ifdef WINDOWS

#include <windows.h>
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


bool System::isDirectory(const std::string & path){
    DWORD attributes = GetFileAttributes(path.c_str());
    if (attributes != INVALID_FILE_ATTRIBUTES) {
        return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    }
    return false;
}

bool System::readableFile(const std::string & path){
    return !isDirectory(path) && readable(path);
}
    
bool System::readable(const std::string & path){
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
}

void System::makeDirectory(const std::string & path){    
    if (CreateDirectory(path.c_str(), nullptr) || ERROR_ALREADY_EXISTS == GetLastError()) {
        return;
    } else {
        DebugLog2 << "Error creating directory: " << GetLastError() << std::endl;
    }
}

uint64_t System::currentMilliseconds(){
#ifdef USE_SDL
    return SDL_GetTicks();
#elif USE_SDL2
    return SDL_GetTicks();
#else
    LARGE_INTEGER ticksPerSecond;
    LARGE_INTEGER tick;  
    QueryPerformanceFrequency(&ticksPerSecond);
    QueryPerformanceCounter(&tick);
    return (tick.QuadPart)/(ticksPerSecond.QuadPart/1000000)/1000;
#endif
}
    
uint64_t System::getModificationTime(const std::string & path){
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
            return 0;
        }
    } 

    DebugLog2 << "Error opening file: " << GetLastError() << std::endl;
    return 0;
}

unsigned long System::memoryUsage(){
    /*
    HANDLE id = GetCurrentProcess(); PROCESS_MEMORY_COUNTERS info; BOOL okay =
    GetProcessMemoryInfo(id, &info, sizeof(info)); if (okay){ return
    info.WorkingSetSize; }
    */
    return 0; }

/* call startMemoryUsage once at the very beginning of the program */
void System::startMemoryUsage(){
    /* FIXME */
}

#endif
