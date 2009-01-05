#ifndef _paintown_script_h
#define _paintown_script_h

#include <string>
#include <exception>

class World;
class Character;

namespace Script{

    class Character;

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
        virtual void * createCharacter(Character * character) = 0;
        virtual void destroyCharacter(void * handle) = 0;
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

    class NoEngine: public Engine {
        public:
            NoEngine();
            virtual void init();
            virtual void shutdown();

            virtual void createWorld(const World & world);
            virtual void destroyWorld(const World & world);
            virtual void * createCharacter(Character * character);
            virtual void destroyCharacter(void * handle);
            virtual void tick();

            virtual ~NoEngine();
    };

    /* mirrors an enemy in the game */
    class Character{
    public:
        Character(::Character * guy);
        virtual ~Character();
    private:
        /* opaque object that the engine used to communicate between
         * the paintown engine and the script engine
         */
        void * handle;
        ::Character * guy;
    };
}

#endif
