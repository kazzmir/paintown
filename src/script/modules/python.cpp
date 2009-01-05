#ifdef HAVE_PYTHON

#include <Python.h>

#include "script/character.h"
#include "object/character.h"
#include "script/script.h"
#include "python.h"
#include <string>
#include "globals.h"
#include "util/funcs.h"
#include "world.h"
#include "factory/object_factory.h"
#include <sstream>

/* the public api */
static const char * paintown_api = "paintown";

/* the internal api */
static const char * paintown_internal = "paintown_internal";

using namespace std;
using namespace PythonModule;

AutoObject::AutoObject(PyObject* object):
object(object){
    if (object == NULL){
        PyErr_Print();
    }
}

AutoObject::~AutoObject(){
    Py_DECREF(object);
}

PyObject * AutoObject::getObject(){
    return object;
}

namespace PaintownLevel{

/* get the length of the level */
static PyObject * levelLength(PyObject * dummy, PyObject * args){

    PyObject * cobject;

    if (PyArg_ParseTuple(args, "O", &cobject)){
        World * world = (World*) PyCObject_AsVoidPtr(cobject);
        int length = world->levelLength();
        return Py_BuildValue("i", length);
    }

    Py_INCREF(Py_None);
    return Py_None;
}

/* Returns a list of existing objects in a block
 *   paintown_internal.getBlockObjects(self.world, type)
 */
static PyObject * getBlockObjects(PyObject * dummy, PyObject * args){
    Py_INCREF(Py_None);
    return Py_None;
}

/* Returns the id corresponding to latest block
 */
static PyObject * currentBlock(PyObject * dummy, PyObject * args){

    PyObject * cobject;

    if (PyArg_ParseTuple(args, "O", &cobject)){
        World * world = (World*) PyCObject_AsVoidPtr(cobject);
        return Py_BuildValue("i", world->currentBlock());
    }

    Py_INCREF(Py_None);
    return Py_None;
}

/* Returns the object type for a given identifier */
static PyObject * objectType(PyObject * dummy, PyObject * args){
    Py_INCREF(Py_None);
    return Py_None;
}

/* Returns the integer corresponding to enemy types
 */
static PyObject * enemyType(PyObject * dummy, PyObject * args){
    return Py_BuildValue("i", ObjectFactory::EnemyType);
}

/* Returns the integer corresponding to item types
 */
static PyObject * itemType(PyObject * dummy, PyObject * args){
    return Py_BuildValue("i", ObjectFactory::ItemType);
}

}

namespace PaintownCharacter{
    static PyObject * getHealth(PyObject * dummy, PyObject * args){

        PyObject * cobject;

        if (PyArg_ParseTuple(args, "O", &cobject)){
            Script::Character * character = (Script::Character*) PyCObject_AsVoidPtr(cobject);
            int length = character->getCharacter()->getHealth();
            return Py_BuildValue("i", length);
        }

        Py_INCREF(Py_None);
        return Py_None;
    }
}

/* methods that the python world use to talk to the paintown engine */
static PyMethodDef PaintownModule[] = {
    {"levelLength",  PaintownLevel::levelLength, METH_VARARGS, "Get the length of the level."},
    {"currentBlock", PaintownLevel::currentBlock, METH_VARARGS, "Get the current block."},
    {"enemyType", PaintownLevel::enemyType, METH_VARARGS, "Get the type of the enemy class."},
    {"itemType", PaintownLevel::itemType, METH_VARARGS, "Get the type of the item class."},
    {"getBlockObjects", PaintownLevel::getBlockObjects, METH_VARARGS, "Get the objects in a block."},
    {"objectType", PaintownLevel::objectType, METH_VARARGS, "Get the type of an object."},
    {"getHealth", PaintownCharacter::getHealth, METH_VARARGS, "Get the health of a character."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

/* initialize the intepreter and set up the module paths */
PythonEngine::PythonEngine(const string & path):
Script::Engine(),
path(path){
    Global::debug(1) << "Loading python.." << endl;
    Py_Initialize();

    Py_InitModule((char*) paintown_internal, PaintownModule);
    Global::debug(1) << "Load module " << path << endl;

    /* TODO: Use PySys_GetObject() to get sys.path and then use
     * PyString_FromStringAndSize() and PyList_Append()
     */
    ostringstream python_string;
    python_string << "x = \"" << (Util::getDataPath() + path) << "\"; import sys; sys.path.append(x[0:x.rfind('/')]); sys.path.append('scripts.zip/build/script/modules');";
    Global::debug(1) << "Executing '" << python_string.str() << "'" << endl;
    Global::debug(1) << "Python: " << PyRun_SimpleString(python_string.str().c_str()) << endl;
    int from = path.rfind("/")+1;
    int to = path.rfind(".");
    module = path.substr(from, to - from);

    /* Load the user module so that it can register itself */
    Global::debug(1) << "Loading module " << module << endl;
    PyObject * user_module = PyImport_ImportModule((char*) module.c_str());
    Global::debug(1) << "Loaded " << user_module << endl;
    if (user_module == NULL){
        PyErr_Print();
    }
    Py_DECREF(user_module);
}

void PythonEngine::init(){
}

void PythonEngine::shutdown(){
}
    
void * PythonEngine::createCharacter(Script::Character * obj){
    Global::debug(1) << "Create a character from " << obj << endl;
    AutoObject api_module(PyImport_ImportModule((char*)paintown_api));
    AutoObject create(PyObject_GetAttrString(api_module.getObject(), "createCharacter"));
    AutoObject cobject(PyCObject_FromVoidPtr((void*) obj, NULL));
    AutoObject result(PyObject_CallFunction(create.getObject(), (char*) "(O)", cobject.getObject()));

    PyObject * ret = result.getObject();
    Py_INCREF(ret);
    Global::debug(1) << "Created python character " << ret << endl;
    return ret;
}

void PythonEngine::destroyCharacter(void * handle){
    PyObject * obj = (PyObject*) handle;
    Py_DECREF(obj);
}
    
void PythonEngine::characterTick(void * handle){
    PyObject * character = (PyObject*) handle;
    AutoObject tick(PyObject_GetAttrString(character, "tick"));
    AutoObject result(PyObject_CallFunction(tick.getObject(), NULL));
}

/* called when for each logic frame */
void PythonEngine::tick(){
    PyObject * api_module = PyImport_ImportModule((char*)paintown_api);
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

/* called when the world is created */
void PythonEngine::createWorld(const World & world){
    /* call our api to create the world */
    PyObject * api_module = PyImport_ImportModule((char*)paintown_api);
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
