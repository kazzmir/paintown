#ifndef _paintown_script_h
#define _paintown_script_h

#include <string>
#include <exception>

class World;
class Character;

namespace Script{

    class Engine{
    private:
        /* there is one global engine in existence at any time */
        static Engine * theEngine;
    public:
        Engine();

        static Engine * getEngine();

        virtual void init() = 0;
        virtual void shutdown() = 0;

        virtual void createWorld(const World & world) = 0;
        virtual void destroyWorld(const World & world) = 0;
        virtual void * createCharacter(void * character) = 0;
        virtual void destroyObject(void * handle) = 0;
        virtual void objectTick(void * handle) = 0;
        virtual void objectTakeDamage(void * who, void * handle, int damage) = 0;
        virtual void objectCollided(void * me, void * him) = 0;
        virtual void characterAttacked(void * me, void * him) = 0;
        virtual void tick() = 0;

        virtual ~Engine();
    };
    

    Engine * getEngine();
    void newEngine(const std::string & name, const std::string & path);

    class NoSuchEngine: public std::exception {
    public:
        NoSuchEngine(const std::string & s){
        }
    };

    /* a dummy engine class that doesn't do anything */
    class NoEngine: public Engine {
        public:
            NoEngine();
            virtual void init();
            virtual void shutdown();

            virtual void createWorld(const World & world);
            virtual void destroyWorld(const World & world);
            virtual void * createCharacter(void * character);
            virtual void destroyObject(void * handle);
            virtual void tick();
            virtual void objectTick(void * handle);
            virtual void objectTakeDamage(void * who, void * handle, int damage);
            virtual void objectCollided(void * me, void * him);
            virtual void characterAttacked(void * me, void * him);

            virtual ~NoEngine();
    };
}

#endif
