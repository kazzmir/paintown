#ifndef _paintown_ruby_h
#define _paintown_ruby_h

#ifdef HAVE_RUBY

#include "script/script.h"
#include <string>

class World;

class RubyEngine: public Script::Engine {
public:
    RubyEngine(const std::string & path);

    virtual void init();
    virtual void shutdown();
    virtual void createWorld(const World & world);
    virtual void destroyWorld(const World & world);
    virtual void * createCharacter(void * character);
    virtual void * createPlayer(void * player);
    virtual void destroyObject(void * handle);
    virtual void objectTick(void * handle);
    virtual void objectTakeDamage(void * who, void * handle, int damage);
    virtual void objectCollided(void * me, void * him);
    virtual void characterAttacked(void * me, void * him);
    virtual void tick();

    virtual ~RubyEngine();
protected:
};

#endif

#endif
