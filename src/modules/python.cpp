#ifdef HAVE_PYTHON

#include "../script.h"
#include "python.h"
#include <string>
#include "../globals.h"

#include <Python.h>

using namespace std;

PythonEngine::PythonEngine(const string & path):
Script::Engine(),
path(path){
    Global::debug(0) << "Loading python.." << endl;
    Py_Initialize();
    Global::debug(0) << "Load module " << path << endl;
    PyObject* module = PyImport_ImportModule(path.c_str());
    Global::debug(0) << "Loaded " << module << endl;
}

void PythonEngine::init(){
}

void PythonEngine::shutdown(){
}

PythonEngine::~PythonEngine(){
    Py_Finalize();
}

#endif
