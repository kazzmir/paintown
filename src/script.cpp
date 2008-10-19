#include "script.h"
#ifdef HAVE_PYTHON
#include "modules/python.h"
#endif

void script_init(){
#ifdef HAVE_PYTHON
    Python::init();
#endif
}
