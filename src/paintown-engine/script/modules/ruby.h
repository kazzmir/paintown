#ifndef _paintown_ruby_h
#define _paintown_ruby_h

#ifdef HAVE_RUBY

#include "../script.h"
#include <string>
#include "util/file-system.h"

class World;

class RubyEngine: public Script::Engine {
public:
    RubyEngine(const Filesystem::RelativePath & path);

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
