#ifndef mugen_converter_test
#define mugen_converter_test

#include <iostream>

#include <Python.h>

using namespace std;

int main(int argc, char ** argv){
    if (argc > 1){
        std::cout << "Starting character: " << argv[1] << endl;
    } else {
        std::cout << "Usage: ./test character.py" << endl;
    }
}

#endif
