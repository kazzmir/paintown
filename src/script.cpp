#include "script.h"
#include <string>
#ifdef HAVE_PYTHON
#include "modules/python.h"
#endif

using namespace std;

namespace Script{

    Engine::Engine(){
    }

    Engine::~Engine(){
    }

    Engine * getEngine(const string & name, const string & path){
#ifdef HAVE_PYTHON
        if (name == "python"){
            return new PythonEngine(path);
        }
        throw NoSuchEngine(name);
    }
#endif
}
