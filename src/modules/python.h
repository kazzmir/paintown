#ifndef _paintown_python_h
#define _paintown_python_h

#ifdef HAVE_PYTHON

#include "../script.h"
#include <string>

class PythonEngine: public Script::Engine {
public:
    PythonEngine(const std::string & path);

    virtual void init();
    virtual void shutdown();

    virtual ~PythonEngine();
protected:
    std::string path;
};
#endif

#endif
