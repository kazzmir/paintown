#ifdef HAVE_RUBY

/* *the* ruby library */
#include <ruby.h>
/* ruby script engine interface */
#include "ruby.h"
#include "game/world.h"
#include "util/funcs.h"
#include "util/file-system.h"
#include "globals.h"

using namespace std;

namespace PaintownLevel{
    static VALUE _register(VALUE obj, VALUE arg){
        Global::debug(0) << "Registered something" << endl;
        return arg;
    }
}

RubyEngine::RubyEngine(const Filesystem::RelativePath & path):
Script::Engine(){
    ruby_init();
    ruby_init_loadpath();
    ruby_script("paintown ruby engine");

    VALUE module = rb_define_module("PaintownInternal");
    Global::debug(1) << "Defined ruby module " << module << endl;
    rb_define_module_function(module, "register", RUBY_METHOD_FUNC(PaintownLevel::_register), 1);
    ruby_incpush(Filesystem::find(Filesystem::RelativePath("scripts")).path().c_str());

    rb_load_file(Filesystem::find(path).path().c_str());
    int result = ruby_exec();
    if (result != 0){
        Global::debug(0) << "Ruby returned " << result << endl;
        ruby_cleanup(result);
    }
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
    /* can the ruby runtime be shutdown? doesn't seem like it.. */
}

#endif
