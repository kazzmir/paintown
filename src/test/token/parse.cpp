#include <iostream>
#include "util/token.h"
#include "util/tokenreader.h"
#include "util/token_exception.h"
#include "util/debug.h"

void read(const char * path){
    TokenReader reader;
    reader.readTokenFromFile(path);
}

int main(int argc, char ** argv){
    if (argc > 1){
        try{
            read(argv[1]);
            Global::debug(0) << "Pass" << std::endl;
        } catch (const TokenException & t){
            Global::debug(0) << "Failed: " << t.getTrace() << std::endl;
        }
    }
}
