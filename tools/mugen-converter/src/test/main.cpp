#include <iostream>

#include <Python.h>

#include "character.h"

using namespace std;

static int error(const std::string & message){
    std::cout << message << std::endl;
    //PyErr_Print();
    return -1;
}

int main(int argc, char ** argv){
    if (argc > 1){
        Py_Initialize();
        
        /* NOTE need to make sure we are trying to load in the same directory (is there a work around?) */
        PyRun_SimpleString("import sys"); 
        PyRun_SimpleString("sys.path.append('./')"); 
       
        try {
            Character * character = new Character(argv[1]);
            character->addAttribute("name", Character::String);
            std::cout << "Character Name: " << character->getStringValue("name") << std::endl;
            delete character;
        } catch (const PyException & ex){
            error("Problem loading module! Reason: " + ex.getReason());
        }
        
        Py_Finalize();
        
        return 0;
    }
    
    std::cout << "Usage: ./test character.py" << std::endl;
    
    return 0;
    
}
