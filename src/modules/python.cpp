#ifdef HAVE_PYTHON

#include "../script.h"
#include "python.h"
#include <string>
#include "../globals.h"
#include "../util/funcs.h"
#include <sstream>

#include <Python.h>

using namespace std;

PythonEngine::PythonEngine(const string & path):
Script::Engine(),
path(path){
    Global::debug(1) << "Loading python.." << endl;
    Py_Initialize();
    Global::debug(1) << "Load module " << path << endl;
    ostringstream python_string;
    python_string << "x = \"" << (Util::getDataPath() + path) << "\"; import sys; sys.path.append(x[0:x.rfind('/')])";
    Global::debug(1) << "Executing '" << python_string.str() << "'" << endl;
    Global::debug(1) << "Python: " << PyRun_SimpleString(python_string.str().c_str()) << endl;
    int from = path.rfind("/")+1;
    int to = path.rfind(".");
    string module = path.substr(from, to - from);
    Global::debug(1) << "Loading module " << module << endl;
    PyObject * python_module = PyImport_ImportModule(module.c_str());
    Global::debug(1) << "Loaded " << python_module << endl;
    if (python_module == NULL){
        PyErr_Print();
    }
}

void PythonEngine::init(){
}

void PythonEngine::shutdown(){
}

PythonEngine::~PythonEngine(){
    Py_Finalize();
}

#endif
