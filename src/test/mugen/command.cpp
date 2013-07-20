#include "mugen/command.h"
#include "mugen/ast/key.h"
#include "util/debug.h"
#include <vector>
#include <string>
#include <set>
#include <sstream>

using std::vector;
using std::string;
using std::ostringstream;

class Constraint{
public:
    enum Type{
        PressA,
        ReleaseA,
        PressB,
        ReleaseB
    };

    Constraint(Type type, double time, bool dominate):
    type(type),
    satisfied(false),
    dominate(dominate),
    time(time),
    emit(false){
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
        }

        out << ":" << time;

        return out.str();
    }

    virtual void satisfy(const Mugen::Input & input){
        if (dependenciesSatisfied()){
            switch (type){
                case PressA: satisfied = input.pressed.a; break;
                case ReleaseA: satisfied = input.released.a; break;
                case PressB: satisfied = input.pressed.b; break;
                case ReleaseB: satisfied = input.released.b; break;
            }
        }
    }

    virtual bool dependenciesSatisfied() const {
        for (std::set<Util::ReferenceCount<Constraint> >::const_iterator it = dependsOn.begin(); it != dependsOn.end(); it++){
            const Util::ReferenceCount<Constraint> & constraint = *it;
            if (!constraint->isSatisfied()){
                return false;
            }
        }

        return true;
    }

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
    
    virtual bool isSatisfied() const {
        return satisfied;
    }

protected:
    Type type;
    bool satisfied;
    bool dominate;
    double time;
    bool emit;
    std::set<Util::ReferenceCount<Constraint> > dependsOn;
};

vector<Util::ReferenceCount<Constraint> > makeConstraints(Ast::Key * key, double time, bool last){
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
            string name = key.toString();
            if (name == "a"){
                addPressRelease(Constraint::PressA, Constraint::ReleaseA);
            } else if (name == "b"){
                addPressRelease(Constraint::PressB, Constraint::ReleaseB);
            }

            /* TODO: rest */
        }

        virtual void onKeyModifier(const Ast::KeyModifier & key){
            switch (key.getModifierType()){
                case Ast::KeyModifier::MustBeHeldDown: {
                    break;
                }
                case Ast::KeyModifier::Release: {

                    class ReleaseWalker: public Ast::Walker {
                    public:
                        ReleaseWalker(ConstraintWalker & outer):
                        outer(outer){
                        }

                        ConstraintWalker & outer;

                        virtual void onKeySingle(const Ast::KeySingle & key){
                            string name = key.toString();
                            if (name == "a"){
                                outer.addPressRelease(Constraint::ReleaseA, Constraint::PressA);
                            } else if (name == "b"){
                                outer.addPressRelease(Constraint::ReleaseB, Constraint::PressB);
                            }
                            /* TODO: rest */
                        }
        
                        virtual void onKeyCombined(const Ast::KeyCombined & key){
                            throw std::exception();
                        }
        
                        virtual void onKeyModifier(const Ast::KeyModifier & key){
                            throw std::exception();
                        }
                    };

                    const Ast::Key * sub = key.getKey();
                    ReleaseWalker walker(*this);
                    sub->walk(walker);

                    break;
                }
                case Ast::KeyModifier::Direction: {
                    break;
                }
                case Ast::KeyModifier::Only: {
                    break;
                }
            }
        }

        virtual void onKeyCombined(const Ast::KeyCombined & key){
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

    return constraints;
}

class NewCommand{
public:
    NewCommand(Ast::KeyList * keys):
    constraints(makeConstraints(keys)){
    }
            
    bool handle(const Mugen::Input & input){
        /* keep checking constraints until we reach a fix point. */
        bool more = true;
        bool emit = false;
        while (more){
            more = false;

            for (vector<Util::ReferenceCount<Constraint> >::iterator it = constraints.begin(); it != constraints.end(); it++){
                Util::ReferenceCount<Constraint> constraint = *it;
                if (!constraint->isSatisfied()){
                    constraint->satisfy(input);
                    if (constraint->isSatisfied()){
                        more = true;
                        if (constraint->isEmit()){
                            emit = true;
                        }
                    }
                }
            }
        }

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
    NewCommand command(keys);

    /* Make sure we emit something */
    bool emitted = false;
    for (vector<Mugen::Input>::const_iterator it = inputs.begin(); it != inputs.end(); it++){
        const Mugen::Input & input = *it;
        bool last = false;
        if (&input == &inputs.back()){
            last = true;
        }

        if (!last){
            /* If its not the last input then the command should not fire, so if it does then its an error */
            if (command.handle(input)){
                return 1;
            }
        } else {
            /* And similarly if its the last input the command should fire, otherwise error */
            if (!command.handle(input)){
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

int runTest(int (*test)(), const std::string & name){
    if (test()){
        Global::debug(0) << name << " failed" << std::endl;
        return 1;
    }
    return 0;
}

int main(int argc, char ** argv){
    if (runTest(test1, "Test1") ||
        runTest(test2, "Test2") ||
        runTest(test3, "Test3") ||
        runTest(test4, "Test4") ||
        runTest(test5, "Test5") ||
        runTest(test6, "Test6") ||
        runTest(test7, "Test7") ||
        runTest(test8, "Test8") ||
        /* having false here lets us copy/paste a runTest line easily */
        false
        ){
        return 1;
    }
}
