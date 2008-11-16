#ifndef _paintown_python_h
#define _paintown_python_h

#include "../script.h"

#ifdef HAVE_PYTHON
class PythonEngine: public Script::Engine {
public:
    PythonEngine();

    virtual void init();
    virtual void shutdown();

    virtual ~PythonEngine();
};
#endif

#endif
