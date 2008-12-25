#ifndef _paintown_python_h
#define _paintown_python_h

#ifdef HAVE_PYTHON

#include "../script.h"
#include <string>
#include <Python.h>

class World;

class PythonEngine: public Script::Engine {
public:
    PythonEngine(const std::string & path);

    virtual void init();
    virtual void shutdown();
    virtual void createWorld(const World & world);
    virtual void destroyWorld(const World & world);

    virtual ~PythonEngine();
protected:
    std::string path;
    std::string module;
    PyObject * user;
};
#endif

#endif
