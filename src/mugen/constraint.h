#ifndef _paintown_mugen_constraint_h
#define _paintown_mugen_constraint_h

#include <set>
#include <vector>
#include <string>
#include "command.h"
#include "util/pointer.h"

class Token;
namespace PaintownUtil = ::Util;

namespace Mugen{

class Constraint{
public:
    enum Type{
        PressA,
        ReleaseA,
        PressB,
        ReleaseB,
        PressC,
        ReleaseC,
        PressX,
        ReleaseX,
        PressY,
        ReleaseY,
        PressZ,
        ReleaseZ,
        PressForward,
        ReleaseForward,
        PressBack,
        ReleaseBack,
        PressUp,
        ReleaseUp,
        PressDown,
        ReleaseDown,
        PressStart,
        ReleaseStart,

        /* Its simpler to model UF, DB, etc as logical presses but it may be harder for
         * the user to press them because the individual keys (U and F) have to be pressed
         * at the same tick.
         */
        PressForwardUp,
        ReleaseForwardUp,
        PressBackUp,
        ReleaseBackUp,
        PressForwardDown,
        ReleaseForwardDown,
        PressBackDown,
        ReleaseBackDown,

        Combine
    };

    Constraint(Type type, double time, bool dominate);

    virtual ~Constraint();

    Type getType() const; 
    void setEmit();
    bool isEmit() const; 

    void setUpConstraints(const std::vector<PaintownUtil::ReferenceCount<Constraint> > & constraints);

    bool sameKey(const Constraint & him);

    virtual std::string toString() const;

    virtual bool doSatisfy(const Mugen::Input & input, int tick);

    virtual bool satisfy(const Mugen::Input & input, int tick);

    int getSatisfiedTick() const;

    virtual void depends(const PaintownUtil::ReferenceCount<Constraint> & him);

    virtual void reset();

    bool isDominate() const;

    double getTime() const;

    virtual Token * serialize() const;
    /* Updates the state of the object */
    virtual void deserialize(TokenView & view);
    virtual void deserialize(const Token * token);
    
    /*
    virtual bool isSatisfied() const {
        return satisfied;
    }
    */
    
    const std::set<PaintownUtil::ReferenceCount<Constraint> > & getDepends();

protected:
    const Type type;
    bool satisfied;
    const bool dominate;
    const double time;
    bool emit;
    int satisfiedTick;
    std::set<PaintownUtil::ReferenceCount<Constraint> > dependsOn;
};

class Command2{
public:
    Command2(const std::string & name, Ast::KeyList * keys, int maxTime, int bufferTime);

    const std::string & getName() const;
            
    bool handle(const Mugen::Input & input, int ticks);

    Token * serialize() const;
    void deserialize(const Token * token);

protected:
    void resetConstraints();
    int activeTicks(int ticks);

    std::vector<PaintownUtil::ReferenceCount<Constraint> > constraints;
    std::string name;
    int maxTime;
    int bufferTime;
    int useBufferTime;
    bool emitted;
};

}

#endif
