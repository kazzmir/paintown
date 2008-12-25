#ifndef _paintown_script_h
#define _paintown_script_h

#include <string>
#include <exception>

class World;

namespace Script{

    class Engine{
    public:
        Engine();

        virtual void init() = 0;
        virtual void shutdown() = 0;

        virtual void createWorld(const World & world) = 0;
        virtual void destroyWorld(const World & world) = 0;

        virtual ~Engine();
    };

    Engine * getEngine(const std::string & name, const std::string & path);

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

            virtual ~NoEngine();
    };
}

#endif
