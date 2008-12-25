#ifdef HAVE_PYTHON

#include "../script.h"
#include "python.h"
#include <string>
#include "../globals.h"
#include "../util/funcs.h"
#include "../world.h"
#include <sstream>

#include <Python.h>

using namespace std;

/* get the length of the level */
static PyObject * paintown_levelLength(PyObject * dummy, PyObject * args){
    PyObject * cobject;

    if (PyArg_ParseTuple(args, "O", &cobject)){
        World * world = (World*) PyCObject_AsVoidPtr(cobject);
        int length = world->levelLength();
        Py_DECREF(cobject);
        return Py_BuildValue("i", length);
    }

    Py_INCREF(Py_None);
    return Py_None;
}

/* methods that the python world use to talk to the paintown engine */
static PyMethodDef PaintownModule[] = {
    {"levelLength",  paintown_levelLength, METH_VARARGS, "Register a paintown engine."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PythonEngine::PythonEngine(const string & path):
Script::Engine(),
path(path){
    Global::debug(1) << "Loading python.." << endl;
    Py_Initialize();
    Py_InitModule("paintown", PaintownModule);
    Global::debug(1) << "Load module " << path << endl;
    
    /* TODO: Use PySys_GetObject() to get sys.path and then use
     * PyString_FromStringAndSize() and PyList_Append()
     */
    ostringstream python_string;
    python_string << "x = \"" << (Util::getDataPath() + path) << "\"; import sys; sys.path.append(x[0:x.rfind('/')]); sys.path.append('scripts.zip/build/modules');";
    Global::debug(1) << "Executing '" << python_string.str() << "'" << endl;
    Global::debug(1) << "Python: " << PyRun_SimpleString(python_string.str().c_str()) << endl;
    int from = path.rfind("/")+1;
    int to = path.rfind(".");
    module = path.substr(from, to - from);
}

void PythonEngine::init(){
}

void PythonEngine::shutdown(){
}

void PythonEngine::tick(){
    PyObject * api_module = PyImport_ImportModule("api");
    if (api_module == NULL){
        PyErr_Print();
    }
    PyObject * create = PyObject_GetAttrString(api_module, "tick");
    if (create == NULL){
        PyErr_Print();
    }
    Py_DECREF(api_module);
    PyObject * result = PyObject_CallFunction(create, NULL);
    if (result == NULL){
        PyErr_Print();
    } else {
        Py_DECREF(result);
    }
    Py_DECREF(create);
}
        
void PythonEngine::createWorld(const World & world){
    Global::debug(1) << "Loading module " << module << endl;
    PyObject * python_module = PyImport_ImportModule(module.c_str());
    Global::debug(1) << "Loaded " << python_module << endl;
    if (python_module == NULL){
        PyErr_Print();
    }

    PyObject * api_module = PyImport_ImportModule("api");
    if (api_module == NULL){
        PyErr_Print();
    }
    PyObject * create = PyObject_GetAttrString(api_module, "createWorld");
    if (create == NULL){
        PyErr_Print();
    }
    Py_DECREF(api_module);
    PyObject * cobject = PyCObject_FromVoidPtr((void*) &world, NULL);
    if (cobject == NULL){
        PyErr_Print();
    }
    PyObject * result = PyObject_CallFunction(create, (char*) "(O)", cobject);
    if (result == NULL){
        PyErr_Print();
    } else {
        Py_DECREF(result);
    }
    Py_DECREF(create);
    Py_DECREF(cobject);

    /*
    ostringstream python_string;
    Global::debug(1) << "[python] Create world" << endl;
    python_string << "if True: import build.modules.api; build.modules.api.createWorld()" << endl;
    Global::debug(1) << "Python: " << PyRun_SimpleString(python_string.str().c_str()) << endl;
    */
}

void PythonEngine::destroyWorld(const World & world){
}

PythonEngine::~PythonEngine(){
    Py_Finalize();
}

#endif
