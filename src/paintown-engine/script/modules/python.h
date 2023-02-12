#ifndef _paintown_python_h
#define _paintown_python_h

#ifdef HAVE_PYTHON

#include <Python.h>
#include "../script.h"
#include <string>
#include "r-tech1/file-system.h"

class World;
/* forward declaring this doesn't work.
 * struct PyObject;
 */

class PythonEngine: public Script::Engine {
public:
    PythonEngine(const Filesystem::RelativePath & path);

    virtual void init();
    virtual void shutdown();
    virtual void createWorld(const World & world);
    virtual void destroyWorld(const World & world);
    virtual void * createCharacter(void * character);
    virtual void * createPlayer(void * player);
    virtual void destroyObject(void * handle);
    virtual void objectTick(void * handle);
    virtual void objectTakeDamage(void * who, void * handle, int damage);
    virtual void objectCollided(void * me, void * him);
    virtual void characterAttacked(void * me, void * him);
    virtual void tick();

    virtual ~PythonEngine();
protected:
    Filesystem::RelativePath path;
    std::string module;
    PyObject * user;
};

namespace PythonModule{
    class AutoObject{
    public:
        AutoObject(PyObject*);
        virtual ~AutoObject();

        PyObject * getObject();

    private:
        PyObject * object;
    };
}

#endif

#endif
