#ifndef mugen_converter_main
#define mugen_converter_main

#include "example.h"
#include "generator.h"

using namespace std;

int main(int argc, char ** argv){
    if (argc > 1){
        //Mugen::exampleParseCmd(argv[1]);
        Mugen::CharacterGenerator character(argv[1]);
        character.output("test-character.py");
    }
}

#endif
