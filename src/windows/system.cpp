#ifdef WINDOWS

#include <windows.h>
#include <fstream>
#include "util/system.h"

namespace System{

static bool isDirectory(const std::string & path){
    return GetFileAttributes(path.c_str()) & FILE_ATTRIBUTE_DIRECTORY;
}

bool readableFile(const std::string & path){
    return !isDirectory(path) && readable(path);
}
    
bool readable(const std::string & path){
	std::ifstream stream(path.c_str());
	return stream.good();
}

}

#endif
