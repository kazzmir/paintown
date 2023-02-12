#include "command.h"
#include "constraint.h"
#include "ast/key.h"
#include "r-tech1/debug.h"
#include "r-tech1/token.h"
#include <math.h>
#include <vector>
#include <string>
#include <set>
#include <sstream>
#include <exception>

using std::string;
using std::vector;
using std::set;

namespace Mugen{

typedef PaintownUtil::ReferenceCount<Constraint> ConstraintRef;

ConstraintCompare::ConstraintCompare(ConstraintRef constraint):
constraint(constraint){
}

bool ConstraintCompare::operator==(const ConstraintCompare & him) const {
    return constraint->getId() == him.constraint->getId();
}

bool ConstraintCompare::operator<(const ConstraintCompare & him) const {
    return constraint->getId() < him.constraint->getId();
}

class DelayConstraint: public Constraint {
public:
    DelayConstraint(uint32_t id, int delayTime, Constraint::Type type, double time, bool dominate):
    Constraint(type, id, time, dominate),
    delayTime(delayTime),
    held(0){
    }
 
    Token * serialize() const {
        Token * out = Constraint::serialize();
        *out << held;
        return out;
    }

    using Constraint::deserialize;

    void deserialize(const Token * token){
        TokenView view = token->view();
        Constraint::deserialize(view);
        view >> held;
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
    HoldConstraint(Constraint::Type type, uint32_t id, double time, bool dominate):
    Constraint(type, id, time, dominate){
    }

    virtual string toString() const {
        std::ostringstream out;

        out << "/";
        out << Constraint::toString();

        return out.str();
    }
 
    /* Nothing to serialize because there is no state */
    Token * serialize() const {
        Token * out = new Token();
        *out << "h";
        return out;
    }

    using Constraint::deserialize;
    virtual void deserialize(const Token * token){
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
    CombinedConstraint(uint32_t id,
                       const ConstraintRef & key1,
                       const ConstraintRef & key2,
                       double time, bool dominate):
    Constraint(Combine, id, time, dominate),
    key1(key1),
    key2(key2){
    }

    virtual Token * serialize() const {
        Token * token = new Token();
        *token << "z" << key1->serialize() << key2->serialize();
        return token;
    }

    using Constraint::deserialize;
    virtual void deserialize(const Token * token){
        const Token * token1 = NULL;
        const Token * token2 = NULL;
        token->view() >> token1 >> token2;
        key1->deserialize(token1);
        key2->deserialize(token2);
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

    ConstraintRef key1;
    ConstraintRef key2;
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
    if (name == "s"){
        return Constraint::PressStart;
    }

    /* utter failure */
    throw MugenException("Unknown key press", __FILE__, __LINE__);
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
    if (name == "s"){
        return Constraint::ReleaseStart;
    }

    /* utter failure */
    throw MugenException("Unknown key press", __FILE__, __LINE__);
    return Constraint::ReleaseA;
}

class IdMaker{
public:
    IdMaker():
        id(0){
        }

    uint32_t next(){
        uint32_t value = id;
        id += 1;
        return value;
    }

    uint32_t id;
};

vector<ConstraintRef > makeConstraints(IdMaker & id, const Ast::Key * key, double time, bool last){
    vector<ConstraintRef> constraints;

    class ConstraintWalker: public Ast::Walker {
    public:
        ConstraintWalker(vector<ConstraintRef> & constraints, double time, bool last, IdMaker & id):
        constraints(constraints),
        time(time),
        last(last),
        id(id){
        }
        
        vector<ConstraintRef> & constraints;
        double time;
        bool last;
        IdMaker & id;

        virtual void addPressRelease(Constraint::Type pressKey, Constraint::Type releaseKey){
            ConstraintRef press = ConstraintRef(new Constraint(pressKey, id.next(), time, true));
            ConstraintRef release = ConstraintRef(new Constraint(releaseKey, id.next(), time + 0.5, false));
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
                        HoldWalker(ConstraintWalker & outer, double time, bool last, IdMaker & id):
                        outer(outer),
                        time(time),
                        last(last),
                        id(id){
                        }
                        
                        ConstraintWalker & outer;
                        const double time;
                        const bool last;
                        IdMaker & id;
                        
                        virtual void onKeySingle(const Ast::KeySingle & key){
                            Constraint::Type pressKey = getPress(key);
                            ConstraintRef constraint = ConstraintRef(new HoldConstraint(pressKey, id.next(), time, true));
                            outer.constraints.push_back(constraint);
                            if (last){
                                constraint->setEmit();
                            }
                        }

                        virtual void onKeyCombined(const Ast::KeyCombined & key){
                            throw MugenException("Cannot handle key combined", __FILE__, __LINE__);
                        }
        
                        virtual void onKeyModifier(const Ast::KeyModifier & key){
                            switch (key.getModifierType()){
                                case Ast::KeyModifier::Direction: {
                                    key.getKey()->walk(*this);
                                    break;
                                }
                                default: {
                                    throw MugenException("Cannot handle modifier type", __FILE__, __LINE__);
                                }
                            }
                        }
                    };
                    
                    const Ast::Key * sub = key.getKey();
                    HoldWalker walker(*this, time, last, id);
                    sub->walk(walker);

                    break;
                }
                case Ast::KeyModifier::Release: {

                    class ReleaseWalker: public Ast::Walker {
                    public:
                        ReleaseWalker(ConstraintWalker & outer, int hold, double time, IdMaker & id):
                        outer(outer),
                        hold(hold),
                        time(time),
                        id(id){
                        }

                        ConstraintWalker & outer;
                        int hold;
                        double time;
                        IdMaker & id;

                        virtual void onKeySingle(const Ast::KeySingle & key){
                            string name = key.toString();

                            Constraint::Type pressKey = getPress(key);
                            Constraint::Type releaseKey = getRelease(key);

                            if (hold < 1){
                                hold = 1;
                            }

                            outer.constraints.push_back(ConstraintRef(new DelayConstraint(id.next(), hold, pressKey, time, true)));
                            ConstraintRef release = ConstraintRef(new Constraint(releaseKey, id.next(), time + 0.1, true));
                            if (outer.last){
                                release->setEmit();
                            }
                            outer.constraints.push_back(release);

                        }
        
                        virtual void onKeyCombined(const Ast::KeyCombined & key){
                            throw MugenException("Cannot handle key combined", __FILE__, __LINE__);
                        }
        
                        virtual void onKeyModifier(const Ast::KeyModifier & key){
                            switch (key.getModifierType()){
                                case Ast::KeyModifier::Direction: {
                                    key.getKey()->walk(*this);
                                    break;
                                }
                                default: {
                                    throw MugenException("Cannot handle modifier type", __FILE__, __LINE__);
                                }
                            }
                        }
                    };

                    const Ast::Key * sub = key.getKey();
                    ReleaseWalker walker(*this, key.getExtra(), time, id);
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

        ConstraintRef findDominate(const vector<ConstraintRef> & keys){
            for (vector<ConstraintRef>::const_iterator it = keys.begin(); it != keys.end(); it++){
                const ConstraintRef & constraint = *it;
                if (constraint->isDominate()){
                    return constraint;
                }
            }

            return ConstraintRef(NULL);
        }

        /* For a combined key we have to check that the two sub-keys were pressed
         * at roughly the same time. We can implement that by checking when their Press
         * constraint was satisfied. If both constraints are satisfied within 0-4 ticks, or whatever,
         * then the combined constraint should satisfy.
         */
        virtual void onKeyCombined(const Ast::KeyCombined & key){
            vector<ConstraintRef> key1 = makeConstraints(id, key.getKey1(), time, false);
            vector<ConstraintRef> key2 = makeConstraints(id, key.getKey2(), time, false);
            
            constraints.insert(constraints.end(), key1.begin(), key1.end()); 
            constraints.insert(constraints.end(), key2.begin(), key2.end()); 
            ConstraintRef dominateKey1 = findDominate(key1);
            ConstraintRef dominateKey2 = findDominate(key2);
            ConstraintRef combined = ConstraintRef(new CombinedConstraint(id.next(), dominateKey1, dominateKey2, time + 0.9, true));
            constraints.push_back(combined);
            if (last){
                combined->setEmit();
            }
        }
    };

    ConstraintWalker walker(constraints, time, last, id);
    key->walk(walker);

    return constraints;
}

void constructDependencies(const vector<ConstraintRef> & constraints){
    for (vector<ConstraintRef>::const_iterator it = constraints.begin(); it != constraints.end(); it++){
        const ConstraintRef & constraint = *it;
        constraint->setUpConstraints(constraints);
    }
}

static void topologicalVisit(ConstraintCompare constraint, vector<ConstraintRef> & sorted, set<ConstraintCompare> & unsorted){
    const set<ConstraintCompare> & depends = constraint.constraint->getDepends();
    unsorted.erase(constraint);
    for (set<ConstraintCompare>::const_iterator it = depends.begin(); it != depends.end(); it++){
        ConstraintCompare depend = *it;
        if (unsorted.find(depend) != unsorted.end()){
            topologicalVisit(depend, sorted, unsorted);
        }
    }
    sorted.push_back(constraint.constraint);
}

vector<ConstraintRef> topologicalSort(const vector<ConstraintRef> & constraints){
    vector<ConstraintRef> out;
    set<ConstraintCompare> unsorted;
    for (vector<ConstraintRef>::const_iterator it = constraints.begin(); it != constraints.end(); it++){
        unsorted.insert(*it);
    }

    while (unsorted.size() > 0){
        ConstraintCompare first = *unsorted.begin();
        topologicalVisit(first, out, unsorted);
    }

    return out;
}

vector<ConstraintRef> makeConstraints(Ast::KeyList * keys){
    vector<ConstraintRef> constraints;
    double time = 1;
    IdMaker id;
    for (vector<Ast::Key*>::const_iterator it = keys->getKeys().begin(); it != keys->getKeys().end(); it++){
        bool last = false;
        Ast::Key * key = *it;
        if (key == keys->getKeys().back()){
            last = true;
        }

        vector<ConstraintRef> more = makeConstraints(id, *it, time, last);
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

Constraint::Constraint(Type type, uint32_t id, double time, bool dominate):
type(type),
    satisfied(false),
    dominate(dominate),
    time(time),
    emit(false),
    satisfiedTick(0),
    id(id){
    }

Constraint::~Constraint(){
}

Constraint::Type Constraint::getType() const {
    return type;
}

void Constraint::deserialize(TokenView & view){
    view >> satisfied >> satisfiedTick;
}

void Constraint::deserialize(const Token * token){
    TokenView view = token->view();
    deserialize(view);
}

Token * Constraint::serialize() const {
    Token * top = new Token();
    *top << "c";
    *top << satisfied;
    *top << satisfiedTick;

    return top;
}

void Constraint::setEmit(){
    this->emit = true;
}

bool Constraint::isEmit() const {
    return emit;
}

void Constraint::setUpConstraints(const vector<ConstraintRef> & constraints){
    for (vector<ConstraintRef>::const_iterator it = constraints.begin(); it != constraints.end(); it++){
        const ConstraintRef & constraint = *it;
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

bool Constraint::sameKey(const Constraint & him){
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

        case PressStart:
        case ReleaseStart: return him.getType() == PressStart ||
                                  him.getType() == ReleaseStart;

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

std::string Constraint::toString() const {
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
        case PressStart: out << "s"; break;
        case ReleaseStart: out << "~s"; break;
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

bool Constraint::doSatisfy(const Mugen::Input & input, int tick){
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
        case PressStart: return input.pressed.start; break;
        case ReleaseStart: return input.released.start; break;

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

bool Constraint::satisfy(const Mugen::Input & input, int tick){
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

int Constraint::getSatisfiedTick() const {
    return satisfiedTick;
}

void Constraint::depends(const ConstraintRef & him){
    dependsOn.insert(him);
}

void Constraint::reset(){
    satisfied = false;
    satisfiedTick = 0;
}

uint32_t Constraint::getId() const {
    return id;
}

bool Constraint::isDominate() const {
    return dominate;
}

double Constraint::getTime() const {
    return time;
}
    
const std::set<ConstraintCompare> & Constraint::getDepends(){
    return dependsOn;
}

Command2::Command2(const std::string & name, Ast::KeyList * keys, int maxTime, int bufferTime):
constraints(makeConstraints(keys)),
name(name),
maxTime(maxTime),
bufferTime(bufferTime),
useBufferTime(0),
emitted(false){
}
    
const std::string & Command2::getName() const {
    return name;
}

void Command2::resetConstraints(){
    for (vector<ConstraintRef>::iterator it = constraints.begin(); it != constraints.end(); it++){
        ConstraintRef ref = *it;
        ref->reset();
    }

    emitted = false;
}

int Command2::activeTicks(int ticks){
    if (constraints.size() > 0){
        ConstraintRef ref = constraints[0];
        int constraintTick = ref->getSatisfiedTick();
        if (constraintTick > 0 && constraintTick < ticks){
            return ticks - constraintTick;
        }
    }

    return 0;
}

static bool emptyInput(const Mugen::Input & input){
    return !input.pressed.a &&
           !input.pressed.b &&
           !input.pressed.c &&
           !input.pressed.x &&
           !input.pressed.y &&
           !input.pressed.z &&
           !input.pressed.start &&
           !input.pressed.up &&
           !input.pressed.down &&
           !input.pressed.forward &&
           !input.pressed.back &&

           !input.released.a &&
           !input.released.b &&
           !input.released.c &&
           !input.released.x &&
           !input.released.y &&
           !input.released.z &&
           !input.released.start &&
           !input.released.up &&
           !input.released.down &&
           !input.released.forward &&
           !input.released.back;
}

bool Command2::handle(const Mugen::Input & input, int ticks){
    if (useBufferTime > 0){
        useBufferTime -= 1;
        return true;
    }

    if (activeTicks(ticks) > maxTime && emptyInput(input)){
        resetConstraints();
    }

    bool emit = false;
    std::set<ConstraintRef> satisfied;
    for (vector<ConstraintRef>::iterator it = constraints.begin(); it != constraints.end(); it++){
        bool all = true;
        ConstraintRef constraint = *it;
        const std::set<ConstraintCompare> & depends = constraint->getDepends();
        for (std::set<ConstraintCompare>::const_iterator it2 = depends.begin(); it2 != depends.end(); it2++){
            /* If its not in the satisified set then not all dependencies can be satisifed */
            if (satisfied.find(it2->constraint) == satisfied.end()){
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

    /* Can only emit once until a reset */
    if (emitted){
        emit = false;
    }

    if (emit){
        useBufferTime = bufferTime - 1;
        emitted = true;
    }

    /* Reset the constraints if they were all satisfied. */
    if (satisfied.size() == constraints.size()){
        resetConstraints();
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
    
Token * Command2::serialize() const {
    Token * out = new Token();
    *out << getName();
    *out << useBufferTime;
    *out << emitted;
    for (vector<ConstraintRef>::const_iterator it = constraints.begin(); it != constraints.end(); it++){
        const ConstraintRef & constraint = *it;
        *out << constraint->serialize();
    }
    return out;
}

void Command2::deserialize(const Token * token){
    TokenView view = token->view();
    view >> useBufferTime >> emitted;
    for (vector<ConstraintRef>::iterator it = constraints.begin(); it != constraints.end(); it++){
        ConstraintRef & ref = *it;
        const Token * next = NULL;
        view >> next;
        ref->deserialize(next);
    }
}

}
