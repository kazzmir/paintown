#include "util/bitmap.h"
#include "ast/all.h"
#include "state-controller.h"
#include "util/parameter.h"
#include "character.h"
#include "helper.h"
#include "config.h"
#include "stage.h"
#include "projectile.h"
#include "sound.h"
#include "util/debug.h"
#include <sstream>
#include "exception.h"

using namespace std;

/* TODO: go through each trigger and write how far done it is, 50% or 100%
 * TODO: Remove Compiler::Value* and replace it with Value
 */

namespace Mugen{
        
typedef PaintownUtil::ClassPointer<Compiler::Value> Value;

void compileError(const std::string & fail, const std::string & where, int line){
    throw MugenException(fail, where, line);
}

Value copy(const Value & value){
    if (value != NULL){
        return Value(value->copy());
    }
    return Value(NULL);
}

StateController::StateController(const string & name, int state):
type(Unknown),
name(name),
debug(false),
persistent(1),
currentPersistent(1),
state(state){
}

StateController::StateController(const string & name, int state, Ast::Section * section):
type(Unknown),
name(name),
debug(false),
persistent(1),
currentPersistent(1),
state(state),
spritePriority(0){
    class Walker: public Ast::Walker {
    public:
        Walker(StateController & controller):
            controller(controller){
            }

        StateController & controller;

        virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
            if (simple == "triggerall"){
                controller.addTriggerAll(Compiler::compile(simple.getValue()));
            } else if (PaintownUtil::matchRegex(PaintownUtil::lowerCaseAll(simple.idString()), "trigger[0-9]+")){
                int trigger = atoi(PaintownUtil::captureRegex(PaintownUtil::lowerCaseAll(simple.idString()), "trigger([0-9]+)", 0).c_str());
                controller.addTrigger(trigger, Compiler::compile(simple.getValue()));
            } else if (simple == "persistent"){
                try{
                    simple.view() >> controller.persistent;
                    controller.currentPersistent = controller.persistent;
                } catch (const Ast::Exception & fail){
                    /* No values for persistent.. */
                }
            } else if (simple == "debug"){
                controller.debug = true;
            } else if (simple == "sprpriority"){
                controller.spritePriority = Compiler::compile(simple.getValue());
            } else if (simple == "ignorehitpause"){
                if (simple.getValue() != NULL){
                    controller.ignoreHitPauseValue = Compiler::compile(simple.getValue());
                }
            }
        }
    };
    Walker walker(*this);
    section->walk(walker);
}
    
StateController::StateController(const StateController & you):
type(you.type),
name(you.name),
debug(you.debug),
persistent(you.persistent),
currentPersistent(you.currentPersistent),
ignoreHitPauseValue(copy(you.ignoreHitPauseValue)),
state(you.state),
spritePriority(copy(you.spritePriority)){
    for (map<int, vector<Compiler::Value*> >::const_iterator it = you.triggers.begin(); it != you.triggers.end(); it++){
        int number = it->first;
        const vector<Compiler::Value*> & all = it->second;
        for (vector<Compiler::Value*>::const_iterator all_it = all.begin(); all_it != all.end(); all_it++){
            Compiler::Value * copied = Compiler::copy(*all_it);
            if (copied == NULL){
                throw MugenFatalRuntimeException("Trigger should not be null", __FILE__, __LINE__);
            }
            addTrigger(number, copied);
        }
    }
}

bool StateController::handled(const Ast::AttributeSimple & simple){
    string name = PaintownUtil::lowerCaseAll(simple.idString());
    return name.find("trigger") == 0 ||
           name == "sprpriority" ||
           name == "debug" ||
           name == "persistent";
}

StateController::~StateController(){
    for (map<int, vector<Compiler::Value*> >::iterator it = triggers.begin(); it != triggers.end(); it++){
        vector<Compiler::Value*> values = (*it).second;
        for (vector<Compiler::Value*>::iterator value_it = values.begin(); value_it != values.end(); value_it++){
            Compiler::Value * value = *value_it;
            delete value;
        }
    }
}

double evaluateNumber(Compiler::Value * value, const Environment & env, double default_){
    if (value != NULL){
        return value->evaluate(env).toNumber();
    }
    return default_;
}

double evaluateNumber(const Value & value, const Environment & env, double default_){
    if (value != NULL){
        return value->evaluate(env).toNumber();
    }
    return default_;
}

string evaluateString(const Value & value, const Environment & env, string default_){
    if (value != NULL){
        return value->evaluate(env).getStringValue();
    }
    return default_;
}

double evaluateBool(Compiler::Value * value, const Environment & env, double default_){
    if (value != NULL){
        return value->evaluate(env).toBool();
    }
    return default_;
}

bool evaluateBool(const Value & value, const Environment & env, double default_){
    if (value != NULL){
        return value->evaluate(env).toBool();
    }
    return default_;
}

   
void StateController::resetPersistent(){
    currentPersistent = persistent;
}
    
bool StateController::ignoreHitPause(const Environment & environment) const {
    return evaluateBool(ignoreHitPauseValue, environment, false);
}

bool StateController::persistentOk(){
    /* count down from the persistent value to 0, then reset back to
     * the persistent value. the controller can activate if the current
     * persistent value reaches 0.
     */
    if (currentPersistent > 0){
        currentPersistent -= 1;
        bool b = currentPersistent == 0;
        if (b){
            currentPersistent = persistent;
        }
        return b;
    } else {
        /* if the persistent value was originally 0 then it can only activate
         * once while the state is active.
         */
        bool b = currentPersistent == 0;
        if (b){
            currentPersistent -= 1;
        }
        return b;
    }
}

void StateController::addTriggerAll(Compiler::Value * trigger){
    triggers[-1].push_back(trigger);
}

void StateController::addTrigger(int number, Compiler::Value * trigger){
    triggers[number].push_back(trigger);
}

bool StateController::canTrigger(const Compiler::Value * expression, const Environment & environment) const {
    try{
        /* this makes it easy to break in gdb */
        /*
        if (debug){
            int x = 2;
            x += 1;
        }
        */
        RuntimeValue result = expression->evaluate(environment);
        return result.toBool();
    } catch (const MugenNormalRuntimeException e){
        ostringstream out;
        out << "Expression `" << expression->toString() << "' " << e.getFullReason();
        throw MugenNormalRuntimeException(out.str(), __FILE__, __LINE__);
    } catch (const MugenFatalRuntimeException e){
        ostringstream out;
        out << "Expression `" << expression->toString() << "' " << e.getFullReason();
        throw MugenFatalRuntimeException(out.str(), __FILE__, __LINE__);
    } catch (const MugenException & e){
        ostringstream out;
        out << "Expression `" << expression->toString() << "' " << e.getFullReason();
        throw MugenException(out.str(), __FILE__, __LINE__);
    }
}

bool StateController::canTrigger(const vector<Compiler::Value*> & expressions, const Environment & environment) const {
    for (vector<Compiler::Value*>::const_iterator it = expressions.begin(); it != expressions.end(); it++){
        const Compiler::Value * value = *it;
        if (!canTrigger(value, environment)){
            // Global::debug(2*!getDebug()) << "'" << value->toString() << "' did not trigger" << endl;
            if (getDebug() || Global::getDebug() >= 2){
                Global::debug(2*!getDebug()) << "'" << value->toString() << "' did not trigger" << endl;
            }
            return false;
        } else {
            // Global::debug(2*!getDebug()) << "'" << value->toString() << "' did trigger" << endl;
            if (getDebug() || Global::getDebug() >= 2){
                Global::debug(2*!getDebug()) << "'" << value->toString() << "' did trigger" << endl;
            }
        }
    }
    return true;
}

vector<int> StateController::sortTriggers() const {
    vector<int> out;

    for (map<int, vector<Compiler::Value*> >::const_iterator it = triggers.begin(); it != triggers.end(); it++){
        int number = it->first;
        /* ignore triggerall (-1) */
        if (number != -1){
            out.push_back(number);
        }
    }

    sort(out.begin(), out.end());

    return out;
}

bool StateController::canTrigger(const Environment & environment) const {
    /* Only characters that are shaking from the hit and in an attack state have to deal with
     * ignorehitpause.
     * The character that is hit will probably be in a hit state (or idle) but his states
     * continue to execute as normal.
     */
    if (!ignoreHitPause(environment) && environment.getCharacter().isPaused() && environment.getCharacter().getMoveType() == Move::Attack){
        return false;
    }

    map<int, vector<Compiler::Value*> >::const_iterator triggerAll = triggers.find(-1);
    if (triggerAll != triggers.end()){
        const vector<Compiler::Value*> & values = triggerAll->second;
        /* if the triggerall fails then no triggers will work */
        if (!canTrigger(values, environment)){
            return false;
        }
    }

    vector<int> keys = sortTriggers();
    for (vector<int>::iterator it = keys.begin(); it != keys.end(); it++){
        const vector<Compiler::Value*> & values = triggers.find(*it)->second;
        /* if a trigger succeeds then stop processing and just return true */
        if (canTrigger(values, environment)){
            return true;
        }
    }

    return false;
}

static TransType parseTrans(const string & what){
    if (what == "default"){
        return Default;
    } else if (what == "none"){
        return None;
    } else if (what == "add"){
        return Add;
    } else if (what == "addalpha"){
        return AddAlpha;
    } else if (what == "add1"){
        return Add1;
    } else if (what == "sub"){
        return Sub;
    } else {
        Global::debug(0) << "Invalid trans type `" << what << "'" << endl;
        return Default;
    }
}

/* helpers */

/* Compile a single value from some attribute in a section.
 * [foo]
 * x = 2
 * 
 * extractAttribute(foo, "x") -> 2
 */
Compiler::Value * extractAttribute(Ast::Section * section, const string & name){
    class Walker: public Ast::Walker {
    public:
        Walker(const string & name):
        name(name),
        value(NULL){
        }

        const string & name;
        Compiler::Value * value;

        virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
            if (simple == name){
                value = Compiler::compile(simple.getValue());
            }
        }
    };

    Walker walker(name);
    section->walk(walker);
    return walker.value;
}

const Ast::AttributeSimple * findAttribute(Ast::Section * section, const string & name){
    class Walker: public Ast::Walker {
    public:
        Walker(const string & name):
        name(name),
        attribute(NULL){
        }

        const string & name;
        const Ast::AttributeSimple * attribute;

        virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
            if (simple == name){
                attribute = &simple;
            }
        }
    };

    Walker walker(name);
    section->walk(walker);
    return walker.attribute;
}

static void extractValue(Value & value, Ast::Section * section){
    value = extractAttribute(section, "value");
    /*
    class Walker: public Ast::Walker {
    public:
        Walker(Compiler::Value *& value, name):
        value(value),
        name(name){
        }

        Compiler::Value *& value;
        const string & name;

        virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
            if (simple == name){
                value = Compiler::compile(simple.getValue());
            }
        }
    };
    Walker walker(value, name);
    section->walk(walker);
    */
}

static void readValues(const Ast::Value * value, Value & out1){
    const Ast::Value * value1 = NULL;
    value->view() >> value1;
    if (value1 != NULL){
        out1 = Compiler::compile(value1);
    }
}

static void readValues(const Ast::Value * value, Value & out1, Value & out2){
    const Ast::Value * value1 = NULL;
    const Ast::Value * value2 = NULL;
    if (value->hasMultiple()){
        try{
            value->view() >> value1 >> value2;
        } catch (const Ast::Exception & fail){
            /* dont care I guess */
        }
    } else {
        /* just one value */
        value->view() >> value1;
    }
    if (value1 != NULL){
        out1 = Compiler::compile(value1);
    }
    if (value2 != NULL){
        out2 = Compiler::compile(value2);
    }
}

static void readValues(const Ast::Value * value, Value & out1, Value & out2, Value & out3){
    const Ast::Value * value1 = NULL;
    const Ast::Value * value2 = NULL;
    const Ast::Value * value3 = NULL;
    if (value->hasMultiple()){
        try{
            value->view() >> value1 >> value2 >> value3;
        } catch (const Ast::Exception & fail){
            /* dont care I guess */
        }
    } else {
        /* just one value */
        value->view() >> value1;
    }
    if (value1 != NULL){
        out1 = Compiler::compile(value1);
    }
    if (value2 != NULL){
        out2 = Compiler::compile(value2);
    }
    if (value3 != NULL){
        out3 = Compiler::compile(value3);
    }
}

/* reads at most 4 values from an attribute, if a value is missing then the resulting
 * out parameter will just be null.
 */
static void readValues(const Ast::Value * value, Value & out1, Value & out2, Value & out3, Value & out4){
    const Ast::Value * value1 = NULL;
    const Ast::Value * value2 = NULL;
    const Ast::Value * value3 = NULL;
    const Ast::Value * value4 = NULL;
    if (value->hasMultiple()){
        try{
            value->view() >> value1 >> value2 >> value3 >> value4;
        } catch (const Ast::Exception & fail){
            /* dont care I guess */
        }
    } else {
        /* just one value */
        value->view() >> value1;
    }
    if (value1 != NULL){
        out1 = Compiler::compile(value1);
    }
    if (value2 != NULL){
        out2 = Compiler::compile(value2);
    }
    if (value3 != NULL){
        out3 = Compiler::compile(value3);
    }
    if (value4 != NULL){
        out4 = Compiler::compile(value4);
    }
}

struct Resource{
    Resource():
        own(false),
        fight(false),
        value(NULL){
        }

    bool own;
    bool fight;
    const Ast::Value * value;
};

/* Either the value is a Resource in which case we should extract the underlying value
 * or its just some value so use it directly.
 */
static Resource extractResource(const Ast::Value * value){
    class Walker: public Ast::Walker {
    public:
        virtual void onResource(const Ast::Resource & resource){
            this->resource.own = resource.isOwn();
            this->resource.fight = resource.isFight();
            this->resource.value = resource.getValue();
        }

        Resource resource;
    };

    Walker walker;
    value->walk(walker);
    if (walker.resource.value == NULL){
        walker.resource.own = false;
        walker.resource.fight = false;
        walker.resource.value = value;
    }
    return walker.resource;
}

/* 50% */
class ControllerChangeAnim: public StateController {
public:
    ControllerChangeAnim(Ast::Section * section, const string & name, int state):
        StateController(name, state, section){
            parse(section);
        }

    ControllerChangeAnim(const ControllerChangeAnim & you):
    StateController(you),
    value(copy(you.value)),
    element(copy(you.element)){
    }

    Value value;
    Value element;

    void parse(Ast::Section * section){
        extractValue(value, section);
        element = extractAttribute(section, "elem");

        if (value == NULL){
            ostringstream out;
            out << "Expected the `value' attribute for state " << name;
            throw MugenException(out.str(), __FILE__, __LINE__);
        }
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment environment(stage, guy, commands);
        RuntimeValue result = value->evaluate(environment);
        if (result.isDouble()){
            int value = (int) result.getDoubleValue();
            int element = evaluateNumber(this->element, environment, 0);
            guy.setAnimation(value, element);
        }
    }

    StateController * deepCopy() const {
        return new ControllerChangeAnim(*this);
    }
};

class ControllerChangeState: public StateController {
public:
    ControllerChangeState(Ast::Section * section, const std::string & name, int state):
        StateController(name, state, section){
            parse(section);
        }

    ControllerChangeState(const ControllerChangeState & you):
    StateController(you),
    value(copy(you.value)),
    control(copy(you.control)){
    }

    Value value;
    Value control;

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(Value & value, Value & control):
                value(value),
                control(control){
                }

            Value & value;
            Value & control;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "value"){
                    value = Compiler::compile(simple.getValue());
                } else if (simple == "ctrl"){
                    control = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(value, control);
        section->walk(walker);
        if (value == NULL){
            ostringstream out;
            out << "Expected the `value' attribute for state " << name;
            throw MugenException(out.str(), __FILE__, __LINE__);
        }
    }

    virtual ~ControllerChangeState(){
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        if (control != NULL){
            guy.setControl(control->evaluate(FullEnvironment(stage, guy)).toBool());
        }

        RuntimeValue result = value->evaluate(FullEnvironment(stage, guy));
        if (result.isDouble()){
            int value = (int) result.getDoubleValue();
            guy.changeState(stage, value);
        }
    }

    StateController * deepCopy() const {
        return new ControllerChangeState(*this);
    }
};

class ControllerCtrlSet: public StateController {
public:
    ControllerCtrlSet(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        parse(section);
    }

    ControllerCtrlSet(const ControllerCtrlSet & you):
    StateController(you),
    value(copy(you.value)){
    }

    Value value;

    void parse(Ast::Section * section){
        extractValue(value, section);
        if (value == NULL){
            ostringstream out;
            out << "Expected the `value' attribute for state " << name;
            throw MugenException(out.str(), __FILE__, __LINE__);
        }
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        RuntimeValue result = value->evaluate(FullEnvironment(stage, guy));
        guy.setControl(toBool(result));
    }

    StateController * deepCopy() const {
        return new ControllerCtrlSet(*this);
    }
};

class ControllerPlaySound: public StateController {
public:
    ControllerPlaySound(Ast::Section * section, const string & name, int state):
        StateController(name, state, section),
        own(false){
            parse(section);
        }

    ControllerPlaySound(const ControllerPlaySound & you):
    StateController(you),
    group(copy(you.group)),
    own(you.own),
    item(copy(you.item)){
    }

    Value group;
    bool own;
    Value item;

    virtual ~ControllerPlaySound(){
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerPlaySound & controller):
                controller(controller){
                }

            ControllerPlaySound & controller;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "value"){
                    controller.parseSound(simple.getValue());
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void parseSound(const Ast::Value * value){
        try{
            const Ast::Value * group = NULL;
            const Ast::Value * item = NULL;
            if (value->hasMultiple()){
                value->view() >> group >> item;
            } else {
                group = value;
            }

            if (group == NULL){
                compileError("No group given for value in PlaySnd", __FILE__, __LINE__);
            }

            Resource resource = extractResource(group);
            this->own = resource.fight;
            this->group = Compiler::compile(resource.value);

            if (item != NULL){
                this->item = Compiler::compile(item);
            } else {
                this->item = Compiler::compile(0);
            }
        } catch (const MugenException & e){
            // Global::debug(0) << "Error with PlaySnd " << controller.name << ": " << e.getReason() << endl;
            Global::debug(0) << "Error with PlaySnd: " << e.getReason() << endl;
        }

    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        PaintownUtil::ReferenceCount<Mugen::Sound> sound = PaintownUtil::ReferenceCount<Mugen::Sound>(NULL);
        if (item != NULL){
            int groupNumber = (int) group->evaluate(FullEnvironment(stage, guy)).toNumber();
            int itemNumber = (int) item->evaluate(FullEnvironment(stage, guy)).toNumber();
            if (own){
                sound = guy.getCommonSound(groupNumber, itemNumber);
            } else {
                sound = guy.getSound(groupNumber, itemNumber);
            }
        }

        if (sound != NULL){
            sound->play();
        }
    }

    StateController * deepCopy() const {
        return new ControllerPlaySound(*this);
    }
};

class ControllerVarSet: public StateController {
public:
    ControllerVarSet(Ast::Section * section, const string & name, int state):
        StateController(name, state, section){
            parse(section);
        }

    ControllerVarSet(const ControllerVarSet & you):
    StateController(you),
    value(copy(you.value)),
    variable(copy(you.variable)){
        copy_map(variables, you.variables);
        copy_map(floatVariables, you.floatVariables);
        copy_map(systemVariables, you.systemVariables);
    }

    void copy_map(map<int, Compiler::Value*>& mine, const map<int, Compiler::Value*> & his){
        for (map<int, Compiler::Value*>::const_iterator it = his.begin(); it != his.end(); it++){
            mine[it->first] = Compiler::copy(it->second);
        }
    }

    Value value;
    Value variable;
    map<int, Compiler::Value*> variables;
    map<int, Compiler::Value*> floatVariables;
    map<int, Compiler::Value*> systemVariables;

    virtual ~ControllerVarSet(){
        delete_map(variables);
        delete_map(floatVariables);
        delete_map(systemVariables);
    }

    void delete_map(const map<int, Compiler::Value*> & values){
        for (map<int, Compiler::Value*>::const_iterator it = values.begin(); it != values.end(); it++){
            Compiler::Value * value = (*it).second;
            delete value;
        }
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerVarSet & controller):
                controller(controller){
                }

            ControllerVarSet & controller;

            virtual void onAttributeArray(const Ast::AttributeArray & simple){
                if (simple == "var"){
                    int index = simple.getIndex();
                    const Ast::Value * value = simple.getValue();
                    controller.variables[index] = Compiler::compile(value);
                } else if (simple == "fvar"){
                    int index = simple.getIndex();
                    const Ast::Value * value = simple.getValue();
                    controller.floatVariables[index] = Compiler::compile(value);
                } else if (simple == "sysvar"){
                    int index = simple.getIndex();
                    const Ast::Value * value = simple.getValue();
                    controller.systemVariables[index] = Compiler::compile(value);
                }
            }

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "value"){
                    controller.value = Compiler::compile(simple.getValue());
                } else if (simple == "v"){
                    controller.variable = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment environment(stage, guy, commands);
        for (map<int, Compiler::Value*>::const_iterator it = variables.begin(); it != variables.end(); it++){
            int index = (*it).first;
            Compiler::Value * value = (*it).second;
            guy.setVariable(index, value->evaluate(environment));
        }

        for (map<int, Compiler::Value*>::const_iterator it = floatVariables.begin(); it != floatVariables.end(); it++){
            int index = (*it).first;
            Compiler::Value * value = (*it).second;
            guy.setFloatVariable(index, value->evaluate(environment));
        }

        for (map<int, Compiler::Value*>::const_iterator it = systemVariables.begin(); it != systemVariables.end(); it++){
            int index = (*it).first;
            Compiler::Value * value = (*it).second;
            guy.setSystemVariable(index, value->evaluate(environment));
        }

        if (value != NULL && variable != NULL){
            FullEnvironment environment(stage, guy, commands);
            /* 'value = 23' is value1
             * 'v = 9' is value2
             */
            /* FIXME: whats the default here? 0? */
            guy.setVariable((int) evaluateNumber(variable, environment, 0),
                            value->evaluate(environment));
        }
    }

    StateController * deepCopy() const {
        return new ControllerVarSet(*this);
    }
};

class ControllerVelSet: public StateController {
public:
    ControllerVelSet(Ast::Section * section, const string & name, int state):
        StateController(name, state, section){
            parse(section);
        }

    ControllerVelSet(const ControllerVelSet & you):
    StateController(you),
    x(copy(you.x)),
    y(copy(you.y)){
    }

    Value x;
    Value y;

    virtual ~ControllerVelSet(){
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerVelSet & controller):
                controller(controller){
                }

            ControllerVelSet & controller;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "x"){
                    controller.x = Compiler::compile(simple.getValue());
                } else if (simple == "y"){
                    controller.y = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        if (x != NULL){
            RuntimeValue result = x->evaluate(FullEnvironment(stage, guy));
            if (result.isDouble()){
                guy.setXVelocity(result.getDoubleValue());
            }
        }
        if (y != NULL){
            RuntimeValue result = y->evaluate(FullEnvironment(stage, guy));
            if (result.isDouble()){
                guy.setYVelocity(result.getDoubleValue());
            }
        }
    }

    StateController * deepCopy() const {
        return new ControllerVelSet(*this);
    }
};

class ControllerSndPan: public StateController {
public:
    ControllerSndPan(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        parse(section);
    }
    
    ControllerSndPan(const ControllerSndPan & you):
    StateController(you),
    channel(copy(you.channel)),
    pan(copy(you.pan)),
    absolutePan(copy(you.absolutePan)){
    }

    Value channel;
    Value pan;
    Value absolutePan;

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerSndPan & controller):
            controller(controller){
            }

            ControllerSndPan & controller;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "channel"){
                    controller.channel = Compiler::compile(simple.getValue());
                } else if (simple == "pan"){
                    controller.pan = Compiler::compile(simple.getValue());
                } else if (simple == "abspan"){
                    controller.absolutePan = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);

        if (channel == NULL){
            compileError("No channel given for SndPan", __FILE__, __LINE__);
        }

        if (pan == NULL && absolutePan == NULL){
            compileError("No pan or abspan given for SndPan", __FILE__, __LINE__);
        }
    }
    
    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment environment(stage, guy, commands);
        int channel = (int) evaluateNumber(this->channel, environment, 0);
        int pan = 0;
        if (this->pan != NULL){
            /* FIXME: These parameters cannot both be specified at the same time. p determines the sound offset in pixels from the player (in the case of pan) or from the center of the screen (in the case of abspan). See PlaySnd for a description of the panning parameters.
             */
            pan = (int) evaluateNumber(this->pan, environment, 0);
        }
        if (this->absolutePan != NULL){
            pan = (int) evaluateNumber(this->absolutePan, environment, 0);
        }

        /* TODO: adjust the pan of a sound */
    }
    
    StateController * deepCopy() const {
        return new ControllerSndPan(*this);
    }
};

class ControllerTargetVelSet: public StateController {
public:
    ControllerTargetVelSet(Ast::Section * section, const string & name, int state):
        StateController(name, state, section){
            parse(section);
        }

    ControllerTargetVelSet(const ControllerTargetVelSet & you):
    StateController(you),
    x(copy(you.x)),
    y(copy(you.y)),
    id(copy(you.id)){
    }

    Value x;
    Value y;
    Value id;

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerTargetVelSet & controller):
                controller(controller){
                }

            ControllerTargetVelSet & controller;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "x"){
                    controller.x = Compiler::compile(simple.getValue());
                } else if (simple == "y"){
                    controller.y = Compiler::compile(simple.getValue());
                } else if (simple == "id"){
                    controller.id = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment environment(stage, guy, commands);
        double vx = 0;
        double vy = 0;
        if (x != NULL){
            RuntimeValue result = x->evaluate(environment);
            if (result.isDouble()){
                vx = result.getDoubleValue();
            }
        }
        if (y != NULL){
            RuntimeValue result = y->evaluate(environment);
            if (result.isDouble()){
                vy = result.getDoubleValue();
            }
        }

        vector<Character*> targets = stage.getTargets((int) evaluateNumber(this->id, environment, -1), &guy);
        for (vector<Character*>::iterator it = targets.begin(); it != targets.end(); it++){
            Character * target = *it;
            guy.setXVelocity(vx);
            guy.setYVelocity(vy);
        }
    }

    StateController * deepCopy() const {
        return new ControllerTargetVelSet(*this);
    }
};

class ControllerTargetDrop: public StateController {
public:
    ControllerTargetDrop(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        parse(section);
    }

    ControllerTargetDrop(const ControllerTargetDrop & you):
    StateController(you),
    exclude(copy(you.exclude)),
    keep(copy(you.keep)){
    }

    Value exclude;
    Value keep;

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerTargetDrop & controller):
                controller(controller){
                }

            ControllerTargetDrop & controller;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "excludeID"){
                    controller.exclude = Compiler::compile(simple.getValue());
                } else if (simple == "keepone"){
                    controller.keep = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment environment(stage, guy, commands);
        int id = (int) evaluateNumber(this->exclude, environment, -1);
        bool keep = evaluateBool(this->keep, environment, true);

        /* FIXME: targets should be a map of int to vector<Object*> in which case 'keep'
         * would have an effect.
         */
        map<int, vector<Character*> > & targets = guy.getTargets();
        for (map<int, vector<Character*> >::iterator it = targets.begin(); it != targets.end(); /**/){
            if (it->first != id){
                if (keep){
                    vector<Character*> & objects = it->second;
                    if (objects.size() > 0){
                        /* Save a random object */
                        Character * save = objects[PaintownUtil::rnd(objects.size())];
                        objects.clear();
                        objects.push_back(save);
                    }
                } else {
                    /* Otherwise remove all targets */
                    map<int, vector<Character*> >::iterator kill = it;
                    it++;
                    targets.erase(kill);
                }
            } else {
                it++;
            }
        }
    }

    StateController * deepCopy() const {
        return new ControllerTargetDrop(*this);
    }
};

class ControllerHitVelSet: public StateController {
public:
    ControllerHitVelSet(Ast::Section * section, const string & name, int state):
        StateController(name, state, section){
            parse(section);
        }

    ControllerHitVelSet(const ControllerHitVelSet & you):
    StateController(you),
    x(copy(you.x)),
    y(copy(you.y)){
    }

    Value x;
    Value y;

    virtual ~ControllerHitVelSet(){
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerHitVelSet & controller):
                controller(controller){
                }

            ControllerHitVelSet & controller;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "x"){
                    controller.x = Compiler::compile(simple.getValue());
                } else if (simple == "y"){
                    controller.y = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        if (x != NULL){
            RuntimeValue result = x->evaluate(FullEnvironment(stage, guy));
            if (toBool(result)){
                /* FIXME: should velocity be based on the facing direction? The
                 * documentation doesn't say.
                 */ 
                Character * enemy = stage.getEnemy(&guy);
                /* If both players are facing the same direction then negate the
                 * velocity.
                 */
                int multiplier = (enemy->getFacing() == guy.getFacing()) ? -1 : 1;
                guy.setXVelocity(guy.getHitState().xVelocity * multiplier);
            }
        }

        if (y != NULL){
            RuntimeValue result = y->evaluate(FullEnvironment(stage, guy));
            if (toBool(result)){
                guy.setYVelocity(guy.getHitState().yVelocity);
            }
        }
    }

    StateController * deepCopy() const {
        return new ControllerHitVelSet(*this);
    }
};

class ControllerPosAdd: public StateController {
public:
    ControllerPosAdd(Ast::Section * section, const string & name, int state):
        StateController(name, state, section){
            parse(section);
        }

    ControllerPosAdd(const ControllerPosAdd & you):
    StateController(you),
    x(copy(you.x)),
    y(copy(you.y)){
    }

    Value x;
    Value y;

    virtual ~ControllerPosAdd(){
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerPosAdd & controller):
                controller(controller){
                }

            ControllerPosAdd & controller;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "x"){
                    controller.x = Compiler::compile(simple.getValue());
                } else if (simple == "y"){
                    controller.y = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        if (x != NULL){
            RuntimeValue result = x->evaluate(FullEnvironment(stage, guy));
            if (result.isDouble()){
                guy.moveX(result.getDoubleValue());
            }
        }
        if (y != NULL){
            RuntimeValue result = y->evaluate(FullEnvironment(stage, guy));
            if (result.isDouble()){
                guy.moveY(result.getDoubleValue());
            }
        }
    }

    StateController * deepCopy() const {
        return new ControllerPosAdd(*this);
    }
};

class ControllerPosSet: public StateController {
public:
    ControllerPosSet(Ast::Section * section, const string & name, int state):
        StateController(name, state, section){
            parse(section);
        }

    ControllerPosSet(const ControllerPosSet & you):
    StateController(you),
    x(copy(you.x)),
    y(copy(you.y)){
    }

    Value x;
    Value y;

    virtual ~ControllerPosSet(){
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerPosSet & controller):
                controller(controller){
                }

            ControllerPosSet & controller;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "x"){
                    controller.x = Compiler::compile(simple.getValue());
                } else if (simple == "y"){
                    controller.y = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        if (x != NULL){
            RuntimeValue result = x->evaluate(FullEnvironment(stage, guy));
            if (result.isDouble()){
                guy.setX(result.getDoubleValue());
            }
        }
        if (y != NULL){
            RuntimeValue result = y->evaluate(FullEnvironment(stage, guy));
            if (result.isDouble()){
                guy.setY(result.getDoubleValue());
            }
        }
    }

    StateController * deepCopy() const {
        return new ControllerPosSet(*this);
    }
};

class ControllerVelAdd: public StateController {
public:
    ControllerVelAdd(Ast::Section * section, const string & name, int state):
        StateController(name, state, section){
            parse(section);
        }

    ControllerVelAdd(const ControllerVelAdd & you):
    StateController(you),
    x(copy(you.x)),
    y(copy(you.y)){
    }

    Value x;
    Value y;

    virtual ~ControllerVelAdd(){
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerVelAdd & controller):
                controller(controller){
                }

            ControllerVelAdd & controller;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "x"){
                    controller.x = Compiler::compile(simple.getValue());
                } else if (simple == "y"){
                    controller.y = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        if (x != NULL){
            RuntimeValue result = x->evaluate(FullEnvironment(stage, guy));
            if (result.isDouble()){
                guy.setXVelocity(guy.getXVelocity() + result.getDoubleValue());
            }
        }
        if (y != NULL){
            RuntimeValue result = y->evaluate(FullEnvironment(stage, guy));
            if (result.isDouble()){
                guy.setYVelocity(guy.getYVelocity() + result.getDoubleValue());
            }
        }
    }

    StateController * deepCopy() const {
        return new ControllerVelAdd(*this);
    }
};

class ControllerTargetVelAdd: public StateController {
public:
    ControllerTargetVelAdd(Ast::Section * section, const string & name, int state):
        StateController(name, state, section){
            parse(section);
        }

    ControllerTargetVelAdd(const ControllerTargetVelAdd & you):
    StateController(you),
    x(copy(you.x)),
    y(copy(you.y)),
    id(copy(you.id)){
    }

    Value x;
    Value y;
    Value id;

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerTargetVelAdd & controller):
                controller(controller){
                }

            ControllerTargetVelAdd & controller;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "x"){
                    controller.x = Compiler::compile(simple.getValue());
                } else if (simple == "y"){
                    controller.y = Compiler::compile(simple.getValue());
                } else if (simple == "id"){
                    controller.id = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment environment(stage, guy, commands);
        double vx = 0;
        double vy = 0;
        if (x != NULL){
            RuntimeValue result = x->evaluate(environment);
            if (result.isDouble()){
                vx = result.getDoubleValue();
            }
        }
        if (y != NULL){
            RuntimeValue result = y->evaluate(environment);
            if (result.isDouble()){
                vy = result.getDoubleValue();
            }
        }

        vector<Character*> targets = stage.getTargets((int) evaluateNumber(this->id, environment, -1), &guy);
        for (vector<Character*>::iterator it = targets.begin(); it != targets.end(); it++){
            Character * target = *it;
            guy.setXVelocity(target->getXVelocity() + vx);
            guy.setYVelocity(target->getYVelocity() + vy);
        }
    }

    StateController * deepCopy() const {
        return new ControllerTargetVelAdd(*this);
    }
};

class ControllerVelMul: public StateController {
public:
    ControllerVelMul(Ast::Section * section, const string & name, int state):
        StateController(name, state, section){
            parse(section);
        }

    ControllerVelMul(const ControllerVelMul & you):
    StateController(you),
    x(copy(you.x)),
    y(copy(you.y)){
    }

    Value x;
    Value y;

    virtual ~ControllerVelMul(){
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerVelMul & controller):
                controller(controller){
                }

            ControllerVelMul & controller;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "x"){
                    controller.x = Compiler::compile(simple.getValue());
                } else if (simple == "y"){
                    controller.y = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        if (x != NULL){
            RuntimeValue result = x->evaluate(FullEnvironment(stage, guy));
            if (result.isDouble()){
                guy.setXVelocity(guy.getXVelocity() * result.getDoubleValue());
            }
        }

        if (y != NULL){
            RuntimeValue result = y->evaluate(FullEnvironment(stage, guy));
            if (result.isDouble()){
                guy.setYVelocity(guy.getYVelocity() * result.getDoubleValue());
            }
        }
    }

    StateController * deepCopy() const {
        return new ControllerVelMul(*this);
    }
};

struct HitDefinitionData{
    HitDefinitionData():
        guardFlag(),
        animationType(AttackType::Light),
        animationTypeAir(AttackType::NoAnimation),
        animationTypeFall(AttackType::NoAnimation),
        groundType(AttackType::High),
        airType(AttackType::None){
        }

    HitDefinitionData(const HitDefinitionData & you):
        attribute(you.attribute),
        hitFlag(you.hitFlag),
        guardFlag(you.guardFlag),
        animationType(you.animationType),
        animationTypeAir(you.animationTypeAir),
        animationTypeFall(you.animationTypeFall),
        priority(you.priority),
        damage(you.damage),
        pause(you.pause),
        guardPause(you.guardPause),
        spark(you.spark),
        guardSpark(you.guardSpark),
        sparkPosition(you.sparkPosition),
        hitSound(you.hitSound),
        guardHitSound(you.guardHitSound),
        groundType(you.groundType),
        airType(you.airType),
        groundSlideTime(copy(you.groundSlideTime)),
        guardSlideTime(copy(you.guardSlideTime)),
        groundHitTime(copy(you.groundHitTime)),
        guardGroundHitTime(copy(you.guardGroundHitTime)),
        airHitTime(copy(you.airHitTime)),
        guardControlTime(copy(you.guardControlTime)),
        guardDistance(copy(you.guardDistance)),
        yAcceleration(copy(you.yAcceleration)),
        groundVelocity(you.groundVelocity),
        guardVelocity(copy(you.guardVelocity)),
        airVelocity(you.airVelocity),
        airGuardVelocity(you.airGuardVelocity),
        groundCornerPushoff(copy(you.groundCornerPushoff)),
        airCornerPushoff(copy(you.airCornerPushoff)),
        downCornerPushoff(copy(you.downCornerPushoff)),
        guardCornerPushoff(copy(you.guardCornerPushoff)),
        airGuardCornerPushoff(copy(you.airGuardCornerPushoff)),
        airGuardControlTime(copy(you.airGuardControlTime)),
        airJuggle(copy(you.airJuggle)),
        minimum(you.minimum),
        maximum(you.maximum),
        snap(you.snap),
        player1SpritePriority(copy(you.player1SpritePriority)),
        player2SpritePriority(copy(you.player2SpritePriority)),
        player1Facing(copy(you.player1Facing)),
        player1GetPlayer2Facing(copy(you.player1GetPlayer2Facing)),
        player2Facing(copy(you.player2Facing)),
        player1State(copy(you.player1State)),
        player2State(copy(you.player2State)),
        player2GetPlayer1State(copy(you.player2GetPlayer1State)),
        forceStand(copy(you.forceStand)),
        fall(you.fall),
        getPower(you.getPower),
        givePower(you.givePower),
        down(you.down),
        id(copy(you.id)),
        chainId(copy(you.chainId)),
        envShake(you.envShake){
        }

    typedef PaintownUtil::ClassPointer<Compiler::Value> Value;
    /*
     * Required parameters:
     * attr = hit_attribute (string)
     * This is the attribute of the attack. It is used to determine if the attack can hit P2. It has the format:
     *
     * attr = arg1, arg2
     *
     * Where: arg1 is either "S", "C" or "A". Similar to "statetype" for the StateDef, this says whether the attack is a standing, crouching, or aerial attack.
     *
     * arg2 is a 2-character string. The first character is either "N" for "normal", "S" for "special", or "H" for "hyper" (or "super", as it is commonly known). The second character must be either "A" for "attack" (a normal hit attack), "T" for "throw", or "P" for projectile.
     */
    struct Attribute{
        /* StateType */
        string state;
        /* AttackType */
        string attackType;
        /* PhysicalAttack */
        string physics;
    } attribute;

    /* Optional parameters:
     * hitflag = hit_flags (string)
     * This determines what type of state P2 must be in for P1 to hit. hit_flags is a string containing a combination of the following characters:
     *
     * "H" for "high", "L" for "low" or "A" for air. "M" (mid) is equivalent to saying "HL". "F" is for fall, and if included will allow P1 to juggle falling opponents in the air. "D" is for "lying Down", and if included allows P1 to hit opponents lying down on the ground. "H", "L" or "A" (or "M") must be present in the hitflag string.
     *
     * Two optional characters are "+" and "-". If "+" is added, then the hit only affects people in a gethit state. This may be useful for chain-moves that should not affect opponents who were not hit by the first move in the chain attack. If "-" is added, then the hit only affects players that are NOT in a gethit state. You should use "-" for throws and other moves you do not want P1 to be able to combo into. "+" and "-" are mutually exclusive, ie. cannot be used at the same time.
     *
     * If omitted, this defaults to "MAF".
     */
    /* HitFlag */
    HitDefinition::HitFlags hitFlag;

    /* guardflag = hit_flags (string)
     * This determines how P2 may guard the attack. hit_flags is a string containing a combination of the following characters:
     *
     * "H" for "high", "L" for "low" or "A" for air. "M" (mid) is equivalent to saying "HL". If omitted, defaults to an empty string, meaning P2 cannot guard the attack.
     * affectteam = team_type (string)
     * team_type specifies which team's players can be hit by this HitDef. Use B for both teams (all players), E for enemy team (opponents), or F for friendly team (your own team). The default is E.
     */
    HitDefinition::HitFlags guardFlag;

    /* animtype = anim_type (string)
     * This refers to the type of animation that P2 will go into when hit by the attack. Choose from "light", "medium", "hard", "back", "up", or "diagup". The first three are self-explanatory. "Back" is the animation where P2 is knocked off her feet. "Up" should be used when the character is knocked straight up in the air (for instance, by an uppercut), and "DiagUp" should be used when the character is knocked up and backwards in the air, eventually landing on his head. The default is "Light".
     * animtype: Returns the animation type of the hit. (0 for light, 1 for medium, 2 for hard, 3 for back, 4 for up, 5 for diag-up)
     */
    AttackType::Animation animationType;

    /* air.animtype = anim_type (string)
     * Similar to the "animtype" parameter, this is the animtype to set P2 to if P2 is in the air, instead of on the ground. Defaults to the same value as the "animtype" parameter if omitted.
     */
    AttackType::Animation animationTypeAir;

    /* fall.animtype = anim_type (string)
     * Similar to the "animtype" parameter, this is the animtype to set P2 to if P2 is hit while falling. Defaults to Up if air.animtype is Up, or Back otherwise.
     */
    AttackType::Animation animationTypeFall;

    /* priority = hit_prior (int), hit_type (string)
     * Specifies the priority for this hit. Hits with higher priorities take precedence over hits with lower priorities. Valid values for hit_prior are 1-7. Defaults to 4.
     *
     * hit_type, if specified, gives the priority class of the hit. Valid priority classes are Dodge, Hit, and Miss. The priority class determines the tiebreaking behavior when P1 and P2 hit each other simultaneously with equal priorities. The behavior for P1 vs. P2 is as follows:
     *
     * Hit vs. Hit: both P1 and P2 are hit
     * Hit vs. Miss: P1 hits, P2 misses
     * Hit vs. Dodge: Both miss
     * Dodge vs. Dodge: Both miss
     * Dodge vs. Miss: Both miss
     * Miss vs. Miss: Both miss
     * In the case of a no-hit tie, the respective HitDefs stay enabled. "Miss" or "Dodge" are typically used for throws, where P1 and P2 should not be able to simultaneously hit each other. The default for hit_type is "Hit".
     */
    struct Priority{
        Priority():
            type("Hit"){
            }

        Priority(const Priority & you):
            hit(copy(you.hit)),
            type(you.type){
            }

        Value hit;
        string type;
    } priority;

    /* damage = hit_damage, guard_damage (int, int)
     * hit_damage is the damage that P2 takes when hit by P2. The optional guard_damage parameter is the damage taken by P2 if the hit is guarded. Both default to zero if omitted.
     */
    struct Damage{
        Damage(){
        }

        Damage(const Damage & you):
            damage(copy(you.damage)),
            guardDamage(copy(you.guardDamage)){
            }

        Value damage;
        Value guardDamage;
    } damage;

    /* pausetime = p1_pausetime, p2_shaketime (int, int)
     * This is the time that each player will pause on the hit. p1_pausetime is the time to freeze P1, measured in game-ticks. p2_pausetime is the time to make P2 shake before recoiling from the hit. Defaults to 0,0 if omitted.
     */
    struct PauseTime{
        PauseTime(){
        }

        PauseTime(const PauseTime & you):
            player1(copy(you.player1)),
            player2(copy(you.player2)){
            }

        Value player1;
        Value player2;
    } pause, guardPause;

    /* guard.pausetime = p1_pausetime, p2_shaketime (int, int)
     * Similar to the "pausetime" parameter, these are the times to pause each player if the hit was guarded. Defaults to the same values as the "pausetime" parameter if omitted.
     */

    struct Resource{
        Resource():
            own(false){
            }

        Resource(const Resource & you):
            own(you.own),
            group(copy(you.group)),
            item(copy(you.item)){
            }

        bool own;
        Value group;
        Value item;
    };

    /* sparkno = action_no (int)
     * This is the action number of the spark to display if the hit is successful. To play a spark out of the player's .AIR file, precede the action number with an S, e.g. "sparkno = S10". Defaults to the value set in the player variables if omitted.
     */
    Resource spark;

    /* guard.sparkno = action_no (int)
     * This is the action number of the spark to display if the hit was guarded. To play a spark out of the player's .AIR file, precede the action number with an S. Defaults to the value set in the player variables if omitted.
     */
    Resource guardSpark;

    /* sparkxy = spark_x, spark_y (int, int)
     * This is where to make the hit/guard spark. spark_x is a coordinate relative to the front of P2. A negative value makes the spark deeper inside P2. "Front" refers to the x- position at P2's axis offset towards P1 by the corresponding width value in the [Size] group in P2's player variables. spark_y is relative to P1. A negative value makes a spark higher up. You can use a tool like AirView to determine this value by positioning the cursor at the "attack spot" and reading off the value of the y-position. Defaults to 0,0 if omitted.
     */
    struct SparkPosition{
        SparkPosition(){
        }

        SparkPosition(const SparkPosition & you):
            x(copy(you.x)),
            y(copy(you.y)){
            }

        Value x, y;
    } sparkPosition;

    /* hitsound = snd_grp, snd_item (int, int)
     * This is the sound to play on hit (from common.snd). The included fight.snd lets you choose from 5,0 (light hit sound) through to 5,4 (painful whack). To play a sound from the player's own SND file, precede the first number with an "S". For example, "hitsound = S1,0". Defaults to the value set in the player variables if omitted.
     */
    Resource hitSound;

    /* guardsound = snd_grp, snd_item (int, int)
     * This is the sound to play on guard (from common.snd). Only 6,0 is available at this time. To play a sound from the player's own SND file, precede the first number with an "S". There is no facility to play a sound from the opponent's SND file. Defaults to the value set in the player variables if omitted.
     */
    Resource guardHitSound;

    /* ground.type = attack_type (string)
     * This is the kind of attack if P2 is on the ground. Choose from: - "High": for attacks that make P2's head snap backwards. - "Low": for attacks that hit P2 in the stomach. - "Trip": for low sweep attacks. If you use "Trip" type, the ground.velocity parameter should have a non-zero y-velocity, and the fall parameter should be set to 1. A tripped opponent does not bounce upon falling on the ground. - "None": for attacks that do nothing besides pause P1 and P2 for the duration in the pausetime parameter.
     * If P2 is hit from behind, "High" will be displayed as "Low" and vice-versa. P2's animation for "High" and "Low" types will be superseded if the AnimType parameter is "Back". Defaults to "High" if omitted.
     */
    AttackType::Ground groundType;

    /* air.type = attack_type (string)
     * This is the kind of attack if P2 is in the air. Defaults to the same value as "ground.type" if omitted.
     */
    AttackType::Ground airType;

    /* ground.slidetime = slide_time (int)
     * This is the time in game-ticks that P2 will slide back for after being hit (this time does not include the pausetime for P2). Applicable only to hits that keep P2 on the ground. Defaults to 0 if omitted.
     */
    Value groundSlideTime;

    /* guard.slidetime = slide_time (int)
     * Same as "ground.slidetime", but this is the value if P2 guards the hit. Defaults to same value as "guard.hittime".
     */
    Value guardSlideTime;

    /* ground.hittime = hit_time (int)
     * Time that P2 stays in the hit state after being hit. Adjust this value carefully, to make combos possible. Applicable only to hits that keep P2 on the ground. Defaults to 0 if omitted.
     */
    Value groundHitTime;

    /* guard.hittime = hit_time (int)
     * Same as "ground.hittime", but this is the value if P2 guards the hit. Defaults to same value as "ground.hittime".
     */
    Value guardGroundHitTime;

    /* air.hittime = hit_time (int)
     * Time that p2 stays in the hit state after being hit in or into the air, before being able to guard again. This parameter has no effect if the "fall" parameter is set to 1. Defaults to 20 if omitted.
     */
    Value airHitTime;

    /* guard.ctrltime = ctrl_time (int)
     * This is the time before p2 regains control in the ground guard state. Defaults to the same value as "guard.slidetime" if omitted.
     */
    Value guardControlTime;

    /* guard.dist = x_dist (int)
     * This is the x-distance from P1 in which P2 will go into a guard state if P2 is holding the direction away from P1. Defaults to the value in the player variables if omitted. You normally do not need to use this parameter.
     */
    Value guardDistance;

    /* yaccel = accel (float)
     * Specifies the y acceleration to impart to P2 if the hit connects. Defaults to .35 in 240p, .7 in 480p, 1.4 in 720p.
     */
    Value yAcceleration;

    /* ground.velocity = x_velocity, y_velocity (float, float)
     * Initial velocity to give P2 after being hit, if P2 is on the ground. If y_velocity is not zero, P2 will be knocked into the air. Both values default to 0 if omitted. You can leave out the y_velocity if you want P2 to remain on the ground.
     */
    struct GroundVelocity{
        GroundVelocity(){
        }

        GroundVelocity(const GroundVelocity & you):
            x(copy(you.x)),
            y(copy(you.y)){
            }

        Value x, y;
    } groundVelocity;

    /* guard.velocity = x_velocity (float)
     * Velocity to give P2 if P2 guards the hit on the ground. Defaults to the x_velocity value of the "ground.velocity" parameter if omitted.
     */
    Value guardVelocity;

    /* air.velocity = x_velocity, y_velocity (float, float)
     * Initial velocity to give P2 if P2 is hit in the air. Defaults to 0,0 if omitted.
     */
    struct AirVelocity{
        AirVelocity(){
        }

        AirVelocity(const AirVelocity & you):
            x(copy(you.x)),
            y(copy(you.y)){
            }

        Value x, y;
    } airVelocity;

    /* airguard.velocity = x_velocity, y_velocity (float float)
     * Velocity to give P2 if P2 guards the hit in the air. Defaults to x_velocity * 1.5, y_velocity / 2, where x_velocity and y_velocity are values of the "air.velocity" parameter.
     */
    struct AirGuardVelocity{
        AirGuardVelocity(){
        }

        AirGuardVelocity(const AirGuardVelocity & you):
            x(copy(you.x)),
            y(copy(you.y)){
            }

        Value x, y;
    } airGuardVelocity;

    /* ground.cornerpush.veloff = x_velocity (float)
     * Determines the additional velocity (velocity offset) to impart to the player if he lands a ground hit in the corner. Setting this to a higher value will cause the player to be "pushed back" farther out of the corner. If omitted, default value depends on the attr parameter. If arg1 of attr is "A", default value is 0. Otherwise, defaults to 1.3 * guard.velocity.
     */
    Value groundCornerPushoff;

    /* air.cornerpush.veloff = x_velocity (float)
     * Determines the additional velocity (velocity offset) to impart to the player if he lands a hit to an aerial opponent in the corner. Setting this to a higher value will cause the player to be "pushed back" farther out of the corner. Defaults to ground.cornerpush.veloff if omitted.
     */
    Value airCornerPushoff;

    /* down.cornerpush.veloff = x_velocity (float)
     * Determines the additional velocity (velocity offset) to impart to the player if he lands a hit on a downed opponent in the corner. Setting this to a higher value will cause the player to be "pushed back" farther out of the corner. Defaults to ground.cornerpush.veloff if omitted.
     */
    Value downCornerPushoff;

    /* guard.cornerpush.veloff = x_velocity (float)
     * Determines the additional velocity (velocity offset) to impart to the player if his hit is guarded in the corner. Setting this to a higher value will cause the player to be "pushed back" farther out of the corner. Defaults to ground.cornerpush.veloff if omitted.
     */
    Value guardCornerPushoff;

    /* airguard.cornerpush.veloff = x_velocity (float)
     * Determines the additional velocity (velocity offset) to impart to the player if his hit is guarded in the corner. Setting this to a higher value will cause the player to be "pushed back" farther out of the corner. Defaults to guard.cornerpush.veloff if omitted.
     */
    Value airGuardCornerPushoff;

    /* airguard.ctrltime = ctrl_time (int)
     * This is the time before p2 regains control in the air guard state. Defaults to the same value as "guard.ctrltime" if omitted.
     */
    Value airGuardControlTime;

    /* air.juggle = juggle_points (int)
     * The amount of additional juggle points the hit requires. Not to be confused with the "juggle" parameter in the StateDef. You typically do not need this parameter, except for HitDefs of projectiles. Defaults to 0 if omitted.
     */
    Value airJuggle;

    /* mindist = x_pos, y_pos (int, int)
     * See below.
     * maxdist = x_pos, y_pos (int, int)
     * These let you control the minimum and maximum distance of P2 relative to P1, after P2 has been hit. These parameters are not commonly used. Defaults to no change in P2's position if omitted.
     */
    struct Distance{
        Distance(){
        }

        Distance(const Distance & you):
            x(copy(you.x)),
            y(copy(you.y)){
            }

        Value x, y;
    };

    Distance minimum, maximum;

    /* snap = x_pos, y_pos (int, int)
     * This moves P2 to the specified position relative to P1 if hit. This parameter is not normally used. If you want to snap P2 to a particular position for a throw, it is recommended you use a "TargetBind" controller in P1's throwing state instead. Defaults to no change in P2's position if omitted.
     */
    Distance snap;

    /* p1sprpriority = drawing_priority (int)
     * This is the drawing priority of P1's sprite if the move hits or is guarded by P2. Together with the p2sprpriority parameter, it controls whether or not P1 is drawn in front of or behind P2. The default value is 1.
     */
    Value player1SpritePriority;

    /* p2sprpriority = drawing_priority (int)
     * This is the drawing priority of P2's sprite if the move hits or is guarded by P2. The default value is 0.
     */
    Value player2SpritePriority;

    /* p1facing = facing (int)
     * Set to -1 to make P1 turn around if the hit is successful. The default value is no change in where P1 is facing.
     */
    Value player1Facing;

    /* p1getp2facing = facing (int)
     * Set to 1 to have P1 face in the same direction as P2 is facing after the hit connects, and -1 to have P1 face the opposite direction from P2. Defaults to 0 (no change). If nonzero, this parameter takes precedence over p1facing.
     */
    Value player1GetPlayer2Facing;

    /* p2facing = facing (int)
     * Set to 1 to make P2 face the same direction as P1 if the hit is successful, -1 to make P2 face away. The default value is 0, no change in where P2 is facing.
     */
    Value player2Facing;

    /* p1stateno = state_no (int)
     * This is the number of the state to set P1 to if the hit is successful. The state must be an attack state (movetype = A) for at least 1 tick. Used mainly for throws. Defaults to -1, no change.
     */
    Value player1State;

    /* p2stateno = state_no (int)
     * This is the number of the state to set P2 to if the hit is successful. P2 will get P1's state and animation data. P2 will retain P1's states and animation data until P2 is hit, or a SelfState controller is used to return P2 to his own states. The state must be a get-hit state (movetype = H) for at least 1 tick. Used mainly for throws; can also be used for custom hit reactions. Defaults to -1, no change.
     */
    Value player2State;

    /* p2getp1state = bvalue (boolean)
     * Set to 0 to prevent P2 from getting P1's state and animation data, in case you do not want that default behaviour of the "p2stateno" parameter. Defaults to 1 if the "p2stateno" parameter is used. Ignored otherwise.
     */
    Value player2GetPlayer1State;

    /* forcestand = bvalue (boolean)
     * Set to 1 to force P2 to a standing state-type if the hit is successful, and P2 is in a crouching state. Has no effect if P2 is in an air state. Normally defaults to 0, but if the y_velocity of the "ground.velocity" parameter is non-zero, it defaults to 1.
     */
    Value forceStand;

    struct Fall{
        Fall(){
        }

        Fall(const Fall & you):
            fall(copy(you.fall)),
            xVelocity(copy(you.xVelocity)),
            yVelocity(copy(you.yVelocity)),
            recover(copy(you.recover)),
            recoverTime(copy(you.recoverTime)),
            damage(copy(you.damage)),
            airFall(copy(you.airFall)),
            forceNoFall(copy(you.forceNoFall)),
            envShake(you.envShake){
            }
        /* fall = bvalue (boolean)
         * Set to 1 if you want P2 to go into a "fall" state (where P2 hits the ground without regaining control in the air). Use if you want a move to "knock down" P2. Defaults to 0.
         */
        Value fall;

        /* fall.xvelocity = x_velocity (float)
         * This is the x-velocity that P2 gets when bouncing off the ground in the "fall" state. Defaults to no change if omitted.
         */
        Value xVelocity;

        /* fall.yvelocity = y_velocity (float)
         * This is the y-velocity that P2 gets when bouncing off the ground in the "fall" state. Defaults to -4.5 in 240p, -9 in 480p, -18 in 720p.
         */
        Value yVelocity;

        /* fall.recover = bvalue (boolean)
         * Set to 0 if you do not want P2 to be able to recover from the "fall" state. Defaults to 1 if omitted (can recover).
         */
        Value recover;

        /* fall.recovertime = recover_time (int)
         * This is the time that must pass before P2 is able to recover from the "fall" state by inputting his recovery command. Does not include the time that P2 is paused for while shaking from the hit. Defaults to 4 if omitted.
         */
        Value recoverTime;

        /* fall.damage = damage_amt (int)
         * Indicates the amount of damage to deal when P2 hits the ground out of a falling state. Defaults to 0 if omitted.
         */
        Value damage;

        /* air.fall = bvalue (boolean)
         * Set to 1 if you want P2 to go into a "fall" state (where P2 hits the ground without regaining control in the air) if hit while P2 is in the air. Defaults to the same value as fall.
         */
        Value airFall;

        /* forcenofall = bvalue (boolean)
         * Set to 1 to force P2 out of a "fall" state, if he is in one. This parameter has no effect on P2 if he is not in a "fall" state. This parameter is ignored if the "fall" parameter is set to 1. Defaults to 0 if omitted.
         */
        Value forceNoFall;

        struct Shake{
            Shake(){
            }

            Shake(const Shake & you):
                time(copy(you.time)),
                frequency(copy(you.frequency)),
                amplitude(copy(you.amplitude)),
                phase(copy(you.phase)){
                }

            Value time;
            Value frequency;
            Value amplitude;
            Value phase;
        } envShake;
    } fall;

    /* getpower = p1power, p1gpower (int, int)
     * p1power specifies the amount of power to give P1 if this HitDef connects successfully. p1gpower specifies the amount of power to give P1 if this HitDef is guarded. If omitted, p1power defaults to hit_damage (from "damage" parameter) multiplied by the value of Default.Attack.LifeToPowerMul specified in data/mugen.cfg. If p1gpower is omitted, it defaults to the value specified for p1power divided by 2.
     */
    struct GetPower{
        GetPower(){
        }

        GetPower(const GetPower & you):
            hit(copy(you.hit)),
            guarded(copy(you.guarded)){
            }

        Value hit;
        Value guarded;
    } getPower;

    struct GivePower{
        GivePower(){
        }

        GivePower(const GivePower & you):
            hit(copy(you.hit)),
            guarded(copy(you.guarded)){
            }

        Value hit;
        Value guarded;
    } givePower;

    struct Down{
        Down(){
        }

        Down(const Down & you):
            x(copy(you.x)),
            y(copy(you.y)),
            time(copy(you.time)),
            bounce(you.bounce){
            }

        Value x;
        Value y;
        Value time;
        bool bounce;
    } down;

    Value id;
    Value chainId;

    /* down.velocity = x_velocity, y_velocity (float, float)
     * This is the velocity to assign P2 if P2 is hit while lying down. If the y_velocity is non-zero, P2 will be hit into the air. If it is zero, then P2 will slide back on the ground. Defaults to the same values as the "air.velocity" parameter if omitted.
     * down.hittime = hit_time (int)
     * This is the time that P2 will slide back for if P2 is hit while lying down. This parameter is ignored if the y_velocity is non- zero for the "down.velocity" parameter.
     * down.bounce = bvalue (boolean)
     * Set to 1 if you want P2 to bounce off the ground one time (using the fall.xvelocity and fall.yvelocity values) after hitting the ground from the hit. This parameter is ignored if the y_velocity is zero for the "down.velocity" parameter. Defaults to 0 if omitted (P2 hits the ground and stays there).
     * id = id_number (int)
     * Idetifier for the hit. Used for chain moves. You can use this number to later detect if a player was last hit by this particular HitDef. This number is called the targetID. It is used in controllers such as TargetBind, or in the target(ID) redirection keyword. Valid values are all values >= 1. If omitted, defaults to 0 (no ID). TargetID is not to be confused with PlayerID.
     * chainID = id_number (int)
     * Main use of this is for chain moves. If P2 was last hit by a move by P1 with this ID, only then can he be hit by the HitDef with this chainID. You can use this in the following parts of a chain move. Note that chain moves are still possible even without the use of the "id" and "chainid" parameters. Valid values are all values >= 1. If omitted, defaults to -1 (chain from any hit).
     * nochainID = nochain_1, nochain_2 (int)
     * nochainID specifies up to 2 ID numbers of hits which cannot be chained into this hit. If these are -1 (the default), then chaining is not explicitly disabled for any hit ID numbers. nochain_2 can be omitted. Except for -1, the values specified must not coincide with the value for chainID. This parameter has no effect if P2 is hit by a third party between P1's previous HitDef and the current HitDef.
     * hitonce = hitonce_flag (boolean)
     * If set to 1, the HitDef only affects one opponent. If the hit is successful, all other targets will be dropped. Normally defaults to 0. The exception is if the "attr" parameter is a throw type, which makes it default to 1.
     * kill = bvalue (boolean)
     * Set to 0 if this hit should not be able to KO the opponent when the hit is successful. Defaults to 1.
     * guard.kill = bvalue (boolean)
     * Set to 0 if this hit should not be able to KO the opponent when the hit is guarded. Defaults to 1.
     * fall.kill = bvalue (boolean)
     * Set to 0 to prevent this attack from KO'ing the opponent when he falls on the ground (see fall.damage). Defaults to 1.
     * numhits = hit_count (int)
     * hit_count indicates how many hits this hitdef should add to the combo counter. Must be 0 or greater. Defaults to 1.
     * givepower = p2power, p2gpower (int, int)
     * p2power specifies the amount of power to give P2 if this HitDef connects successfully. p2gpower specifies the amount of power to give P2 if this HitDef is guarded. If omitted, p1power defaults to hit_damage (from "damage" parameter) multiplied by the value of Default.GetHit.LifeToPowerMul specified in data/mugen.cfg. If p1gpower is omitted, it defaults to the value specified for p1power divided by 2.
     * palfx.time = palfx_time (int)
     * See below.
     * palfx.mul = r1, g1, b1 (int, int, int)
     * See below.
     * palfx.add = r2, g2, b2 (int, int, int)
     * If included, this allows for palette effects on P2 if the hit is successful. palfx_time is the time in game-ticks to apply palette effects on P2. palfx_time is 0 by default (no effect). The rest of the parameters are the same as in the PalFX controller.
     */

    struct EnvShake{
        EnvShake(){
        }

        EnvShake(const EnvShake & you):
            time(copy(you.time)),
            frequency(copy(you.frequency)),
            amplitude(copy(you.amplitude)),
            phase(copy(you.phase)){
            }

        Value time;
        Value frequency;
        Value amplitude;
        Value phase;
    } envShake;

    /*
     * envshake.time = envshake_time (int)
     * See below.
     * envshake.freq = envshake_freq (float)
     * See below.
     * envshake.ampl = envshake_ampl (int)
     * See below.
     * envshake.phase = envshake_phase (float)
     * If included, this shakes the screen if the hit is successful. envshake_time is the time in game-ticks to shake the screen. The rest of the parameters are the same as in the EnvShake controller.
     * fall.envshake.time = envshake_time (int)
     * See below.
     * fall.envshake.freq = envshake_freq (float)
     * See below.
     * fall.envshake.ampl = envshake_ampl (int)
     * See below.
     * fall.envshake.phase = envshake_phase (float)
     * Similar to the envshake.* parameters, except the effects are applied only when P2 hits the ground.
     * Notes:
     */
};

static void parseHitDefinition(Ast::Section * section, HitDefinitionData & hit){
    class Walker: public Ast::Walker {
    public:
        Walker(HitDefinitionData & hit):
            hit(hit){
            }

        HitDefinitionData & hit;

        string toString(char x){
            ostringstream out;
            out << x;
            return out.str();
        }

        AttackType::Animation parseAnimationType(string type, const Ast::AttributeSimple & simple){
            type = Util::fixCase(type);
            if (type == "light"){
                return AttackType::Light;
            } else if (type == "medium" || type == "med"){
                return AttackType::Medium;
            } else if (type == "hard" || type == "heavy"){
                return AttackType::Hard;
            } else if (type == "back"){
                return AttackType::Back;
            } else if (type == "up"){
                return AttackType::Up;
            } else if (type == "diagup"){
                return AttackType::DiagonalUp;
            } else {
                Global::debug(0) << "Unknown hitdef animation type '" << type << "' in file " << stateFileParameter.current().path() << " at line " << simple.getLine() << " column " << simple.getColumn() << endl;
            }
            return AttackType::NoAnimation;
        }

        bool isType(const Ast::AttributeSimple & simple){
            return PaintownUtil::lowerCaseAll(simple.idString()).find("type") == 0;
        }

        bool shouldIgnore(const Ast::AttributeSimple & simple){
            return isType(simple) || StateController::handled(simple);
        }

        HitDefinition::HitFlags parseHitFlags(const string & flags){
            HitDefinition::HitFlags hitFlags;

            /* First disable all the flags, the default is MAF */
            hitFlags.reset();

            for (unsigned int index = 0; index < flags.size(); index++){
                switch (flags[index]){
                    case 'L': hitFlags.low = true; break;
                    case 'M': hitFlags.low = true; hitFlags.high = true; break;
                    case 'H': hitFlags.high = true; break;
                    case 'A': hitFlags.air = true; break;
                    case 'F': hitFlags.fall = true; break;
                    case 'D': hitFlags.down = true; break;
                    case '+': hitFlags.getHitState = true; break;
                    case '-': hitFlags.notGetHitState = true; break;
                }
            }

            return hitFlags;
        }

        virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
            if (simple == "attr"){
                string type, attack;
                simple.view() >> type >> attack;
                type = PaintownUtil::upperCaseAll(type);
                if (type == StateType::Stand ||
                    type == StateType::Crouch ||
                    type == StateType::Air){
                    hit.attribute.state = type;
                }

                if (attack.size() >= 2){
                    string type = toString(attack[0]);
                    string physics = toString(attack[1]);
                    if (type == AttackType::Normal ||
                        type == AttackType::Special ||
                        type == AttackType::Hyper){
                        hit.attribute.attackType = type;
                    }
                    if (physics == PhysicalAttack::Normal ||
                        physics == PhysicalAttack::Throw ||
                        physics == PhysicalAttack::Projectile){
                        hit.attribute.physics = physics;
                    }
                }
            } else if (simple == "hitflag"){
                string flags;
                simple.view() >> flags;
                hit.hitFlag = parseHitFlags(flags);
            } else if (simple == "guardflag"){
                try{
                    string flags;
                    simple.view() >> flags;
                    hit.guardFlag = parseHitFlags(flags);
                } catch (const Ast::Exception & fail){
                }
            } else if (simple == "animtype"){
                string anim;
                simple.view() >> anim;
                hit.animationType = parseAnimationType(anim, simple);
            } else if (simple == "air.animtype"){
                string anim;
                simple.view() >> anim;
                hit.animationTypeAir = parseAnimationType(anim, simple);
            } else if (simple == "debug"){
                // setDebug(true);
            } else if (simple == "fall.animtype"){
                string anim;
                simple.view() >> anim;
                hit.animationTypeFall = parseAnimationType(anim, simple);
            } else if (simple == "priority"){
                const Ast::Value * hit;
                Ast::View view = simple.view();
                view >> hit;
                this->hit.priority.hit = Compiler::compile(hit);
                try{
                    string type;
                    view >> type;
                    this->hit.priority.type = type;
                } catch (const Ast::Exception & e){
                }
            } else if (simple == "damage"){
                readValues(simple.getValue(), hit.damage.damage, hit.damage.guardDamage);
            } else if (simple == "pausetime"){
                try{
                    const Ast::Value * player1;
                    const Ast::Value * player2;
                    simple.view() >> player1 >> player2;
                    hit.pause.player1 = Compiler::compile(player1);
                    hit.pause.player2 = Compiler::compile(player2);
                } catch (const Ast::Exception & e){
                }
            } else if (simple == "guard.pausetime"){
                try{
                    const Ast::Value * player1;
                    const Ast::Value * player2;
                    simple.view() >> player1 >> player2;
                    hit.guardPause.player1 = Compiler::compile(player1);
                    hit.guardPause.player2 = Compiler::compile(player2);
                } catch (const Ast::Exception & e){
                }
            } else if (simple == "sparkno"){
                if (simple.getValue()->hasMultiple()){
                    const Ast::Value * item;
                    const Ast::Value * group;
                    simple.view() >> group >> item;
                    Resource resource = extractResource(group);
                    hit.spark.own = resource.own;
                    hit.spark.item = Compiler::compile(item);
                    hit.spark.group = Compiler::compile(resource.value);
                } else {
                    const Ast::Value * group = simple.getValue();
                    Resource resource = extractResource(group);
                    hit.spark.own = resource.own;
                    hit.spark.item = Compiler::compile(0);
                    hit.spark.group = Compiler::compile(resource.value);
                }
            } else if (simple == "guard.sparkno"){
                if (simple.getValue()->hasMultiple()){
                    const Ast::Value * item;
                    const Ast::Value * group;
                    simple.view() >> group >> item;
                    Resource resource = extractResource(group);
                    hit.guardSpark.own = resource.own;
                    hit.guardSpark.item = Compiler::compile(item);
                    hit.guardSpark.group = Compiler::compile(resource.value);
                } else {
                    const Ast::Value * group = simple.getValue();
                    Resource resource = extractResource(group);
                    hit.guardSpark.own = resource.own;
                    hit.guardSpark.item = Compiler::compile(0);
                    hit.guardSpark.group = Compiler::compile(resource.value);
                }
            } else if (simple == "getpower"){
                const Ast::Value * hit;
                const Ast::Value * guarded;
                simple.view() >> hit >> guarded;
                /* One or both might not be given */
                try{
                    this->hit.getPower.hit = Compiler::compile(hit);
                    this->hit.getPower.guarded = Compiler::compile(guarded);
                } catch (const Ast::Exception & ignore){
                }
            } else if (simple == "givepower"){
                const Ast::Value * hit;
                const Ast::Value * guarded;
                simple.view() >> hit >> guarded;
                /* One or both might not be given */
                try{
                    this->hit.givePower.hit = Compiler::compile(hit);
                    this->hit.givePower.guarded = Compiler::compile(guarded);
                } catch (const Ast::Exception & ignore){
                }
            } else if (simple == "sparkxy"){
                try{
                    const Ast::Value * x;
                    const Ast::Value * y;
                    simple.view() >> x >> y;
                    hit.sparkPosition.x = Compiler::compile(x);
                    hit.sparkPosition.y = Compiler::compile(y);
                } catch (const Ast::Exception & e){
                }
            } else if (simple == "hitsound"){
                if (simple.getValue()->hasMultiple()){
                    const Ast::Value * item;
                    const Ast::Value * group;
                    simple.view() >> group >> item;
                    Resource resource = extractResource(group);
                    hit.hitSound.own = resource.own;
                    hit.hitSound.item = Compiler::compile(item);
                    hit.hitSound.group = Compiler::compile(resource.value);
                } else {
                    const Ast::Value * group = simple.getValue();
                    Resource resource = extractResource(group);
                    hit.hitSound.own = resource.own;
                    hit.hitSound.item = Compiler::compile(0);
                    hit.hitSound.group = Compiler::compile(resource.value);
                }
            } else if (simple == "guardsound"){
                if (simple.getValue()->hasMultiple()){
                    const Ast::Value * item;
                    const Ast::Value * group;
                    simple.view() >> group >> item;
                    Resource resource = extractResource(group);
                    hit.guardHitSound.own = resource.own;
                    hit.guardHitSound.item = Compiler::compile(item);
                    hit.guardHitSound.group = Compiler::compile(resource.value);
                } else {
                    const Ast::Value * group = simple.getValue();
                    Resource resource = extractResource(group);
                    hit.guardHitSound.own = resource.own;
                    hit.guardHitSound.item = Compiler::compile(0);
                    hit.guardHitSound.group = Compiler::compile(resource.value);
                }
            } else if (simple == "ground.type"){
                string type;
                simple.view() >> type;
                type = Util::fixCase(type);
                if (type == "low"){
                    hit.groundType = AttackType::Low;
                } else if (type == "high"){
                    hit.groundType = AttackType::High;
                } else if (type == "trip"){
                    hit.groundType = AttackType::Trip;
                }
            } else if (simple == "air.type"){
                string type;
                simple.view() >> type;
                type = Util::fixCase(type);
                if (type == "low"){
                    hit.airType = AttackType::Low;
                } else if (type == "high"){
                    hit.airType = AttackType::High;
                } else if (type == "trip"){
                    hit.airType = AttackType::Trip;
                }
            } else if (simple == "ground.slidetime"){
                hit.groundSlideTime = Compiler::compile(simple.getValue());
            } else if (simple == "guard.slidetime"){
                hit.guardSlideTime = Compiler::compile(simple.getValue());
            } else if (simple == "ground.hittime"){
                hit.groundHitTime = Compiler::compile(simple.getValue());
            } else if (simple == "guard.hittime"){
                hit.guardGroundHitTime = Compiler::compile(simple.getValue());
            } else if (simple == "air.hittime"){
                hit.airHitTime = Compiler::compile(simple.getValue());
            } else if (simple == "guard.ctrltime"){
                hit.guardControlTime = Compiler::compile(simple.getValue());
            } else if (simple == "guard.dist"){
                hit.guardDistance = Compiler::compile(simple.getValue());
            } else if (simple == "yaccel"){
                hit.yAcceleration = Compiler::compile(simple.getValue());
            } else if (simple == "ground.velocity"){
                if (simple.getValue()->hasMultiple()){
                    try{
                        const Ast::Value * x;
                        const Ast::Value * y;
                        simple.view() >> x >> y;
                        hit.groundVelocity.x = Compiler::compile(x);
                        hit.groundVelocity.y = Compiler::compile(y);
                    } catch (const Ast::Exception & e){
                    }
                } else {
                    hit.groundVelocity.x = Compiler::compile(simple.getValue());
                }
            } else if (simple == "guard.velocity"){
                hit.guardVelocity = Compiler::compile(simple.getValue());
            } else if (simple == "air.velocity"){
                if (simple.getValue()->hasMultiple()){
                    try{
                        const Ast::Value * x;
                        const Ast::Value * y;
                        simple.view() >> x >> y;
                        hit.airVelocity.x = Compiler::compile(x);
                        hit.airVelocity.y = Compiler::compile(y);
                    } catch (const Ast::Exception & e){
                    }
                } else {
                    hit.airVelocity.x = Compiler::compile(simple.getValue());
                }
            } else if (simple == "airguard.velocity"){
                try{
                    const Ast::Value * x;
                    const Ast::Value * y;
                    simple.view() >> x >> y;
                    hit.airGuardVelocity.x = Compiler::compile(x);
                    hit.airGuardVelocity.y = Compiler::compile(y);
                } catch (const Ast::Exception & e){
                }
            } else if (simple == "ground.cornerpush.veloff"){
                hit.groundCornerPushoff = Compiler::compile(simple.getValue());
            } else if (simple == "air.cornerpush.veloff"){
                hit.airCornerPushoff = Compiler::compile(simple.getValue());
            } else if (simple == "down.cornerpush.veloff"){
                hit.downCornerPushoff = Compiler::compile(simple.getValue());
            } else if (simple == "guard.cornerpush.veloff"){
                hit.guardCornerPushoff = Compiler::compile(simple.getValue());
            } else if (simple == "airguard.cornerpush.veloff"){
                hit.airGuardCornerPushoff = Compiler::compile(simple.getValue());
            } else if (simple == "airguard.ctrltime"){
                hit.airGuardControlTime = Compiler::compile(simple.getValue());
            } else if (simple == "air.juggle"){
                hit.airJuggle = Compiler::compile(simple.getValue());
            } else if (simple == "mindist"){
                const Ast::Value * x;
                const Ast::Value * y;
                simple.view() >> x >> y;
                hit.minimum.x = Compiler::compile(x);
                hit.minimum.y = Compiler::compile(y);
            } else if (simple == "maxdist"){
                const Ast::Value * x;
                const Ast::Value * y;
                simple.view() >> x >> y;
                hit.maximum.x = Compiler::compile(x);
                hit.maximum.y = Compiler::compile(y);
            } else if (simple == "snap"){
                const Ast::Value * x;
                const Ast::Value * y;
                simple.view() >> x >> y;
                hit.snap.x = Compiler::compile(x);
                hit.snap.y = Compiler::compile(y);
            } else if (simple == "id"){
                const Ast::Value * id;
                simple.view() >> id;
                hit.id = Compiler::compile(id);
            } else if (simple == "chainid"){
                const Ast::Value * id;
                simple.view() >> id;
                hit.chainId = Compiler::compile(id);
            } else if (simple == "p1sprpriority"){
                hit.player1SpritePriority = Compiler::compile(simple.getValue());
            } else if (simple == "p2sprpriority"){
                hit.player2SpritePriority = Compiler::compile(simple.getValue());
            } else if (simple == "p1facing"){
                hit.player1Facing = Compiler::compile(simple.getValue());
            } else if (simple == "p2facing"){
                hit.player2Facing = Compiler::compile(simple.getValue());
            } else if (simple == "p1getp2facing"){
                hit.player1GetPlayer2Facing = Compiler::compile(simple.getValue());
            } else if (simple == "player2Facing"){
                hit.player2Facing = Compiler::compile(simple.getValue());
            } else if (simple == "p1stateno"){
                hit.player1State = Compiler::compile(simple.getValue());
            } else if (simple == "p2stateno"){
                hit.player2State = Compiler::compile(simple.getValue());
            } else if (simple == "p2getp1state"){
                hit.player2GetPlayer1State = Compiler::compile(simple.getValue());
            } else if (simple == "forcestand"){
                hit.forceStand = Compiler::compile(simple.getValue());
            } else if (simple == "fall"){
                hit.fall.fall = Compiler::compile(simple.getValue());
            } else if (simple == "fall.xvelocity"){
                hit.fall.xVelocity = Compiler::compile(simple.getValue());
            } else if (simple == "fall.yvelocity"){
                hit.fall.yVelocity = Compiler::compile(simple.getValue());
            } else if (simple == "fall.recover"){
                hit.fall.recover = Compiler::compile(simple.getValue());
            } else if (simple == "fall.recovertime"){
                hit.fall.recoverTime = Compiler::compile(simple.getValue());
            } else if (simple == "fall.damage"){
                hit.fall.damage = Compiler::compile(simple.getValue());
            } else if (simple == "fall.envshake.time"){
                hit.fall.envShake.time = Compiler::compile(simple.getValue());
            } else if (simple == "fall.envshake.freq"){
                hit.fall.envShake.frequency = Compiler::compile(simple.getValue());
            } else if (simple == "fall.envshake.ampl"){
                hit.fall.envShake.amplitude = Compiler::compile(simple.getValue());
            } else if (simple == "fall.envshake.phase"){
                hit.fall.envShake.phase = Compiler::compile(simple.getValue());
            } else if (simple == "air.fall"){
                hit.fall.airFall = Compiler::compile(simple.getValue());
            } else if (simple == "forcenofall"){
                hit.fall.forceNoFall = Compiler::compile(simple.getValue());
            } else if (simple == "down.velocity"){
                const Ast::Value * x;
                const Ast::Value * y;
                simple.view() >> x >> y;
                hit.down.x = Compiler::compile(x);
                hit.down.y = Compiler::compile(y);
            } else if (simple == "down.hittime"){
                const Ast::Value * x;
                simple.view() >> x;
                hit.down.time = Compiler::compile(x);
            } else if (simple == "envshake.time"){
                const Ast::Value * x;
                simple.view() >> x;
                hit.envShake.time = Compiler::compile(x);
            } else if (simple == "envshake.freq"){
                const Ast::Value * x;
                simple.view() >> x;
                hit.envShake.frequency = Compiler::compile(x);
            } else if (simple == "envshake.ampl"){
                const Ast::Value * x;
                simple.view() >> x;
                hit.envShake.amplitude = Compiler::compile(x);
            } else if (simple == "envshake.phase"){
                const Ast::Value * x;
                simple.view() >> x;
                hit.envShake.phase = Compiler::compile(x);
            } else if (shouldIgnore(simple)){
                /* ignore triggers, parsed already */
            } else {
                // Global::debug(1) << "Unhandled hitdef attribute: " << simple.toString() << endl;
            }
        }
    };

    Walker walker(hit);
    section->walk(walker);
}

static void evaluateHitDefinition(const HitDefinitionData & hit, HitDefinition & his, const FullEnvironment & env){
#define evaluateNumberLocal(value, default_) evaluateNumber(value, env, default_)
#define evaluateBoolLocal(value, default_) evaluateBool(value, env, default_)
    his.pause.player1 = evaluateNumberLocal(hit.pause.player1, 0);
    his.pause.player2 = evaluateNumberLocal(hit.pause.player2, 0);
    his.guardPause.player1 = evaluateNumberLocal(hit.guardPause.player1, his.pause.player1);
    his.guardPause.player2 = evaluateNumberLocal(hit.guardPause.player2, his.pause.player2);
    his.groundType = hit.groundType;
    his.airType = hit.airType;

    his.hitFlag = hit.hitFlag;
    his.guardFlag = hit.guardFlag;

    his.attribute.state = hit.attribute.state;
    his.attribute.attackType = hit.attribute.attackType;
    his.attribute.physics = hit.attribute.physics;

    his.guardDistance = evaluateNumberLocal(hit.guardDistance, -1);

    his.yAcceleration = evaluateNumberLocal(hit.yAcceleration, 0.35);
    his.animationTypeFall = hit.animationTypeFall;
    his.airHitTime = evaluateNumberLocal(hit.airHitTime, 20);
    his.airVelocity.x = evaluateNumberLocal(hit.airVelocity.x, 0);
    his.airVelocity.y = evaluateNumberLocal(hit.airVelocity.y, 0);
    his.fall.fall = evaluateBoolLocal(hit.fall.fall, false);
    his.fall.yVelocity = evaluateNumberLocal(hit.fall.yVelocity, -4.5);
    if (hit.fall.xVelocity != NULL){
        his.fall.changeXVelocity = true;
        his.fall.xVelocity = evaluateNumberLocal(hit.fall.xVelocity, 0);
    } else {
        his.fall.changeXVelocity = false;
    }
    his.fall.envShake.time = evaluateNumberLocal(hit.fall.envShake.time, 0);
    his.fall.damage = evaluateNumberLocal(hit.fall.damage, 0);
    his.fall.recover = evaluateBoolLocal(hit.fall.recover, true);
    his.fall.recoverTime = (int) evaluateNumberLocal(hit.fall.recoverTime, 4);
    his.id = (int) evaluateNumberLocal(hit.id, 0);
    his.chainId = (int) evaluateNumberLocal(hit.chainId, -1);
    his.groundSlideTime = evaluateNumberLocal(hit.groundSlideTime, 0);
    his.groundHitTime = evaluateNumberLocal(hit.groundHitTime, 0);
    his.guardGroundHitTime = evaluateNumberLocal(hit.guardGroundHitTime, 0);
    his.guardControlTime = evaluateNumberLocal(hit.guardControlTime, his.groundSlideTime);
    his.airGuardControlTime = evaluateNumberLocal(hit.airGuardControlTime, his.guardControlTime);
    his.groundVelocity.x = evaluateNumberLocal(hit.groundVelocity.x, 0);
    his.groundVelocity.y = evaluateNumberLocal(hit.groundVelocity.y, 0);
    his.damage.damage = evaluateNumberLocal(hit.damage.damage, 0);
    his.damage.guardDamage = evaluateNumberLocal(hit.damage.guardDamage, 0);
    his.airJuggle = evaluateNumberLocal(hit.airJuggle, 0);
    his.hitSound.own = hit.hitSound.own;
    his.hitSound.group = evaluateNumberLocal(hit.hitSound.group, -1);
    his.hitSound.item = evaluateNumberLocal(hit.hitSound.item, -1);
    his.guardHitSound.own = hit.guardHitSound.own;
    his.guardHitSound.group = evaluateNumberLocal(hit.guardHitSound.group, -1);
    his.guardHitSound.item = evaluateNumberLocal(hit.guardHitSound.item, -1);
    his.sparkPosition.x = evaluateNumberLocal(hit.sparkPosition.x, 0);
    his.sparkPosition.y = evaluateNumberLocal(hit.sparkPosition.y, 0);
    his.spark.own = hit.spark.own;
    his.spark.group = evaluateNumberLocal(hit.spark.group, -1);
    his.spark.item = evaluateNumberLocal(hit.spark.item, -1);
    his.guardSpark.own = hit.guardSpark.own;
    his.guardSpark.group = evaluateNumberLocal(hit.guardSpark.group, -1);
    his.guardSpark.item = evaluateNumberLocal(hit.guardSpark.item, -1);

    his.player1State = evaluateNumberLocal(hit.player1State, -1);
    his.player2State = evaluateNumberLocal(hit.player2State, -1);

    his.player1Facing = evaluateNumberLocal(hit.player1Facing, 0);
    his.player2Facing = evaluateNumberLocal(hit.player2Facing, 0);
    
    his.player1SpritePriority = evaluateNumberLocal(hit.player1SpritePriority, 1);
    his.player2SpritePriority = evaluateNumberLocal(hit.player2SpritePriority, 0);

    his.getPower.hit = evaluateNumberLocal(hit.getPower.hit, his.damage.damage * Data::getInstance().getDefaultAttackLifeToPowerMultiplier());
    his.getPower.guarded = evaluateNumberLocal(hit.getPower.guarded, his.getPower.hit / 2);
    
    his.givePower.hit = evaluateNumberLocal(hit.givePower.hit, his.damage.damage * Data::getInstance().getDefaultGetHitLifeToPowerMultiplier());
    his.givePower.guarded = evaluateNumberLocal(hit.getPower.guarded, his.getPower.hit / 2);

#undef evaluateNumber
#undef evaluateBool
}

class ControllerHitDef: public StateController {
public:
    ControllerHitDef(Ast::Section * section, const string & name, int state):
        StateController(name, state, section){
            parseHitDefinition(section, hit);
        }

    ControllerHitDef(const ControllerHitDef & you):
    StateController(you),
    hit(you.hit){
    }

    
    HitDefinitionData hit;

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        /* If not in an attack state don't do anything */
        if (guy.getMoveType() == Move::Attack){
            guy.enableHit();
            guy.nextTicket();
            /* set all the hitdef crap */
            FullEnvironment env(stage, guy, commands);
            HitDefinition & his = guy.getHit();
            evaluateHitDefinition(hit, his, env);
        }
    }

    StateController * deepCopy() const {
        return new ControllerHitDef(*this);
    }
};

class ControllerStateTypeSet: public StateController {
public:
    ControllerStateTypeSet(Ast::Section * section, const string & name, int state):
        StateController(name, state, section),
        changeMoveType(false),
        changeStateType(false),
        changePhysics(false){
            parse(section);
        }

    ControllerStateTypeSet(const ControllerStateTypeSet & you):
    StateController(you),
    changeMoveType(you.changeMoveType),
    changeStateType(you.changeStateType),
    changePhysics(you.changePhysics),
    moveType(you.moveType),
    stateType(you.stateType),
    physics(you.physics){
    }

    bool changeMoveType;
    bool changeStateType;
    bool changePhysics;
    std::string moveType;
    std::string stateType;
    Physics::Type physics;

    void setMoveType(const string & type){
        changeMoveType = true;
        moveType = type;
    }

    void setStateType(const string & type){
        changeStateType = true;
        stateType = type;
    }

    void setPhysics(Physics::Type type){
        changePhysics = true;
        physics = type;
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerStateTypeSet & controller):
                controller(controller){
                }

            ControllerStateTypeSet & controller;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "movetype"){
                    string type;
                    simple.view() >> type;
                    controller.setMoveType(type);
                } else if (simple == "physics"){
                    string type;
                    simple.view() >> type;
                    if (type == "S"){
                        controller.setPhysics(Physics::Stand);
                    } else if (type == "N"){
                        controller.setPhysics(Physics::None);
                    } else if (type == "C"){
                        controller.setPhysics(Physics::Crouch);
                    } else if (type == "A"){
                        controller.setPhysics(Physics::Air);
                    }
                } else if (simple == "statetype"){
                    string type;
                    simple.view() >> type;
                    controller.setStateType(type);
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        if (changeMoveType){
            guy.setMoveType(moveType);
        }

        if (changeStateType){
            guy.setStateType(stateType);
        }

        if (changePhysics){
            guy.setCurrentPhysics(physics);
        }
    }

    StateController * deepCopy() const {
        return new ControllerStateTypeSet(*this);
    }
};

class ControllerSuperPause: public StateController {
public:
    ControllerSuperPause(Ast::Section * section, const string & name, int state):
    StateController(name, state, section),
    ownAnimation(false){
        parse(section);
    }

    ControllerSuperPause(const ControllerSuperPause & you):
    StateController(you),
    time(copy(you.time)),
    posX(copy(you.posX)),
    posY(copy(you.posY)),
    animation(copy(you.animation)),
    ownAnimation(you.ownAnimation),
    darken(copy(you.darken)),
    p2defmul(copy(you.p2defmul)),
    poweradd(copy(you.poweradd)),
    unhittable(copy(you.unhittable)),
    sound(you.sound){
    }

    struct Sound{
        Sound():
        own(false){
        }

        Sound(const Sound & you):
        group(copy(you.group)),
        item(copy(you.item)),
        own(you.own){
        }

        ~Sound(){
        }

        Value group;
        Value item;
        bool own;
    };

    Value time;
    Value posX;
    Value posY;
    Value animation;
    bool ownAnimation;
    Value darken;
    Value p2defmul;
    Value poweradd;
    Value unhittable;
    Sound sound;

    virtual ~ControllerSuperPause(){
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerSuperPause & super):
                super(super){
                }

            ControllerSuperPause & super;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "time"){
                    super.time = Compiler::compile(simple.getValue());
                } else if (simple == "anim"){
                    Resource resource = extractResource(simple.getValue());
                    super.ownAnimation = resource.own; 
                    super.animation = Compiler::compile(resource.value);
                } else if (simple == "darken"){
                    super.darken = Compiler::compile(simple.getValue());
                } else if (simple == "p2defmul"){
                    super.p2defmul = Compiler::compile(simple.getValue());
                } else if (simple == "poweradd"){
                    super.poweradd = Compiler::compile(simple.getValue());
                } else if (simple == "unhittable"){
                    super.unhittable = Compiler::compile(simple.getValue());
                } else if (simple == "sound"){
                    const Ast::Value * group = NULL;
                    const Ast::Value * item = NULL;
                    if (simple.getValue()->hasMultiple()){
                        simple.view() >> group >> item;
                    } else {
                        group = simple.getValue();
                    }

                    if (group == NULL){
                        compileError("No group given for 'sound'", __FILE__, __LINE__);
                    }

                    Resource resource = extractResource(group);
                    super.sound.own = resource.own;
                    super.sound.group = Compiler::compile(resource.value);

                    if (item != NULL){
                        super.sound.item = Compiler::compile(item);
                    } else {
                        super.sound.item = Compiler::compile(0);
                    }
                } else if (simple == "pos"){
                    if (simple.getValue()->hasMultiple()){
                        const Ast::Value * posX;
                        const Ast::Value * posY;
                        simple.view() >> posX >> posY;
                        super.posX = Compiler::compile(posX);
                        super.posY = Compiler::compile(posY);
                    }
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    int computeX(Character & guy, const Environment & env) const {
        int pos = 0;
        if (posX != NULL){
            pos = (int) posX->evaluate(env).toNumber() * (guy.getFacing() == FacingLeft ? -1 : 1);
        }
        return (int)(guy.getX() + pos);
    }

    int computeY(Character & guy, const Environment & env) const {
        int pos = 0;
        if (posY != NULL){
            pos = (int) posY->evaluate(env).toNumber();
        }
        return (guy.getRY() + pos);
    }

    void playSound(Character & guy, const Environment & environment) const {
        PaintownUtil::ReferenceCount<Mugen::Sound> sound = PaintownUtil::ReferenceCount<Mugen::Sound>(NULL);
        if (this->sound.item != NULL){
            int groupNumber = (int) this->sound.group->evaluate(environment).toNumber();
            int itemNumber = (int) this->sound.item->evaluate(environment).toNumber();
            if (this->sound.own){
                sound = guy.getCommonSound(groupNumber, itemNumber);
            } else {
                sound = guy.getSound(groupNumber, itemNumber);
            }
        }

        if (sound != NULL){
            sound->play();
        }
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment env(stage, guy);
        int x = computeX(guy, env);
        int y = computeY(guy, env);
        int time = (int) evaluateNumber(this->time, env, 30);
        int animation = (int) evaluateNumber(this->animation, env, 30);

        /* FIXME: handle darken, p2defmul, unhittable */

        playSound(guy, env);
        stage.doSuperPause(time, guy, animation, ownAnimation, x, y);
    }

    StateController * deepCopy() const {
        return new ControllerSuperPause(*this);
    }
};

class ControllerAfterImage: public StateController {
public:
    ControllerAfterImage(Ast::Section * section, const string & name, int state):
        StateController(name, state, section),
        translucent(Default){
            parse(section);
        }

    ControllerAfterImage(const ControllerAfterImage & you):
    StateController(you),
    time(copy(you.time)),
    length(copy(you.length)),
    timeGap(copy(you.timeGap)),
    frameGap(copy(you.frameGap)),
    paletteColor(copy(you.paletteColor)),
    invertColor(copy(you.invertColor)),
    bright(you.bright),
    contrast(you.contrast),
    postbright(you.postbright),
    add(you.add),
    multiply(you.multiply),
    translucent(you.translucent){
    }

    Value time;
    Value length;
    Value timeGap;
    Value frameGap;
    Value paletteColor;
    Value invertColor;

    struct RGBColors{
        RGBColors(){
        }

        RGBColors(const RGBColors & him):
        red(copy(him.red)),
        green(copy(him.green)),
        blue(copy(him.blue)){
        }

        Value red, green, blue;
    };

    RGBColors bright;
    RGBColors contrast;
    RGBColors postbright;
    RGBColors add;
    RGBColors multiply;
    TransType translucent;

    virtual ~ControllerAfterImage(){
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerAfterImage & image):
                image(image){
                }

            ControllerAfterImage & image;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "time"){
                    image.time = Compiler::compile(simple.getValue());
                } else if (simple == "length"){
                    image.length = Compiler::compile(simple.getValue());
                } else if (simple == "palcolor"){
                    image.paletteColor = Compiler::compile(simple.getValue());
                } else if (simple == "palinvertall"){
                    image.invertColor = Compiler::compile(simple.getValue());
                } else if (simple == "palbright"){
                    // * palbright = add_r, add_g, add_b (int)
                    const Ast::Value * red;
                    const Ast::Value * green;
                    const Ast::Value * blue;
                    simple.view() >> red >> green >> blue;
                    image.bright.red = Compiler::compile(red);
                    image.bright.green = Compiler::compile(green);
                    image.bright.blue = Compiler::compile(blue);
                } else if (simple == "palcontrast"){
                    const Ast::Value * red;
                    const Ast::Value * green;
                    const Ast::Value * blue;
                    simple.view() >> red >> green >> blue;
                    image.contrast.red = Compiler::compile(red);
                    image.contrast.green = Compiler::compile(green);
                    image.contrast.blue = Compiler::compile(blue);
                } else if (simple == "palpostbright"){
                    const Ast::Value * red;
                    const Ast::Value * green;
                    const Ast::Value * blue;
                    simple.view() >> red >> green >> blue;
                    image.postbright.red = Compiler::compile(red);
                    image.postbright.green = Compiler::compile(green);
                    image.postbright.blue = Compiler::compile(blue);
                } else if (simple == "paladd"){
                    const Ast::Value * red;
                    const Ast::Value * green;
                    const Ast::Value * blue;
                    simple.view() >> red >> green >> blue;
                    image.add.red = Compiler::compile(red);
                    image.add.green = Compiler::compile(green);
                    image.add.blue = Compiler::compile(blue);
                } else if (simple == "palmul"){
                    const Ast::Value * red;
                    const Ast::Value * green;
                    const Ast::Value * blue;
                    simple.view() >> red >> green >> blue;
                    image.multiply.red = Compiler::compile(red);
                    image.multiply.green = Compiler::compile(green);
                    image.multiply.blue = Compiler::compile(blue);
                } else if (simple == "timegap"){
                    image.timeGap = Compiler::compile(simple.getValue());
                } else if (simple == "framegap"){
                    image.frameGap = Compiler::compile(simple.getValue());
                } else if (simple == "trans"){
                    string type;
                    simple.view() >> type;
                    image.translucent = parseTrans(PaintownUtil::lowerCaseAll(type));
                }
                /* time = duration (int)
                 * Specifies the number of ticks that the afterimages should be displayed for. Set to -1 to display indefinitely. Defaults to 1.
                 * length = no_of_frames (int)
                 * Sets the capacity of the frame history buffer. The history will hold up to no_of_frames of the character's most recently saved frames. Assuming constant values for timegap and framegap, increasing the length can increase the number and "age" (for lack of a better term) of afterimages displayed at one time. The maximum length is 60, and the default is 20.
                 * palcolor = col (int)
                 * See below.
                 * palinvertall = invertall (bool)
                 * See below.
                 * palbright = add_r, add_g, add_b (int)
                 * See below.
                 * palcontrast = mul_r, mul_g, mul_b (int)
                 * See below.
                 * palpostbright = add2_r, add2_g, add2_b (int)
                 * These parameters determine palette effects to be applied to all afterimages. First the color level is adjusted according to the palcolor value, then if invertall is non-zero the colors are inverted. Afterwards, the palbright components are added to the corresponding component of the player's palette, then each component is multiplied by the corresponding palcontrast component divided by 256, then the palpostbright components are added to the result. The value of palcolor ranges from 0 (greyscale) to 256 (normal color). For instance, if the red component of the character's palette is denoted pal_r, then the red component of the afterimage palette is given by (pal_r + add_r) * mul_r / 256 + add2_r, assuming palcolor and palinvert are left at their default values. Valid values are 0-256 for palcolor, 0-255 for palbright and palpostbright components, and any non-negative integer for palcontrast components. The defaults are:
                 *
                 * palcolor = 256
                 * palinvertall = 0
                 * palbright = 30,30,30
                 * palcontrast = 120,120,220
                 * palpostbright = 0,0,0
                 * paladd = add_r, add_g, add_b (int)
                 * See below.
                 * palmul = mul_r, mul_g, mul_b (float)
                 * These parameters specify palette effects that are applied repeatedly to successive frames in the afterimage. In one application of these palette effects, first the paladd components are added to the afterimage palette, then the components are multiplied by the palmul multipliers. These effects are applied zero times to the most recent afterimage frame, once to the second-newest afterimage frame, twice in succession to the third-newest afterimage frame, etc. Valid values are 0-255 for the paladd components, and any non-negative float value for the palmul multipliers. The defaults are:
                 *
                 * paladd = 10,10,25
                 * palmul = .65,.65,.75
                 * timegap = value (int)
                 * This parameter controls how many frames to skip between saving player frames to the history buffer for afterimage display. The default is 1 (skip no frames). To save every third frame (for example), you would use timegap = 3.
                 * framegap = value (int)
                 * Every value'th frame in the history buffer will be displayed as an afterimage. For instance, if framegap = 4 (the default), then the first, fifth, ninth, ... frames of the history buffer will be displayed as afterimages.
                 * trans = type (string)
                 * Specifies the transparency type for the afterimages. Valid values for type are "none" for an opaque afterimage, "add", "add1", and "sub". Defaults to "none".
                 */
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment environment(stage, guy, commands);
        int timegap = evaluateNumber(this->timeGap, environment, 1);
        int framegap = evaluateNumber(this->frameGap, environment, 1);
        int time = evaluateNumber(this->time, environment, 1);
        int length = evaluateNumber(this->length, environment, 20);
        int paletteColor = evaluateNumber(this->paletteColor, environment, 256);
        bool invertColor = evaluateBool(this->invertColor, environment, false);
        Character::AfterImage::RGBx bright, contrast, postBright, add, multiply;
        bright.red = evaluateNumber(this->bright.red, environment, 30);
        bright.green = evaluateNumber(this->bright.green, environment, 30);
        bright.blue = evaluateNumber(this->bright.blue, environment, 30);
        
        contrast.red = evaluateNumber(this->contrast.red, environment, 120);
        contrast.green = evaluateNumber(this->contrast.green, environment, 120);
        contrast.blue = evaluateNumber(this->contrast.blue, environment, 120);
        
        postBright.red = evaluateNumber(this->postbright.red, environment, 0);
        postBright.green = evaluateNumber(this->postbright.green, environment, 0);
        postBright.blue = evaluateNumber(this->postbright.blue, environment, 0);

        add.red = evaluateNumber(this->add.red, environment, 10);
        add.green = evaluateNumber(this->add.green, environment, 10);
        add.blue = evaluateNumber(this->add.blue, environment, 25);
        
        multiply.red = evaluateNumber(this->multiply.red, environment, 0.65);
        multiply.green = evaluateNumber(this->multiply.green, environment, 0.65);
        multiply.blue = evaluateNumber(this->multiply.blue, environment, 0.75);

        guy.setAfterImage(time, length, timegap, framegap, translucent,
                          paletteColor, invertColor,
                          bright, contrast, postBright, add, multiply);
    }

    StateController * deepCopy() const {
        return new ControllerAfterImage(*this);
    }
};

class ControllerAfterImageTime: public StateController {
public:
    ControllerAfterImageTime(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        parse(section);
    }

    ControllerAfterImageTime(const ControllerAfterImageTime & you):
    StateController(you),
    time(copy(you.time)){
    }

    Value time;

    virtual ~ControllerAfterImageTime(){
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(Value & time):
                time(time){
                }

            Value & time;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "time"){
                    time = Compiler::compile(simple.getValue());
                } else if (simple == "value"){
                    time = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(time);
        section->walk(walker);
        if (time == NULL){
            throw MugenException("Time not specified for AfterImageTime", __FILE__, __LINE__);
        }
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment environment(stage, guy);
        int time = this->time->evaluate(environment).toNumber();
        guy.setAfterImageTime(time);
    }

    StateController * deepCopy() const {
        return new ControllerAfterImageTime(*this);
    }
};

class ControllerAngleAdd: public StateController {
public:
    ControllerAngleAdd(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        parse(section);
    }

    ControllerAngleAdd(const ControllerAngleAdd & you):
    StateController(you),
    value(copy(you.value)){
    }

    Value value;

    virtual ~ControllerAngleAdd(){
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(Value & value):
                value(value){
                }

            Value & value;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "value"){
                    value = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(value);
        section->walk(walker);
        if (value == NULL){
            throw MugenException("`value' is a required attribute of AngleAdd", __FILE__, __LINE__);
        }
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        int value = this->value->evaluate(FullEnvironment(stage, guy)).toNumber();
        guy.updateAngleEffect(value + guy.getAngleEffect());
    }

    StateController * deepCopy() const {
        return new ControllerAngleAdd(*this);
    }
};

class ControllerAngleMul: public StateController {
public:
    ControllerAngleMul(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        parse(section);
    }

    ControllerAngleMul(const ControllerAngleMul & you):
    StateController(you),
    value(copy(you.value)){
    }

    Value value;

    virtual ~ControllerAngleMul(){
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(Value & value):
                value(value){
                }

            Value & value;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "value"){
                    value = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(value);
        section->walk(walker);
        if (value == NULL){
            throw MugenException("`value' is a required attribute of AngleMul", __FILE__, __LINE__);
        }
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        double value = this->value->evaluate(FullEnvironment(stage, guy)).toNumber();
        guy.updateAngleEffect(guy.getAngleEffect() * value);
    }

    StateController * deepCopy() const {
        return new ControllerAngleMul(*this);
    }
};

class ControllerAngleSet: public StateController {
public:
    ControllerAngleSet(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        parse(section);
    }

    ControllerAngleSet(const ControllerAngleSet & you):
    StateController(you),
    value(copy(you.value)){
    }

    Value value;

    virtual ~ControllerAngleSet(){
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(Value & value):
                value(value){
                }

            Value & value;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "value"){
                    value = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(value);
        section->walk(walker);
        if (value == NULL){
            throw MugenException("`value' is a required attribute of AngleMul", __FILE__, __LINE__);
        }
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        double value = this->value->evaluate(FullEnvironment(stage, guy)).toNumber();
        guy.updateAngleEffect(value);
    }

    StateController * deepCopy() const {
        return new ControllerAngleSet(*this);
    }
};

class ControllerAngleDraw: public StateController {
public:
    ControllerAngleDraw(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        parse(section);
    }

    ControllerAngleDraw(const ControllerAngleDraw & you):
    StateController(you),
    value(copy(you.value)),
    scaleX(copy(you.scaleX)),
    scaleY(copy(you.scaleY)){
    }

    Value value;
    Value scaleX;
    Value scaleY;

    virtual ~ControllerAngleDraw(){
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(Value & value, Value & scaleX, Value & scaleY):
                value(value),
                scaleX(scaleX),
                scaleY(scaleY){
                }

            Value & value;
            Value & scaleX;
            Value & scaleY;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "value"){
                    value = Compiler::compile(simple.getValue());
                } else if (simple == "scale"){
                    const Ast::Value * x = NULL;
                    const Ast::Value * y = NULL;
                    simple.view() >> x >> y;
                    scaleX = Compiler::compile(x);
                    scaleY = Compiler::compile(y);
                }
            }
        };

        Walker walker(value, scaleX, scaleY);
        section->walk(walker);
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        double value = 0;
        bool setValue = false;
        if (this->value != NULL){
            value = this->value->evaluate(FullEnvironment(stage, guy)).toNumber();
            setValue = true;
        }

        double scaleX = 1;
        double scaleY = 1;
        if (this->scaleX != NULL){
            scaleX = this->scaleX->evaluate(FullEnvironment(stage, guy)).toNumber();
        }
        if (this->scaleY != NULL){
            scaleY = this->scaleY->evaluate(FullEnvironment(stage, guy)).toNumber();
        }
        
        guy.drawAngleEffect(value, setValue, scaleX, scaleY);
    }

    StateController * deepCopy() const {
        return new ControllerAngleDraw(*this);
    }
};

class ControllerAssertSpecial: public StateController {
public:
    ControllerAssertSpecial(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        parse(section);
    }

    ControllerAssertSpecial(const ControllerAssertSpecial & you):
    StateController(you),
    asserts(you.asserts){
    }

    vector<Character::Specials> asserts;

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(vector<Character::Specials> & asserts):
                asserts(asserts){
                    names["intro"] = Character::Intro;
                    names["invisible"] = Character::Invisible;
                    names["roundnotover"] = Character::RoundNotOver;
                    names["nobardisplay"] = Character::NoBarDisplay;
                    names["nobg"] = Character::NoBG;
                    names["nofg"] = Character::NoFG;
                    names["nostandguard"] = Character::NoStandGuard;
                    names["nocrouchguard"] = Character::NoCrouchGuard;
                    names["noairguard"] = Character::NoAirGuard;
                    names["noautoturn"] = Character::NoAutoTurn;
                    names["nojugglecheck"] = Character::NoJuggleCheck;
                    names["nokosnd"] = Character::NoKOSnd;
                    names["nokoslow"] = Character::NoKOSlow;
                    names["noshadow"] = Character::NoShadow;
                    names["globalnoshadow"] = Character::GlobalNoShadow;
                    names["nomusic"] = Character::NoMusic;
                    names["nowalk"] = Character::NoWalk;
                    names["timerfreeze"] = Character::TimerFreeze;
                    names["unguardable"] = Character::UnGuardable;
                }

            vector<Character::Specials> & asserts;
            map<string, Character::Specials> names;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "flag" || simple == "flag2" || simple == "flag3"){
                    string what;
                    simple.view() >> what;
                    what = Util::fixCase(what);
                    if (names.find(what) != names.end()){
                        asserts.push_back(names[what]);
                    } else {
                        Global::debug(0) << "Warning: unknown special '" << what << "'" << endl;
                    }
                }
            }
        };

        Walker walker(asserts);
        section->walk(walker);
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        for (vector<Character::Specials>::const_iterator it = asserts.begin(); it != asserts.end(); it++){
            Character::Specials special = *it;
            guy.assertSpecial(special);
        }
    }

    StateController * deepCopy() const {
        return new ControllerAssertSpecial(*this);
    }
};

class ControllerAttackDist: public StateController {
public:
    ControllerAttackDist(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        parse(section);
    }

    ControllerAttackDist(const ControllerAttackDist & you):
    StateController(you),
    value(copy(you.value)){
    }

    Value value;

    void parse(Ast::Section * section){
        extractValue(value, section);
        if (value == NULL){
            throw MugenException("AttackDist must have a `value' attribute", __FILE__, __LINE__);
        }
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        int value = (int) this->value->evaluate(FullEnvironment(stage, guy)).toNumber();
        guy.getHit().guardDistance = value;
    }

    StateController * deepCopy() const {
        return new ControllerAttackDist(*this);
    }
};

class ControllerNull: public StateController {
public:
    ControllerNull(const string & name, int state):
    StateController(name, state){
    }

    ControllerNull(const ControllerNull & you):
    StateController(you){
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        /* nothing */
    }

    StateController * deepCopy() const {
        return new ControllerNull(*this);
    }
};

class ControllerTurn: public StateController {
public:
    ControllerTurn(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
    }

    ControllerTurn(const ControllerTurn & you):
    StateController(you){
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        guy.reverseFacing();
    }

    StateController * deepCopy() const {
        return new ControllerTurn(*this);
    }
};

class ControllerVarAdd: public StateController {
public:
    ControllerVarAdd(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        parse(section, name);
    }

    ControllerVarAdd(const ControllerVarAdd & you):
    StateController(you),
    integerIndex(copy(you.integerIndex)),
    floatIndex(copy(you.floatIndex)),
    sysIndex(copy(you.sysIndex)),
    value(copy(you.value)){
    }

    Value integerIndex;
    Value floatIndex;
    Value sysIndex;
    Value value;

    virtual ~ControllerVarAdd(){
    }

    void parse(Ast::Section * section, const string & name){
        class Walker: public Ast::Walker {
        public:
            Walker(Value & integerIndex, Value & floatIndex, Value & sysIndex, Value & value):
            integerIndex(integerIndex),
            floatIndex(floatIndex),
            sysIndex(sysIndex),
            value(value){
            }

            Value & integerIndex;
            Value & floatIndex;
            Value & sysIndex;
            Value & value;

            virtual void onAttributeArray(const Ast::AttributeArray & simple){
                if (simple == "var"){
                    int index = simple.getIndex();
                    const Ast::Value * value = simple.getValue();
                    this->value = Compiler::compile(value);
                    this->integerIndex = Compiler::compile(index);
                } else if (simple == "fvar"){
                    int index = simple.getIndex();
                    const Ast::Value * value = simple.getValue();
                    this->value = Compiler::compile(value);
                    this->floatIndex = Compiler::compile(index);
                } else if (simple == "sysvar"){
                    int index = simple.getIndex();
                    const Ast::Value * value = simple.getValue();
                    this->value = Compiler::compile(value);
                    this->sysIndex = Compiler::compile(index);
                }
            }

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "v"){
                    integerIndex = Compiler::compile(simple.getValue());
                } else if (simple == "fv"){
                    floatIndex = Compiler::compile(simple.getValue());
                } else if (simple == "value"){
                    value = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(integerIndex, floatIndex, sysIndex, value);
        section->walk(walker);
        if (value == NULL){
            throw MugenException("Must set `value' for controller VarAdd " + name, __FILE__, __LINE__);
        }
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        if (integerIndex != NULL){
            int index = (int)integerIndex->evaluate(FullEnvironment(stage, guy)).toNumber();
            double old = guy.getVariable(index).toNumber();
            double new_ = value->evaluate(FullEnvironment(stage, guy)).toNumber();
            guy.setVariable(index, RuntimeValue(old + new_));
        } else if (floatIndex != NULL){
            int index = (int) floatIndex->evaluate(FullEnvironment(stage, guy)).toNumber();
            double old = guy.getFloatVariable(index).toNumber();
            double new_ = value->evaluate(FullEnvironment(stage, guy)).toNumber();
            guy.setFloatVariable(index, RuntimeValue(old + new_));
        } else if (sysIndex != NULL){
            int index = (int) sysIndex->evaluate(FullEnvironment(stage, guy)).toNumber();
            double old = guy.getSystemVariable(index).toNumber();
            double new_ = value->evaluate(FullEnvironment(stage, guy)).toNumber();
            guy.setSystemVariable(index, RuntimeValue(old + new_));
        }
    }

    StateController * deepCopy() const {
        return new ControllerVarAdd(*this);
    }
};

class ControllerForceFeedback: public StateController {
public:
    ControllerForceFeedback(Ast::Section * section, const string & name, int state):
    StateController(name, state){
    }

    ControllerForceFeedback(const ControllerForceFeedback & you):
    StateController(you){
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        /* TODO, if we care about this controller */
    }

    StateController * deepCopy() const {
        return new ControllerForceFeedback(*this);
    }
};

class ControllerWidth: public StateController {
public:
    ControllerWidth(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        parse(section);
    }

    ControllerWidth(const ControllerWidth & you):
    StateController(you),
    edgeFront(copy(you.edgeFront)),
    edgeBack(copy(you.edgeBack)),
    playerFront(copy(you.playerFront)),
    playerBack(copy(you.playerBack)){
    }

    Value edgeFront;
    Value edgeBack;
    Value playerFront;
    Value playerBack;

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(Value & edgeFront,
                   Value & edgeBack,
                   Value & playerFront,
                   Value & playerBack):
                   edgeFront(edgeFront),
                   edgeBack(edgeBack),
                   playerFront(playerFront),
                   playerBack(playerBack){
                   }

            Value & edgeFront;
            Value & edgeBack;
            Value & playerFront;
            Value & playerBack;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "edge"){
                    const Ast::Value * front;
                    const Ast::Value * back;
                    simple.view() >> front >> back;
                    edgeFront = Compiler::compile(front);
                    edgeBack = Compiler::compile(back);
                } else if (simple == "player"){
                    const Ast::Value * front;
                    const Ast::Value * back;
                    simple.view() >> front >> back;
                    playerFront = Compiler::compile(front);
                    playerBack = Compiler::compile(back);
                } else if (simple == "value"){
                    const Ast::Value * front;
                    const Ast::Value * back;
                    simple.view() >> front >> back;
                    edgeFront = Compiler::compile(front);
                    edgeBack = Compiler::compile(back);
                    playerFront = Compiler::compile(front);
                    playerBack = Compiler::compile(back);
                }
            }
        };

        Walker walker(edgeFront, edgeBack, playerFront, playerBack);
        section->walk(walker);
    }

    virtual ~ControllerWidth(){
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        int edgeFront = 0;
        int edgeBack = 0;
        int playerFront = 0;
        int playerBack = 0;
        FullEnvironment environment(stage, guy);
        if (this->edgeFront != NULL){
            edgeFront = (int) this->edgeFront->evaluate(environment).toNumber();
        }
        if (this->edgeBack != NULL){
            edgeBack = (int) this->edgeBack->evaluate(environment).toNumber();
        }
        if (this->playerFront != NULL){
            playerFront = (int) this->playerFront->evaluate(environment).toNumber();
        }
        if (this->playerBack != NULL){
            playerBack = (int) this->playerBack->evaluate(environment).toNumber();
        }
        guy.setWidthOverride(edgeFront, edgeBack, playerFront, playerBack);
    }

    StateController * deepCopy() const {
        return new ControllerWidth(*this);
    }
};

class ControllerMakeDust: public StateController {
public:
    ControllerMakeDust(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        parse(section);
    }

    ControllerMakeDust(const ControllerMakeDust & you):
    StateController(you),
    posX(copy(you.posX)),
    posY(copy(you.posY)),
    posX2(copy(you.posX2)),
    posY2(copy(you.posY2)),
    spacing(copy(you.spacing)){
    }

    Value posX;
    Value posY;
    Value posX2;
    Value posY2;
    Value spacing;

    virtual ~ControllerMakeDust(){
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(Value & posX,
                   Value & posY,
                   Value & posX2,
                   Value & posY2,
                   Value & spacing):
                   posX(posX),
                   posY(posY),
                   posX2(posX2),
                   posY2(posY2),
                   spacing(spacing){
            }

            Value & posX;
            Value & posY;
            Value & posX2;
            Value & posY2;
            Value & spacing;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "pos"){
                    const Ast::Value * x;
                    const Ast::Value * y;
                    simple.view() >> x >> y;
                    posX = Compiler::compile(x);
                    posY = Compiler::compile(y);
                } else if (simple == "pos2"){
                    const Ast::Value * x;
                    const Ast::Value * y;
                    simple.view() >> x >> y;
                    posX2 = Compiler::compile(x);
                    posY2 = Compiler::compile(y);
                } else if (simple == "spacing"){
                    spacing = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(posX, posY, posX2, posY2, spacing);
        section->walk(walker);
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment environment(stage, guy);
        int x = 0;
        int y = 0;
        if (posX != NULL && posY != NULL){
            x = (int) posX->evaluate(environment).toNumber();
            y = (int) posY->evaluate(environment).toNumber();
        }
        stage.createDust((int)(guy.getX() + x), (int)(guy.getRY() + y));

        if (posX2 != NULL && posY2 != NULL){
            x = (int) posX2->evaluate(environment).toNumber();
            y = (int) posY2->evaluate(environment).toNumber();
            stage.createDust((int)(guy.getX() + x), (int)(guy.getRY() + y));
        }

        /* FIXME: use spacing somehow */
    }

    StateController * deepCopy() const {
        return new ControllerMakeDust(*this);
    }
};

class ControllerFallEnvShake: public StateController {
public:
    ControllerFallEnvShake(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
    }

    ControllerFallEnvShake(const ControllerFallEnvShake & you):
    StateController(you){
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        HitState & state = guy.getHitState();
        if (state.fall.envShake.time != 0){
            stage.Quake(state.fall.envShake.time);
            state.fall.envShake.time = 0;
        }
    }

    StateController * deepCopy() const {
        return new ControllerFallEnvShake(*this);
    }
};

class ControllerExplod: public StateController {
private:
    enum PositionType{
        Player1, /* Interprets pos relative to p1's axis. A positive x offset is toward the front of p1. This is the default value for postype. Refer to the note at the end of this controller's description. */
        Player2, /* Interprets pos relative to p2's axis. A positive x offset is toward the front of p2. Refer to the note at the end of this controller's description. */
        Front, /* Interprets x_pos relative to the edge of the screen that p1 is facing toward, and y_pos relative to the top of the screen. A positive x offset is away from the center of the screen, whereas a negative x offset is toward the center. */
        Back, /* Interprets x_pos relative to the edge of the screen that p1 is facing away from, and y_pos relative to the top of the screen. A positive x offset is toward the center of the screen, whereas a negative x offset is away from the center. */
        Left, /* Interprets x_pos and y_pos relative to the upper-left corner of the screen. A positive x offset is toward the right of the screen. */
        Right /* Interprets x_pos and y_pos relative to the upper-right corner of the screen. A positive x offset is toward the left of the screen. */
        
    };
public:
    ControllerExplod(Ast::Section * section, const string & name, int state):
    StateController(name, state, section),
    ownAnimation(true),
    positionType(Player1){
        parse(section);
    }

    ControllerExplod(const ControllerExplod & you):
    StateController(you),
    animation(copy(you.animation)),
    ownAnimation(you.ownAnimation),
    id(copy(you.id)),
    posX(copy(you.posX)),
    posY(copy(you.posY)),
    velocityX(copy(you.velocityX)),
    velocityY(copy(you.velocityY)),
    accelerationX(copy(you.accelerationX)),
    accelerationY(copy(you.accelerationY)),
    removeTime(copy(you.removeTime)),
    bindTime(copy(you.bindTime)),
    spritePriority(copy(you.spritePriority)),
    scaleX(copy(you.scaleX)),
    scaleY(copy(you.scaleY)),
    superMove(copy(you.superMove)),
    superMoveTime(copy(you.superMoveTime)),
    facing(copy(you.facing)),
    verticalFlip(copy(you.verticalFlip)),
    randomX(copy(you.randomX)),
    randomY(copy(you.randomY)),
    removeOnHit(copy(you.removeOnHit)),
    positionType(you.positionType),
    ownPalette(copy(you.ownPalette)){
    }

    Value animation;
    bool ownAnimation;
    Value id;
    Value posX;
    Value posY;
    Value velocityX;
    Value velocityY;
    Value accelerationX;
    Value accelerationY;
    Value removeTime;
    Value bindTime;
    Value spritePriority;
    Value scaleX;
    Value scaleY;
    Value superMove;
    Value superMoveTime;
    Value facing;
    Value verticalFlip;
    Value randomX, randomY;
    Value removeOnHit;
    PositionType positionType;
    Value ownPalette;

    /* Returns true if the sprite should flip */
    static bool computeFacing(int facingPositive, const Character & guy, const Stage & stage, PositionType positionType, int bindTime){
        switch (positionType){
            case Player1: {
                if (facingPositive == 1){
                    return guy.getFacing() == FacingLeft;
                } else {
                    return guy.getFacing() == FacingRight;
                }
                break;
            }
            case Player2: {
                Character * enemy = stage.getEnemy(&guy);
                if (enemy != NULL){
                    if (facingPositive == 1){
                        return enemy->getFacing() == FacingLeft;
                    } else {
                        return enemy->getFacing() == FacingRight;
                    }
                }
                break;
            }
            case Front: {
                /* Hm, is this a hack? */
                if (bindTime == 0){
                    return guy.getFacing() == FacingLeft;
                }

                if (facingPositive == -1){
                    return guy.getFacing() == FacingLeft;
                } else {
                    return guy.getFacing() == FacingRight;
                }
                break;
            }
            case Back: {
                if (facingPositive == 1){
                    return guy.getFacing() == FacingLeft;
                } else {
                    return guy.getFacing() == FacingRight;
                }
                break;
            }
            case Left: {
                if (facingPositive == 1){
                    return false;
                } else {
                    return true;
                }
                break;
            }
            case Right: {
                if (bindTime == 0){
                    return false;
                }

                if (facingPositive == 1){
                    return true;
                } else {
                    return false;
                }
                break;
            }
        }

        return true;
    }
    
    /* Special case for when bindtime = 0, to replicate a bug in mugen
     */
    static void computePositionBind0(double posX, double posY, const Character * owner, const Stage & stage, PositionType positionType, bool horizontalFlip, double & x, double & y){
        /* at p1:
         *  p1: relative to 0, 0. positive X
         *  p2: relative to 0, 0. negative X
         *  left: relative to the starting screen left, 0. positive X
         *  right: relative to the starting screen right, 0. positive X
         *  back: relative to the starting screen opposite facing, 0. X towards facing
         *  front: like back, but towards facing. X towards facing
         */

        switch (positionType){
            case Player1: {
                x = stage.maximumLeft(owner) + posX;
                y = posY;
                break;
            }
            case Player2: {
                /* FIXME: use the enemy for maximum left? */
                x = stage.maximumLeft(owner) - posX;
                y = posY;
                break;
            }
            case Front: {
                /* Means facing left */
                if (horizontalFlip){
                    x = stage.getStartingLeft() - posX;
                    y = posY;
                } else {
                    x = stage.getStartingRight() + posX;
                    y = posY;
                }
                break;
            }
            case Back: {
                if (horizontalFlip){
                    x = stage.getStartingRight() + posX;
                    y = posY;
                } else {
                    x = stage.getStartingLeft() - posX;
                    y = posY;
                }
                break;
            }
            case Left: {
                x = stage.getStartingLeft() + posX;
                y = posY;
                break;
            }
            case Right: {
                x = stage.getStartingRight() + posX;
                y = posY;
                break;
            }
            default: {
                break;
            }
        }
    }

    static void computePosition(double posX, double posY, const Character * owner, const Stage & stage, PositionType positionType, bool horizontalFlip, double & x, double & y){
        posX *= horizontalFlip ? -1 : 1;
        switch (positionType){
            case Player1: {
                x = posX + owner->getX();
                y = posY + owner->getRY();
                break;
            }
            case Player2: {
                Character * enemy = stage.getEnemy(owner);
                if (enemy != NULL){
                    x = posX + enemy->getX();
                    y = posX + enemy->getRY();
                }
                break;
            }
            case Front: {
                if (owner->getFacing() == FacingRight){
                    x = stage.maximumRight(owner) + posX;
                } else {
                    x = stage.maximumLeft(owner) + posX;
                }
                y = stage.maximumUp() + posY;
                break;
            }
            case Back: {
                if (owner->getFacing() == FacingLeft){
                    x = stage.maximumRight(owner) + posX;
                } else {
                    x = stage.maximumLeft(owner) + posX;
                }
                y = stage.maximumUp() + posY;
                break;
            }
            case Left: {
                x = stage.maximumLeft(owner) + posX;
                y = stage.maximumUp() + posY;
                break;
            }
            case Right: {
                x = stage.maximumRight(owner) + posX;
                y = stage.maximumUp() + posY;
                break;
            }
            default: {
                x = posX;
                y = posY;
                break;
            }
        }
    }

    class ExplodeEffect: public Effect {
    public:
        ExplodeEffect(const Character * owner, const Mugen::Stage & stage, PaintownUtil::ReferenceCount<Animation> animation, int id, int x, int y, double velocityX, double velocityY, double accelerationX, double accelerationY, int removeTime, int bindTime, PositionType positionType, int posX, int posY, double scaleX, double scaleY, int spritePriority, bool superMove, int superMoveTime, bool horizontalFlip, bool verticalFlip, bool ownPalette, bool removeOnHit):
            Effect(owner, animation, id, x, y, scaleX, scaleY, spritePriority),
            stage(stage),
            velocityX(velocityX),
            velocityY(velocityY),
            accelerationX(accelerationX),
            accelerationY(accelerationY),
            removeTime(removeTime),
            bindTime(bindTime),
            positionType(positionType),
            posX(posX),
            posY(posY),
            frozen(false),
            superMovePersist(superMove),
            superMoveTime(superMoveTime),
            horizontalFlip(horizontalFlip),
            verticalFlip(verticalFlip),
            ownPalette(ownPalette),
            removeOnHit(removeOnHit),
            hitCount(owner->getWasHitCount()),
            shouldRemove(false){
            }

        void setVelocityX(double x){
            velocityX = x;
        }

        double getVelocityX() const {
            return velocityX;
        }

        void setVelocityY(double y){
            velocityY = y;
        }
        
        double getVelocityY() const {
            return velocityY;
        }

        void setAccelerationX(double x){
            accelerationX = x;
        }

        void setBindTime(int time){
            bindTime = time;
        }

        double getAccelerationX() const {
            return accelerationX;
        }

        void setAccelerationY(double y){
            accelerationY = y;
        }

        double getAccelerationY() const {
            return accelerationY;
        }

        void setRemoveTime(int time){
            removeTime = time;
        }

        int getRemoveTime() const {
            return removeTime;
        }
    
        virtual void superPauseStart(){
            if (!superMovePersist && superMoveTime == 0){
                frozen = true;
            }
        }

        virtual void superPauseEnd(){
            /* Unfreeze no matter what */
            frozen = false;
            superMoveTime = 0;
        }

        const Mugen::Stage & stage;
        double velocityX;
        double velocityY;
        double accelerationX;
        double accelerationY;
        int removeTime;
        int bindTime;
        PositionType positionType;
        const int posX;
        const int posY;
        bool frozen;
        const bool superMovePersist;
        int superMoveTime;
        const bool horizontalFlip;
        const bool verticalFlip;
        const bool ownPalette;
        const bool removeOnHit;
        const unsigned int hitCount;
        bool shouldRemove;

        virtual void logic(){

            if (removeOnHit){
                shouldRemove = owner->getWasHitCount() > hitCount;
            }

            if (!frozen){
                Effect::logic();

                /* either stopped or not set at all.
                 * FIXME: sort of a hack.. find a more elegant solution
                 */
                if (bindTime == 0 || bindTime == -2){
                    int reverse = horizontalFlip ? -1 : 1;
                    x += velocityX * reverse;
                    y += velocityY;
                    velocityX += accelerationX * reverse;
                    velocityY += accelerationY;
                } else {
                }

                if (removeTime > 0){
                    removeTime -= 1;
                }
            } else {
                if (superMoveTime > 0){
                    superMoveTime -= 1;
                    /* If we run out of super move time then we should freeze */
                    if (superMoveTime == 0){
                        frozen = true;
                    }
                }
            }

            /* FIXME: should we do the bind even if we are frozen? */
            if (bindTime != 0){
                /* bindTime could be negative in which case its active forever */
                if (bindTime > 0){
                    bindTime -= 1;
                }

                /* -2 is the default bindtime that means it wasn't set in the explod.
                 * if the bindtime is -1 then it should act indefinetely.
                 */
                if (bindTime != -2){
                    computePosition(posX, posY, owner, stage, positionType, horizontalFlip, x, y);
                }
            }
        }
	
        virtual void draw(const Graphics::Bitmap & work, int cameraX, int cameraY){
            animation->render(horizontalFlip, verticalFlip, (int)(getX() - cameraX), (int)(getY() - cameraY), work, scaleX, scaleY, NULL);
        }

        virtual bool isDead(){
            if (shouldRemove){
                return true;
            }

            switch (removeTime){
                case -2: return Effect::isDead();
                case -1: return false;
                default : return removeTime == 0;
            }

            return true;
        }
    };

    virtual ~ControllerExplod(){
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerExplod & controller):
                controller(controller){
                }

            ControllerExplod & controller;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "anim"){
                    Resource resource = extractResource(simple.getValue());
                    if (resource.fight){
                        controller.ownAnimation = false;
                    }
                    controller.animation = Compiler::compile(resource.value);
                } else if (simple == "id"){
                    controller.id = Compiler::compile(simple.getValue());
                } else if (simple == "pos"){
                    const Ast::Value * x;
                    const Ast::Value * y;
                    simple.view() >> x >> y;
                    controller.posX = Compiler::compile(x);
                    controller.posY = Compiler::compile(y);
                } else if (simple == "postype"){
                    string type;
                    simple.view() >> type;
                    type = PaintownUtil::lowerCaseAll(type);
                    if (type == "p1"){
                        controller.positionType = Player1;
                    } else if (type == "p2"){
                        controller.positionType = Player2;
                    } else if (type == "front"){
                        controller.positionType = Front;
                    } else if (type == "back"){
                        controller.positionType = Back;
                    } else if (type == "left"){
                        controller.positionType = Left;
                    } else if (type == "right"){
                        controller.positionType = Right;
                    } else {
                        Global::debug(0) << "Unknown position type '" << type << "'" << endl;
                    }
                } else if (simple == "facing"){
                    controller.facing = Compiler::compile(simple.getValue());
                } else if (simple == "vfacing"){
                    controller.verticalFlip = Compiler::compile(simple.getValue());
                } else if (simple == "bindtime"){
                    controller.bindTime = Compiler::compile(simple.getValue());
                } else if (simple == "velocity" || simple == "vel"){
                    readValues(simple.getValue(), controller.velocityX, controller.velocityY);
                } else if (simple == "accel"){
                    readValues(simple.getValue(), controller.accelerationX, controller.accelerationY);
                } else if (simple == "random"){
                    readValues(simple.getValue(), controller.randomX, controller.randomY);
                } else if (simple == "removetime"){
                    controller.removeTime = Compiler::compile(simple.getValue());
                } else if (simple == "supermove"){
                    controller.superMove = Compiler::compile(simple.getValue());
                } else if (simple == "supermovetime"){
                    controller.superMoveTime = Compiler::compile(simple.getValue());
                } else if (simple == "pausemovetime"){
                    /* TODO */
                } else if (simple == "scale"){
                    readValues(simple.getValue(), controller.scaleX, controller.scaleY);
                } else if (simple == "sprpriority"){
                    controller.spritePriority = Compiler::compile(simple.getValue());
                } else if (simple == "ontop"){
                    /* TODO */
                } else if (simple == "shadow"){
                    /* TODO */
                } else if (simple == "ownpal"){
                    controller.ownPalette = Compiler::compile(simple.getValue());
                } else if (simple == "removeongethit"){
                    controller.removeOnHit = Compiler::compile(simple.getValue());
                } else if (simple == "trans"){
                    /* TODO */
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        int facingLeft = guy.getFacing() == FacingLeft ? -1 : 1;
        FullEnvironment env(stage, guy);

        double randomX = evaluateNumber(this->randomX, env, 0);
        double randomY = evaluateNumber(this->randomX, env, 0);

#define evaluateNumber(value, default_) (value != NULL ? value->evaluate(env).toNumber() : default_)
        int animation_value = (int) evaluateNumber(this->animation, -1);
        int id_value = (int) evaluateNumber(id, -1);
        double posX_value = evaluateNumber(posX, 0) + PaintownUtil::rnd(randomX) - randomX / 2;
        double posY_value = evaluateNumber(posY, 0) + PaintownUtil::rnd(randomY) - randomY / 2;
        double velocityX_value = evaluateNumber(velocityX, 0);
        double velocityY_value = evaluateNumber(velocityY, 0);
        double accelerationX_value = evaluateNumber(accelerationX, 0);
        double accelerationY_value = evaluateNumber(accelerationY, 0);
        int removeTime_value = (int) evaluateNumber(removeTime, -2);
        int bindTime_value = (int) evaluateNumber(bindTime, -2);
        int spritePriority_value = (int) evaluateNumber(spritePriority, 0);
#undef evaluateNumber

        int facing = evaluateNumber(this->facing, env, 1);
        double scaleX = evaluateNumber(this->scaleX, env, 1);
        double scaleY = evaluateNumber(this->scaleY, env, 1);
        bool removeOnHit = evaluateBool(this->removeOnHit, env, false);
        
        bool superMove = evaluateBool(this->superMove, env, false);
        int superMoveTime = evaluateBool(this->superMoveTime, env, 0);

        bool horizontalFlip = computeFacing(facing, guy, stage, positionType, bindTime_value);
        bool verticalFlip = (int) evaluateNumber(this->verticalFlip, env, 1) == -1;

        bool ownPalette = evaluateBool(this->ownPalette, env, false);

        PaintownUtil::ReferenceCount<Animation> animation;
        if (ownAnimation){
            animation = guy.getAnimation(animation_value);
        } else {
            animation = stage.getFightAnimation(animation_value);
        }

        if (animation == NULL){
            ostringstream out;
            out << "Cannot find animation for " << animation_value << endl;
            throw MugenException(out.str(), __FILE__, __LINE__);
        }

        double x = 0;
        double y = 0;

        /* If bindtime is 0 and postype is p1 (either by default or specified)
         * then the explod should be created relative to screen coordinates 0,
         * 0. If the player is facing right then positive x means a positive x
         * for the explod, if the player is facing left then a positive x means
         * a negative x for the explod.
         */

        if (bindTime_value == 0){
            computePositionBind0(posX_value, posY_value, &guy, stage, positionType, horizontalFlip, x, y);
        } else {
            computePosition(posX_value, posY_value, &guy, stage, positionType, horizontalFlip, x, y);
        }

        /* FIXME: handle rest of the explod parameters
         */
        ExplodeEffect * effect = new ExplodeEffect(&guy, stage, animation, id_value, x, y, velocityX_value, velocityY_value, accelerationX_value, accelerationY_value, removeTime_value, bindTime_value, positionType, posX_value, posY_value, scaleX, scaleY, spritePriority_value, superMove, superMoveTime, horizontalFlip, verticalFlip, ownPalette, removeOnHit);
        stage.addEffect(effect);
    }

    StateController * deepCopy() const {
        return new ControllerExplod(*this);
    }
};

class ControllerGameMakeAnim: public StateController {
public:
    ControllerGameMakeAnim(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        parse(section);
    }

    ControllerGameMakeAnim(const ControllerGameMakeAnim & you):
    StateController(you),
    value(copy(you.value)),
    under(copy(you.under)),
    posX(copy(you.posX)),
    posY(copy(you.posY)),
    random(copy(you.random)){
    }

    Value value;
    Value under;
    Value posX, posY;
    Value random;

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerGameMakeAnim & controller):
            controller(controller){
            }

            ControllerGameMakeAnim & controller;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "value"){
                    controller.value = Compiler::compile(simple.getValue());
                } else if (simple == "under"){
                    controller.under = Compiler::compile(simple.getValue());
                } else if (simple == "pos"){
                    const Ast::Value * x;
                    const Ast::Value * y;
                    simple.view() >> x >> y;
                    controller.posX = Compiler::compile(x);
                    controller.posY = Compiler::compile(y);
                } else if (simple == "random"){
                    controller.random = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment environment(stage, guy);
        int animation_value = evaluateNumber(value, environment, 0);
        int x = (int)(evaluateNumber(posX, environment, 0) + guy.getX());
        int y = (int)(evaluateNumber(posY, environment, 0) + guy.getRY());

        int random = evaluateNumber(this->random, environment, 0);
        x += PaintownUtil::rnd(random) - random / 2;
        y += PaintownUtil::rnd(random) - random / 2;

        PaintownUtil::ReferenceCount<Animation> animation;
        animation = stage.getFightAnimation(animation_value);
        if (animation == NULL){
            ostringstream out;
            out << "Cannot find animation for " << animation_value << endl;
            throw MugenException(out.str(), __FILE__, __LINE__);
        }

        class GameAnimation: public Effect {
        public:
            GameAnimation(PaintownUtil::ReferenceCount<Animation> animation, int x, int y):
            /* FIXME: sprite priority */
            Effect(NULL, animation, -1, x, y, 1, 1, 0){
            }
        };

        stage.addEffect(new GameAnimation(animation, x, y));
    }

    StateController * deepCopy() const {
        return new ControllerGameMakeAnim(*this);
    }
};

struct HitByAttributes{
    HitByAttributes():
        slot(-1),
        standing(false),
        crouching(false),
        aerial(false){
        }

    int slot;
    bool standing;
    bool crouching;
    bool aerial;

    vector<AttackType::Attribute> attributes;
};

static HitByAttributes parseHitByAttributes(const Ast::AttributeSimple & simple){
    HitByAttributes hit;

    if (simple == "value"){
        hit.slot = 0;
    } else if (simple == "value2"){
        hit.slot = 1;
    }
    string type;
    vector<string> moreTypes;
    if (simple.getValue() != NULL && ! simple.getValue()->hasMultiple()){
        string type;
        simple.view() >> type;
        type = PaintownUtil::lowerCaseAll(type);
    } else {
        Ast::View view = simple.view();
        view >> type;
        try{
            while (true){
                string what;
                view >> what;
                what = PaintownUtil::lowerCaseAll(what);
                moreTypes.push_back(what);
            }
        } catch (const Ast::Exception & e){
        }
    }

    if (type.find('s') != string::npos){
        hit.standing = true;
    }

    if (type.find('c') != string::npos){
        hit.crouching = true;
    }

    if (type.find('a') != string::npos){
        hit.aerial = true;
    }

    map<string, AttackType::Attribute> attributes;
    attributes["na"] = AttackType::NormalAttack;
    attributes["nt"] = AttackType::NormalThrow;
    attributes["np"] = AttackType::NormalProjectile;
    attributes["sa"] = AttackType::SpecialAttack;
    attributes["st"] = AttackType::SpecialThrow;
    attributes["sp"] = AttackType::SpecialProjectile;
    attributes["ha"] = AttackType::HyperAttack;
    attributes["ht"] = AttackType::HyperThrow;
    attributes["hp"] = AttackType::HyperProjectile;

    for (vector<string>::iterator it = moreTypes.begin(); it != moreTypes.end(); it++){
        string what = *it;
        /* A = all */
        if (what[0] == 'a'){
            switch (what[1]){
                case 'a': {
                    hit.attributes.push_back(AttackType::NormalAttack);
                    hit.attributes.push_back(AttackType::SpecialAttack);
                    hit.attributes.push_back(AttackType::HyperAttack);
                    break;
                }
                case 'p': {
                    hit.attributes.push_back(AttackType::NormalProjectile);
                    hit.attributes.push_back(AttackType::SpecialProjectile);
                    hit.attributes.push_back(AttackType::HyperProjectile);
                    break;
                }
                case 't': {
                    hit.attributes.push_back(AttackType::NormalThrow);
                    hit.attributes.push_back(AttackType::SpecialThrow);
                    hit.attributes.push_back(AttackType::HyperThrow);
                    break;
                }
            }
        } else {
            if (attributes.find(what) != attributes.end()){
                hit.attributes.push_back(attributes[what]);
            }
        }
    }

    return hit;
}

class ControllerHitBy: public StateController {
public:
    ControllerHitBy(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        parse(section);
    }

    ControllerHitBy(const ControllerHitBy & you):
    StateController(you),
    time(copy(you.time)),
    attributes(you.attributes){
    }

    Value time;

    HitByAttributes attributes;

    virtual ~ControllerHitBy(){
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerHitBy & controller):
            controller(controller){
            }

            ControllerHitBy & controller;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "time"){
                    controller.time = Compiler::compile(simple.getValue());
                } else if (simple == "value" || simple == "value2"){
                    controller.attributes = parseHitByAttributes(simple);
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        if (attributes.slot != -1){
            guy.setHitByOverride(attributes.slot, (int) evaluateNumber(time, FullEnvironment(stage, guy), 1), attributes.standing, attributes.crouching, attributes.aerial, attributes.attributes);
        }
    }

    StateController * deepCopy() const {
        return new ControllerHitBy(*this);
    }
};

class ControllerNotHitBy: public StateController {
public:
    ControllerNotHitBy(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        parse(section);
    }

    ControllerNotHitBy(const ControllerNotHitBy & you):
    StateController(you),
    time(copy(you.time)),
    attributes(you.attributes){
    }

    Value time;

    HitByAttributes attributes;

    virtual ~ControllerNotHitBy(){
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerNotHitBy & controller):
            controller(controller){
            }

            ControllerNotHitBy & controller;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "time"){
                    controller.time = Compiler::compile(simple.getValue());
                } else if (simple == "value" || simple == "value2"){
                    controller.attributes = parseHitByAttributes(simple);
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    bool inSet(const vector<AttackType::Attribute> & set, AttackType::Attribute what) const {
        for (vector<AttackType::Attribute>::const_iterator it = set.begin(); it != set.end(); it++){
            if (*it == what){
                return true;
            }
        }

        return false;
    }

    vector<AttackType::Attribute> difference(const vector<AttackType::Attribute> & all, const vector<AttackType::Attribute> & set) const {
        vector<AttackType::Attribute> result;
        for (vector<AttackType::Attribute>::const_iterator it = all.begin(); it != all.end(); it++){
            if (!inSet(set, *it)){
                result.push_back(*it);
            }
        }
        return result;
    }

    vector<AttackType::Attribute> allAttributes() const {
        vector<AttackType::Attribute> all;
        all.push_back(AttackType::NormalAttack);
        all.push_back(AttackType::NormalThrow);
        all.push_back(AttackType::NormalProjectile);
        all.push_back(AttackType::SpecialAttack);
        all.push_back(AttackType::SpecialThrow);
        all.push_back(AttackType::SpecialProjectile);
        all.push_back(AttackType::HyperAttack);
        all.push_back(AttackType::HyperThrow);
        all.push_back(AttackType::HyperProjectile);
        return all;
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        if (attributes.slot != -1){
            vector<AttackType::Attribute> notAttributes = difference(allAttributes(), attributes.attributes);
            guy.setHitByOverride(attributes.slot, (int) evaluateNumber(time, FullEnvironment(stage, guy), 1), attributes.standing, attributes.crouching, attributes.aerial, notAttributes);
        }
    }

    StateController * deepCopy() const {
        return new ControllerNotHitBy(*this);
    }
};

class ControllerEnvShake: public StateController {
public:
    ControllerEnvShake(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        parse(section);
    }

    ControllerEnvShake(const ControllerEnvShake & you):
    StateController(you),
    time(copy(you.time)),
    frequency(copy(you.frequency)),
    amplitude(copy(you.amplitude)),
    phase(copy(you.phase)){
    }

    Value time;
    Value frequency;
    Value amplitude;
    Value phase;

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerEnvShake & controller):
                controller(controller){
                }

            ControllerEnvShake & controller;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "time"){
                    controller.time = Compiler::compile(simple.getValue());
                } else if (simple == "freq"){
                    controller.frequency = Compiler::compile(simple.getValue());
                } else if (simple == "ampl"){
                    controller.amplitude = Compiler::compile(simple.getValue());
                } else if (simple == "phase"){
                    controller.phase = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment environment(stage, guy);
        /* FIXME: EnvShake is supposed to only shake in the vertical direction.
         * Also handle frequency, amplitude, and phase here
         */
        int quake = evaluateNumber(time, environment, 0);
        stage.Quake(quake);
    }

    StateController * deepCopy() const {
        return new ControllerEnvShake(*this);
    }
};

class ControllerTargetBind: public StateController {
public:
    ControllerTargetBind(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        parse(section);
    }

    ControllerTargetBind(const ControllerTargetBind & you):
    StateController(you),
    time(copy(you.time)),
    id(copy(you.id)),
    posX(copy(you.posX)),
    posY(copy(you.posY)){
    }

    Value time;
    Value id;
    Value posX, posY;

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerTargetBind & controller):
            controller(controller){
            }

            ControllerTargetBind & controller;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "time"){
                    controller.time = Compiler::compile(simple.getValue());
                } else if (simple == "id"){
                    controller.id = Compiler::compile(simple.getValue());
                } else if (simple == "pos"){
                    const Ast::Value * x;
                    const Ast::Value * y;
                    simple.view() >> x >> y;
                    controller.posX = Compiler::compile(x);
                    controller.posY = Compiler::compile(y);
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment environment(stage, guy, commands);
        vector<Character*> targets = stage.getTargets((int) evaluateNumber(this->id, environment, -1), &guy);
        int time = (int) evaluateNumber(this->time, environment, 1);
        double x = (double) evaluateNumber(this->posX, environment, 0);
        double y = (double) evaluateNumber(this->posY, environment, 0);
        for (vector<Character*>::iterator it = targets.begin(); it != targets.end(); it++){
            Character * target = *it;
            target->bindTo(&guy, time, 0, x, y);
        }
    }

    StateController * deepCopy() const {
        return new ControllerTargetBind(*this);
    }
};

class ControllerTargetPowerAdd: public StateController {
public:
    ControllerTargetPowerAdd(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        parse(section);
    }

    ControllerTargetPowerAdd(const ControllerTargetPowerAdd & you):
    StateController(you),
    id(copy(you.id)),
    value(copy(you.value)){
    }

    Value id;
    Value value;

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerTargetPowerAdd & controller):
            controller(controller){
            }

            ControllerTargetPowerAdd & controller;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "id"){
                    controller.id = Compiler::compile(simple.getValue());
                } else if (simple == "value"){
                    controller.value = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment environment(stage, guy, commands);
        vector<Character*> targets = stage.getTargets((int) evaluateNumber(this->id, environment, -1), &guy);
        int power = (int) evaluateNumber(this->value, FullEnvironment(stage, guy, commands), 0);
        for (vector<Character*>::iterator it = targets.begin(); it != targets.end(); it++){
            Character * target = *it;
            target->addPower(power);
        }
    }

    StateController * deepCopy() const {
        return new ControllerTargetPowerAdd(*this);
    }
};

class ControllerDefenceMulSet: public StateController {
public:
    ControllerDefenceMulSet(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        parse(section);
    }

    ControllerDefenceMulSet(const ControllerDefenceMulSet & you):
    StateController(you),
    defense(copy(you.defense)){
    }

    Value defense;

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(Value & defense):
            defense(defense){
            }

            Value & defense;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "value"){
                    defense = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(defense);
        section->walk(walker);
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        guy.setDefenseMultiplier(evaluateNumber(defense, FullEnvironment(stage, guy, commands), 1));
    }

    StateController * deepCopy() const {
        return new ControllerDefenceMulSet(*this);
    }
};

class ControllerVarRandom: public StateController {
public:
    ControllerVarRandom(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        parse(section);
    }

    ControllerVarRandom(const ControllerVarRandom & you):
    StateController(you),
    minimum(copy(you.minimum)),
    maximum(copy(you.maximum)),
    index(copy(you.index)){
    }

    Value minimum;
    Value maximum;
    Value index;

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerVarRandom & controller):
            controller(controller){
            }

            ControllerVarRandom & controller;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "v"){
                    controller.index = Compiler::compile(simple.getValue());
                } else if (simple == "range"){
                    if (simple.getValue()->hasMultiple()){
                        const Ast::Value * min;
                        const Ast::Value * max;
                        simple.view() >> min >> max;
                        controller.minimum = Compiler::compile(min);
                        controller.maximum = Compiler::compile(max);
                    } else {
                        controller.minimum = Compiler::compile(0);
                        controller.maximum = Compiler::compile(simple.getValue());
                    }
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
        if (index == NULL){
            throw MugenException("v not specified in the VarRandom controller", __FILE__, __LINE__);
        }
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment environment(stage, guy, commands);
        int index = (int) evaluateNumber(this->index, environment, 0);
        int minimum = (int) evaluateNumber(this->minimum, environment, 0);
        int maximum = (int) evaluateNumber(this->maximum, environment, 0);
        guy.setVariable(index, PaintownUtil::rnd(minimum, maximum));
    }

    StateController * deepCopy() const {
        return new ControllerVarRandom(*this);
    }
};

class ControllerHitFallDamage: public StateController {
public:
    ControllerHitFallDamage(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
    }

    ControllerHitFallDamage(const ControllerHitFallDamage & you):
    StateController(you){
    }

    bool isFalling(const Character & guy) const {
        /* FIXME */
        return true;
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        if (isFalling(guy)){
            guy.takeDamage(stage, stage.getEnemy(&guy), guy.getHitState().fall.damage);
        }
    }

    StateController * deepCopy() const {
        return new ControllerHitFallDamage(*this);
    }
};

class ControllerPosFreeze: public StateController {
public:
    ControllerPosFreeze(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
    }

    ControllerPosFreeze(const ControllerPosFreeze & you):
    StateController(you){
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        guy.doFreeze();
    }

    StateController * deepCopy() const {
        return new ControllerPosFreeze(*this);
    }
};

class ControllerHitFallVel: public StateController {
public:
    ControllerHitFallVel(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
    }

    ControllerHitFallVel(const ControllerHitFallVel & you):
    StateController(you){
    }

    bool isFalling(const Character & guy) const {
        /* FIXME */
        return true;
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        if (isFalling(guy)){
            if (guy.getHitState().fall.changeXVelocity){
                guy.setXVelocity(guy.getHitState().fall.xVelocity);
            }
            guy.setYVelocity(guy.getHitState().fall.yVelocity);
        }
    }

    StateController * deepCopy() const {
        return new ControllerHitFallVel(*this);
    }
};

class ControllerHitFallSet: public StateController {
public:
    ControllerHitFallSet(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        parse(section);
    }

    ControllerHitFallSet(const ControllerHitFallSet & you):
    StateController(you),
    value(copy(you.value)),
    xVelocity(copy(you.xVelocity)),
    yVelocity(copy(you.yVelocity)){
    }

    Value value;
    Value xVelocity;
    Value yVelocity;

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerHitFallSet & controller):
            controller(controller){
            }

            ControllerHitFallSet & controller;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "value"){
                    controller.value = Compiler::compile(simple.getValue());
                } else if (simple == "xvel"){
                    controller.xVelocity = Compiler::compile(simple.getValue());
                } else if (simple == "yvel"){
                    controller.yVelocity = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment environment(stage, guy, commands);
        int set = evaluateNumber(value, environment, -1);
        switch (set){
            case -1 : break;
            case 0 : guy.getHitState().fall.fall = false; break;
            case 1 : guy.getHitState().fall.fall = true; break;
            default : break;
        }

        if (xVelocity != NULL){
            guy.getHitState().fall.xVelocity = evaluateNumber(xVelocity, environment, 0);
        }

        if (yVelocity != NULL){
            guy.getHitState().fall.yVelocity = evaluateNumber(yVelocity, environment, 0);
        }
    }

    StateController * deepCopy() const {
        return new ControllerHitFallSet(*this);
    }
};

/* 90% */
class ControllerSelfState: public ControllerChangeState {
public:
    ControllerSelfState(Ast::Section * section, const string & name, int state):
    ControllerChangeState(section, name, state){
    }

    ControllerSelfState(const ControllerSelfState & you):
    ControllerChangeState(you){
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        if (control != NULL){
            guy.setControl(control->evaluate(FullEnvironment(stage, guy)).toBool());
        }

        RuntimeValue result = value->evaluate(FullEnvironment(stage, guy));
        if (result.isDouble()){
            int value = (int) result.getDoubleValue();
            guy.changeOwnState(stage, value, commands);
        }
    }

    StateController * deepCopy() const {
        return new ControllerSelfState(*this);
    }
};

/* 100% */
class ControllerPalFX: public StateController {
public:
    ControllerPalFX(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        parse(section);
    }

    ControllerPalFX(const ControllerPalFX & you):
    StateController(you),
    time(copy(you.time)),
    addRed(copy(you.addRed)),
    addGreen(copy(you.addGreen)),
    addBlue(copy(you.addBlue)),
    multiplyRed(copy(you.multiplyRed)),
    multiplyGreen(copy(you.multiplyGreen)),
    multiplyBlue(copy(you.multiplyBlue)),
    sinRed(copy(you.sinRed)),
    sinGreen(copy(you.sinGreen)),
    sinBlue(copy(you.sinBlue)),
    period(copy(you.period)),
    invert(copy(you.invert)),
    color(copy(you.color)){
    }

    Value time;
    Value addRed;
    Value addGreen;
    Value addBlue;

    Value multiplyRed;
    Value multiplyGreen;
    Value multiplyBlue;

    Value sinRed;
    Value sinGreen;
    Value sinBlue;
    Value period;

    Value invert;
    Value color;

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerPalFX & controller):
            controller(controller){
            }

            ControllerPalFX & controller;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "time"){
                    const Ast::Value * time;
                    simple.view() >> time;
                    controller.time = Compiler::compile(time);
                } else if (simple == "add"){
                    const Ast::Value * red;
                    const Ast::Value * green;
                    const Ast::Value * blue;
                    simple.view() >> red >> green >> blue;
                    controller.addRed = Compiler::compile(red);
                    controller.addGreen = Compiler::compile(green);
                    controller.addBlue = Compiler::compile(blue);

                } else if (simple == "mul"){
                    const Ast::Value * red;
                    const Ast::Value * green;
                    const Ast::Value * blue;
                    simple.view() >> red >> green >> blue;
                    controller.multiplyRed = Compiler::compile(red);
                    controller.multiplyGreen = Compiler::compile(green);
                    controller.multiplyBlue = Compiler::compile(blue);
                } else if (simple == "sinadd"){
                    readValues(simple.getValue(), controller.sinRed, controller.sinGreen, controller.sinBlue, controller.period);
                } else if (simple == "invertall"){
                    const Ast::Value * invert;
                    simple.view() >> invert;
                    controller.invert = Compiler::compile(invert);
                } else if (simple == "color"){
                    const Ast::Value * color;
                    simple.view() >> color;
                    controller.color = Compiler::compile(color);
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);

        /* TODO */
        /*
         * time = duration (int)
         * Specifies the number of ticks that the palette effects should last. Specify -1 to have the palette effects last indefinitely. Specify 0 to stop any ongoing palette effects.
         * add = add_r, add_g, add_b (int, int, int)
         * See below.
         * mul = mul_r, mul_g, mul_b (int, int, int)
         * Each add component is added to the appropriate component of the player's palette, and the result is multiplied by the appropriate mul component divided by 256. For instance, if pal_r is the red component of the character's original palette, then the new red component is (pal_r + add_r) * mul_r / 256. The values for mul must be >= 0. The defaults for these parameters are for no change, i.e. add = 0,0,0 and mul = 256,256,256.
         * sinadd = ampl_r, ampl_g, ampl_b, period (int, int, int, int)
         * Creates an additional sine-wave palette addition effect. Period specifies the period of the sine wave in game ticks, and the amplitude parameters control the amplitude of the sine wave for the respective components. For instance, if t represents the number of ticks elapsed since the activation of the PalFX controller, and pal_r is the red component of the character's original palette, then the red component of the character's palette at time t is (pal_r + add_r + ampl_r * sin(2 * pi * t / period)) * mul_r / 256.
         * invertall = bvalue (bool)
         * If bvalue is non-zero, then the colors in the palette will be inverted, creating a "film negative" effect. Color inversion is applied before effects of add and mul. bvalue defaults to 0.
         * color = value (int)
         * This affects the color level of the palette. If value is 0, the palette will be greyscale. If value is 256, there is no change in palette. Values in between will have an intermediate effect. This parameter's effects are applied before invertall, add and mul. Values must be in range 0 to 256. Default value is 256.
         */
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment environment(stage, guy, commands);
        int addRed = (int) evaluateNumber(this->addRed, environment, 0);
        int addGreen = (int) evaluateNumber(this->addGreen, environment, 0);
        int addBlue = (int) evaluateNumber(this->addBlue, environment, 0);
        
        int multiplyRed = (int) evaluateNumber(this->multiplyRed, environment, 256);
        int multiplyGreen = (int) evaluateNumber(this->multiplyGreen, environment, 256);
        int multiplyBlue = (int) evaluateNumber(this->multiplyBlue, environment, 256);
        
        int sinRed = (int) evaluateNumber(this->sinRed, environment, 0);
        int sinGreen = (int) evaluateNumber(this->sinGreen, environment, 0);
        int sinBlue = (int) evaluateNumber(this->sinBlue, environment, 0);
        
        int period = (int) evaluateNumber(this->period, environment, 0);

        int time = (int) evaluateNumber(this->time, environment, 0);
        
        int color = (int) evaluateNumber(this->color, environment, 256);

        bool invert = evaluateBool(this->invert, environment, false);

        guy.setPaletteEffects(time, addRed, addGreen, addBlue,
                              multiplyRed, multiplyGreen, multiplyBlue,
                              sinRed, sinGreen, sinBlue, period, invert, color);
    }

    StateController * deepCopy() const {
        return new ControllerPalFX(*this);
    }
};

class ControllerBGPalFX: public ControllerPalFX {
public:
    ControllerBGPalFX(Ast::Section * section, const string & name, int state):
    ControllerPalFX(section, name, state){
    }

    ControllerBGPalFX(const ControllerBGPalFX & you):
    ControllerPalFX(you){
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment environment(stage, guy, commands);
        int addRed = (int) evaluateNumber(this->addRed, environment, 0);
        int addGreen = (int) evaluateNumber(this->addGreen, environment, 0);
        int addBlue = (int) evaluateNumber(this->addBlue, environment, 0);

        int multiplyRed = (int) evaluateNumber(this->multiplyRed, environment, 256);
        int multiplyGreen = (int) evaluateNumber(this->multiplyGreen, environment, 256);
        int multiplyBlue = (int) evaluateNumber(this->multiplyBlue, environment, 256);

        int sinRed = (int) evaluateNumber(this->sinRed, environment, 0);
        int sinGreen = (int) evaluateNumber(this->sinGreen, environment, 0);
        int sinBlue = (int) evaluateNumber(this->sinBlue, environment, 0);
        
        int period = (int) evaluateNumber(this->period, environment, 0);

        int time = (int) evaluateNumber(this->time, environment, 0);

        int color = (int) evaluateNumber(this->color, environment, 256);
        bool invert = evaluateBool(this->invert, environment, false);

        stage.setPaletteEffects(time, addRed, addGreen, addBlue,
                                multiplyRed, multiplyGreen, multiplyBlue,
                                sinRed, sinGreen, sinBlue, period, invert, color);
    }

    StateController * deepCopy() const {
        return new ControllerBGPalFX(*this);
    }
};

class ControllerVarRangeSet: public StateController {
public:
    ControllerVarRangeSet(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        parse(section);
    }

    ControllerVarRangeSet(const ControllerVarRangeSet & you):
    StateController(you),
    value(copy(you.value)),
    floatValue(copy(you.floatValue)),
    start(copy(you.start)),
    end(copy(you.end)){
    }

    Value value;
    Value floatValue;
    Value start;
    Value end;

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerVarRangeSet & controller):
            controller(controller){
            }

            ControllerVarRangeSet & controller;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "value"){
                    controller.value = Compiler::compile(simple.getValue());
                } else if (simple == "fvalue"){
                    controller.floatValue = Compiler::compile(simple.getValue());
                } else if (simple == "first"){
                    controller.start = Compiler::compile(simple.getValue());
                } else if (simple == "last"){
                    controller.end = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment environment(stage, guy, commands);
        if (value != NULL){
            int minimum = (int) evaluateNumber(start, environment, 0);
            int maximum = (int) evaluateNumber(end, environment, 59);
            int value = (int) evaluateNumber(this->value, environment, 0);
            for (int i = minimum; i < maximum; i++){
                guy.setVariable(i, value);
            }
        }

        if (floatValue != NULL){
            int minimum = (int) evaluateNumber(start, environment, 0);
            int maximum = (int) evaluateNumber(end, environment, 39);
            double value = evaluateNumber(this->value, environment, 0);
            for (int i = minimum; i < maximum; i++){
                guy.setFloatVariable(i, value);
            }
        }
    }

    StateController * deepCopy() const {
        return new ControllerVarRangeSet(*this);
    }
};

class ControllerSprPriority: public StateController {
public:
    ControllerSprPriority(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        parse(section);
    }

    ControllerSprPriority(const ControllerSprPriority & you):
    StateController(you),
    value(copy(you.value)){
    }

    Value value;

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerSprPriority & controller):
            controller(controller){
            }

            ControllerSprPriority & controller;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "value"){
                    controller.value = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        int priority = (int) evaluateNumber(value, FullEnvironment(stage, guy, commands), 0);
        guy.setSpritePriority(priority);
    }

    StateController * deepCopy() const {
        return new ControllerSprPriority(*this);
    }
};

class ControllerTargetFacing: public StateController {
public:
    ControllerTargetFacing(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        parse(section);
    }

    ControllerTargetFacing(const ControllerTargetFacing & you):
    StateController(you),
    value(copy(you.value)),
    id(copy(you.id)){
    }

    Value value;
    Value id;

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerTargetFacing & controller):
            controller(controller){
            }

            ControllerTargetFacing & controller;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "value"){
                    controller.value = Compiler::compile(simple.getValue());
                } else if (simple == "id"){
                    controller.id = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
        if (value == NULL){
            throw MugenException("the `value' attribute must be provided for TargetFacing", __FILE__, __LINE__);
        }
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment environment(stage, guy, commands);
        bool same = (int) evaluateNumber(value, environment, 1) > 0;
        vector<Character*> targets = stage.getTargets((int) evaluateNumber(id, environment, -1), &guy);
        for (vector<Character*>::iterator it = targets.begin(); it != targets.end(); it++){
            Character * target = *it;
            if (same){
                target->setFacing(guy.getFacing());
            } else {
                target->setFacing(guy.getOppositeFacing());
            }
        }
    }

    StateController * deepCopy() const {
        return new ControllerTargetFacing(*this);
    }
};

class ControllerTargetLifeAdd: public StateController {
public:
    ControllerTargetLifeAdd(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        parse(section);
    }

    ControllerTargetLifeAdd(const ControllerTargetLifeAdd & you):
    StateController(you),
    value(copy(you.value)),
    id(copy(you.id)),
    kill(copy(you.kill)),
    absolute(copy(you.absolute)){
    }

    Value value;
    Value id;
    Value kill;
    Value absolute;

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerTargetLifeAdd & controller):
            controller(controller){
            }

            ControllerTargetLifeAdd & controller;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "value"){
                    controller.value = Compiler::compile(simple.getValue());
                } else if (simple == "id"){
                    controller.id = Compiler::compile(simple.getValue());
                } else if (simple == "kill"){
                    controller.kill = Compiler::compile(simple.getValue());
                } else if (simple == "absolute"){
                    controller.absolute = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
        if (value == NULL){
            throw MugenException("the `value' attribute must be specified for the TargetAddLife controller", __FILE__, __LINE__);
        }
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment environment(stage, guy, commands);
        double amount = evaluateNumber(this->value, environment, 0);
        int id = evaluateNumber(this->id, environment, -1);
        bool kill = evaluateBool(this->kill, environment, true);
        bool absolute = evaluateBool(this->absolute, environment, false);
        vector<Character*> targets = stage.getTargets(id, &guy);
        for (vector<Character*>::iterator it = targets.begin(); it != targets.end(); it++){
            Character * target = *it;
            target->takeDamage(stage, &guy, -amount, kill, absolute);
        }
    }

    StateController * deepCopy() const {
        return new ControllerTargetLifeAdd(*this);
    }
};

class ControllerTargetState: public StateController {
public:
    ControllerTargetState(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        parse(section);
    }

    ControllerTargetState(const ControllerTargetState & you):
    StateController(you),
    value(copy(you.value)),
    id(copy(you.id)){
    }

    Value value;
    Value id;

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerTargetState & controller):
            controller(controller){
            }

            ControllerTargetState & controller;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "value"){
                    controller.value = Compiler::compile(simple.getValue());
                } else if (simple == "id"){
                    controller.id = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment environment(stage, guy, commands);
        int state = (int) evaluateNumber(value, environment, 0);
        int id = (int) evaluateNumber(this->id, environment, -1);
        vector<Character*> targets = stage.getTargets(id, &guy);
        for (vector<Character*>::iterator it = targets.begin(); it != targets.end(); it++){
            Character * target = *it;
            target->changeState(stage, state);
        }
    }

    StateController * deepCopy() const {
        return new ControllerTargetState(*this);
    }
};

class ControllerChangeAnim2: public ControllerChangeAnim {
public:
    ControllerChangeAnim2(Ast::Section * section, const string & name, int state):
    ControllerChangeAnim(section, name, state){
    }

    ControllerChangeAnim2(const ControllerChangeAnim2 & you):
    ControllerChangeAnim(you){
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        int animation = (int) evaluateNumber(value, FullEnvironment(stage, guy, commands), 0);
        PaintownUtil::ReferenceCount<Animation> show = stage.getEnemy(&guy)->getAnimation(animation);
        if (show != NULL){
            guy.setForeignAnimation(show, animation);
        } else {
            ostringstream out;
            out << "No animation found for " << animation;
            throw MugenNormalRuntimeException(out.str());
        }
    }

    StateController * deepCopy() const {
        return new ControllerChangeAnim2(*this);
    }
};

class ControllerScreenBound: public StateController {
public:
    ControllerScreenBound(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        parse(section);
    }

    ControllerScreenBound(const ControllerScreenBound & you):
    StateController(you),
    value(copy(you.value)),
    moveCameraX(copy(you.moveCameraX)),
    moveCameraY(copy(you.moveCameraY)){
    }

    Value value;
    Value moveCameraX;
    Value moveCameraY;

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerScreenBound & controller):
            controller(controller){
            }

            ControllerScreenBound & controller;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "value"){
                    controller.value = Compiler::compile(simple.getValue());
                } else if (simple == "movecamera"){
                    const Ast::Value * x;
                    const Ast::Value * y;
                    simple.view() >> x >> y;
                    controller.moveCameraX = Compiler::compile(x);
                    controller.moveCameraY = Compiler::compile(y);
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment environment(stage, guy, commands);
        bool unbound = evaluateBool(value, environment, false) == false;
        bool cameraX = evaluateBool(moveCameraX, environment, false);
        bool cameraY = evaluateBool(moveCameraY, environment, false);
        stage.enableScreenBound(&guy, unbound, cameraX, cameraY);
    }

    StateController * deepCopy() const {
        return new ControllerScreenBound(*this);
    }
};
        
/* 100% */
class ControllerPowerAdd: public StateController {
public:
    ControllerPowerAdd(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        parse(section);
    }

    ControllerPowerAdd(const ControllerPowerAdd & you):
    StateController(you),
    value(copy(you.value)){
    }

    Value value;

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(Value & value):
            value(value){
            }

            Value & value;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "value"){
                    value = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(value);
        section->walk(walker);

        if (value == NULL){
            throw MugenException("`value' is a required attribute of PowerAdd", __FILE__, __LINE__);
        }
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        int power = (int) evaluateNumber(this->value, FullEnvironment(stage, guy, commands), 0);
        guy.addPower(power);
    }

    StateController * deepCopy() const {
        return new ControllerPowerAdd(*this);
    }
};

/* 100% */
class ControllerEnvColor: public StateController {
public:
    ControllerEnvColor(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        parse(section);
    }

    ControllerEnvColor(const ControllerEnvColor & you):
    StateController(you),
    red(copy(you.red)),
    green(copy(you.green)),
    blue(copy(you.blue)),
    time(copy(you.time)),
    under(copy(you.under)){
    }

    Value red, green, blue;
    Value time;
    Value under;

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerEnvColor & controller):
            controller(controller){
            }

            ControllerEnvColor & controller;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "value"){
                    const Ast::Value * red;
                    const Ast::Value * green;
                    const Ast::Value * blue;
                    simple.view() >> red >> green >> blue;
                    controller.red = Compiler::compile(red);
                    controller.green = Compiler::compile(green);
                    controller.blue = Compiler::compile(blue);
                } else if (simple == "time"){
                    controller.time = Compiler::compile(simple.getValue());
                } else if (simple == "under"){
                    controller.under = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment environment(stage, guy, commands);
        int red = PaintownUtil::clamp((int) evaluateNumber(this->red, environment, 0), 0, 255);
        int green = PaintownUtil::clamp((int) evaluateNumber(this->green, environment, 0), 0, 255);
        int blue = PaintownUtil::clamp((int) evaluateNumber(this->blue, environment, 0), 0, 255);
        int time = (int) evaluateNumber(this->time, environment, 1);
        bool under = evaluateBool(this->under, environment, false);
        stage.setEnvironmentColor(Graphics::makeColor(red, green, blue), time, under);
    }

    StateController * deepCopy() const {
        return new ControllerEnvColor(*this);
    }
};

class ControllerDestroySelf: public StateController {
public:
    ControllerDestroySelf(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
    }

    ControllerDestroySelf(const ControllerDestroySelf & you):
    StateController(you){
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        if (guy.isHelper()){
            stage.removeHelper(&guy);
        }
    }

    StateController * deepCopy() const {
        return new ControllerDestroySelf(*this);
    }
};

class ControllerLifeAdd: public StateController {
public:
    ControllerLifeAdd(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        parse(section);
    }

    ControllerLifeAdd(const ControllerLifeAdd & you):
    StateController(you),
    value(copy(you.value)),
    kill(copy(you.kill)),
    absolute(copy(you.absolute)){
    }

    Value value;
    Value kill;
    Value absolute;

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerLifeAdd & controller):
            controller(controller){
            }

            ControllerLifeAdd & controller;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "value"){
                    controller.value = Compiler::compile(simple.getValue());
                } else if (simple == "kill"){
                    controller.kill = Compiler::compile(simple.getValue());
                } else if (simple == "absolute"){
                    controller.absolute = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment environment(stage, guy, commands);
        int value = (int) evaluateNumber(this->value, environment, 0);
        bool kill = evaluateBool(this->kill, environment, true);
        bool absolute = evaluateBool(this->absolute, environment, false);
        /* Negate value because its LifeAdd, not LifeSubtract */
        guy.takeDamage(stage, &guy, -value, kill, absolute);
    }

    StateController * deepCopy() const {
        return new ControllerLifeAdd(*this);
    }
};

class ControllerLifeSet: public StateController {
public:
    ControllerLifeSet(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        extractValue(value, section);
        // value = extractAttribute(section, "value");
        if (value == NULL){
            throw MugenException("LifeSet controller must specify the `value' attribute", __FILE__, __LINE__);
        }
    }

    ControllerLifeSet(const ControllerLifeSet & you):
    StateController(you),
    value(copy(you.value)){
    }

    Value value;

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        int life = (int) evaluateNumber(value, FullEnvironment(stage, guy, commands), 0);
        guy.setHealth(life);
    }

    StateController * deepCopy() const {
        return new ControllerLifeSet(*this);
    }
};

class ControllerRemoveExplod: public StateController {
public:
    ControllerRemoveExplod(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        id = extractAttribute(section, "id");
    }

    ControllerRemoveExplod(const ControllerRemoveExplod & you):
    StateController(you),
    id(copy(you.id)){
    }

    Value id;

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        int id = evaluateNumber(this->id, FullEnvironment(stage, guy, commands), -1);
        stage.removeEffects(&guy, id);
    }

    StateController * deepCopy() const {
        return new ControllerRemoveExplod(*this);
    }
};
        
class ControllerModifyExplod: public ControllerExplod {
public:
    ControllerModifyExplod(Ast::Section * section, const string & name, int state):
    ControllerExplod(section, name, state){
    }

    ControllerModifyExplod(const ControllerModifyExplod & you):
    ControllerExplod(you){
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment environment(stage, guy, commands);
        int id = (int) evaluateNumber(this->id, environment, -1);
        /* this hopefully shouldn't be a dangerous cast because the only effects
         * the character can add is ExplodeEffects
         */
        ExplodeEffect * effect = (ExplodeEffect*) stage.findEffect(&guy, id);
        if (effect == NULL){
            ostringstream out;
            out << "No explode found for id " << id;
            throw MugenNormalRuntimeException(out.str());
        }

        effect->setX((int) evaluateNumber(this->posX, environment, effect->getX()));
        effect->setY((int) evaluateNumber(this->posY, environment, effect->getY()));
        effect->setVelocityX(evaluateNumber(this->velocityX, environment, effect->getVelocityX()));
        effect->setVelocityY(evaluateNumber(this->velocityY, environment, effect->getVelocityY()));
        effect->setAccelerationX(evaluateNumber(this->accelerationX, environment, effect->getAccelerationX()));
        effect->setAccelerationY(evaluateNumber(this->accelerationY, environment, effect->getAccelerationY()));
        effect->setRemoveTime((int) evaluateNumber(this->removeTime, environment, effect->getRemoveTime()));
    }

    StateController * deepCopy() const {
        return new ControllerModifyExplod(*this);
    }
};

class ControllerHelper: public StateController {
public:
    enum PosType{
        Player1,
        Player2,
        Front,
        Back,
        Left,
        Right
    };

    ControllerHelper(Ast::Section * section, const string & name, int state):
    StateController(name, state, section),
    posType(Player1){
        parse(section);
    }

    ControllerHelper(const ControllerHelper & you):
    StateController(you),
    name(you.name),
    id(copy(you.id)),
    posX(copy(you.posX)),
    posY(copy(you.posY)),
    posType(you.posType),
    facing(copy(you.facing)),
    state(copy(you.state)),
    key(copy(you.key)),
    ownPalette(copy(you.ownPalette)),
    superMoveTime(copy(you.superMoveTime)),
    pauseMoveTime(copy(you.pauseMoveTime)),
    xscale(copy(you.xscale)),
    yscale(copy(you.yscale)),
    ground(you.ground),
    air(you.air),
    height(copy(you.height)),
    projectileScale(copy(you.projectileScale)),
    headPosition(copy(you.headPosition)),
    midPosition(copy(you.midPosition)){
    }

    string name;
    Value id;
    Value posX, posY;
    PosType posType;
    Value facing;
    Value state;
    Value key;
    Value ownPalette;
    Value superMoveTime;
    Value pauseMoveTime;
    Value xscale;
    Value yscale;

    struct Ground{
        Ground(){
        }

        Ground(const Ground & you):
        back(copy(you.back)),
        front(copy(you.front)){
        }

        Value back;
        Value front;
    } ground;

    struct Air{
        Air(){
        }

        Air(const Air & you):
        back(copy(you.back)),
        front(copy(you.front)){
        }

        Value back;
        Value front;
    } air;

    Value height;
    Value projectileScale;
    Value headPosition;
    Value midPosition;
    Value shadowOffset;

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerHelper & controller):
            controller(controller){
            }

            ControllerHelper & controller;

            PosType parsePosType(const string & type){
                if (type == "p1"){
                    return Player1;
                } else if (type == "p2"){
                    return Player2;
                } else if (type == "front"){
                    return Front;
                } else if (type == "back"){
                    return Back;
                } else if (type == "left"){
                    return Left;
                } else if (type == "right"){
                    return Right;
                }
                return Player1;
            }

            /* extract an item and compile it */
            Compiler::Value * compile1(const Ast::AttributeSimple & simple){
                const Ast::Value * item;
                simple.view() >> item;
                return Compiler::compile(item);
            }

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "helpertype"){
                    /* helpertype = type_string (string)
                     * This parameter is deprecated; player-type helpers are not supported. If helpertype = normal, then the helper will be allowed to move off the edge of the screen. Furthermore, the camera will not move to try to keep the helper on screen. If helpertype = player, then the helper will be constrained to the screen and will be followed by the camera, just like a normal player. Defaults to normal. If you plan to use a helper for camera manipulation, do not use a player-type helper; instead use the ScreenBound controller in a normal helper with the "movecamera" parameter.
                     */
                    /* TODO */
                } else if (simple == "name"){
                    /* name = "name_string" (string)
                     * Specifies a name for this helper, which must be enclosed in double quotes. If omitted, the name defaults to "<parent>'s helper", where <parent> represents the name of the player creating the helper.
                     */
                    simple.view() >> controller.name;
                } else if (simple == "ID"){
                    /* ID = id_no (int)
                     * Sets an ID number to refer to this helper by. Defaults to 0.
                     */
                    controller.id = compile1(simple);
                } else if (simple == "pos"){
                    /* pos = x_off, y_off (int)
                     * Specifies the x and y offsets to create this helper at. The precise meaning of these parameters is dependent on the postype. Defaults to 0,0.
                     */
                    const Ast::Value * x;
                    const Ast::Value * y;
                    simple.view() >> x >> y;
                    controller.posX = Compiler::compile(x);
                    controller.posY = Compiler::compile(y);
                } else if (simple == "postype"){
                    /* postype = postype_string (string)
                     * postype works nearly the same as in the Explod controller. postype_string specifies the postype -- how to interpret the pos parameters. In all cases, a positive y offset means a downward displacement.
                     *
                     * Valid values for postype_string are the following:
                     * p1
                     * Interprets pos relative to p1's axis. A positive x offset is toward the front of p1. This is the default value for postype.
                     * p2
                     * Interprets pos relative to p2's axis. A positive x offset is toward the front of p2. If p2 does not exist, the helper is created with respect to p1.
                     * front
                     * Interprets xpos relative to the edge of the screen that p1 is facing toward, and ypos relative to p1's axis. A positive x offset is away from the center of the screen, whereas a negative x offset is toward the center.
                     * back
                     * Interprets xpos relative to the edge of the screen that p1 is facing away from, and ypos relative to p1's axis. A positive x offset is toward the center of the screen, whereas a negative x offset is away from the center.
                     * left
                     * Interprets xpos relative to the left edge of the screen, and ypos relative to p1's axis. A positive x offset is toward the right of the screen.
                     * right
                     * Interprets xpos relative to the right edge of the screen, and ypos relative to p1's axis. A positive x offset is toward the left of the screen.
                     */
                    string type;
                    simple.view() >> type;
                    controller.posType = parsePosType(PaintownUtil::lowerCaseAll(type));
                } else if (simple == "facing"){
                     /* facing = facing (int)
                      * If postype is left or right, setting facing to 1 will make the helper face the right, and a value of -1 makes the helper face left. For all other values of postype except p2, if facing is 1, the helper will face the same direction as the player. If facing is -1, the helper will face the opposite direction. In the case of postype = p2, facing has the same effect as above, except it is with respect to p2's facing. Defaults to 1.
                      */
                    controller.facing = compile1(simple);
                } else if (simple == "stateno"){
                  /* stateno = start_state (int)
                   * Determines the state number that the helper starts off in. Defaults to 0.
                   */
                    controller.state = compile1(simple);
                } else if (simple == "keyctrl"){
                    /* keyctrl = ctrl_flag (boolean)
                     * If ctrl_flag is 1, then the helper is able to read command input from the player (e.g., the keyboard or joystick). Also, the helper will inherit its root's State -1. If ctrl_flag is 0, then the helper does not have access to command input, and does not inherit State -1. The default value of ctrl_flag is 0.
                     */
                    controller.key = compile1(simple);
                } else if (simple == "ownpal"){
                    /* ownpal = pal_flag (boolean)
                     * If pal_flag is 0, the helper will inherit its parent's palette. If the parent's palette is temporarily changed (eg. by a PalFX controller), the changes will be reflected in the helper too. If pal_flag is 1, the helper will receive its own working palette, that is independent of its parent's. Defaults to 0.
                     */
                    controller.ownPalette = compile1(simple);
                } else if (simple == "supermovetime"){
                    /* supermovetime = move_time (int)
                     * Specifies the number of ticks that the helper should be unfrozen during a SuperPause. Defaults to 0.
                     */
                    controller.superMoveTime = compile1(simple);
                } else if (simple == "pausemovetime"){
                    /* pausemovetime = move_time (int)
                     * Determines the number of ticks that the helper should be unfrozen during a Pause. Defaults to 0.
                     */
                    controller.pauseMoveTime = compile1(simple);
                } else if (simple == "size.xscale"){
                    controller.xscale = compile1(simple);
                } else if (simple == "size.yscale"){
                    controller.yscale = compile1(simple);
                } else if (simple == "size.ground.back"){
                    controller.ground.back = compile1(simple);
                } else if (simple == "size.ground.front"){
                    controller.ground.front = compile1(simple);
                } else if (simple == "size.air.back"){
                    controller.air.back = compile1(simple);
                } else if (simple == "size.air.front"){
                    controller.air.front = compile1(simple);
                } else if (simple == "size.height"){
                    controller.height = compile1(simple);
                } else if (simple == "size.proj.doscale"){
                    controller.projectileScale = compile1(simple);
                } else if (simple == "size.head.pos"){
                    controller.headPosition = compile1(simple);
                } else if (simple == "size.mid.pos"){
                    controller.midPosition = compile1(simple);
                } else if (simple == "size.shadowoffset"){
                    controller.shadowOffset = compile1(simple);
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment environment(stage, guy, commands);
        /* FIXME */
        Mugen::Helper * helper = new Mugen::Helper(&guy, guy.getRoot(), (int) evaluateNumber(id, environment, 0), name);

        helper->setOwnPalette(evaluateBool(ownPalette, environment, false));

        switch (posType){
            case Player1: {
                double x = evaluateNumber(posX, environment, 0) * (guy.getFacing() == FacingLeft ? -1 : 1) + guy.getX();
                double y = evaluateNumber(posY, environment, 0) + guy.getY();
                helper->setX(x);
                helper->setY(y);
                break;
            }
            case Player2: {
                Character * enemy = stage.getEnemy(&guy);
                if (enemy != NULL){
                    double x = evaluateNumber(posX, environment, 0) * (enemy->getFacing() == FacingLeft ? -1 : 1) + enemy->getX();
                    double y = evaluateNumber(posY, environment, 0) + enemy->getY();
                    helper->setX(x);
                    helper->setY(y);
                }
                break;
            }
            case Front: {
                double x = evaluateNumber(posX, environment, 0);
                switch (guy.getFacing()){
                    case FacingRight: x = environment.getStage().maximumRight(&guy) + x; break;
                    case FacingLeft: x = environment.getStage().maximumLeft(&guy) - x; break;
                }
                double y = evaluateNumber(posY, environment, 0) + guy.getY();
                helper->setX(x);
                helper->setY(y);
                break;
            }
            case Back: {
                double x = evaluateNumber(posX, environment, 0);
                switch (guy.getFacing()){
                    case FacingRight: x = environment.getStage().maximumRight(&guy) - x; break;
                    case FacingLeft: x = environment.getStage().maximumLeft(&guy) + x; break;
                }
                double y = evaluateNumber(posY, environment, 0) + guy.getY();
                helper->setX(x);
                helper->setY(y);
                break;
            }
            case Left: {
                double x = evaluateNumber(posX, environment, 0) + environment.getStage().maximumLeft(&guy);
                double y = evaluateNumber(posY, environment, 0) + guy.getY();
                helper->setX(x);
                helper->setY(y);
                break;
            }
            case Right: {
                double x = environment.getStage().maximumRight(&guy) - evaluateNumber(posX, environment, 0);
                double y = evaluateNumber(posY, environment, 0) + guy.getY();
                helper->setX(x);
                helper->setY(y);
                break;
            }
            default: {
                Global::debug(0) << "Warning: Unimplemented postype " << posType << std::endl;
                break;
            }
        }
        stage.addObject(helper);
        helper->changeState(stage, (int) evaluateNumber(state, environment, guy.getCurrentState()));
    }

    StateController * deepCopy() const {
        return new ControllerHelper(*this);
    }
};

class ControllerStopSnd: public StateController {
public:
    ControllerStopSnd(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
    }

    ControllerStopSnd(const ControllerStopSnd & you):
    StateController(you){
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        /* TODO: but im not sure we care about this one */
    }

    StateController * deepCopy() const {
        return new ControllerStopSnd(*this);
    }
};

class ControllerHitAdd: public StateController {
public:
    ControllerHitAdd(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        extractValue(value, section);
        if (value == NULL){
            throw MugenException("HitAdd requires a `value' attribute", __FILE__, __LINE__);
        }
    }

    ControllerHitAdd(const ControllerHitAdd & you):
    StateController(you),
    value(copy(you.value)){
    }

    Value value;

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        int combo = (int) evaluateNumber(this->value, FullEnvironment(stage, guy, commands), 0);
        guy.addCombo(combo);
    }

    StateController * deepCopy() const {
        return new ControllerHitAdd(*this);
    }
};

class ControllerGravity: public StateController {
public:
    ControllerGravity(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
    }

    ControllerGravity(const ControllerGravity & you):
    StateController(you){
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        guy.setYVelocity(guy.getYVelocity() + guy.getGravity());
    }

    StateController * deepCopy() const {
        return new ControllerGravity(*this);
    }
};

class ControllerPlayerPush: public StateController {
public:
    ControllerPlayerPush(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        extractValue(value, section);
        if (value == NULL){
            throw MugenException("PlayerPush requires a `value' attribute", __FILE__, __LINE__);
        }
    }

    ControllerPlayerPush(const ControllerPlayerPush & you):
    StateController(you),
    value(copy(you.value)){
    }

    Value value;

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        if (!evaluateBool(value, FullEnvironment(stage, guy, commands), false)){
            guy.disablePushCheck();
        }
    }

    StateController * deepCopy() const {
        return new ControllerPlayerPush(*this);
    }
};

class ControllerPause: public StateController {
public:
    ControllerPause(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        time = extractAttribute(section, "time");
        buffer = extractAttribute(section, "endcmdbuftime");
        move = extractAttribute(section, "movetime");
        background = extractAttribute(section, "pausebg");
    }

    ControllerPause(const ControllerPause & you):
    StateController(you),
    time(copy(you.time)),
    buffer(copy(you.buffer)),
    move(copy(you.move)),
    background(copy(you.background)){
    }

    Value time;
    Value buffer;
    Value move;
    Value background;

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment environment(stage, guy, commands);
        stage.doPause((int) evaluateNumber(time, environment, 0),
                      (int) evaluateNumber(buffer, environment, 0),
                      (int) evaluateNumber(move, environment, 0),
                      evaluateBool(background, environment, true));
    }

    StateController * deepCopy() const {
        return new ControllerPause(*this);
    }
};

class ControllerParentVarSet: public StateController {
public:
    ControllerParentVarSet(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        integerIndex = extractAttribute(section, "v");
        integerValue = extractAttribute(section, "value");
        floatIndex = extractAttribute(section, "fv");
        floatValue = extractAttribute(section, "value");
        parse(section);
    }

    ControllerParentVarSet(const ControllerParentVarSet & you):
    StateController(you),
    integerIndex(copy(you.integerIndex)),
    floatIndex(copy(you.floatIndex)),
    integerValue(copy(you.integerValue)),
    floatValue(copy(you.floatValue)){
    }

    Value integerIndex;
    Value floatIndex;
    Value integerValue;
    Value floatValue;

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerParentVarSet & controller):
            controller(controller){
            }

            ControllerParentVarSet & controller;

            virtual void onAttributeArray(const Ast::AttributeArray & simple){
                if (simple == "var"){
                    controller.integerIndex = Compiler::compile(simple.getIndex());
                    const Ast::Value * value = simple.getValue();
                    controller.integerValue = Compiler::compile(value);
                } else if (simple == "fvar"){
                    controller.floatIndex = Compiler::compile(simple.getIndex());
                    const Ast::Value * value = simple.getValue();
                    controller.floatValue = Compiler::compile(value);
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        if (guy.isHelper()){
            Mugen::Helper & helper = *(Mugen::Helper*)&guy;
            Character * parent = helper.getParent();
            if (parent == NULL){
                throw MugenNormalRuntimeException("No parent for helper");
            }
            FullEnvironment environment(stage, guy, commands);
            if (floatIndex != NULL && floatValue != NULL){
                parent->setFloatVariable((int) evaluateNumber(floatIndex, environment, 0), floatValue->evaluate(environment));
            }

            if (integerIndex != NULL && integerValue != NULL){
                parent->setVariable((int) evaluateNumber(integerIndex, environment, 0), integerValue->evaluate(environment));
            }
        } else {
            Global::debug(1) << "Warning, trying to use ParentVarSet on a non-helper" << endl;
        }
    }

    StateController * deepCopy() const {
        return new ControllerParentVarSet(*this);
    }
};

class ControllerParentVarAdd: public StateController {
public:
    ControllerParentVarAdd(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        integerIndex = extractAttribute(section, "v");
        integerValue = extractAttribute(section, "value");
        floatIndex = extractAttribute(section, "fv");
        floatValue = extractAttribute(section, "value");
        parse(section);
    }

    ControllerParentVarAdd(const ControllerParentVarAdd & you):
    StateController(you),
    integerIndex(copy(you.integerIndex)),
    floatIndex(copy(you.floatIndex)),
    integerValue(copy(you.integerValue)),
    floatValue(copy(you.floatValue)){
    }

    Value integerIndex;
    Value floatIndex;
    Value integerValue;
    Value floatValue;

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerParentVarAdd & controller):
            controller(controller){
            }

            ControllerParentVarAdd & controller;

            virtual void onAttributeArray(const Ast::AttributeArray & simple){
                if (simple == "var"){
                    controller.integerIndex = Compiler::compile(simple.getIndex());
                    const Ast::Value * value = simple.getValue();
                    controller.integerValue = Compiler::compile(value);
                } else if (simple == "fvar"){
                    controller.floatIndex = Compiler::compile(simple.getIndex());
                    const Ast::Value * value = simple.getValue();
                    controller.floatValue = Compiler::compile(value);
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        if (guy.isHelper()){
            Mugen::Helper & helper = *(Mugen::Helper*)&guy;
            Character * parent = helper.getParent();
            if (parent == NULL){
                throw MugenNormalRuntimeException("No parent for helper");
            }
            FullEnvironment environment(stage, guy, commands);
            if (floatIndex != NULL && floatValue != NULL){
                int index = (int) evaluateNumber(floatIndex, environment, 0);
                parent->setFloatVariable(index, floatValue->evaluate(environment) + parent->getFloatVariable(index));
            }

            if (integerIndex != NULL && integerValue != NULL){
                int index = (int) evaluateNumber(integerIndex, environment, 0);
                parent->setVariable(index, integerValue->evaluate(environment) + parent->getVariable(index));
            }
        } else {
            Global::debug(1) << "Warning, trying to use ParentVarSet on a non-helper" << endl;
        }
    }

    StateController * deepCopy() const {
        return new ControllerParentVarAdd(*this);
    }
};
        
class ControllerDisplayToClipboard: public StateController {
public:
    ControllerDisplayToClipboard(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        class Walker: public Ast::Walker {
        public:
            Walker(string & text, vector<Compiler::Value*> & parameters):
            text(text),
            parameters(parameters){
            }

            string & text;
            vector<Compiler::Value*> & parameters;

            void getParameters(const Ast::Value * value){
                if (value->hasMultiple()){
                    try{
                        Ast::View view = value->view();
                        while (true){
                            const Ast::Value * next;
                            view >> next;
                            parameters.push_back(Compiler::compile(next));
                        }
                    } catch (const Ast::Exception & fail){
                    }
                } else {
                    Compiler::Value * compiled = Compiler::compile(value);
                    parameters.push_back(compiled);
                }
            }

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "text"){
                    simple.view() >> text;
                } else if (simple == "params"){
                    getParameters(simple.getValue());
                }
            }
        };

        Walker walker(text, parameters);
        section->walk(walker);
    }

    ControllerDisplayToClipboard(const ControllerDisplayToClipboard & you):
    StateController(you),
    text(you.text){
        for (vector<Compiler::Value*>::const_iterator it = you.parameters.begin(); it != you.parameters.end(); it++){
            Compiler::Value * parameter = copy(*it);
            parameters.push_back(parameter);
        }
    }

    virtual ~ControllerDisplayToClipboard(){
        for (vector<Compiler::Value*>::iterator it = parameters.begin(); it != parameters.end(); it++){
            delete *it;
        }
    }

    string text;
    vector<Compiler::Value*> parameters;

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        ostringstream out;
        out << "[" << guy.getName() << ", " << stage.getTicks() << "] ";

        vector<Compiler::Value*>::const_iterator current = parameters.begin();
        for (unsigned int i = 0; i < text.size(); i++){
            if (text[i] == '%'){
                i += 1;
                if (i < text.size()){
                    switch (text[i]){
                        case 'u':
                        case 'i':
                        case 'd': {
                            if (current == parameters.end()){
                                out << "not enough parameters";
                            } else {
                                out << (int) evaluateNumber(*current, FullEnvironment(stage, guy, commands), 0);
                            }
                            current++;
                            break;
                        }
                        case 'f': {
                            if (current == parameters.end()){
                                out << "not enough parameters";
                            } else {
                                out << evaluateNumber(*current, FullEnvironment(stage, guy, commands), 0);
                            }
                            current++;
                            break;
                        }
                        case 's': {
                            if (current == parameters.end()){
                                out << "not enough parameters";
                            } else {
                                out << (*current)->evaluate(FullEnvironment(stage, guy, commands)).getStringValue();
                            }
                            current++;
                            break;
                        }
                        case 'b': {
                            if (current == parameters.end()){
                                out << "not enough parameters";
                            } else {
                                out << (*current)->evaluate(FullEnvironment(stage, guy, commands)).getBoolValue();
                            }
                            current++;
                            break;
                        }
                        default: {
                            out << text[i];
                            break;
                        }
                    }
                }
            } else {
                out << text[i];
            }
        }

        Global::info(out.str());
    }

    StateController * deepCopy() const {
        return new ControllerDisplayToClipboard(*this);
    }
};

class ControllerAttackMulSet: public StateController {
public:
    ControllerAttackMulSet(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        extractValue(value, section);
        if (value == NULL){
            throw MugenException("AttackMulSet requires the `value' attribute", __FILE__, __LINE__);
        }
    }

    ControllerAttackMulSet(const ControllerAttackMulSet & you):
    StateController(you),
    value(copy(you.value)){
    }

    Value value;

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        guy.setAttackMultiplier(evaluateNumber(value, FullEnvironment(stage, guy, commands), 1));
    }

    StateController * deepCopy() const {
        return new ControllerAttackMulSet(*this);
    }
};

class ControllerHitOverride: public StateController {
public:
    ControllerHitOverride(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        slot = extractAttribute(section, "slot");
        this->state = extractAttribute(section, "stateno");
        time = extractAttribute(section, "time");
        forceAir = extractAttribute(section, "forceair");
        parse(section);
    }

    ControllerHitOverride(const ControllerHitOverride & you):
    StateController(you),
    attribute(you.attribute),
    slot(copy(you.slot)),
    state(copy(you.state)),
    time(copy(you.time)),
    forceAir(copy(you.forceAir)){
    }

    string attribute;
    Value slot;
    Value state;
    Value time;
    Value forceAir;

    StateController * deepCopy() const {
        return new ControllerHitOverride(*this);
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerHitOverride & controller):
            controller(controller){
            }

            ControllerHitOverride & controller;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "attr"){
                    simple.view() >> controller.attribute;
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment environment(stage, guy, commands);
        int slot = (int) evaluateNumber(this->slot, environment, 0);
        int state = (int) evaluateNumber(this->state, environment, -1);
        int time = (int) evaluateNumber(this->time, environment, 1);
        bool air = evaluateBool(this->forceAir, environment, false);
        guy.setHitOverride(slot, attribute, state, time, air);
    }
};

class ControllerBindToParent: public StateController {
public:
    ControllerBindToParent(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        time = extractAttribute(section, "time");
        facing = extractAttribute(section, "facing");
        parse(section);
    }

    ControllerBindToParent(const ControllerBindToParent & you):
    StateController(you),
    time(copy(you.time)),
    facing(copy(you.facing)),
    x(copy(you.x)),
    y(copy(you.y)){
    }

    Value time;
    Value facing;
    Value x;
    Value y;

    StateController * deepCopy() const {
        return new ControllerBindToParent(*this);
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerBindToParent & controller):
            controller(controller){
            }

            ControllerBindToParent & controller;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "pos"){
                    readValues(simple.getValue(), controller.x, controller.y);
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        if (guy.isHelper()){
            FullEnvironment environment(stage, guy, commands);
            int time = (int) evaluateNumber(this->time, environment, 1);
            int facing = (int) evaluateNumber(this->facing, environment, 0);
            double x = evaluateNumber(this->x, environment, 0);
            double y = evaluateNumber(this->y, environment, 0);
            guy.bindTo(((Mugen::Helper*)&guy)->getParent(), time, facing, x, y);
        }
    }
};

class ControllerReversalDef: public StateController {
public:
    ControllerReversalDef(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        parse(section);
    }

    ControllerReversalDef(const ControllerReversalDef & you):
    StateController(you),
    hit(you.hit){
    }

    struct Data{
        Data(){
        }

        Data(const Data & you):
        pause(you.pause),
        spark(you.spark),
        hitSound(you.hitSound),
        player1State(copy(you.player1State)),
        player2State(copy(you.player2State)),
        standing(you.standing),
        crouching(you.crouching),
        aerial(you.aerial),
        attributes(you.attributes){
        }

        struct Pause{
            Pause(){
            }

            Pause(const Pause & you):
            player1(copy(you.player1)),
            player2(copy(you.player2)){
            }

            Value player1;
            Value player2;
        } pause;

        int spark;

        struct HitSound{
            HitSound(const HitSound & you):
            own(you.own),
            group(copy(you.group)),
            item(copy(you.item)){
            }

            HitSound():
                own(false){
                }

            bool own;
            Value group;
            Value item;
        } hitSound;

        Value player1State;
        Value player2State;

        bool standing;
        bool crouching;
        bool aerial;
        std::vector<AttackType::Attribute> attributes;
    } hit;

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(Data & hit):
            hit(hit){
            }

            Data & hit;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "pausetime"){
                    try{
                        const Ast::Value * player1;
                        const Ast::Value * player2;
                        simple.view() >> player1 >> player2;
                        hit.pause.player1 = Compiler::compile(player1);
                        hit.pause.player2 = Compiler::compile(player2);
                    } catch (const Ast::Exception & e){
                    }
                } else if (simple == "sparkno"){
                    string what;
                    simple.view() >> what;
                    /* FIXME: either S123 or 123 */
                    if (PaintownUtil::matchRegex(what, "[0-9]+")){
                        hit.spark = atoi(what.c_str());
                    }
                } else if (simple == "hitsound"){
                    const Ast::Value * group = NULL;
                    const Ast::Value * item = NULL;
                    if (simple.getValue()->hasMultiple()){
                        simple.view() >> group >> item;
                    } else {
                        group = simple.getValue();
                    }

                    if (group == NULL){
                        compileError("No group given for 'sound'", __FILE__, __LINE__);
                    }

                    Resource resource = extractResource(group);
                    hit.hitSound.own = resource.own;
                    hit.hitSound.group = Compiler::compile(resource.value);

                    if (item != NULL){
                        hit.hitSound.item = Compiler::compile(item);
                    } else {
                        hit.hitSound.item = Compiler::compile(0);
                    }
                } else if (simple == "p1stateno"){
                    hit.player1State = Compiler::compile(simple.getValue());
                } else if (simple == "p2stateno"){
                    hit.player2State = Compiler::compile(simple.getValue());
                } else if (simple == "reversal.attr"){
                    string type;
                    vector<string> moreTypes;
                    if (! simple.getValue()->hasMultiple()){
                        string type;
                        simple.view() >> type;
                        type = PaintownUtil::lowerCaseAll(type);
                    } else {
                        Ast::View view = simple.view();
                        view >> type;
                        try{
                            while (true){
                                string what;
                                view >> what;
                                what = PaintownUtil::lowerCaseAll(what);
                                moreTypes.push_back(what);
                            }
                        } catch (const Ast::Exception & e){
                        }
                    }

                    if (type.find('s') != string::npos){
                        hit.standing = false;
                    }

                    if (type.find('c') != string::npos){
                        hit.crouching = false;
                    }

                    if (type.find('a') != string::npos){
                        hit.aerial = false;
                    }

                    map<string, AttackType::Attribute> attributes;
                    attributes["na"] = AttackType::NormalAttack;
                    attributes["nt"] = AttackType::NormalThrow;
                    attributes["np"] = AttackType::NormalProjectile;
                    attributes["sa"] = AttackType::SpecialAttack;
                    attributes["st"] = AttackType::SpecialThrow;
                    attributes["sp"] = AttackType::SpecialProjectile;
                    attributes["ha"] = AttackType::HyperAttack;
                    attributes["ht"] = AttackType::HyperThrow;
                    attributes["hp"] = AttackType::HyperProjectile;

                    for (vector<string>::iterator it = moreTypes.begin(); it != moreTypes.end(); it++){
                        string what = *it;
                        if (attributes.find(what) != attributes.end()){
                            hit.attributes.push_back(attributes[what]);
                        }
                    }
                }
            }
        };

        Walker walker(hit);
        section->walk(walker);
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment environment(stage, guy, commands);
        guy.setReversalActive();
        ReversalData & data = guy.getReversal();
        data.pause.player1 = (int) evaluateNumber(hit.pause.player1, environment, 0);
        data.pause.player2 = (int) evaluateNumber(hit.pause.player2, environment, 0);
        data.spark = hit.spark;
        data.hitSound.own = hit.hitSound.own;
        data.hitSound.group = (int) evaluateNumber(hit.hitSound.group, environment, 0);
        data.hitSound.item = (int) evaluateNumber(hit.hitSound.item, environment, 0);
        data.player1State = (int) evaluateNumber(hit.player1State, environment, -1);
        data.player2State = (int) evaluateNumber(hit.player1State, environment, -1);
        data.standing = hit.standing;
        data.crouching = hit.crouching;
        data.aerial = hit.aerial;
        data.attributes = hit.attributes;
    }

    StateController * deepCopy() const {
        return new ControllerReversalDef(*this);
    }
};

class ControllerProjectile: public StateController {
public:
    ControllerProjectile(Ast::Section * section, const string & name, int state):
    StateController(name, state, section),
    positionType("p1"){
        parse(section);
        parseHitDefinition(section, hit);
    }

    ControllerProjectile(const ControllerProjectile & you):
    StateController(you),
    id(copy(you.id)),
    animation(copy(you.animation)),
    hitAnimation(copy(you.hitAnimation)),
    dieAnimation(copy(you.dieAnimation)),
    cancelAnimation(copy(you.cancelAnimation)),
    scaleX(copy(you.scaleX)),
    scaleY(copy(you.scaleY)),
    autoRemove(copy(you.autoRemove)),
    removeTime(copy(you.removeTime)),
    velocityX(copy(you.velocityX)),
    velocityY(copy(you.velocityY)),
    removeVelocityX(copy(you.removeVelocityX)),
    removeVelocityY(copy(you.removeVelocityY)),
    accelerateX(copy(you.accelerateX)),
    accelerateY(copy(you.accelerateY)),
    velocityXMultipler(copy(you.velocityXMultipler)),
    velocityYMultipler(copy(you.velocityYMultipler)),
    hits(copy(you.hits)),
    missTime(copy(you.missTime)),
    priority(copy(you.priority)),
    spritePriority(copy(you.spritePriority)),
    edge(copy(you.edge)),
    stageDistance(copy(you.stageDistance)),
    heightLow(copy(you.heightLow)),
    heightHigh(copy(you.heightHigh)),
    offsetX(copy(you.offsetX)),
    offsetY(copy(you.offsetY)),
    positionType(you.positionType),
    shadowRed(copy(you.shadowRed)),
    shadowGreen(copy(you.shadowGreen)),
    shadowBlue(copy(you.shadowBlue)),
    superPauseTime(copy(you.superPauseTime)),
    pauseMoveTime(copy(you.pauseMoveTime)),
    afterImageTime(copy(you.afterImageTime)),
    afterImageLength(copy(you.afterImageLength)),
    hit(you.hit){
    }

    Value id;
    Value animation;
    Value hitAnimation;
    Value dieAnimation;
    Value cancelAnimation;
    Value scaleX;
    Value scaleY;
    Value autoRemove;
    Value removeTime;
    Value velocityX;
    Value velocityY;
    Value removeVelocityX;
    Value removeVelocityY;
    Value accelerateX;
    Value accelerateY;
    Value velocityXMultipler;
    Value velocityYMultipler;
    Value hits;
    Value missTime;
    Value priority;
    Value spritePriority;
    Value edge;
    Value stageDistance;
    Value heightLow;
    Value heightHigh;
    Value offsetX;
    Value offsetY;
    string positionType;
    Value shadowRed;
    Value shadowGreen;
    Value shadowBlue;
    Value superPauseTime;
    Value pauseMoveTime;
    Value afterImageTime;
    Value afterImageLength;

    HitDefinitionData hit;

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerProjectile & projectile):
            projectile(projectile){
            }

            ControllerProjectile & projectile;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "projid"){
                    readValues(simple.getValue(), projectile.id);
                } else if (simple == "projanim"){
                    readValues(simple.getValue(), projectile.animation);
                } else if (simple == "projhitanim"){
                    readValues(simple.getValue(), projectile.hitAnimation);
                } else if (simple == "projremanim"){
                    readValues(simple.getValue(), projectile.dieAnimation);
                } else if (simple == "projcancelanim"){
                    readValues(simple.getValue(), projectile.cancelAnimation);
                } else if (simple == "projscale"){
                    readValues(simple.getValue(), projectile.scaleX, projectile.scaleY);
                } else if (simple == "projremove"){
                    readValues(simple.getValue(), projectile.autoRemove);
                } else if (simple == "projremovetime"){
                    readValues(simple.getValue(), projectile.removeTime);
                } else if (simple == "velocity"){
                    readValues(simple.getValue(), projectile.velocityX, projectile.velocityY);
                } else if (simple == "remvelocity"){
                    readValues(simple.getValue(), projectile.removeVelocityX, projectile.removeVelocityY);
                } else if (simple == "accel"){
                    readValues(simple.getValue(), projectile.accelerateX, projectile.accelerateY);
                } else if (simple == "velmul"){
                    readValues(simple.getValue(), projectile.velocityXMultipler, projectile.velocityYMultipler);
                } else if (simple == "projhits"){
                    readValues(simple.getValue(), projectile.hits);
                } else if (simple == "projmisstime"){
                    readValues(simple.getValue(), projectile.missTime);
                } else if (simple == "projpriority"){
                    readValues(simple.getValue(), projectile.priority);
                } else if (simple == "projsprpriority"){
                    readValues(simple.getValue(), projectile.spritePriority);
                } else if (simple == "projedgebound"){
                    readValues(simple.getValue(), projectile.edge);
                } else if (simple == "projstagebound"){
                    readValues(simple.getValue(), projectile.stageDistance);
                } else if (simple == "projheightbound"){
                    readValues(simple.getValue(), projectile.heightLow, projectile.heightHigh);
                } else if (simple == "offset"){
                    readValues(simple.getValue(), projectile.offsetX, projectile.offsetY);
                } else if (simple == "postype"){
                    simple.getValue()->view() >> projectile.positionType;
                } else if (simple == "projshadow"){
                    readValues(simple.getValue(), projectile.shadowRed, projectile.shadowGreen, projectile.shadowBlue);
                } else if (simple == "supermovetime"){
                    readValues(simple.getValue(), projectile.superPauseTime);
                } else if (simple == "pausemovetime"){
                    readValues(simple.getValue(), projectile.pauseMoveTime);
                } else if (simple == "afterimage.time"){
                    readValues(simple.getValue(), projectile.afterImageTime);
                } else if (simple == "afterimage.length"){
                    readValues(simple.getValue(), projectile.afterImageLength);
                }
            }
        };
        
        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(Stage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment environment(stage, guy, commands);
        int id = (int) evaluateNumber(this->id, environment, 0);
        int animation = (int) evaluateNumber(this->animation, environment, 0);
        int hitAnimation = (int) evaluateNumber(this->hitAnimation, environment, -1);
        int dieAnimation = (int) evaluateNumber(this->dieAnimation, environment, hitAnimation);
        int cancelAnimation = (int) evaluateNumber(this->cancelAnimation, environment, dieAnimation);
        double scaleX = evaluateNumber(this->scaleX, environment, 1);
        double scaleY = evaluateNumber(this->scaleY, environment, 1);
        bool autoRemove = evaluateBool(this->autoRemove, environment, true);
        int removeTime = (int) evaluateNumber(this->removeTime, environment, -1);
        double velocityX = evaluateNumber(this->velocityX, environment, 0);
        double velocityY = evaluateNumber(this->velocityY, environment, 0);
        double removeVelocityX = evaluateNumber(this->removeVelocityX, environment, 0);
        double removeVelocityY = evaluateNumber(this->removeVelocityY, environment, 0);
        double accelerateX = evaluateNumber(this->accelerateX, environment, 0);
        double accelerateY = evaluateNumber(this->accelerateY, environment, 0);
        double velocityXMultipler = evaluateNumber(this->velocityXMultipler, environment, 1);
        double velocityYMultipler = evaluateNumber(this->velocityYMultipler, environment, 1);
        int hits = (int) evaluateNumber(this->hits, environment, 1);
        int missTime = (int) evaluateNumber(this->missTime, environment, 0);
        int priority = (int) evaluateNumber(this->priority, environment, 1);
        int spritePriority = (int) evaluateNumber(this->spritePriority, environment, 3);
        /* 40 in 240p, 80 in 480p, 160 in 720p */
        int edge = (int) evaluateNumber(this->edge, environment, 40);
        /* 40 in 240p, 80 in 480p, 160 in 720p */
        int stageDistance = (int) evaluateNumber(this->stageDistance, environment, 40);
        /* -240 in 240p, -480 in 480p, -720 in 720p */
        int lowBound = (int) evaluateNumber(this->heightLow, environment, -240);
        /* 1 in 240p, 2 in 480p, 4 in 720p */
        int highBound = (int) evaluateNumber(this->heightHigh, environment, 1);
        int offsetX = (int) evaluateNumber(this->offsetX, environment, 0);
        int offsetY = (int) evaluateNumber(this->offsetY, environment, 0);
        int shadowRed = (int) evaluateNumber(this->shadowRed, environment, 0);
        int shadowGreen = (int) evaluateNumber(this->shadowGreen, environment, 0);
        int shadowBlue = (int) evaluateNumber(this->shadowBlue, environment, 0);
        int superMoveTime = (int) evaluateNumber(this->superPauseTime, environment, 0);
        int pauseMoveTime = (int) evaluateNumber(this->pauseMoveTime, environment, 0);
        int afterImageTime = (int) evaluateNumber(this->afterImageTime, environment, 0);
        int afterImageLength = (int) evaluateNumber(this->afterImageLength, environment, 0);

        HitDefinition hitDefinition;
        evaluateHitDefinition(this->hit, hitDefinition, environment);

        Facing facing = guy.getFacing();

        double x = 0;
        double y = 0;
        string check = PaintownUtil::lowerCaseAll(positionType);
        if (check == "p1"){
            x = guy.forwardPoint(offsetX);
            y = guy.getRY() + offsetY;
        } else if (check == "p2"){
            Character * enemy = stage.getEnemy(&guy);
            if (enemy != NULL){
                x = enemy->forwardPoint(offsetX);
                y = enemy->getRY() + offsetY;
                /* FIXME: figure out if facing should change here */
                facing = enemy->getFacing();
            }
        } else if (check == "front"){
            if (guy.getFacing() == FacingRight){
                x = stage.maximumRight(&guy) + offsetX;
            } else {
                x = stage.maximumLeft(&guy) - offsetX;
            }
            y = guy.getRY() + offsetY;
        } else if (check == "back"){
            if (guy.getFacing() == FacingLeft){
                x = stage.maximumRight(&guy) - offsetX;
            } else {
                x = stage.maximumLeft(&guy) + offsetX;
            }
            y = guy.getRY() + offsetY;
        } else if (check == "left"){
            x = stage.maximumLeft(&guy) + offsetX;
            y = stage.maximumUp() + offsetY;
            /* FIXME: figure out if facing should change here */
            facing = FacingRight;
        } else if (check == "right"){
            x = stage.maximumRight(&guy) - offsetX;
            y = stage.maximumUp() + offsetY;
            /* FIXME: figure out if facing should change here */
            facing = FacingLeft;
        }

        /* FIXME: we have to cast the root to a non-const Character* */
        stage.addProjectile(new Mugen::Projectile(x, y, id, (Character*) guy.getRoot(), animation, hitAnimation, dieAnimation,
                                           cancelAnimation, scaleX, scaleY, autoRemove, removeTime, 
                                           velocityX, velocityY, removeVelocityX, removeVelocityY,
                                           accelerateX, accelerateY, velocityXMultipler, 
                                           velocityYMultipler, hits, missTime, priority, spritePriority,
                                           edge, stageDistance, lowBound, highBound, shadowRed,
                                           shadowGreen, shadowBlue, superMoveTime, pauseMoveTime,
                                           afterImageTime, afterImageLength, facing, hitDefinition));
    }

    StateController * deepCopy() const {
        return new ControllerProjectile(*this);
    }
};

class ControllerAllPalFX: public ControllerPalFX {
public:
    ControllerAllPalFX(Ast::Section * section, const string & name, int state):
    ControllerPalFX(section, name, state){
    }

    ControllerAllPalFX(const ControllerAllPalFX & you):
    ControllerPalFX(you){
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        /* TODO */
    }

    StateController * deepCopy() const {
        return new ControllerAllPalFX(*this);
    }
};

class ControllerPowerSet: public StateController {
public:
    ControllerPowerSet(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        extractValue(value, section);
    }

    ControllerPowerSet(const ControllerPowerSet & you):
    StateController(you),
    value(copy(you.value)){
    }

    Value value;

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        guy.setPower(evaluateNumber(value, FullEnvironment(stage, guy, commands), 0));
    }

    StateController * deepCopy() const {
        return new ControllerPowerSet(*this);
    }
};

/* 90% */
class ControllerOffset: public StateController {
public:
    ControllerOffset(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        x = extractAttribute(section, "x");
        y = extractAttribute(section, "y");
    }

    ControllerOffset(const ControllerOffset & you):
    StateController(you),
    x(copy(you.x)),
    y(copy(you.y)){
    }

    Value x, y;

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment environment(stage, guy, commands);
        guy.setDrawOffset(evaluateNumber(x, environment, 0),
                          evaluateNumber(y, environment, 0));
    }

    StateController * deepCopy() const {
        return new ControllerOffset(*this);
    }
};

class ControllerExplodBindTime: public StateController {
public:
    ControllerExplodBindTime(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        id = extractAttribute(section, "id");
        Compiler::Value * time = extractAttribute(section, "time");
        Compiler::Value * value = extractAttribute(section, "value");
        if (time != NULL){
            this->time = time;
        }
        if (value != NULL){
            this->time = value;
        }
    }

    ControllerExplodBindTime(const ControllerExplodBindTime & you):
    StateController(you),
    id(copy(you.id)),
    time(copy(you.time)){
    }

    Value id;
    Value time;

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment environment(stage, guy, commands);
        vector<Effect*> effects = stage.findEffects(&guy, (int) evaluateNumber(id, environment, -1));
        int bind = (int) evaluateNumber(time, environment, 1);
        for (vector<Effect*>::iterator it = effects.begin(); it != effects.end(); it++){
            ControllerExplod::ExplodeEffect * effect = (ControllerExplod::ExplodeEffect*) (*it);
            effect->setBindTime(bind);
        }
    }
    
    StateController * deepCopy() const {
        return new ControllerExplodBindTime(*this);
    }
};

class ControllerTrans: public StateController {
public:
    ControllerTrans(Ast::Section * section, const string & name, int state):
    StateController(name, state, section),
    trans(Default){
        parse(section);
    }

    ControllerTrans(const ControllerTrans & you):
    StateController(you),
    trans(you.trans),
    alphaSource(copy(you.alphaSource)),
    alphaDestination(copy(you.alphaDestination)){
    }

    TransType trans;
    Value alphaSource;
    Value alphaDestination;

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerTrans & controller):
            controller(controller){
            }

            ControllerTrans & controller;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "trans"){
                    string type;
                    simple.view() >> type;
                    controller.trans = parseTrans(PaintownUtil::lowerCaseAll(type));
                } else if (simple == "alpha"){
                    const Ast::Value * from;
                    const Ast::Value * to;
                    simple.view() >> from >> to;
                    controller.alphaSource = Compiler::compile(from);
                    controller.alphaDestination = Compiler::compile(to);
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        int alphaFrom = 256;
        int alphaTo = 128;
        if (trans == AddAlpha){
            FullEnvironment environment(stage, guy, commands);
            alphaFrom = (int) evaluateNumber(alphaSource, environment, 256);
            alphaTo = (int) evaluateNumber(alphaDestination, environment, 0);
        }

        if (trans != Default){
            guy.setTransOverride(trans, alphaFrom, alphaTo);
        }
    }

    StateController * deepCopy() const {
        return new ControllerTrans(*this);
    }
};

class ControllerAppendToClipboard: public StateController {
public:
    ControllerAppendToClipboard(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
    }

    ControllerAppendToClipboard(const ControllerAppendToClipboard & you):
    StateController(you){
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        /* FIXME: implement */
    }

    StateController * deepCopy() const {
        return new ControllerAppendToClipboard(*this);
    }
};

class ControllerClearClipboard: public StateController {
public:
    ControllerClearClipboard(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
    }

    ControllerClearClipboard(const ControllerClearClipboard & you):
    StateController(you){
    }

    StateController * deepCopy() const {
        return new ControllerClearClipboard(*this);
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        Global::debug(0) << "ClearClipboard is not implemented" << endl;
    }
};

class ControllerMoveHitReset: public StateController {
public:
    ControllerMoveHitReset(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
    }

    ControllerMoveHitReset(const ControllerMoveHitReset & you):
    StateController(you){
    }

    StateController * deepCopy() const {
        return new ControllerMoveHitReset(*this);
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        guy.resetHitFlag();
    }
};

class ControllerBindToRoot: public StateController {
public:
    ControllerBindToRoot(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
        time = extractAttribute(section, "time");
        facing = extractAttribute(section, "facing");

        const Ast::AttributeSimple * pos = findAttribute(section, "pos");
        if (pos != NULL){
            const Ast::Value * posX = NULL;
            const Ast::Value * posY = NULL;
            try{
                pos->view() >> posX >> posY;
            } catch (const Ast::Exception & ignore){
            }

            if (posX != NULL){
                positionX = Compiler::compile(posX);
            }
            if (posY != NULL){
                positionY = Compiler::compile(posY);
            }
        }
    }

    ControllerBindToRoot(const ControllerBindToRoot & you):
    StateController(you),
    time(copy(you.time)),
    facing(copy(you.facing)),
    positionX(copy(you.positionX)),
    positionY(copy(you.positionY)){
    }

    Value time;
    Value facing;
    Value positionX, positionY;

    StateController * deepCopy() const {
        return new ControllerBindToRoot(*this);
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        if (guy.isHelper()){
            FullEnvironment environment(stage, guy, commands);
            int time = (int) evaluateNumber(this->time, environment, 1);
            int facing = (int) evaluateNumber(this->facing, environment, 0);
            double x = evaluateNumber(this->positionX, environment, 0);
            double y = evaluateNumber(this->positionY, environment, 0);
            guy.bindTo(guy.getRoot(), time, facing, x, y);
        }
    }
};

class ControllerBindToTarget: public StateController {
public:
    ControllerBindToTarget(Ast::Section * section, const string & name, int state):
    StateController(name, state, section),
    positionType("foot"){
        time = extractAttribute(section, "time");
        target = extractAttribute(section, "id");

        const Ast::AttributeSimple * pos = findAttribute(section, "pos");
        if (pos != NULL){
            const Ast::Value * posX = NULL;
            const Ast::Value * posY = NULL;
            try{
                pos->view() >> posX >> posY >> positionType;
            } catch (const Ast::Exception & ignore){
            }

            if (posX != NULL){
                positionX = Compiler::compile(posX);
            }
            if (posY != NULL){
                positionY = Compiler::compile(posY);
            }
        }
    }

    ControllerBindToTarget(const ControllerBindToTarget & you):
    StateController(you),
    time(copy(you.time)),
    target(copy(you.target)),
    positionX(copy(you.positionX)),
    positionY(copy(you.positionY)),
    positionType(you.positionType){
    }

    Value time;
    Value target;
    Value positionX, positionY;
    string positionType;

    StateController * deepCopy() const {
        return new ControllerBindToTarget(*this);
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment environment(stage, guy, commands);
        int id = (int) evaluateNumber(this->target, environment, -1);
        int time = (int) evaluateNumber(this->time, environment, 1);
        double x = evaluateNumber(this->positionX, environment, 0);
        double y = evaluateNumber(this->positionY, environment, 0);
        
        vector<Character*> targets = stage.getTargets(id, &guy);
        if (targets.size() > 0){
            Character * target = targets[0];
            double pointX = 0;
            double pointY = 0;
            if (PaintownUtil::lowerCaseAll(positionType) == "foot"){
                /* bind point stays 0 */
            } else if (PaintownUtil::lowerCaseAll(positionType) == "mid"){
                Point position = guy.getMidPosition();
                pointX = position.x;
                pointY = position.y;
            } else if (PaintownUtil::lowerCaseAll(positionType) == "head"){
                Point position = guy.getHeadPosition();
                pointX = position.x;
                pointY = position.y;
            }
            guy.bindTo(target, time, 0, pointX + x, pointY + y);
        }
    }
};

class ControllerDebug: public StateController {
public:
    ControllerDebug(Ast::Section * section, const string & name, int state):
    StateController(name, state, section){
    }

    StateController * deepCopy() const {
        return new ControllerDebug(*this);
    }

    virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
        int x = 1;
        x = x + 2;
    }
};

static string toString(StateController::Type type){
    switch (type){
        case StateController::ChangeAnim : return "ChangeAnim";
        case StateController::ChangeState : return "ChangeState";
        case StateController::CtrlSet : return "CtrlSet";
        case StateController::PlaySnd : return "PlaySnd";
        case StateController::VarSet : return "VarSet";
        case StateController::VelSet : return "VelSet";
        case StateController::HitVelSet : return "HitVelSet";
        case StateController::PosAdd : return "PosAdd";
        case StateController::PosSet : return "PosSet";
        case StateController::VelAdd : return "VelAdd";
        case StateController::VelMul : return "VelMul";
        case StateController::HitDef : return "HitDef";
        case StateController::StateTypeSet : return "StateTypeSet";
        case StateController::SuperPause : return "SuperPause";
        case StateController::AfterImage : return "AfterImage";
        case StateController::AfterImageTime : return "AfterImageTime";
        case StateController::AngleAdd : return "AngleAdd";
        case StateController::AngleDraw : return "AngleDraw";
        case StateController::AngleMul : return "AngleMul";
        case StateController::AngleSet : return "AngleSet";
        case StateController::AllPalFX : return "AllPalFX";
        case StateController::AppendToClipboard : return "AppendToClipboard";
        case StateController::AssertSpecial : return "AssertSpecial";
        case StateController::AttackDist : return "AttackDist";
        case StateController::AttackMulSet : return "AttackMulSet";
        case StateController::BGPalFX : return "BGPalFX";
        case StateController::BindToParent : return "BindToParent";
        case StateController::BindToRoot : return "BindToRoot";
        case StateController::BindToTarget : return "BindToTarget";
        case StateController::ChangeAnim2 : return "ChangeAnim2";
        case StateController::ClearClipboard : return "ClearClipboard";
        case StateController::DefenceMulSet : return "DefenceMulSet";
        case StateController::DestroySelf : return "DestroySelf";
        case StateController::DisplayToClipboard : return "DisplayToClipboard";
        case StateController::EnvColor : return "EnvColor";
        case StateController::EnvShake : return "EnvShake";
        case StateController::Explod : return "Explod";
        case StateController::ExplodBindTime : return "ExplodBindTime";
        case StateController::ForceFeedback : return "ForceFeedback";
        case StateController::FallEnvShake : return "FallEnvShake";
        case StateController::GameMakeAnim : return "GameMakeAnim";
        case StateController::Gravity : return "Gravity";
        case StateController::Helper : return "Helper";
        case StateController::HitAdd : return "HitAdd";
        case StateController::HitBy : return "HitBy";
        case StateController::HitFallDamage : return "HitFallDamage";
        case StateController::HitFallSet : return "HitFallSet";
        case StateController::HitFallVel : return "HitFallVel";
        case StateController::HitOverride : return "HitOverride";
        case StateController::LifeAdd : return "LifeAdd";
        case StateController::LifeSet : return "LifeSet";
        case StateController::MakeDust : return "MakeDust";
        case StateController::ModifyExplod : return "ModifyExplod";
        case StateController::MoveHitReset : return "MoveHitReset";
        case StateController::NotHitBy : return "NotHitBy";
        case StateController::Null : return "Null";
        case StateController::Offset : return "Offset";
        case StateController::PalFX : return "PalFX";
        case StateController::ParentVarAdd : return "ParentVarAdd";
        case StateController::ParentVarSet : return "ParentVarSet";
        case StateController::Pause : return "Pause";
        case StateController::PlayerPush : return "PlayerPush";
        case StateController::PosFreeze : return "PosFreeze";
        case StateController::PowerAdd : return "PowerAdd";
        case StateController::PowerSet : return "PowerSet";
        case StateController::Projectile : return "Projectile";
        case StateController::RemoveExplod : return "RemoveExplod";
        case StateController::ReversalDef : return "ReversalDef";
        case StateController::ScreenBound : return "ScreenBound";
        case StateController::SelfState : return "SelfState";
        case StateController::SprPriority : return "SprPriority";
        case StateController::SndPan : return "SndPan";
        case StateController::StopSnd : return "StopSnd";
        case StateController::TargetBind : return "TargetBind";
        case StateController::TargetDrop : return "TargetDrop";
        case StateController::TargetFacing : return "TargetFacing";
        case StateController::TargetLifeAdd : return "TargetLifeAdd";
        case StateController::TargetPowerAdd : return "TargetPowerAdd";
        case StateController::TargetState : return "TargetState";
        case StateController::TargetVelAdd : return "TargetVelAdd";
        case StateController::TargetVelSet : return "TargetVelSet";
        case StateController::Trans : return "Trans";
        case StateController::Turn : return "Turn";
        case StateController::VarAdd : return "VarAdd";
        case StateController::VarRandom : return "VarRandom";
        case StateController::VarRangeSet : return "VarRangeSet";
        case StateController::Width :  return "Width";
        case StateController::Unknown :  return "Unknown";
        case StateController::Debug: return "Debug";
    }
    return "???";
}

StateController * StateController::compile(Ast::Section * section, const string & name, int state, StateController::Type type){
    switch (type){
        case StateController::ChangeAnim : return new ControllerChangeAnim(section, name, state);
        case StateController::ChangeState : return new ControllerChangeState(section, name, state);
        case StateController::CtrlSet : return new ControllerCtrlSet(section, name, state);
        case StateController::PlaySnd : return new ControllerPlaySound(section, name, state);
        case StateController::VarSet : return new ControllerVarSet(section, name, state);
        case StateController::VelSet : return new ControllerVelSet(section, name, state);
        case StateController::HitVelSet : return new ControllerHitVelSet(section, name, state);
        case StateController::PosAdd : return new ControllerPosAdd(section, name, state);
        case StateController::PosSet : return new ControllerPosSet(section, name, state);
        case StateController::VelAdd : return new ControllerVelAdd(section, name, state);
        case StateController::VelMul : return new ControllerVelMul(section, name, state);
        case StateController::HitDef : return new ControllerHitDef(section, name, state);
        case StateController::StateTypeSet : return new ControllerStateTypeSet(section, name, state);
        case StateController::SuperPause : return new ControllerSuperPause(section, name, state);
        case StateController::AfterImage : return new ControllerAfterImage(section, name, state);
        case StateController::AfterImageTime : return new ControllerAfterImageTime(section, name, state);
        case StateController::AngleAdd : return new ControllerAngleAdd(section, name, state);
        case StateController::AngleDraw : return new ControllerAngleDraw(section, name, state);
        case StateController::AngleMul : return new ControllerAngleMul(section, name, state);
        case StateController::AngleSet : return new ControllerAngleSet(section, name, state);
        case StateController::AssertSpecial : return new ControllerAssertSpecial(section, name, state);
        case StateController::AttackDist : return new ControllerAttackDist(section, name, state);
        case StateController::Null : return new ControllerNull(name, state);
        case StateController::Turn : return new ControllerTurn(section, name, state);
        case StateController::VarAdd : return new ControllerVarAdd(section, name, state);
        case StateController::ForceFeedback : return new ControllerForceFeedback(section, name, state);
        case StateController::Width : return new ControllerWidth(section, name, state);
        case StateController::MakeDust : return new ControllerMakeDust(section, name, state);
        case StateController::FallEnvShake : return new ControllerFallEnvShake(section, name, state);
        case StateController::Explod : return new ControllerExplod(section, name, state);
        case StateController::HitBy : return new ControllerHitBy(section, name, state);
        case StateController::NotHitBy : return new ControllerNotHitBy(section, name, state);
        case StateController::GameMakeAnim : return new ControllerGameMakeAnim(section, name, state);
        case StateController::EnvShake : return new ControllerEnvShake(section, name, state);
        case StateController::TargetBind : return new ControllerTargetBind(section, name, state);
        case StateController::DefenceMulSet : return new ControllerDefenceMulSet(section, name, state);
        case StateController::VarRandom : return new ControllerVarRandom(section, name, state);
        case StateController::HitFallDamage : return new ControllerHitFallDamage(section, name, state);
        case StateController::PosFreeze : return new ControllerPosFreeze(section, name, state);
        case StateController::HitFallVel : return new ControllerHitFallVel(section, name, state);
        case StateController::HitFallSet : return new ControllerHitFallSet(section, name, state);
        case StateController::SelfState : return new ControllerSelfState(section, name, state);
        case StateController::PalFX : return new ControllerPalFX(section, name, state);
        case StateController::VarRangeSet : return new ControllerVarRangeSet(section, name, state);
        case StateController::SprPriority : return new ControllerSprPriority(section, name, state);
        case StateController::TargetFacing : return new ControllerTargetFacing(section, name, state);
        case StateController::TargetLifeAdd : return new ControllerTargetLifeAdd(section, name, state);
        case StateController::TargetState : return new ControllerTargetState(section, name, state);
        case StateController::ChangeAnim2 : return new ControllerChangeAnim2(section, name, state);
        case StateController::ScreenBound : return new ControllerScreenBound(section, name, state);
        case StateController::PowerAdd : return new ControllerPowerAdd(section, name, state);
        case StateController::EnvColor : return new ControllerEnvColor(section, name, state);
        case StateController::DestroySelf : return new ControllerDestroySelf(section, name, state);
        case StateController::BGPalFX : return new ControllerBGPalFX(section, name, state);
        case StateController::LifeAdd : return new ControllerLifeAdd(section, name, state);
        case StateController::LifeSet : return new ControllerLifeSet(section, name, state);
        case StateController::RemoveExplod : return new ControllerRemoveExplod(section, name, state);
        case StateController::ModifyExplod : return new ControllerModifyExplod(section, name, state);
        case StateController::Helper : return new ControllerHelper(section, name, state);
        case StateController::StopSnd : return new ControllerStopSnd(section, name, state);
        case StateController::HitAdd : return new ControllerHitAdd(section, name, state);
        case StateController::Gravity : return new ControllerGravity(section, name, state);
        case StateController::PlayerPush : return new ControllerPlayerPush(section, name, state);
        case StateController::Pause : return new ControllerPause(section, name, state);
        case StateController::ParentVarSet : return new ControllerParentVarSet(section, name, state);
        case StateController::ParentVarAdd : return new ControllerParentVarAdd(section, name, state);
        case StateController::DisplayToClipboard : return new ControllerDisplayToClipboard(section, name, state);
        case StateController::AttackMulSet : return new ControllerAttackMulSet(section, name, state);
        case StateController::HitOverride : return new ControllerHitOverride(section, name, state);
        case StateController::BindToParent : return new ControllerBindToParent(section, name, state);
        case StateController::ReversalDef : return new ControllerReversalDef(section, name, state);
        case StateController::Projectile : return new ControllerProjectile(section, name, state);
        case StateController::AllPalFX : return new ControllerAllPalFX(section, name, state);
        case StateController::PowerSet : return new ControllerPowerSet(section, name, state);
        case StateController::Offset : return new ControllerOffset(section, name, state);
        case StateController::ExplodBindTime : return new ControllerExplodBindTime(section, name, state);
        case StateController::Trans : return new ControllerTrans(section, name, state);
        case StateController::AppendToClipboard : return new ControllerDisplayToClipboard(section, name, state);
        case StateController::ClearClipboard : return new ControllerClearClipboard(section, name, state);
        case StateController::MoveHitReset : return new ControllerMoveHitReset(section, name, state);
        case StateController::BindToRoot : return new ControllerBindToRoot(section, name, state);
        case StateController::BindToTarget : return new ControllerBindToTarget(section, name, state);
        case StateController::Debug: return new ControllerDebug(section, name, state);
        case StateController::TargetPowerAdd : return new ControllerTargetPowerAdd(section, name, state);
        case StateController::TargetVelAdd : return new ControllerTargetVelAdd(section, name, state);
        case StateController::TargetVelSet : return new ControllerTargetVelSet(section, name, state);
        case StateController::TargetDrop : return new ControllerTargetDrop(section, name, state);
        case StateController::SndPan : return new ControllerSndPan(section, name, state);
        default: {
            class DefaultController: public StateController {
            public:
                DefaultController(Ast::Section * section, const string & name, int state):
                    StateController(name, state, section){
                    }

                DefaultController(const DefaultController & you):
                StateController(you){
                }

                virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
                    /* nothing */
                }

                StateController * deepCopy() const {
                    return new DefaultController(*this);
                }
            };

            Global::debug(0) << "Warning: unhandled state controller for " << toString(type) << " " << name << endl;
            return new DefaultController(section, name, state);
        }
        case StateController::Unknown : {
            ostringstream out;
            out << "Unknown state controller for " << state << " " << name;
            throw MugenException(out.str(), __FILE__, __LINE__);
            break;
        }
    }

    ostringstream out;
    out << "Unknown state controller for " << state << " " << name << " type (" << type << ")";
    throw MugenException(out.str(), __FILE__, __LINE__);
}

}
