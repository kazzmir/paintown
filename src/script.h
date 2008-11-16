#ifndef _paintown_script_h
#define _paintown_script_h

#include <string>
#include <exception>

namespace Script{

    class Engine{
    public:
        Engine();

        virtual void init() = 0;
        virtual void shutdown() = 0;

        virtual ~Engine();
    };

    Engine * getEngine(const std::string & name);

    class NoSuchEngine: public std::exception {
    public:
        NoSuchEngine(const std::string & s){
        }
    };
}

#endif
