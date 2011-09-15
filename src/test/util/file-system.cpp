#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include "util/file-system.h"

namespace Util{

static int upperCase(int c){
    return toupper(c);
}

std::string upperCaseAll(std::string str){
    std::transform(str.begin(), str.end(), str.begin(), upperCase);
    return str;
}

Filesystem::AbsolutePath getDataPath2(){
    return Filesystem::AbsolutePath(".");
}

}

void testGetFiles(){
    std::vector<Filesystem::AbsolutePath> paths = Storage::instance().getFiles(Filesystem::AbsolutePath("data"), Filesystem::RelativePath("m*/*.txt"), false);
    for (std::vector<Filesystem::AbsolutePath>::iterator it = paths.begin(); it != paths.end(); it++){
        std::cout << it->path() << std::endl;
    }
}

int main(){
    testGetFiles();
}
