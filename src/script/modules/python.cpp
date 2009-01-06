#ifdef HAVE_PYTHON

#include <Python.h>

#include "object/character.h"
#include "script/script.h"
#include "python.h"
#include <string>
#include "globals.h"
#include "util/funcs.h"
#include "world.h"
#include "level/blockobject.h"
#include "factory/object_factory.h"
#include "object/enemy.h"
#include <sstream>

/* the public api */
static const char * paintown_api = "paintown";

/* the internal api */
static const char * paintown_internal = "paintown_internal";

using namespace std;
using namespace PythonModule;

/* automatically decrements the reference count when it goes out of scope
 */
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

    static PyObject * cacheCharacter(PyObject * dummy, PyObject * args){
        char * path;
        if (PyArg_ParseTuple(args, "s", &path)){
            string spath(path);
            BlockObject block;
            block.setType(ObjectFactory::EnemyType);
            block.setPath(Util::getDataPath() + spath);
            Enemy * enemy = (Enemy*) ObjectFactory::createObject(&block);
            delete enemy;
        }

        Py_INCREF(Py_None);
        return Py_None;
    }

    /* create a new character (enemy really) and add it into the world
     * Returns the python object that represents the character.
     */
    static PyObject * addCharacter(PyObject * dummy, PyObject * args){
        PyObject * cworld;
        int health = 0;
        int x = 0;
        int z = 0;
        int map = 0;
        char * name;
        char * path;

        if (PyArg_ParseTuple(args, "Ossiiii", &cworld, &path, &name, &map, &health, &x, &z)){
            string spath(path);
            string sname(name);
            BlockObject block;
            block.setPath(Util::getDataPath() + spath);
            block.setName(sname);
            block.setAlias(sname);
            block.setMap(map);
            block.setId(ObjectFactory::getNextObjectId());
            block.setType(ObjectFactory::EnemyType);
            block.setHealth(health);
            block.setCoords(x,z);
            /* the factory will create an enemy and the enemy
             * will call into the script module to create a new character,
             * so just return that handle
             */
            Enemy * enemy = (Enemy*) ObjectFactory::createObject(&block);
            World * world = (World*) PyCObject_AsVoidPtr(cworld);
            world->addEnemy(enemy);
            Py_INCREF((PyObject*) enemy->getScriptObject());
            return (PyObject*) enemy->getScriptObject();
        }
        
        Py_INCREF(Py_None);
        return Py_None;
    }

    static PyObject * findObject(PyObject * dummy, PyObject * args){
        PyObject * cworld;
        int id = 0;
        if (PyArg_ParseTuple(args, "Oi", &cworld, &id)){
            World * world = (World*) PyCObject_AsVoidPtr(cworld);
            Object * obj = world->findObject(id);
            if (obj == NULL || obj->getScriptObject() == NULL){
                Py_INCREF(Py_None);
                return Py_None;
            }
            PyObject * script = (PyObject*) obj->getScriptObject();
            Py_INCREF(script);
            return script;
        }

        Py_INCREF(Py_None);
        return Py_None;
    }

    static PyObject * getObjects(PyObject * dummy, PyObject * args){
        PyObject * list = PyList_New(0);
        PyObject * cworld = NULL;

        if (PyArg_ParseTuple(args, "O", &cworld)){
            World * world = (World*) PyCObject_AsVoidPtr(cworld);
            vector<Object*> objects = world->getObjects();
            for (vector<Object*>::iterator it = objects.begin(); it != objects.end(); it++){
                Object * object = *it;
                if (object->getScriptObject() != NULL){
                    PyObject * script = (PyObject*) object->getScriptObject();
                    PyList_Append(list, script);
                }
            }
        }

        return list;
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
            Character * object = (Character*) PyCObject_AsVoidPtr(cobject);
            int length = object->getHealth();
            return Py_BuildValue("i", length);
        }

        Py_INCREF(Py_None);
        return Py_None;
    }

    static PyObject * setHealth(PyObject * dummy, PyObject * args){
        PyObject * cobject;

        int much = 0;
        if (PyArg_ParseTuple(args, "Oi", &cobject, &much)){
            Object * object = (Object*) PyCObject_AsVoidPtr(cobject);
            object->setHealth(much);
        }

        Py_INCREF(Py_None);
        return Py_None;
    }

    static PyObject * getDouble(PyObject * dummy, PyObject * args, const double (Object::*get)() const){
        PyObject * cobject;
        if (PyArg_ParseTuple(args, "O", &cobject)){
            Object * object = (Object*) PyCObject_AsVoidPtr(cobject);
            return Py_BuildValue("d", (object->*get)());
        }

        Py_INCREF(Py_None);
        return Py_None;
    }

    static PyObject * getX(PyObject * dummy, PyObject * args){
        return getDouble(dummy, args, &Object::getX);
    }
    
    static PyObject * getY(PyObject * dummy, PyObject * args){
        return getDouble(dummy, args, &Object::getY);
    }
    
    static PyObject * getZ(PyObject * dummy, PyObject * args){
        return getDouble(dummy, args, &Object::getZ);
    }

    static PyObject * setDouble(PyObject * dummy, PyObject * args, void (Object::*set)(const double value)){
        PyObject * cobject;
        double value = 0;
        if (PyArg_ParseTuple(args, "Od", &cobject, &value)){
            Object * object = (Object*) PyCObject_AsVoidPtr(cobject);
            (object->*set)(value);
        }

        Py_INCREF(Py_None);
        return Py_None;
    }
    
    static PyObject * setX(PyObject * dummy, PyObject * args){
        return setDouble(dummy, args, &Object::setX);
    }
    
    static PyObject * setY(PyObject * dummy, PyObject * args){
        return setDouble(dummy, args, &Object::setY);
    }
    
    static PyObject * setZ(PyObject * dummy, PyObject * args){
        return setDouble(dummy, args, &Object::setZ);
    }

    static PyObject * getId(PyObject * dummy, PyObject * args){
        PyObject * cobject;
        if (PyArg_ParseTuple(args, "O", &cobject)){
            Object * object = (Object*) PyCObject_AsVoidPtr(cobject);
            return Py_BuildValue("i", (int) object->getObjectId());
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
    {"addCharacter", PaintownLevel::addCharacter, METH_VARARGS, "Create a new character"},
    {"cacheCharacter", PaintownLevel::cacheCharacter, METH_VARARGS, "Ensure a character is cached for future lookups"},
    {"findObject", PaintownLevel::findObject, METH_VARARGS, "Find an object by its id."},
    {"getObjects", PaintownLevel::getObjects, METH_VARARGS, "Get all the existing objects."},
    {"getHealth", PaintownCharacter::getHealth, METH_VARARGS, "Get the health of a character."},
    {"setHealth", PaintownCharacter::setHealth, METH_VARARGS, "Set the health of a character."},
    {"getX", PaintownCharacter::getX, METH_VARARGS, "Get the X coordinate of an object."},
    {"getY", PaintownCharacter::getY, METH_VARARGS, "Get the Y coordinate of an object."},
    {"getZ", PaintownCharacter::getZ, METH_VARARGS, "Get the Z coordinate of an object."},
    {"setX", PaintownCharacter::setX, METH_VARARGS, "Set the X coordinate of an object."},
    {"setY", PaintownCharacter::setY, METH_VARARGS, "Set the Y coordinate of an object."},
    {"setZ", PaintownCharacter::setZ, METH_VARARGS, "Set the Z coordinate of an object."},
    {"getId", PaintownCharacter::getId, METH_VARARGS, "Get the id of an object."},
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

/* this method is not called from python so the Py_INCREF keeps an
 * extra reference to the python object so that it won't get collected.
 * destroyObject will Py_DECREF this object when the paintown object
 * is deleted.
 */
static void * callCreateMethod(void * obj, const char * method){
    Global::debug(1) << "Create a character from " << obj << endl;
    AutoObject api_module(PyImport_ImportModule((char*)paintown_api));
    AutoObject create(PyObject_GetAttrString(api_module.getObject(), (char*) method));
    AutoObject cobject(PyCObject_FromVoidPtr((void*) obj, NULL));
    AutoObject result(PyObject_CallFunction(create.getObject(), (char*) "(O)", cobject.getObject()));

    PyObject * ret = result.getObject();
    Py_INCREF(ret);
    Global::debug(1) << "Created python character " << ret << endl;
    return ret;
}

void * PythonEngine::createCharacter(void * obj){
    return callCreateMethod(obj, "createCharacter");
}

void * PythonEngine::createPlayer(void * obj){
    return callCreateMethod(obj, "createPlayer");
}

void PythonEngine::destroyObject(void * handle){
    PyObject * obj = (PyObject*) handle;
    Py_DECREF(obj);
}
    
void PythonEngine::characterAttacked(void * from, void * handle){
    PyObject * object = (PyObject*) from;
    AutoObject function(PyObject_GetAttrString(object, "didAttack"));
    PyObject * maybeNull = (PyObject*) handle;
    if (handle == NULL){
        maybeNull = Py_None;
        Py_INCREF(maybeNull);
    }
    AutoObject result(PyObject_CallFunction(function.getObject(), (char*) "(O)", maybeNull));
    if (handle == NULL){
        Py_DECREF(maybeNull);
    }
}

void PythonEngine::objectCollided(void * from, void * handle){
    PyObject * object = (PyObject*) from;
    AutoObject function(PyObject_GetAttrString(object, "didCollide"));
    PyObject * maybeNull = (PyObject*) handle;
    if (handle == NULL){
        maybeNull = Py_None;
        Py_INCREF(maybeNull);
    }
    AutoObject result(PyObject_CallFunction(function.getObject(), (char*) "(O)", maybeNull));
    if (handle == NULL){
        Py_DECREF(maybeNull);
    }
}
    
void PythonEngine::objectTakeDamage(void * from, void * handle, int damage){
    PyObject * object = (PyObject*) from;
    AutoObject function(PyObject_GetAttrString(object, "takeDamage"));
    PyObject * maybeNull = (PyObject*) handle;
    if (handle == NULL){
        maybeNull = Py_None;
        Py_INCREF(maybeNull);
    }
    AutoObject result(PyObject_CallFunction(function.getObject(), (char*) "(Oi)", maybeNull, damage));
    if (handle == NULL){
        Py_DECREF(maybeNull);
    }
}
    
void PythonEngine::objectTick(void * handle){
    PyObject * object = (PyObject*) handle;
    AutoObject tick(PyObject_GetAttrString(object, "tick"));
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
