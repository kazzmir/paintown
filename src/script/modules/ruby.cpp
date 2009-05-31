#ifdef HAVE_RUBY

/* *the* ruby library */
#include <ruby.h>
/* ruby script engine interface */
#include "ruby.h"
#include "world.h"
#include "util/funcs.h"
#include "globals.h"

using namespace std;

namespace PaintownLevel{
    static VALUE _register(VALUE arg){
        Global::debug(0) << "Registered something\n" << endl;
        return arg;
    }
}

RubyEngine::RubyEngine(const std::string & path):
Script::Engine(){
    ruby_init();
    ruby_script("paintown ruby engine");

    VALUE module = rb_define_module("Paintown");
    Global::debug(1) << "Defined ruby module " << module << endl;
    rb_define_module_function(module, "register", RUBY_METHOD_FUNC(PaintownLevel::_register), 1);

    rb_load_file((Util::getDataPath() + path).c_str());
    Global::debug(0) << "Ruby returned " << ruby_exec() << endl;
}

void RubyEngine::init(){
}

void RubyEngine::shutdown(){
}

void RubyEngine::createWorld(const World & world){
}

void RubyEngine::destroyWorld(const World & world){
}

void * RubyEngine::createCharacter(void * character){
    return NULL;
}

void * RubyEngine::createPlayer(void * player){
    return NULL;
}

void RubyEngine::destroyObject(void * handle){
}

void RubyEngine::objectTick(void * handle){
}

void RubyEngine::objectTakeDamage(void * who, void * handle, int damage){
}

void RubyEngine::objectCollided(void * me, void * him){
}

void RubyEngine::characterAttacked(void * me, void * him){
}

void RubyEngine::tick(){
}

RubyEngine::~RubyEngine(){
}

#endif
