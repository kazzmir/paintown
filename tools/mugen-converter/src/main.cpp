#ifndef mugen_converter_main
#define mugen_converter_main

#include "example.h"

using namespace std;

int main(int argc, char ** argv){
    if (argc > 1){
        Mugen::exampleParseCmd(argv[1]);
    }
}

#endif
