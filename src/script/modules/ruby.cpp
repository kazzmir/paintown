#ifdef HAVE_RUBY

/* *the* ruby library */
#include <ruby.h>
/* ruby script engine interface */
#include "ruby.h"
#include "world.h"

RubyEngine::RubyEngine(const std::string & path){
}

void RubyEngine::init(){
    ruby_init();
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
