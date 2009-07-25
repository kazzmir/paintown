#ifdef WINDOWS

#include <windows.h>

#include "util/system.h"

namespace System{

static bool isDirectory(const std::string & path){
    return GetFileAttributes(path.c_str()) & FILE_ATTRIBUTE_DIRECTORY;
}

bool readableFile(const std::string & path){
    return !isDirectory(path);
}

}

#endif
