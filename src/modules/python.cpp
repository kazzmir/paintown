#include "python.h"
#ifdef HAVE_PYTHON
#include <Python.h>

void Python::init(){
    Py_Initialize();
}

#endif
