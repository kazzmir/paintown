#include "mugen/command.h"
#include "mugen/constraint.h"
#include "mugen/ast/key.h"
#include "util/debug.h"
#include <math.h>
#include <vector>
#include <string>
#include <set>
#include <sstream>
#include <exception>

using std::vector;
using std::string;
using std::ostringstream;
using std::set;

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

    Constraint(Type type, double time, bool dominate):
    type(type),
    satisfied(false),
    dominate(dominate),
    time(time),
    emit(false),
    satisfiedTick(0){
    }

    virtual ~Constraint(){
    }

    Type getType() const {
        return type;
    }

    void setEmit(){
        this->emit = true;
    }

    bool isEmit() const {
        return emit;
    }

    void setUpConstraints(const vector<Util::ReferenceCount<Constraint> > & constraints){
        for (vector<Util::ReferenceCount<Constraint> >::const_iterator it = constraints.begin(); it != constraints.end(); it++){
            const Util::ReferenceCount<Constraint> & constraint = *it;
            if (constraint == this){
                continue;
            }

            bool depends = false;
            if (constraint->getTime() < getTime() &&
                constraint->isDominate()){

                depends = true;
            }

            /* If the key was generated as a response to a press/release and this is the same key
             * then this depends on that key.
             */
            if (constraint->getTime() < getTime() &&
                sameKey(*constraint)){

                depends = true;
            }

            if (depends){
                dependsOn.insert(constraint);
            }
        }
    }

    bool sameKey(const Constraint & him){
        switch (type){
            case PressA:
            case ReleaseA: return him.getType() == PressA ||
                                  him.getType() == ReleaseA;

            case PressB:
            case ReleaseB: return him.getType() == PressB ||
                                  him.getType() == ReleaseB;

            case PressC:
            case ReleaseC: return him.getType() == PressC ||
                                  him.getType() == ReleaseC;

            case PressX:
            case ReleaseX: return him.getType() == PressX ||
                                  him.getType() == ReleaseX;

            case PressY:
            case ReleaseY: return him.getType() == PressY ||
                                  him.getType() == ReleaseY;

            case PressZ:
            case ReleaseZ: return him.getType() == PressZ ||
                                  him.getType() == ReleaseZ;

            case PressForward:
            case ReleaseForward: return him.getType() == PressForward ||
                                        him.getType() == ReleaseForward;

            case PressBack:
            case ReleaseBack: return him.getType() == PressBack ||
                                     him.getType() == ReleaseBack;

            case PressUp:
            case ReleaseUp: return him.getType() == PressUp ||
                                   him.getType() == ReleaseUp;

            case PressDown:
            case ReleaseDown: return him.getType() == PressDown ||
                                     him.getType() == ReleaseDown;

            case PressForwardUp:
            case ReleaseForwardUp: return him.getType() == PressForwardUp ||
                                          him.getType() == ReleaseForwardUp;

            case PressBackUp:
            case ReleaseBackUp: return him.getType() == PressBackUp ||
                                       him.getType() == ReleaseBackUp;

            case PressForwardDown:
            case ReleaseForwardDown: return him.getType() == PressForwardDown ||
                                            him.getType() == ReleaseForwardDown;

            case PressBackDown:
            case ReleaseBackDown: return him.getType() == PressBackDown ||
                                         him.getType() == ReleaseBackDown;

            case Combine: return false;
        }

        return false;
    }

    virtual std::string toString() const {
        std::ostringstream out;

        switch (type){
            case PressA: out << "a"; break;
            case ReleaseA: out << "~a"; break;
            case PressB: out << "b"; break;
            case ReleaseB: out << "~b"; break;
            case PressC: out << "c"; break;
            case ReleaseC: out << "~c"; break;
            case PressX: out << "x"; break;
            case ReleaseX: out << "~x"; break;
            case PressY: out << "y"; break;
            case ReleaseY: out << "~y"; break;
            case PressZ: out << "z"; break;
            case ReleaseZ: out << "~z"; break;
            case PressForward: out << "F"; break;
            case ReleaseForward: out << "~F"; break;
            case PressBack: out << "B"; break;
            case ReleaseBack: out << "~B"; break;
            case PressUp: out << "U"; break;
            case ReleaseUp: out << "~U"; break;
            case PressDown: out << "D"; break;
            case ReleaseDown: out << "~D"; break;
            case PressForwardUp: out << "UF"; break;
            case ReleaseForwardUp: out << "~UF"; break;
            case PressForwardDown: out << "DF"; break;
            case ReleaseForwardDown: out << "~DF"; break;
            case PressBackUp: out << "UB"; break;
            case ReleaseBackUp: out << "~UB"; break;
            case PressBackDown: out << "DB"; break;
            case ReleaseBackDown: out << "~DB"; break;
            case Combine: break;
        }

        return out.str();
    }

    virtual bool doSatisfy(const Mugen::Input & input, int tick){
        switch (type){
            case PressA: return input.pressed.a; break;
            case ReleaseA: return input.released.a; break;
            case PressB: return input.pressed.b; break;
            case ReleaseB: return input.released.b; break;
            case PressC: return input.pressed.c; break;
            case ReleaseC: return input.released.c; break;
            case PressX: return input.pressed.x; break;
            case ReleaseX: return input.released.x; break;
            case PressY: return input.pressed.y; break;
            case ReleaseY: return input.released.y; break;
            case PressZ: return input.pressed.z; break;
            case ReleaseZ: return input.released.z; break;

            case PressForward: return input.pressed.forward; break;
            case ReleaseForward: return input.released.forward; break;
            case PressBack: return input.pressed.back; break;
            case ReleaseBack: return input.released.back; break;
            case PressDown: return input.pressed.down; break;
            case ReleaseDown: return input.released.down; break;
            case PressUp: return input.pressed.up; break;
            case ReleaseUp: return input.released.up; break;

            case PressForwardUp: return input.pressed.forward &&
                                        input.pressed.up; break;

            /* For releases its ok just to release one of the keys */
            case ReleaseForwardUp: return input.released.forward ||
                                          input.released.up; break;

            case PressBackUp: return input.pressed.up &&
                                     input.pressed.back; break;

            case ReleaseBackUp: return input.released.up ||
                                       input.released.back; break;

            case PressForwardDown: return input.pressed.forward &&
                                          input.pressed.down; break;

            case ReleaseForwardDown: return input.released.forward ||
                                            input.released.down; break;

            case PressBackDown: return input.pressed.back &&
                                       input.pressed.down; break;

            case ReleaseBackDown: return input.released.back ||
                                         input.released.down; break;

            case Combine: break;
        }

        return false;
    }

    virtual bool satisfy(const Mugen::Input & input, int tick){
        if (!satisfied){
            /*
            if (dependenciesSatisfied()){
                satisfied = doSatisfy(input, tick);
            }
            */
            satisfied = doSatisfy(input, tick);

            if (satisfied){
                satisfiedTick = tick;
            }
        }

        return satisfied;
    }

    int getSatisfiedTick() const {
        return satisfiedTick;
    }

    /*
    virtual bool dependenciesSatisfied() const {
        for (std::set<Util::ReferenceCount<Constraint> >::const_iterator it = dependsOn.begin(); it != dependsOn.end(); it++){
            const Util::ReferenceCount<Constraint> & constraint = *it;
            if (!constraint->isSatisfied()){
                return false;
            }
        }

        return true;
    }
    */

    virtual void depends(const Util::ReferenceCount<Constraint> & him){
        dependsOn.insert(him);
    }

    virtual void reset(){
        satisfied = false;
    }

    bool isDominate() const {
        return dominate;
    }

    double getTime() const {
        return time;
    }
    
    /*
    virtual bool isSatisfied() const {
        return satisfied;
    }
    */
    
    const std::set<Util::ReferenceCount<Constraint> > & getDepends(){
        return dependsOn;
    }

protected:
    Type type;
    bool satisfied;
    bool dominate;
    double time;
    bool emit;
    int satisfiedTick;
    std::set<Util::ReferenceCount<Constraint> > dependsOn;
};

typedef Util::ReferenceCount<Constraint> ConstraintRef;

class DelayConstraint: public Constraint {
public:
    DelayConstraint(int delayTime, Constraint::Type type, double time, bool dominate):
    Constraint(type, time, dominate),
    delayTime(delayTime),
    held(0){
    }
    
    virtual bool satisfy(const Mugen::Input & input, int tick){
        if (satisfied){
            return satisfied;
        } else {
            /* this logic works even if delayTime is 0, which means don't hold the key */
            bool pressed = Constraint::doSatisfy(input, tick);
            if (pressed){
                held += 1;
            } else {
                held = 0;
            }
            satisfied = held >= delayTime;
        }

        return satisfied;
    }

    virtual string toString() const {
        std::ostringstream out;

        out << "~";
        if (delayTime > 0){
            out << delayTime;
        }
        out << Constraint::toString();
        return out.str();
    }

    int delayTime;
    int held;
};

class HoldConstraint: public Constraint {
public:
    HoldConstraint(Constraint::Type type, double time, bool dominate):
    Constraint(type, time, dominate){
    }

    virtual string toString() const {
        std::ostringstream out;

        out << "/";
        out << Constraint::toString();

        return out.str();
    }
 
    /* The constraint is only satisfied when a key is held down and
     * must be held down forever until the end.
     */
    virtual bool satisfy(const Mugen::Input & input, int tick){
        return Constraint::doSatisfy(input, tick);
    }
};

class CombinedConstraint: public Constraint {
public:
    CombinedConstraint(const Util::ReferenceCount<Constraint> & key1,
                       const Util::ReferenceCount<Constraint> & key2,
                       double time, bool dominate):
    Constraint(Combine, time, dominate),
    key1(key1),
    key2(key2){
    }

    virtual bool doSatisfy(const Mugen::Input & input, int tick){
        const int threshold = 4;

        if (key1->satisfy(input, tick) &&
            key2->satisfy(input, tick) &&
            fabs(key1->getSatisfiedTick() - key2->getSatisfiedTick()) < threshold){
            return true;
        }

        return false;
    }

    virtual string toString() const {
        return key1->toString() + "+" + key2->toString();
    }

    Util::ReferenceCount<Constraint> key1;
    Util::ReferenceCount<Constraint> key2;
};

static Constraint::Type getPress(const Ast::KeySingle & key){
    string name = key.toString();
    if (name == "a"){
        return Constraint::PressA;
    }
    if (name == "b"){
        return Constraint::PressB;
    }
    if (name == "c"){
        return Constraint::PressC;
    }
    if (name == "x"){
        return Constraint::PressX;
    }
    if (name == "y"){
        return Constraint::PressY;
    }
    if (name == "z"){
        return Constraint::PressZ;
    }
    if (name == "F"){
        return Constraint::PressForward;
    }
    if (name == "B"){
        return Constraint::PressBack;
    }
    if (name == "U"){
        return Constraint::PressUp;
    }
    if (name == "D"){
        return Constraint::PressDown;
    }
    if (name == "UF"){
        return Constraint::PressForwardUp;
    }
    if (name == "UB"){
        return Constraint::PressBackUp;
    }
    if (name == "DF"){
        return Constraint::PressForwardDown;
    }
    if (name == "DB"){
        return Constraint::PressBackDown;
    }

    /* utter failure */
    throw std::exception();
    return Constraint::PressA;
}

static Constraint::Type getRelease(const Ast::KeySingle & key){
    string name = key.toString();
    if (name == "a"){
        return Constraint::ReleaseA;
    }
    if (name == "b"){
        return Constraint::ReleaseB;
    }
    if (name == "c"){
        return Constraint::ReleaseC;
    }
    if (name == "x"){
        return Constraint::ReleaseX;
    }
    if (name == "y"){
        return Constraint::ReleaseY;
    }
    if (name == "z"){
        return Constraint::ReleaseZ;
    }
    if (name == "F"){
        return Constraint::ReleaseForward;
    }
    if (name == "B"){
        return Constraint::ReleaseBack;
    }
    if (name == "U"){
        return Constraint::ReleaseUp;
    }
    if (name == "D"){
        return Constraint::ReleaseDown;
    }
    if (name == "UF"){
        return Constraint::ReleaseForwardUp;
    }
    if (name == "UB"){
        return Constraint::ReleaseBackUp;
    }
    if (name == "DF"){
        return Constraint::ReleaseForwardDown;
    }
    if (name == "DB"){
        return Constraint::ReleaseBackDown;
    }

    /* utter failure */
    throw std::exception();
    return Constraint::ReleaseA;
}

vector<Util::ReferenceCount<Constraint> > makeConstraints(const Ast::Key * key, double time, bool last){
    vector<Util::ReferenceCount<Constraint> > constraints;

    class ConstraintWalker: public Ast::Walker {
    public:
        ConstraintWalker(vector<Util::ReferenceCount<Constraint> > & constraints, double time, bool last):
        constraints(constraints),
        time(time),
        last(last){
        }
        
        vector<Util::ReferenceCount<Constraint> > & constraints;
        double time;
        bool last;

        virtual void addPressRelease(Constraint::Type pressKey, Constraint::Type releaseKey){
            Util::ReferenceCount<Constraint> press = Util::ReferenceCount<Constraint>(new Constraint(pressKey, time, true));
            Util::ReferenceCount<Constraint> release = Util::ReferenceCount<Constraint>(new Constraint(releaseKey, time + 0.5, false));
            constraints.push_back(press);
            constraints.push_back(release);

            if (last){
                press->setEmit();
            }
        }

        virtual void onKeySingle(const Ast::KeySingle & key){
            addPressRelease(getPress(key), getRelease(key));
        }

        virtual void onKeyModifier(const Ast::KeyModifier & key){
            switch (key.getModifierType()){
                case Ast::KeyModifier::MustBeHeldDown: {

                    class HoldWalker: public Ast::Walker {
                    public:
                        HoldWalker(ConstraintWalker & outer, double time):
                        outer(outer),
                        time(time){
                        }
                        
                        ConstraintWalker & outer;
                        const double time;
                        
                        virtual void onKeySingle(const Ast::KeySingle & key){
                            Constraint::Type pressKey = getPress(key);
                            outer.constraints.push_back(ConstraintRef(new HoldConstraint(pressKey, time, true)));
                        }

                        virtual void onKeyCombined(const Ast::KeyCombined & key){
                            throw std::exception();
                        }
        
                        virtual void onKeyModifier(const Ast::KeyModifier & key){
                            switch (key.getModifierType()){
                                case Ast::KeyModifier::Direction: {
                                    key.getKey()->walk(*this);
                                    break;
                                }
                                default: {
                                    throw std::exception();
                                }
                            }
                        }
                    };
                    
                    const Ast::Key * sub = key.getKey();
                    HoldWalker walker(*this, time);
                    sub->walk(walker);

                    break;
                }
                case Ast::KeyModifier::Release: {

                    class ReleaseWalker: public Ast::Walker {
                    public:
                        ReleaseWalker(ConstraintWalker & outer, int hold, double time):
                        outer(outer),
                        hold(hold),
                        time(time){
                        }

                        ConstraintWalker & outer;
                        int hold;
                        double time;

                        virtual void onKeySingle(const Ast::KeySingle & key){
                            string name = key.toString();

                            Constraint::Type pressKey = getPress(key);
                            Constraint::Type releaseKey = getRelease(key);

                            if (hold < 1){
                                hold = 1;
                            }

                            outer.constraints.push_back(ConstraintRef(new DelayConstraint(hold, pressKey, time, true)));
                            Util::ReferenceCount<Constraint> release = Util::ReferenceCount<Constraint>(new Constraint(releaseKey, time + 0.1, true));
                            if (outer.last){
                                release->setEmit();
                            }
                            outer.constraints.push_back(release);

                        }
        
                        virtual void onKeyCombined(const Ast::KeyCombined & key){
                            throw std::exception();
                        }
        
                        virtual void onKeyModifier(const Ast::KeyModifier & key){
                            throw std::exception();
switch (key.getModifierType()){
                                case Ast::KeyModifier::Direction: {
                                    key.getKey()->walk(*this);
                                    break;
                                }
                                default: {
                                    throw std::exception();
                                }
                            }
                        }
                    };

                    const Ast::Key * sub = key.getKey();
                    ReleaseWalker walker(*this, key.getExtra(), time);
                    sub->walk(walker);

                    break;
                }
                case Ast::KeyModifier::Direction: {
                    /* Direction keys can be handled just like regular keys.
                     * Technically they should only allow direction keys like U, D, F, B
                     * whereas the following code allows other butons like /a, which
                     * is meaningless.
                     */
                    key.getKey()->walk(*this);
                    break;
                }
                case Ast::KeyModifier::Only: {
                    /* F, >a */

                    /* FIXME: for now just treat this as a regular key press. Its probably
                     * not ultra important that we prevent certain commands from executing
                     * if the user pressed an extra button or two.
                     */

                    key.getKey()->walk(*this);

                    break;
                }
            }
        }

        Util::ReferenceCount<Constraint> findDominate(const vector<Util::ReferenceCount<Constraint> > & keys){
            for (vector<Util::ReferenceCount<Constraint> >::const_iterator it = keys.begin(); it != keys.end(); it++){
                const Util::ReferenceCount<Constraint> & constraint = *it;
                if (constraint->isDominate()){
                    return constraint;
                }
            }

            return Util::ReferenceCount<Constraint>(NULL);
        }

        /* For a combined key we have to check that the two sub-keys were pressed
         * at roughly the same time. We can implement that by checking when their Press
         * constraint was satisfied. If both constraints are satisfied within 0-4 ticks, or whatever,
         * then the combined constraint should satisfy.
         */
        virtual void onKeyCombined(const Ast::KeyCombined & key){
            vector<Util::ReferenceCount<Constraint> > key1 = makeConstraints(key.getKey1(), time, false);
            vector<Util::ReferenceCount<Constraint> > key2 = makeConstraints(key.getKey2(), time, false);
            
            constraints.insert(constraints.end(), key1.begin(), key1.end()); 
            constraints.insert(constraints.end(), key2.begin(), key2.end()); 
            Util::ReferenceCount<Constraint> dominateKey1 = findDominate(key1);
            Util::ReferenceCount<Constraint> dominateKey2 = findDominate(key2);
            Util::ReferenceCount<Constraint> combined = Util::ReferenceCount<Constraint>(new CombinedConstraint(dominateKey1, dominateKey2, time + 0.9, true));
            constraints.push_back(combined);
            if (last){
                combined->setEmit();
            }
        }
    };

    ConstraintWalker walker(constraints, time, last);
    key->walk(walker);

    return constraints;
}

void constructDependencies(const vector<Util::ReferenceCount<Constraint> > & constraints){
    for (vector<Util::ReferenceCount<Constraint> >::const_iterator it = constraints.begin(); it != constraints.end(); it++){
        const Util::ReferenceCount<Constraint> & constraint = *it;
        constraint->setUpConstraints(constraints);
    }
}

static void topologicalVisit(ConstraintRef constraint, vector<ConstraintRef> & sorted, set<ConstraintRef> & unsorted){
    const set<ConstraintRef> & depends = constraint->getDepends();
    unsorted.erase(constraint);
    for (set<ConstraintRef>::const_iterator it = depends.begin(); it != depends.end(); it++){
        ConstraintRef depend = *it;
        if (unsorted.find(depend) != unsorted.end()){
            topologicalVisit(depend, sorted, unsorted);
        }
    }
    sorted.push_back(constraint);
}

vector<Util::ReferenceCount<Constraint> > topologicalSort(const vector<ConstraintRef> & constraints){
    vector<ConstraintRef> out;
    set<ConstraintRef> unsorted;
    for (vector<ConstraintRef>::const_iterator it = constraints.begin(); it != constraints.end(); it++){
        unsorted.insert(*it);
    }

    while (unsorted.size() > 0){
        ConstraintRef first = *unsorted.begin();
        topologicalVisit(first, out, unsorted);
    }

    return out;
}

vector<Util::ReferenceCount<Constraint> > makeConstraints(Ast::KeyList * keys){
    vector<Util::ReferenceCount<Constraint> > constraints;
    double time = 1;
    for (vector<Ast::Key*>::const_iterator it = keys->getKeys().begin(); it != keys->getKeys().end(); it++){
        bool last = false;
        Ast::Key * key = *it;
        if (key == keys->getKeys().back()){
            last = true;
        }

        vector<Util::ReferenceCount<Constraint> > more = makeConstraints(*it, time, last);
        time += 1;
        constraints.insert(constraints.end(), more.begin(), more.end());
    }   

    constructDependencies(constraints);

    return topologicalSort(constraints);
}

string debugInput(const Mugen::Input & input){
    std::ostringstream out;

    if (input.pressed.a){
        out << "a, ";
    }
    if (input.released.a){
        out << "~a, ";
    }
    if (input.pressed.b){
        out << "b, ";
    }
    if (input.released.b){
        out << "~b, ";
    }
    if (input.pressed.c){
        out << "c, ";
    }
    if (input.released.c){
        out << "~c, ";
    }
    if (input.pressed.x){
        out << "x, ";
    }
    if (input.released.x){
        out << "~x, ";
    }
    if (input.pressed.y){
        out << "y, ";
    }
    if (input.released.y){
        out << "~y, ";
    }
    if (input.pressed.z){
        out << "z, ";
    }
    if (input.released.z){
        out << "~z, ";
    }
    if (input.pressed.forward){
        out << "F, ";
    }
    if (input.released.forward){
        out << "~F, ";
    }
    if (input.pressed.back){
        out << "B, ";
    }
    if (input.released.back){
        out << "~B, ";
    }
    if (input.pressed.down){
        out << "D, ";
    }
    if (input.released.down){
        out << "~D, ";
    }
    if (input.pressed.up){
        out << "U, ";
    }
    if (input.released.up){
        out << "~U, ";
    }
    
    return out.str();
}

class NewCommand{
public:
    NewCommand(Ast::KeyList * keys):
    constraints(makeConstraints(keys)){
    }
            
    bool handle(const Mugen::Input & input, int ticks){
        /* keep checking constraints until we reach a fix point. */

        bool emit = false;
        std::set<ConstraintRef> satisfied;
        for (vector<ConstraintRef>::iterator it = constraints.begin(); it != constraints.end(); it++){
            bool all = true;
            ConstraintRef constraint = *it;
            const std::set<ConstraintRef> & depends = constraint->getDepends();
            for (std::set<ConstraintRef>::const_iterator it2 = depends.begin(); it2 != depends.end(); it2++){
                /* If its not in the satisified set then not all dependencies can be satisifed */
                if (satisfied.find(*it2) == satisfied.end()){
                    all = false;
                    break;
                }
            }

            /* If `all' is still true then all dependencies have been satisfied and we can continue */
            if (all){
                if (constraint->satisfy(input, ticks)){
                    satisfied.insert(constraint);
                    if (constraint->isEmit()){
                        emit = true;
                    }
                }
            }
        }

        /* Uncomment to debug the commands */
        /*
        Global::debug(0) << "Tick: " << ticks << " Input " << debugInput(input) << std::endl;
        for (vector<ConstraintRef>::iterator it = constraints.begin(); it != constraints.end(); it++){
            ConstraintRef constraint = *it;
            Global::debug(0) << " Constraint " << constraint->getTime() << " " << constraint->toString() << " satisfied " << (satisfied.find(constraint) != satisfied.end()) << std::endl;
        }
        Global::debug(0) << std::endl;
        */

        return emit;
    }

    vector<Util::ReferenceCount<Constraint> > constraints;
};

void parseKey(Mugen::Input & input, const string & key){
    if (key == "a"){
        input.pressed.a = true;
    } else if (key == "b"){
        input.pressed.b = true;
    } else if (key == "c"){
        input.pressed.c = true;
    } else if (key == "x"){
        input.pressed.x = true;
    } else if (key == "y"){
        input.pressed.y = true;
    } else if (key == "z"){
        input.pressed.z = true;
    } else if (key == "U"){
        input.pressed.up = true;
    } else if (key == "D"){
        input.pressed.down = true;
    } else if (key == "F"){
        input.pressed.forward = true;
    } else if (key == "B"){
        input.pressed.back = true;
    } else if (key == "~a"){
        input.released.a = true;
    } else if (key == "~b"){
        input.released.b = true;
    } else if (key == "~c"){
        input.released.c = true;
    } else if (key == "~x"){
        input.released.x = true;
    } else if (key == "~y"){
        input.released.y = true;
    } else if (key == "~z"){
        input.released.z = true;
    } else if (key == "~U"){
        input.released.up = true;
    } else if (key == "~D"){
        input.released.down = true;
    } else if (key == "~F"){
        input.released.forward = true;
    } else if (key == "~B"){
        input.released.back = true;
    } else {
        Global::debug(0) << "Unknown key " << key << std::endl;
    }
}

Mugen::Input convert(string raw){
    size_t semicolon = raw.find(';');
    if (semicolon != -1){
        raw.erase(semicolon, 1);
    }
    vector<string> keys = Util::splitString(raw, ',');
    Mugen::Input input;
    
    for (vector<string>::iterator it = keys.begin(); it != keys.end(); it++){
        parseKey(input, Util::trim(*it));
    }

    return input;
}

vector<Mugen::Input> loadScript(const string & input){
    vector<string> lines = Util::splitString(input, ';');
    vector<Mugen::Input> out;
    
    for (vector<string>::iterator it = lines.begin(); it != lines.end(); it++){
        out.push_back(convert(*it));
    }

    return out;
}

int testKeys(Ast::KeyList * keys, const vector<Mugen::Input> & inputs){
    // Mugen::Command command("test", keys, 100, 1);
    // NewCommand command(keys);
    Mugen::Command2 command("", keys, 1000, 0);

    /* Make sure we emit something */
    bool emitted = false;
    int tick = 0;
    for (vector<Mugen::Input>::const_iterator it = inputs.begin(); it != inputs.end(); it++){
        const Mugen::Input & input = *it;
        bool last = false;
        tick += 1;
        if (&input == &inputs.back()){
            last = true;
        }

        if (!last){
            /* If its not the last input then the command should not fire, so if it does then its an error */
            if (command.handle(input, tick)){
                return 1;
            }
        } else {
            /* And similarly if its the last input the command should fire, otherwise error */
            if (!command.handle(input, tick)){
                return 1;
            } else {
                emitted = true;
            }
        }
    }

    return emitted ? 0 : 1;
}

int test1(){
    std::vector<Ast::Key*> keys;
    keys.push_back(new Ast::KeySingle(0, 0, "a"));
    Ast::KeyList * list = new Ast::KeyList(0, 0, keys);
    // Mugen::Command command("test", list, 100, 1);
    
    std::ostringstream script;
    script << "a;";

    return testKeys(list, loadScript(script.str()));
}

int test1_1(){
    std::vector<Ast::Key*> keys;
    keys.push_back(new Ast::KeySingle(0, 0, "a"));
    Ast::KeyList * list = new Ast::KeyList(0, 0, keys);
    
    std::ostringstream script;
    script << "b;";

    return !testKeys(list, loadScript(script.str()));
}

int test2(){
    std::vector<Ast::Key*> keys;
    keys.push_back(new Ast::KeySingle(0, 0, "a"));
    keys.push_back(new Ast::KeySingle(0, 0, "b"));
    Ast::KeyList * list = new Ast::KeyList(0, 0, keys);

    std::ostringstream script;
    script << "a;";
    script << "b;";

    return testKeys(list, loadScript(script.str()));
}

int test3(){
    std::vector<Ast::Key*> keys;
    keys.push_back(new Ast::KeySingle(0, 0, "a"));
    keys.push_back(new Ast::KeySingle(0, 0, "b"));
    Ast::KeyList * list = new Ast::KeyList(0, 0, keys);
    
    std::ostringstream script;
    script << "a;a,b";

    return testKeys(list, loadScript(script.str()));
}

int test4(){
    std::vector<Ast::Key*> keys;
    keys.push_back(new Ast::KeySingle(0, 0, "a"));
    keys.push_back(new Ast::KeySingle(0, 0, "b"));
    Ast::KeyList * list = new Ast::KeyList(0, 0, keys);
    
    std::ostringstream script;
    script << "a;a;a;a;a;";
    script << "a;a;a;a;a;";
    script << "b;";

    return testKeys(list, loadScript(script.str()));
}

/* ~a, ~a, ~a */
int test5(){
    std::vector<Ast::Key*> keys;
    keys.push_back(new Ast::KeyModifier(0, 0, Ast::KeyModifier::Release, new Ast::KeySingle(0, 0, "a"), 0));
    keys.push_back(new Ast::KeyModifier(0, 0, Ast::KeyModifier::Release, new Ast::KeySingle(0, 0, "a"), 0));
    keys.push_back(new Ast::KeyModifier(0, 0, Ast::KeyModifier::Release, new Ast::KeySingle(0, 0, "a"), 0));
    Ast::KeyList * list = new Ast::KeyList(0, 0, keys);

    std::ostringstream script;
    script << "a;~a;";
    script << ";";
    script << "a;~a;";
    script << ";";
    script << "a;~a;";

    return testKeys(list, loadScript(script.str()));
}

int test6(){
    std::vector<Ast::Key*> keys;
    keys.push_back(new Ast::KeySingle(0, 0, "a"));
    keys.push_back(new Ast::KeySingle(0, 0, "b"));
    keys.push_back(new Ast::KeySingle(0, 0, "a"));
    Ast::KeyList * list = new Ast::KeyList(0, 0, keys);

    std::ostringstream script;
    script << "a;b;~a;~b;a;";
    return testKeys(list, loadScript(script.str()));
}

int test7(){
    std::vector<Ast::Key*> keys;
    keys.push_back(new Ast::KeySingle(0, 0, "a"));
    keys.push_back(new Ast::KeySingle(0, 0, "b"));
    keys.push_back(new Ast::KeySingle(0, 0, "a"));
    Ast::KeyList * list = new Ast::KeyList(0, 0, keys);

    std::ostringstream script;
    script << "a;~a;b;a";
    return testKeys(list, loadScript(script.str()));
}

int test8(){
    std::vector<Ast::Key*> keys;
    keys.push_back(new Ast::KeySingle(0, 0, "a"));
    keys.push_back(new Ast::KeySingle(0, 0, "b"));
    keys.push_back(new Ast::KeySingle(0, 0, "a"));
    Ast::KeyList * list = new Ast::KeyList(0, 0, keys);

    std::ostringstream script;
    script << "a;b;~b;~a;a";
    return testKeys(list, loadScript(script.str()));
}

int test9(){
    std::vector<Ast::Key*> keys;
    keys.push_back(new Ast::KeyCombined(0, 0, new Ast::KeySingle(0, 0, "a"),
                                              new Ast::KeySingle(0, 0, "b")));
    Ast::KeyList * list = new Ast::KeyList(0, 0, keys);

    std::ostringstream script;
    script << "a,b";
    return testKeys(list, loadScript(script.str()));
}

int test10(){
    std::vector<Ast::Key*> keys;
    keys.push_back(new Ast::KeySingle(0, 0, "a"));
    keys.push_back(new Ast::KeyCombined(0, 0, new Ast::KeySingle(0, 0, "a"),
                                              new Ast::KeySingle(0, 0, "b")));
    Ast::KeyList * list = new Ast::KeyList(0, 0, keys);

    std::ostringstream script;
    script << "a;a;~a;";
    script << "a,b";
    return testKeys(list, loadScript(script.str()));
}

int test11(){
    std::vector<Ast::Key*> keys;
    keys.push_back(new Ast::KeySingle(0, 0, "a"));
    keys.push_back(new Ast::KeyCombined(0, 0, new Ast::KeySingle(0, 0, "a"),
                                              new Ast::KeySingle(0, 0, "b")));
    keys.push_back(new Ast::KeySingle(0, 0, "b"));
    Ast::KeyList * list = new Ast::KeyList(0, 0, keys);

    std::ostringstream script;
    script << "a;a;~a;";
    script << "a,b;";
    script << "~b;b";
    return testKeys(list, loadScript(script.str()));
}

int test12(){
    std::vector<Ast::Key*> keys;
    keys.push_back(new Ast::KeyCombined(0, 0, new Ast::KeySingle(0, 0, "a"),
                                              new Ast::KeySingle(0, 0, "b")));
    Ast::KeyList * list = new Ast::KeyList(0, 0, keys);

    std::ostringstream script;
    script << "a";
    return !testKeys(list, loadScript(script.str()));
}

int test13(){
    std::vector<Ast::Key*> keys;
    keys.push_back(new Ast::KeyModifier(0, 0, Ast::KeyModifier::Release, new Ast::KeySingle(0, 0, "a"), 5));
    Ast::KeyList * list = new Ast::KeyList(0, 0, keys);

    std::ostringstream script;
    script << "a;~a";
    return !testKeys(list, loadScript(script.str()));
}

int test14(){
    std::vector<Ast::Key*> keys;
    keys.push_back(new Ast::KeyModifier(0, 0, Ast::KeyModifier::Release, new Ast::KeySingle(0, 0, "a"), 5));
    Ast::KeyList * list = new Ast::KeyList(0, 0, keys);

    std::ostringstream script;
    script << "a;a;a;a;a;~a";
    return testKeys(list, loadScript(script.str()));
}

int test15(){
    std::vector<Ast::Key*> keys;
    keys.push_back(new Ast::KeyModifier(0, 0, Ast::KeyModifier::Release, new Ast::KeySingle(0, 0, "a"), 5));
    Ast::KeyList * list = new Ast::KeyList(0, 0, keys);

    std::ostringstream script;
    script << "a;a;;a;a;a;~a";
    return !testKeys(list, loadScript(script.str()));
}

int test16(){
    std::vector<Ast::Key*> keys;
    keys.push_back(new Ast::KeySingle(0, 0, "a"));
    keys.push_back(new Ast::KeyModifier(0, 0, Ast::KeyModifier::Release, new Ast::KeySingle(0, 0, "b"), 5));
    keys.push_back(new Ast::KeyCombined(0, 0, new Ast::KeySingle(0, 0, "a"),
                                              new Ast::KeySingle(0, 0, "b")));
    Ast::KeyList * list = new Ast::KeyList(0, 0, keys);

    std::ostringstream script;
    script << "a;";
    script << "a,b;~a,b;b;b;b;";
    script << "b;~b;";
    script << "a,b;";
    return testKeys(list, loadScript(script.str()));
}

int test17(){
    std::vector<Ast::Key*> keys;
    keys.push_back(new Ast::KeySingle(0, 0, "a"));
    keys.push_back(new Ast::KeyModifier(0, 0, Ast::KeyModifier::Release, new Ast::KeySingle(0, 0, "b"), 5));
    keys.push_back(new Ast::KeyCombined(0, 0, new Ast::KeySingle(0, 0, "a"),
                                              new Ast::KeySingle(0, 0, "b")));
    Ast::KeyList * list = new Ast::KeyList(0, 0, keys);

    std::ostringstream script;
    script << "a;";
    script << "a,b;~a,b;b;b;b;";
    script << "b;~b;";
    script << "a,b;";
    return testKeys(list, loadScript(script.str()));
}

/* simple test for all keys */
int test18(){
    std::vector<Ast::Key*> keys;
    keys.push_back(new Ast::KeySingle(0, 0, "a"));
    keys.push_back(new Ast::KeySingle(0, 0, "b"));
    keys.push_back(new Ast::KeySingle(0, 0, "c"));
    keys.push_back(new Ast::KeySingle(0, 0, "x"));
    keys.push_back(new Ast::KeySingle(0, 0, "y"));
    keys.push_back(new Ast::KeySingle(0, 0, "z"));
    keys.push_back(new Ast::KeySingle(0, 0, "F"));
    keys.push_back(new Ast::KeySingle(0, 0, "B"));
    keys.push_back(new Ast::KeySingle(0, 0, "U"));
    keys.push_back(new Ast::KeySingle(0, 0, "D"));
    keys.push_back(new Ast::KeySingle(0, 0, "UF"));
    keys.push_back(new Ast::KeySingle(0, 0, "UB"));
    keys.push_back(new Ast::KeySingle(0, 0, "DF"));
    keys.push_back(new Ast::KeySingle(0, 0, "DB"));
    Ast::KeyList * list = new Ast::KeyList(0, 0, keys);

    std::ostringstream script;
    script << "a;b;c;x;y;z;F;~F;B;~B;U;~U;D;~D;";
    script << "U,F;~U,~F;";
    script << "U,B;~U,~B;";
    script << "D,F;~D,~F;";
    script << "D,B;";
    return testKeys(list, loadScript(script.str()));
}

int test19(){
    std::vector<Ast::Key*> keys;
    keys.push_back(new Ast::KeyModifier(0, 0, Ast::KeyModifier::Direction, new Ast::KeySingle(0, 0, "D"), 0));
    Ast::KeyList * list = new Ast::KeyList(0, 0, keys);

    std::ostringstream script;
    script << "D;";
    return testKeys(list, loadScript(script.str()));
}

int test20(){
    std::vector<Ast::Key*> keys;
    keys.push_back(new Ast::KeyModifier(0, 0, Ast::KeyModifier::Direction, new Ast::KeySingle(0, 0, "D"), 0));
    Ast::KeyList * list = new Ast::KeyList(0, 0, keys);

    std::ostringstream script;
    script << "D;";
    return testKeys(list, loadScript(script.str()));
}

int test21(){
    std::vector<Ast::Key*> keys;
    keys.push_back(new Ast::KeyModifier(0, 0, Ast::KeyModifier::MustBeHeldDown, new Ast::KeySingle(0, 0, "a"), 0));
    keys.push_back(new Ast::KeySingle(0, 0, "b"));
    keys.push_back(new Ast::KeySingle(0, 0, "b"));
    keys.push_back(new Ast::KeySingle(0, 0, "c"));
    Ast::KeyList * list = new Ast::KeyList(0, 0, keys);

    std::ostringstream script;
    script << "a;a,b;a,~b;a,b;a,c;";
    return testKeys(list, loadScript(script.str()));
}

int test22(){
    std::vector<Ast::Key*> keys;
    keys.push_back(new Ast::KeyModifier(0, 0, Ast::KeyModifier::MustBeHeldDown, new Ast::KeySingle(0, 0, "a"), 0));
    keys.push_back(new Ast::KeySingle(0, 0, "b"));
    keys.push_back(new Ast::KeySingle(0, 0, "b"));
    keys.push_back(new Ast::KeySingle(0, 0, "c"));
    Ast::KeyList * list = new Ast::KeyList(0, 0, keys);

    std::ostringstream script;
    script << "a;a,b;~b;b;c;";
    return !testKeys(list, loadScript(script.str()));
}

int test23(){
    std::vector<Ast::Key*> keys;
    keys.push_back(new Ast::KeySingle(0, 0, "a"));
    keys.push_back(new Ast::KeySingle(0, 0, "a"));
    keys.push_back(new Ast::KeySingle(0, 0, "b"));
    Ast::KeyList * list = new Ast::KeyList(0, 0, keys);

    bool fail = true;
    bool success = false;
    
    std::ostringstream script;
    script << "a;~a;a;~a;b";

    vector<Mugen::Input> input = loadScript(script.str());
    
    Mugen::Command2 command1("", list, 1000, 0);

    // a
    if (command1.handle(input[0], 0)){
        return fail;
    }
    
    // ~a
    if (command1.handle(input[1], 1)){
        return fail;
    }
    
    // a
    if (command1.handle(input[2], 2)){
        return fail;
    }

    Token * serialize = command1.serialize();
    Mugen::Command2 command2("", list, 1000, 0);
    command2.deserialize(serialize);

    // ~a
    if (command2.handle(input[3], 3)){
        return fail;
    }
    
    // b
    if (!command2.handle(input[4], 4)){
        return fail;
    }
    
    return success;
}

int runTest(int (*test)(), const std::string & name){
    if (test()){
        Global::debug(0) << name << " failed" << std::endl;
        return 1;
    }
    return 0;
}

int main(int argc, char ** argv){
    if (
        runTest(test1, "Test1") ||
        runTest(test1_1, "Test1_1") ||
        runTest(test2, "Test2") ||
        runTest(test3, "Test3") ||
        runTest(test4, "Test4") ||
        runTest(test5, "Test5") ||
        runTest(test6, "Test6") ||
        runTest(test7, "Test7") ||
        runTest(test8, "Test8") ||
        runTest(test9, "Test9") ||
        runTest(test10, "Test10") ||
        runTest(test11, "Test11") ||
        runTest(test12, "Test12") ||
        runTest(test13, "Test13") ||
        runTest(test14, "Test14") ||
        runTest(test15, "Test15") ||
        runTest(test16, "Test16") ||
        runTest(test17, "Test17") ||
        runTest(test18, "Test18") ||
        runTest(test19, "Test19") ||
        runTest(test20, "Test20") ||
        runTest(test21, "Test21") ||
        runTest(test22, "Test22") ||
        runTest(test23, "Test23") ||
        /* having false here lets us copy/paste a runTest line easily */
        false
        ){
        return 1;
    }
}
