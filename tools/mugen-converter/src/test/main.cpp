#ifndef mugen_converter_test
#define mugen_converter_test

#include <iostream>

#include <Python.h>

using namespace std;

static int error(const std::string & message){
    std::cout << message << endl;
    PyErr_Print();
    return -1;
}

int main(int argc, char ** argv){
    if (argc > 1){
        Py_Initialize();
        
        /* NOTE need to make sure we are trying to load in the same directory (is there a work around?) */
        PyRun_SimpleString("import sys"); 
        PyRun_SimpleString("sys.path.append('./')"); 
        
        PyObject * name, * module, * dict, *charClass, * character;
        
        name = PyString_FromString(argv[1]);
        if (name == NULL){
            return error("Couldn't convert string");
        }
        
        std::cout << "Importing Module: " << argv[1] << endl;
        module = PyImport_Import(name);
        if (module == NULL){
            return error("Couldn't load module: " + std::string(argv[1]));
        }
        
        dict = PyModule_GetDict(module);
        
        charClass = PyDict_GetItemString(dict, argv[1]);
        
        if (PyCallable_Check(charClass)){
            character = PyObject_CallObject(charClass, NULL);
        } else {
            return error("Couldn't verify class.");
        }
        //PyString_AsString(PyObject_CallMethod(character, "someFunc", NULL));
        std::string output = PyString_AsString(PyObject_GetAttrString(character, "name"));
        
        std::cout << "Loaded character: " << output << endl;
        
        Py_DECREF(character);
        Py_DECREF(module);
        Py_DECREF(name);
        
        Py_Finalize();
        
        return 0;
    }
    
    std::cout << "Usage: ./test character.py" << endl;
    
    return 0;
    
}

#endif
