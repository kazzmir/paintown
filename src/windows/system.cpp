#ifdef WINDOWS

#include <windows.h>
#include <fstream>
#include "util/system.h"

namespace System{

bool isDirectory(const std::string & path){
    return GetFileAttributes(path.c_str()) & FILE_ATTRIBUTE_DIRECTORY;
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

}

#endif
