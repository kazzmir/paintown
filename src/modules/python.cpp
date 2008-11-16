#include "../script.h"
#include "python.h"
#ifdef HAVE_PYTHON
#include <Python.h>

PythonEngine::PythonEngine():
Script::Engine(){
}

void PythonEngine::init(){
    Py_Initialize();
}

void PythonEngine::shutdown(){
}

PythonEngine::~PythonEngine(){
}

#endif
