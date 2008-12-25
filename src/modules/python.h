#ifndef _paintown_python_h
#define _paintown_python_h

#ifdef HAVE_PYTHON

#include <Python.h>
#include "../script.h"
#include <string>

class World;

class PythonEngine: public Script::Engine {
public:
    PythonEngine(const std::string & path);

    virtual void init();
    virtual void shutdown();
    virtual void createWorld(const World & world);
    virtual void destroyWorld(const World & world);
    virtual void tick();

    virtual ~PythonEngine();
protected:
    std::string path;
    std::string module;
    PyObject * user;
};
#endif

#endif
