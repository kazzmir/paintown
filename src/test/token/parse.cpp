#include <iostream>
#include "r-tech1/token.h"
#include "r-tech1/tokenreader.h"
#include "r-tech1/token_exception.h"
#include "r-tech1/debug.h"
#include "r-tech1/file-system.h"


#ifndef WINDOWS

/* FIXME: dont put these methods in this test file */
Filesystem::AbsolutePath Filesystem::configFile(){
    std::ostringstream str;
    /* what if HOME isn't set? */
    str << getenv("HOME") << "/.paintownrc";
    return Filesystem::AbsolutePath(str.str());
}

Filesystem::AbsolutePath Filesystem::userDirectory(){
    std::ostringstream str;
    char * home = getenv("HOME");
    if (home == NULL){
        str << "/tmp/paintown";
    } else {
        str << home << "/.paintown/";
    }
    return Filesystem::AbsolutePath(str.str());
}

#endif


void read(const char * path){
    TokenReader reader;
    reader.readTokenFromFile(path);
}

int main(int argc, char ** argv){
    Global::setDebug(2);
    if (argc > 1){
        try{
            read(argv[1]);
            Global::debug(0) << "Pass" << std::endl;
        } catch (const TokenException & t){
            Global::debug(0) << "Failed: " << t.getTrace() << std::endl;
        }
    }
}
