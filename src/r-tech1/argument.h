#ifndef _rtech1_util_argument_h
#define _rtech1_util_argument_h

#include <vector>
#include <string>
#include "pointer.h"

namespace Argument {

class Action{
public:
    virtual void act() = 0;
    virtual ~Action();
};

typedef std::vector<Util::ReferenceCount<Action> > ActionRefs;

class Parameter{
public:
    /* Keywords on the command line that should invoke this argument */
    virtual std::vector<std::string> keywords() const = 0;
    
    /* Parse more strings from the command line. Any actions that should take place
     * after the command line has been parsed should be put into 'actions'
     */
    virtual std::vector<std::string>::iterator parse(std::vector<std::string>::iterator current, std::vector<std::string>::iterator end, ActionRefs & actions) = 0;

    /* Description of what this argument does */
    virtual std::string description() const = 0;

    bool isArg(const std::string & what) const;

    virtual ~Parameter();
};

typedef std::vector<Util::ReferenceCount<Parameter> > ParameterRefs;

class Handler{
public:

    Handler();
    
    ~Handler();
    
    void add(Util::ReferenceCount<Parameter>);
    
    void add(const ParameterRefs &);
    
    void setDefaultAction(Util::ReferenceCount<Action>);
    
    void runActions(int argc, char ** argv);
    
private:
    ParameterRefs parameters;
    Util::ReferenceCount<Action> defaultAction;
};

}

#endif
