#ifdef HAVE_PYTHON
#include "modules/python.h"
#endif

#include "script.h"
#include "globals.h"
#include <string>

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
#endif
        Global::debug(0) << "*Warning* no such engine '" << name << "' for path " << path << endl;
        return new NoEngine();
        // throw NoSuchEngine(name);
    }

    NoEngine::NoEngine():Engine(){
    }

    void NoEngine::init(){
    }

    void NoEngine::shutdown(){
    }

    void NoEngine::createWorld(const World & world){
    }

    void NoEngine::destroyWorld(const World & world){
    }
            
    void NoEngine::tick(){
    }

    NoEngine::~NoEngine(){
    }
}
