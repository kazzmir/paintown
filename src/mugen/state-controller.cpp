#include "ast/all.h"
#include "state-controller.h"
#include "character.h"
#include "stage.h"
#include "sound.h"
#include <sstream>
#include "exception.h"

using namespace std;

namespace Mugen{

StateController::StateController(const string & name):
type(Unknown),
name(name),
debug(false),
persistent(1),
currentPersistent(1){
}

StateController::StateController(const string & name, Ast::Section * section):
type(Unknown),
name(name),
debug(false),
persistent(1),
currentPersistent(1){
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
                simple >> controller.persistent;
                controller.currentPersistent = controller.persistent;
            } else if (simple == "debug"){
                controller.debug = true;
            }
        }
    };
    Walker walker(*this);
    section->walk(walker);
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
    
void StateController::resetPersistent(){
    currentPersistent = persistent;
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

bool StateController::canTrigger(const MugenStage & stage, const Character & character, const Compiler::Value * expression, const vector<string> & commands) const {
    /* this makes it easy to break in gdb */
    try{
        /*
        if (debug){
            int x = 2;
            x += 1;
        }
        */
        RuntimeValue result = expression->evaluate(FullEnvironment(stage, character, commands));
        return result.toBool();
    } catch (const MugenException & e){
        ostringstream out;
        out << "Expression `" << expression << "' " << e.getReason();
        throw MugenException(out.str());
    }
}

bool StateController::canTrigger(const MugenStage & stage, const Character & character, const vector<Compiler::Value*> & expressions, const vector<string> & commands) const {
    for (vector<Compiler::Value*>::const_iterator it = expressions.begin(); it != expressions.end(); it++){
        const Compiler::Value * value = *it;
        if (!canTrigger(stage, character, value, commands)){
            // Global::debug(2*!getDebug()) << "'" << value->toString() << "' did not trigger" << endl;
            Global::debug(2*!getDebug()) << "'" << value->toString() << "' did not trigger" << endl;
            return false;
        } else {
            // Global::debug(2*!getDebug()) << "'" << value->toString() << "' did trigger" << endl;
            Global::debug(2*!getDebug()) << "'" << value->toString() << "' did trigger" << endl;
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

bool StateController::canTrigger(const MugenStage & stage, const Character & character, const vector<string> & commands) const {
    if (triggers.find(-1) != triggers.end()){
        vector<Compiler::Value*> values = triggers.find(-1)->second;
        /* if the triggerall fails then no triggers will work */
        if (!canTrigger(stage, character, values, commands)){
            return false;
        }
    }

    vector<int> keys = sortTriggers();
    for (vector<int>::iterator it = keys.begin(); it != keys.end(); it++){
        vector<Compiler::Value*> values = triggers.find(*it)->second;
        /* if a trigger succeeds then stop processing and just return true */
        if (canTrigger(stage, character, values, commands)){
            return true;
        }
    }

    return false;
}

/* helpers */
static void extractValue(Compiler::Value *& value, Ast::Section * section){
    class Walker: public Ast::Walker {
    public:
        Walker(Compiler::Value *& value):
            value(value){
            }

        Compiler::Value *& value;

        virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
            if (simple == "value"){
                value = Compiler::compile(simple.getValue());
            }
        }
    };
    Walker walker(value);
    section->walk(walker);
}

class ControllerChangeAnim: public StateController {
public:
    ControllerChangeAnim(Ast::Section * section, const string & name):
        StateController(name, section),
        value(NULL){
            parse(section);
        }

    Compiler::Value * value;

    void parse(Ast::Section * section){
        extractValue(value, section);

        if (value == NULL){
            ostringstream out;
            out << "Expected the `value' attribute for state " << name;
            throw MugenException(out.str());
        }
    }

    virtual ~ControllerChangeAnim(){
        delete value;
    }

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
        RuntimeValue result = value->evaluate(FullEnvironment(stage, guy));
        if (result.isDouble()){
            int value = (int) result.getDoubleValue();
            guy.setAnimation(value);
        }
    }
};

class ControllerChangeState: public StateController {
public:
    ControllerChangeState(Ast::Section * section, const std::string & name):
        StateController(name, section),
        value(NULL),
        control(NULL){
            parse(section);
        }

    Compiler::Value * value;
    Compiler::Value * control;

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(Compiler::Value *& value, Compiler::Value *& control):
                value(value),
                control(control){
                }

            Compiler::Value *& value;
            Compiler::Value *& control;

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
            throw MugenException(out.str());
        }
    }

    virtual ~ControllerChangeState(){
        delete value;
        delete control;
    }

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
        if (control != NULL){
            guy.setControl(control->evaluate(FullEnvironment(stage, guy)).toBool());
        }

        RuntimeValue result = value->evaluate(FullEnvironment(stage, guy));
        if (result.isDouble()){
            int value = (int) result.getDoubleValue();
            guy.changeState(stage, value, commands);
        }
    }
};

class ControllerCtrlSet: public StateController {
public:
    ControllerCtrlSet(Ast::Section * section, const string & name):
        StateController(name, section),
        value(NULL){
            parse(section);
        }

    Compiler::Value * value;

    virtual ~ControllerCtrlSet(){
        delete value;
    }

    void parse(Ast::Section * section){
        extractValue(value, section);
        if (value == NULL){
            ostringstream out;
            out << "Expected the `value' attribute for state " << name;
            throw MugenException(out.str());
        }
    }

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
        RuntimeValue result = value->evaluate(FullEnvironment(stage, guy));
        guy.setControl(toBool(result));
    }
};

class ControllerPlaySound: public StateController {
public:
    ControllerPlaySound(Ast::Section * section, const string & name):
        StateController(name, section),
        group(-1),
        own(false),
        item(NULL){
            parse(section);
        }

    int group;
    bool own;
    Compiler::Value * item;

    virtual ~ControllerPlaySound(){
        delete item;
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
            string group;
            const Ast::Value * item;
            *value >> group >> item;
            if (PaintownUtil::matchRegex(group, "F[0-9]+")){
                int realGroup = atoi(PaintownUtil::captureRegex(group, "F([0-9]+)", 0).c_str());
                this->group = realGroup;
                this->item = Compiler::compile(item);
                own = true;
            } else if (PaintownUtil::matchRegex(group, "[0-9]+")){
                this->group = atoi(group.c_str());
                this->item = Compiler::compile(item);
                own = false;
            }
        } catch (const MugenException & e){
            // Global::debug(0) << "Error with PlaySnd " << controller.name << ": " << e.getReason() << endl;
            Global::debug(0) << "Error with PlaySnd :" << e.getReason() << endl;
        }

    }

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
        MugenSound * sound = NULL;
        if (item != NULL){
            int itemNumber = (int) item->evaluate(FullEnvironment(stage, guy)).toNumber();
            if (own){
                sound = guy.getCommonSound(group, itemNumber);
            } else {
                sound = guy.getSound(group, itemNumber);
            }
        }

        if (sound != NULL){
            sound->play();
        }
    }
};

class ControllerVarSet: public StateController {
public:
    ControllerVarSet(Ast::Section * section, const string & name):
        StateController(name, section),
        value(NULL),
        variable(NULL){
            parse(section);
        }

    Compiler::Value * value;
    Compiler::Value * variable;
    map<int, Compiler::Value*> variables;
    map<int, Compiler::Value*> floatVariables;
    map<int, Compiler::Value*> systemVariables;

    virtual ~ControllerVarSet(){
        delete value;
        delete variable;
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

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
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
            /* 'value = 23' is value1
             * 'v = 9' is value2
             */
            guy.setVariable((int) variable->evaluate(FullEnvironment(stage, guy, commands)).toNumber(), value);
        }
    }
};

class ControllerVelSet: public StateController {
public:
    ControllerVelSet(Ast::Section * section, const string & name):
        StateController(name, section),
        x(NULL),
        y(NULL){
            parse(section);
        }

    Compiler::Value * x;
    Compiler::Value * y;

    virtual ~ControllerVelSet(){
        delete x;
        delete y;
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

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
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
};

class ControllerHitVelSet: public StateController {
public:
    ControllerHitVelSet(Ast::Section * section, const string & name):
        StateController(name, section),
        x(NULL),
        y(NULL){
            parse(section);
        }

    Compiler::Value * x;
    Compiler::Value * y;

    virtual ~ControllerHitVelSet(){
        delete x;
        delete y;
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

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
        if (x != NULL){
            RuntimeValue result = x->evaluate(FullEnvironment(stage, guy));
            if (result.getBoolValue()){
                guy.setXVelocity(guy.getHitState().xVelocity);
            }
        }

        if (y != NULL){
            RuntimeValue result = y->evaluate(FullEnvironment(stage, guy));
            if (result.getBoolValue()){
                guy.setYVelocity(guy.getHitState().yVelocity);
            }
        }
    }

};

class ControllerPosAdd: public StateController {
public:
    ControllerPosAdd(Ast::Section * section, const string & name):
        StateController(name, section),
        x(NULL),
        y(NULL){
            parse(section);
        }

    Compiler::Value * x;
    Compiler::Value * y;

    virtual ~ControllerPosAdd(){
        delete x;
        delete y;
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

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
        if (x != NULL){
            RuntimeValue result = x->evaluate(FullEnvironment(stage, guy));
            if (result.isDouble()){
                guy.moveX(result.getDoubleValue());
                // guy.setX(guy.getX() + result.getDoubleValue());
            }
        }
        if (y != NULL){
            RuntimeValue result = y->evaluate(FullEnvironment(stage, guy));
            if (result.isDouble()){
                guy.moveYNoCheck(-result.getDoubleValue());
                // guy.setY(guy.getY() + result.getDoubleValue());
            }
        }
    }

};

class ControllerPosSet: public StateController {
public:
    ControllerPosSet(Ast::Section * section, const string & name):
        StateController(name, section),
        x(NULL),
        y(NULL){
            parse(section);
        }

    Compiler::Value * x;
    Compiler::Value * y;

    virtual ~ControllerPosSet(){
        delete x;
        delete y;
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

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
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

};

class ControllerVelAdd: public StateController {
public:
    ControllerVelAdd(Ast::Section * section, const string & name):
        StateController(name, section),
        x(NULL),
        y(NULL){
            parse(section);
        }

    Compiler::Value * x;
    Compiler::Value * y;

    virtual ~ControllerVelAdd(){
        delete x;
        delete y;
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

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
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
};

class ControllerVelMul: public StateController {
public:
    ControllerVelMul(Ast::Section * section, const string & name):
        StateController(name, section),
        x(NULL),
        y(NULL){
            parse(section);
        }

    Compiler::Value * x;
    Compiler::Value * y;

    virtual ~ControllerVelMul(){
        delete x;
        delete y;
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

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
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
};

class ControllerHitDef: public StateController {
public:
    ControllerHitDef(Ast::Section * section, const string & name):
        StateController(name, section){
            parse(section);
        }

    struct Data{
        Data():
            hitFlag("MAF"),
            guardFlag(),
            animationType(AttackType::Light),
            animationTypeAir(AttackType::NoAnimation),
            animationTypeFall(AttackType::NoAnimation),
            spark(-1),
            guardSpark(-1),
            groundType(AttackType::High),
            airType(AttackType::None){
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
        string hitFlag;

        /* guardflag = hit_flags (string)
         * This determines how P2 may guard the attack. hit_flags is a string containing a combination of the following characters:
         *
         * "H" for "high", "L" for "low" or "A" for air. "M" (mid) is equivalent to saying "HL". If omitted, defaults to an empty string, meaning P2 cannot guard the attack.
         * affectteam = team_type (string)
         * team_type specifies which team's players can be hit by this HitDef. Use B for both teams (all players), E for enemy team (opponents), or F for friendly team (your own team). The default is E.
         */
        string guardFlag;

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
            Value hit;
            string type;
        } priority;

        /* damage = hit_damage, guard_damage (int, int)
         * hit_damage is the damage that P2 takes when hit by P2. The optional guard_damage parameter is the damage taken by P2 if the hit is guarded. Both default to zero if omitted.
         */
        struct Damage{
            Damage(){
            }

            Value damage;
            Value guardDamage;
        } damage;

        /* pausetime = p1_pausetime, p2_shaketime (int, int)
         * This is the time that each player will pause on the hit. p1_pausetime is the time to freeze P1, measured in game-ticks. p2_pausetime is the time to make P2 shake before recoiling from the hit. Defaults to 0,0 if omitted.
         */
        struct PauseTime{
            Value player1;
            Value player2;
        } pause;

        /* guard.pausetime = p1_pausetime, p2_shaketime (int, int)
         * Similar to the "pausetime" parameter, these are the times to pause each player if the hit was guarded. Defaults to the same values as the "pausetime" parameter if omitted.
         */
        struct GuardPauseTime{
            Value player1;
            Value player2;
        } guardPause;

        /* sparkno = action_no (int)
         * This is the action number of the spark to display if the hit is successful. To play a spark out of the player's .AIR file, precede the action number with an S, e.g. "sparkno = S10". Defaults to the value set in the player variables if omitted.
         */
        int spark;

        /* guard.sparkno = action_no (int)
         * This is the action number of the spark to display if the hit was guarded. To play a spark out of the player's .AIR file, precede the action number with an S. Defaults to the value set in the player variables if omitted.
         */
        int guardSpark;

        /* sparkxy = spark_x, spark_y (int, int)
         * This is where to make the hit/guard spark. spark_x is a coordinate relative to the front of P2. A negative value makes the spark deeper inside P2. "Front" refers to the x- position at P2's axis offset towards P1 by the corresponding width value in the [Size] group in P2's player variables. spark_y is relative to P1. A negative value makes a spark higher up. You can use a tool like AirView to determine this value by positioning the cursor at the "attack spot" and reading off the value of the y-position. Defaults to 0,0 if omitted.
         */
        struct SparkPosition{
            Value x, y;
        } sparkPosition;

        /* hitsound = snd_grp, snd_item (int, int)
         * This is the sound to play on hit (from common.snd). The included fight.snd lets you choose from 5,0 (light hit sound) through to 5,4 (painful whack). To play a sound from the player's own SND file, precede the first number with an "S". For example, "hitsound = S1,0". Defaults to the value set in the player variables if omitted.
         */
        struct HitSound{
            HitSound():
                own(false),
                group(-1),
                item(-1){
                }

            bool own;
            int group;
            int item;
        } hitSound;

        /* guardsound = snd_grp, snd_item (int, int)
         * This is the sound to play on guard (from common.snd). Only 6,0 is available at this time. To play a sound from the player's own SND file, precede the first number with an "S". There is no facility to play a sound from the opponent's SND file. Defaults to the value set in the player variables if omitted.
         */
        struct GuardHitSound{
            GuardHitSound():
                own(false),
                group(-1),
                item(-1){
                }

            bool own;
            int group;
            int item;
        } guardHitSound;

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
            Value x, y;
        } airVelocity;

        /* airguard.velocity = x_velocity, y_velocity (float float)
         * Velocity to give P2 if P2 guards the hit in the air. Defaults to x_velocity * 1.5, y_velocity / 2, where x_velocity and y_velocity are values of the "air.velocity" parameter.
         */
        struct AirGuardVelocity{
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
                Value time;
            } envShake;
        } fall;

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
         * getpower = p1power, p1gpower (int, int)
         * p1power specifies the amount of power to give P1 if this HitDef connects successfully. p1gpower specifies the amount of power to give P1 if this HitDef is guarded. If omitted, p1power defaults to hit_damage (from "damage" parameter) multiplied by the value of Default.Attack.LifeToPowerMul specified in data/mugen.cfg. If p1gpower is omitted, it defaults to the value specified for p1power divided by 2.
         * givepower = p2power, p2gpower (int, int)
         * p2power specifies the amount of power to give P2 if this HitDef connects successfully. p2gpower specifies the amount of power to give P2 if this HitDef is guarded. If omitted, p1power defaults to hit_damage (from "damage" parameter) multiplied by the value of Default.GetHit.LifeToPowerMul specified in data/mugen.cfg. If p1gpower is omitted, it defaults to the value specified for p1power divided by 2.
         * palfx.time = palfx_time (int)
         * See below.
         * palfx.mul = r1, g1, b1 (int, int, int)
         * See below.
         * palfx.add = r2, g2, b2 (int, int, int)
         * If included, this allows for palette effects on P2 if the hit is successful. palfx_time is the time in game-ticks to apply palette effects on P2. palfx_time is 0 by default (no effect). The rest of the parameters are the same as in the PalFX controller.
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

    Data hit;

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(Data & hit):
                hit(hit){
                }

            Data & hit;

            string toString(char x){
                ostringstream out;
                out << x;
                return out.str();
            }

            AttackType::Animation parseAnimationType(string type){
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
                    Global::debug(0) << "Unknown hitdef animation type " << type << endl;
                }
                return AttackType::NoAnimation;
            }

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "attr"){
                    string type, attack;
                    simple >> type >> attack;
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
                    simple >> flags;
                    hit.hitFlag = flags;
                } else if (simple == "guardflag"){
                    string flags;
                    simple >> flags;
                    hit.guardFlag = flags;
                } else if (simple == "animtype"){
                    string anim;
                    simple >> anim;
                    hit.animationType = parseAnimationType(anim);
                } else if (simple == "air.animtype"){
                    string anim;
                    simple >> anim;
                    hit.animationTypeAir = parseAnimationType(anim);
                } else if (simple == "debug"){
                    // setDebug(true);
                } else if (simple == "fall.animtype"){
                    string anim;
                    simple >> anim;
                    hit.animationTypeFall = parseAnimationType(anim);
                } else if (simple == "priority"){
                    const Ast::Value * hit;
                    simple >> hit;
                    this->hit.priority.hit = Compiler::compile(hit);
                    try{
                        string type;
                        simple >> type;
                        this->hit.priority.type = type;
                    } catch (const Ast::Exception & e){
                    }
                } else if (simple == "damage"){
                    try{
                        const Ast::Value * container = simple.getValue();
                        /*
                           container->reset();
                           Ast::Value * value;
                         *container >> value;
                         controller->getHit().damage.damage = (Ast::Value*) value->copy();
                         */

                        /* has guard */
                        if (container->hasMultiple()){
                            container->reset();
                            const Ast::Value * value;
                            *container >> value;
                            hit.damage.damage = Compiler::compile(value);
                            *container >> value;
                            hit.damage.guardDamage = Compiler::compile(value);
                        } else {
                            /* otherwise its a single expression */
                            hit.damage.damage = Compiler::compile(container);
                        }
                    } catch (const Ast::Exception & e){
                        ostringstream out;
                        out << "Could not read `damage' from '" << simple.toString() << "': " << e.getReason();
                        throw MugenException(out.str());
                    }
                } else if (simple == "pausetime"){
                    try{
                        const Ast::Value * player1;
                        const Ast::Value * player2;
                        simple >> player1;
                        simple >> player2;
                        hit.pause.player1 = Compiler::compile(player1);
                        hit.pause.player2 = Compiler::compile(player2);
                    } catch (const Ast::Exception & e){
                    }
                } else if (simple == "guard.pausetime"){
                    try{
                        const Ast::Value * player1;
                        const Ast::Value * player2;
                        simple >> player1 >> player2;
                        hit.guardPause.player1 = Compiler::compile(player1);
                        hit.guardPause.player2 = Compiler::compile(player2);
                    } catch (const Ast::Exception & e){
                    }
                } else if (simple == "sparkno"){
                    string what;
                    simple >> what;
                    /* FIXME: either S123 or 123 */
                    if (PaintownUtil::matchRegex(what, "[0-9]+")){
                        hit.spark = atoi(what.c_str());
                    }
                } else if (simple == "guard.sparkno"){
                    string what;
                    simple >> what;
                    /* FIXME: either S123 or 123 */
                    if (PaintownUtil::matchRegex(what, "[0-9]+")){
                        hit.guardSpark = atoi(what.c_str());
                    }
                } else if (simple == "sparkxy"){
                    try{
                        const Ast::Value * x;
                        const Ast::Value * y;
                        simple >> x >> y;
                        hit.sparkPosition.x = Compiler::compile(x);
                        hit.sparkPosition.y = Compiler::compile(y);
                    } catch (const Ast::Exception & e){
                    }
                } else if (simple == "hitsound"){
                    string first;
                    bool own = false;
                    int group = 0;
                    int item = 0;
                    /* If not specified, assume item 0 */
                    simple >> first;
                    if (simple.getValue()->hasMultiple()){
                        simple >> item;
                    }
                    if (first[0] == 'S'){
                        own = true;
                        group = atoi(first.substr(1).c_str());
                    } else {
                        group = atoi(first.c_str());
                    }
                    hit.hitSound.own = own;
                    hit.hitSound.group = group;
                    hit.hitSound.item = item;
                } else if (simple == "guardsound"){
                    string first;
                    bool own = false;
                    int group;
                    int item;
                    simple >> first >> item;
                    if (first[0] == 'S'){
                        own = true;
                        group = atoi(first.substr(1).c_str());
                    } else {
                        group = atoi(first.c_str());
                    }
                    hit.guardHitSound.own = own;
                    hit.guardHitSound.group = group;
                    hit.guardHitSound.item = item;
                } else if (simple == "ground.type"){
                    string type;
                    simple >> type;
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
                    simple >> type;
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
                            simple >> x >> y;
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
                            simple >> x >> y;
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
                        simple >> x >> y;
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
                } else if (simple == "guardsound"){
                    try{
                        /* FIXME: parse a string and then determine if its S# or just # */
                        simple >> hit.guardHitSound.group;
                        simple >> hit.guardHitSound.item;
                    } catch (const Ast::Exception & e){
                    }
                } else if (simple == "mindist"){
                    const Ast::Value * x;
                    const Ast::Value * y;
                    simple >> x >> y;
                    hit.minimum.x = Compiler::compile(x);
                    hit.minimum.y = Compiler::compile(y);
                } else if (simple == "maxdist"){
                    const Ast::Value * x;
                    const Ast::Value * y;
                    simple >> x >> y;
                    hit.maximum.x = Compiler::compile(x);
                    hit.maximum.y = Compiler::compile(y);
                } else if (simple == "snap"){
                    const Ast::Value * x;
                    const Ast::Value * y;
                    simple >> x >> y;
                    hit.snap.x = Compiler::compile(x);
                    hit.snap.y = Compiler::compile(y);
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
                } else if (simple == "air.fall"){
                    hit.fall.airFall = Compiler::compile(simple.getValue());
                } else if (simple == "forcenofall"){
                    hit.fall.forceNoFall = Compiler::compile(simple.getValue());
                } else {
                    // Global::debug(0) << "Unhandled state controller '" << getName() << "' attribute: " << simple.toString() << endl;
                }
            }
        };

        Walker walker(hit);
        section->walk(walker);
    }

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
#define evaluateNumber(value, default_) (value != NULL ? value->evaluate(env).toNumber() : default_)
#define evaluateBool(value, default_) (value != NULL ? value->evaluate(env).toBool() : default_)
        guy.enableHit();
        guy.nextTicket();
        /* set all the hitdef crap */
        FullEnvironment env(stage, guy, commands);
        HitDefinition & his = guy.getHit();
        his.pause.player2 = evaluateNumber(hit.pause.player2, 0);
        his.groundType = hit.groundType;
        his.airType = hit.airType;
        his.yAcceleration = evaluateNumber(hit.yAcceleration, 0.35);
        his.animationTypeFall = hit.animationTypeFall;
        his.airHitTime = evaluateNumber(hit.airHitTime, 20);
        his.airVelocity.x = evaluateNumber(hit.airVelocity.x, 0);
        his.airVelocity.y = evaluateNumber(hit.airVelocity.y, 0);
        his.fall.fall = evaluateBool(hit.fall.fall, false);
        his.fall.yVelocity = evaluateNumber(hit.fall.yVelocity, -4.5);
        his.fall.envShake.time = evaluateNumber(hit.fall.envShake.time, 0);
        his.groundSlideTime = evaluateNumber(hit.groundSlideTime, 0);
        his.guardControlTime = evaluateNumber(hit.guardControlTime, his.groundSlideTime);
        his.airGuardControlTime = evaluateNumber(hit.airGuardControlTime, his.guardControlTime);
        his.groundVelocity.x = evaluateNumber(hit.groundVelocity.x, 0);
        his.groundVelocity.y = evaluateNumber(hit.groundVelocity.y, 0);
        his.damage.damage = evaluateNumber(hit.damage.damage, 0);
        his.damage.guardDamage = evaluateNumber(hit.damage.guardDamage, 0);
        his.airJuggle = evaluateNumber(hit.airJuggle, 0);
        his.hitSound.own = hit.hitSound.own;
        his.hitSound.group = hit.hitSound.group;
        his.hitSound.item = hit.hitSound.item;
        his.guardHitSound.own = hit.guardHitSound.own;
        his.guardHitSound.group = hit.guardHitSound.group;
        his.guardHitSound.item = hit.guardHitSound.item;
        his.sparkPosition.x = evaluateNumber(hit.sparkPosition.x, 0);
        his.sparkPosition.y = evaluateNumber(hit.sparkPosition.y, 0);
        his.spark = hit.spark;
        his.guardSpark = hit.guardSpark;

#undef evaluateNumber
#undef evaluateBool

    }
};

class ControllerStateTypeSet: public StateController {
public:
    ControllerStateTypeSet(Ast::Section * section, const string & name):
        StateController(name, section),
        changeMoveType(false),
        changeStateType(false),
        changePhysics(false){
            parse(section);
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
                    simple >> type;
                    controller.setMoveType(type);
                } else if (simple == "physics"){
                    string type;
                    simple >> type;
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
                    simple >> type;
                    controller.setStateType(type);
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
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
};

class ControllerSuperPause: public StateController {
public:
    ControllerSuperPause(Ast::Section * section, const string & name):
        StateController(name, section),
        time(NULL),
        posX(NULL),
        posY(NULL),
        animation(NULL),
        darken(NULL),
        p2defmul(NULL),
        poweradd(NULL),
        unhittable(NULL){
            parse(section);
        }

    struct Sound{
        Sound():
            group(-1),
        item(NULL),
        own(false){
        }

        ~Sound(){
            delete item;
        }

        int group;
        Compiler::Value * item;
        bool own;
    };

    Compiler::Value * time;
    Compiler::Value * posX;
    Compiler::Value * posY;
    Compiler::Value * animation;
    Compiler::Value * darken;
    Compiler::Value * p2defmul;
    Compiler::Value * poweradd;
    Compiler::Value * unhittable;
    Sound sound;

    virtual ~ControllerSuperPause(){
        delete time;
        delete posX;
        delete posY;
        delete animation;
        delete darken;
        delete p2defmul;
        delete poweradd;
        delete unhittable;
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
                    super.animation = Compiler::compile(simple.getValue());
                } else if (simple == "darken"){
                    super.darken = Compiler::compile(simple.getValue());
                } else if (simple == "p2defmul"){
                    super.p2defmul = Compiler::compile(simple.getValue());
                } else if (simple == "poweradd"){
                    super.poweradd = Compiler::compile(simple.getValue());
                } else if (simple == "unhittable"){
                    super.unhittable = Compiler::compile(simple.getValue());
                } else if (simple == "sound"){
                    if (simple.getValue()->hasMultiple()){
                        string first;
                        bool own = false;
                        int group;
                        const Ast::Value * item;
                        simple >> first >> item;
                        if (first[0] == 'S'){
                            own = true;
                            group = atoi(first.substr(1).c_str());
                        } else {
                            group = atoi(first.c_str());
                        }
                        super.sound.own = own;
                        super.sound.group = group;
                        super.sound.item = Compiler::compile(item);
                    }
                } else if (simple == "pos"){
                    if (simple.getValue()->hasMultiple()){
                        const Ast::Value * posX;
                        const Ast::Value * posY;
                        simple >> posX >> posY;
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
            pos = (int) posX->evaluate(env).toNumber() * (guy.getFacing() == Object::FACING_LEFT ? -1 : 1);
        }
        return guy.getRX() + pos;
    }

    int computeY(Character & guy, const Environment & env) const {
        int pos = 0;
        if (posY != NULL){
            pos = (int) posY->evaluate(env).toNumber();
        }
        return guy.getRY() + pos;
    }

    void playSound(Character & guy, const Environment & environment) const {
        MugenSound * sound = NULL;
        if (this->sound.item != NULL){
            int itemNumber = (int) this->sound.item->evaluate(environment).toNumber();
            if (this->sound.own){
                sound = guy.getCommonSound(this->sound.group, itemNumber);
            } else {
                sound = guy.getSound(this->sound.group, itemNumber);
            }
        }

        if (sound != NULL){
            sound->play();
        }
    }

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment env(stage, guy);
        int x = computeX(guy, env);
        int y = computeY(guy, env);
        /* 30 is the default */
        int time = 30;
        if (this->time != NULL){
            time = (int) this->time->evaluate(env).toNumber();
        }
        int animation = 30;
        if (this->animation != NULL){
            animation = (int) this->animation->evaluate(env).toNumber();
        }

        /* FIXME: handle darken, p2defmul, unhittable */

        playSound(guy, env);
        stage.doSuperPause(time, animation, x, y);
    }
};

class ControllerAfterImage: public StateController {
public:
    ControllerAfterImage(Ast::Section * section, const string & name):
        StateController(name, section),
        time(NULL),
        length(NULL),
        timeGap(NULL),
        frameGap(NULL){
            parse(section);
        }

    Compiler::Value * time;
    Compiler::Value * length;
    Compiler::Value * timeGap;
    Compiler::Value * frameGap;

    virtual ~ControllerAfterImage(){
        delete time;
        delete length;
        delete timeGap;
        delete frameGap;
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
                } else if (simple == "palinvertall"){
                } else if (simple == "palbright"){
                } else if (simple == "palcontrast"){
                } else if (simple == "palpostbright"){
                } else if (simple == "paladd"){
                } else if (simple == "palmul"){
                } else if (simple == "timegap"){
                    image.timeGap = Compiler::compile(simple.getValue());
                } else if (simple == "framegap"){
                    image.frameGap = Compiler::compile(simple.getValue());
                } else if (simple == "trans"){
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

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment environment(stage, guy, commands);
        int timegap = 1;
        if (this->timeGap != NULL){
            timegap = this->timeGap->evaluate(environment).toNumber();
        }

        int framegap = 1;
        if (this->frameGap != NULL){
            framegap = this->frameGap->evaluate(environment).toNumber();
        }

        int time = 1;
        if (this->time != NULL){
            time = this->time->evaluate(environment).toNumber();
        }

        int length = 20;
        if (this->length != NULL){
            length = this->length->evaluate(environment).toNumber();
        }

        /* FIXME: handle palette and translucency */
        guy.setAfterImage(time, length, timegap, framegap);
    }
};

class ControllerAfterImageTime: public StateController {
public:
    ControllerAfterImageTime(Ast::Section * section, const string & name):
        StateController(name, section),
        time(NULL){
            parse(section);
        }

    Compiler::Value * time;

    virtual ~ControllerAfterImageTime(){
        delete time;
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(Compiler::Value *& time):
                time(time){
                }

            Compiler::Value *& time;

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
            throw MugenException("Time not specified for AfterImageTime");
        }
    }

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment environment(stage, guy);
        int time = this->time->evaluate(environment).toNumber();
        guy.setAfterImageTime(time);
    }
};

class ControllerAngleAdd: public StateController {
public:
    ControllerAngleAdd(Ast::Section * section, const string & name):
    StateController(name, section),
    value(NULL){
        parse(section);
    }

    Compiler::Value * value;

    virtual ~ControllerAngleAdd(){
        delete value;
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(Compiler::Value *& value):
                value(value){
                }

            Compiler::Value *& value;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "value"){
                    value = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(value);
        section->walk(walker);
        if (value == NULL){
            throw MugenException("`value' is a required attribute of AngleAdd");
        }
    }

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
        int value = this->value->evaluate(FullEnvironment(stage, guy)).toNumber();
        guy.updateAngleEffect(value + guy.getAngleEffect());
    }
};

class ControllerAngleMul: public StateController {
public:
    ControllerAngleMul(Ast::Section * section, const string & name):
    StateController(name, section),
    value(NULL){
        parse(section);
    }

    Compiler::Value * value;

    virtual ~ControllerAngleMul(){
        delete value;
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(Compiler::Value *& value):
                value(value){
                }

            Compiler::Value *& value;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "value"){
                    value = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(value);
        section->walk(walker);
        if (value == NULL){
            throw MugenException("`value' is a required attribute of AngleMul");
        }
    }

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
        double value = this->value->evaluate(FullEnvironment(stage, guy)).toNumber();
        guy.updateAngleEffect(guy.getAngleEffect() * value);
    }
};

class ControllerAngleSet: public StateController {
public:
    ControllerAngleSet(Ast::Section * section, const string & name):
    StateController(name, section),
    value(NULL){
        parse(section);
    }

    Compiler::Value * value;

    virtual ~ControllerAngleSet(){
        delete value;
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(Compiler::Value *& value):
                value(value){
                }

            Compiler::Value *& value;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "value"){
                    value = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(value);
        section->walk(walker);
        if (value == NULL){
            throw MugenException("`value' is a required attribute of AngleMul");
        }
    }

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
        double value = this->value->evaluate(FullEnvironment(stage, guy)).toNumber();
        guy.updateAngleEffect(value);
    }
};

class ControllerAngleDraw: public StateController {
public:
    ControllerAngleDraw(Ast::Section * section, const string & name):
    StateController(name, section),
    value(NULL),
    scaleX(NULL),
    scaleY(NULL){
        parse(section);
    }

    Compiler::Value * value;
    Compiler::Value * scaleX;
    Compiler::Value * scaleY;

    virtual ~ControllerAngleDraw(){
        delete value;
        delete scaleX;
        delete scaleY;
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(Compiler::Value *& value, Compiler::Value *& scaleX, Compiler::Value *& scaleY):
                value(value),
                scaleX(scaleX),
                scaleY(scaleY){
                }

            Compiler::Value *& value;
            Compiler::Value *& scaleX;
            Compiler::Value *& scaleY;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "value"){
                    value = Compiler::compile(simple.getValue());
                } else if (simple == "scale"){
                    const Ast::Value * x = NULL;
                    const Ast::Value * y = NULL;
                    simple >> x >> y;
                    scaleX = Compiler::compile(x);
                    scaleY = Compiler::compile(y);
                }
            }
        };

        Walker walker(value, scaleX, scaleY);
        section->walk(walker);
    }

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
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
};

class ControllerAssertSpecial: public StateController {
public:
    ControllerAssertSpecial(Ast::Section * section, const string & name):
    StateController(name, section){
        parse(section);
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
                    simple >> what;
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

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
        for (vector<Character::Specials>::const_iterator it = asserts.begin(); it != asserts.end(); it++){
            Character::Specials special = *it;
            guy.assertSpecial(special);
        }
    }
};

class ControllerAttackDist: public StateController {
public:
    ControllerAttackDist(Ast::Section * section, const string & name):
    StateController(name, section),
    value(NULL){
        parse(section);
    }

    Compiler::Value * value;

    virtual ~ControllerAttackDist(){
        delete value;
    }

    void parse(Ast::Section * section){
        extractValue(value, section);
        if (value == NULL){
            throw MugenException("AttackDist must have a `value' attribute");
        }
    }

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
        int value = (int) this->value->evaluate(FullEnvironment(stage, guy)).toNumber();
        guy.getHit().guardDistance = value;
    }
};

class ControllerNull: public StateController {
public:
    ControllerNull(const string & name):
    StateController(name){
    }

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
        /* nothing */
    }
};

class ControllerTurn: public StateController {
public:
    ControllerTurn(Ast::Section * section, const string & name):
    StateController(name, section){
    }

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
        guy.doTurn(stage);
    }
};

class ControllerVarAdd: public StateController {
public:
    ControllerVarAdd(Ast::Section * section, const string & name):
    StateController(name, section),
    integerIndex(NULL),
    floatIndex(NULL),
    sysIndex(NULL),
    value(NULL){
        parse(section, name);
    }

    Compiler::Value * integerIndex;
    Compiler::Value * floatIndex;
    Compiler::Value * sysIndex;
    Compiler::Value * value;

    virtual ~ControllerVarAdd(){
        delete integerIndex;
        delete floatIndex;
        delete sysIndex;
        delete value;
    }

    void parse(Ast::Section * section, const string & name){
        class Walker: public Ast::Walker {
        public:
            Walker(Compiler::Value *& integerIndex, Compiler::Value *& floatIndex, Compiler::Value *& sysIndex, Compiler::Value *& value):
            integerIndex(integerIndex),
            floatIndex(floatIndex),
            sysIndex(sysIndex),
            value(value){
            }

            Compiler::Value *& integerIndex;
            Compiler::Value *& floatIndex;
            Compiler::Value *& sysIndex;
            Compiler::Value *& value;

            virtual void onAttributeArray(const Ast::AttributeArray & simple){
                if (simple == "var"){
                    if (value != NULL){
                        delete value;
                    }
                    if (integerIndex != NULL){
                        delete integerIndex;
                    }
                    int index = simple.getIndex();
                    const Ast::Value * value = simple.getValue();
                    this->value = Compiler::compile(value);
                    this->integerIndex = Compiler::compile(index);
                } else if (simple == "fvar"){
                    if (value != NULL){
                        delete value;
                    }
                    if (floatIndex != NULL){
                        delete floatIndex;
                    }
                    int index = simple.getIndex();
                    const Ast::Value * value = simple.getValue();
                    this->value = Compiler::compile(value);
                    this->floatIndex = Compiler::compile(index);
                } else if (simple == "sysvar"){
                    if (value != NULL){
                        delete value;
                    }
                    if (sysIndex != NULL){
                        delete sysIndex;
                    }
                    int index = simple.getIndex();
                    const Ast::Value * value = simple.getValue();
                    this->value = Compiler::compile(value);
                    this->sysIndex = Compiler::compile(index);
                }
            }

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "v"){
                    if (integerIndex != NULL){
                        delete integerIndex;
                    }
                    integerIndex = Compiler::compile(simple.getValue());
                } else if (simple == "fv"){
                    if (floatIndex != NULL){
                        delete floatIndex;
                    }
                    floatIndex = Compiler::compile(simple.getValue());
                } else if (simple == "value"){
                    if (value != NULL){
                        delete value;
                    }
                    value = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(integerIndex, floatIndex, sysIndex, value);
        section->walk(walker);
        if (value == NULL){
            throw MugenException("Must set `value' for controller VarAdd " + name);
        }
    }

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
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
};

class ControllerForceFeedback: public StateController {
public:
    ControllerForceFeedback(Ast::Section * section, const string & name):
    StateController(name){
    }

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
        /* TODO, if we care about this controller */
    }
};

class ControllerWidth: public StateController {
public:
    ControllerWidth(Ast::Section * section, const string & name):
    StateController(name, section),
    edgeFront(NULL),
    edgeBack(NULL),
    playerFront(NULL),
    playerBack(NULL){
        parse(section);
    }

    Compiler::Value * edgeFront;
    Compiler::Value * edgeBack;
    Compiler::Value * playerFront;
    Compiler::Value * playerBack;

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(Compiler::Value * edgeFront,
                   Compiler::Value * edgeBack,
                   Compiler::Value * playerFront,
                   Compiler::Value * playerBack):
                   edgeFront(edgeFront),
                   edgeBack(edgeBack),
                   playerFront(playerFront),
                   playerBack(playerBack){
                   }

            Compiler::Value *& edgeFront;
            Compiler::Value *& edgeBack;
            Compiler::Value *& playerFront;
            Compiler::Value *& playerBack;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "edge"){
                    const Ast::Value * front;
                    const Ast::Value * back;
                    simple >> front >> back;
                    edgeFront = Compiler::compile(front);
                    edgeBack = Compiler::compile(back);
                } else if (simple == "player"){
                    const Ast::Value * front;
                    const Ast::Value * back;
                    simple >> front >> back;
                    playerFront = Compiler::compile(front);
                    playerBack = Compiler::compile(back);
                } else if (simple == "value"){
                    const Ast::Value * front;
                    const Ast::Value * back;
                    simple >> front >> back;
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
        delete edgeFront;
        delete edgeBack;
        delete playerFront;
        delete playerBack;
    }

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
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
};

class ControllerMakeDust: public StateController {
public:
    ControllerMakeDust(Ast::Section * section, const string & name):
    StateController(name, section),
    posX(NULL),
    posY(NULL),
    posX2(NULL),
    posY2(NULL),
    spacing(NULL){
        parse(section);
    }

    Compiler::Value * posX;
    Compiler::Value * posY;
    Compiler::Value * posX2;
    Compiler::Value * posY2;
    Compiler::Value * spacing;

    virtual ~ControllerMakeDust(){
        delete posX;
        delete posY;
        delete posX2;
        delete posY2;
        delete spacing;
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(Compiler::Value *& posX,
                   Compiler::Value *& posY,
                   Compiler::Value *& posX2,
                   Compiler::Value *& posY2,
                   Compiler::Value *& spacing):
                   posX(posX),
                   posY(posY),
                   posX2(posX2),
                   posY2(posY2),
                   spacing(spacing){
            }

            Compiler::Value *& posX;
            Compiler::Value *& posY;
            Compiler::Value *& posX2;
            Compiler::Value *& posY2;
            Compiler::Value *& spacing;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "pos"){
                    const Ast::Value * x;
                    const Ast::Value * y;
                    simple >> x >> y;
                    posX = Compiler::compile(x);
                    posY = Compiler::compile(y);
                } else if (simple == "pos2"){
                    const Ast::Value * x;
                    const Ast::Value * y;
                    simple >> x >> y;
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

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment environment(stage, guy);
        int x = 0;
        int y = 0;
        if (posX != NULL && posY != NULL){
            x = (int) posX->evaluate(environment).toNumber();
            y = (int) posY->evaluate(environment).toNumber();
        }
        stage.createDust(guy.getRX() + x, guy.getRY() + y);

        if (posX2 != NULL && posY2 != NULL){
            x = (int) posX2->evaluate(environment).toNumber();
            y = (int) posY2->evaluate(environment).toNumber();
            stage.createDust(guy.getRX() + x, guy.getRY() + y);
        }

        /* FIXME: use spacing somehow */
    }
};

class ControllerFallEnvShake: public StateController {
public:
    ControllerFallEnvShake(Ast::Section * section, const string & name):
    StateController(name, section){
    }

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
        HitState & state = guy.getHitState();
        if (state.fall.envShake.time != 0){
            stage.Quake(state.fall.envShake.time);
            state.fall.envShake.time = 0;
        }
    }
};

class ControllerExplod: public StateController {
public:
    ControllerExplod(Ast::Section * section, const string & name):
    StateController(name, section),
    animation(-1),
    ownAnimation(true),
    id(NULL),
    posX(NULL),
    posY(NULL),
    velocityX(NULL),
    velocityY(NULL),
    accelerationX(NULL),
    accelerationY(NULL),
    removeTime(NULL){
        parse(section);
    }

    int animation;
    bool ownAnimation;
    Compiler::Value * id;
    Compiler::Value * posX;
    Compiler::Value * posY;
    Compiler::Value * velocityX;
    Compiler::Value * velocityY;
    Compiler::Value * accelerationX;
    Compiler::Value * accelerationY;
    Compiler::Value * removeTime;

    virtual ~ControllerExplod(){
        delete id;
        delete posX;
        delete posY;
        delete velocityX;
        delete velocityY;
        delete accelerationX;
        delete accelerationY;
        delete removeTime;
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
                    if (PaintownUtil::matchRegex(PaintownUtil::lowerCaseAll(simple.valueAsString()), "f[0-9]+")){
                        controller.ownAnimation = false;
                        controller.animation = atoi(PaintownUtil::captureRegex(PaintownUtil::lowerCaseAll(simple.valueAsString()), "f([0-9]+)", 0).c_str());
                    } else {
                        simple >> controller.animation;
                    }
                } else if (simple == "id"){
                    controller.id = Compiler::compile(simple.getValue());
                } else if (simple == "pos"){
                    const Ast::Value * x;
                    const Ast::Value * y;
                    simple >> x >> y;
                    controller.posX = Compiler::compile(x);
                    controller.posY = Compiler::compile(y);
                } else if (simple == "postype"){
                } else if (simple == "facing"){
                } else if (simple == "vfacing"){
                } else if (simple == "bindtime"){
                } else if (simple == "velocity" || simple == "vel"){
                    const Ast::Value * x;
                    const Ast::Value * y;
                    simple >> x >> y;
                    controller.velocityX = Compiler::compile(x);
                    controller.velocityY = Compiler::compile(y);
                } else if (simple == "accel"){
                    const Ast::Value * x;
                    const Ast::Value * y;
                    simple >> x >> y;
                    controller.accelerationX = Compiler::compile(x);
                    controller.accelerationY = Compiler::compile(y);
                } else if (simple == "random"){
                } else if (simple == "removetime"){
                    controller.removeTime = Compiler::compile(simple.getValue());
                } else if (simple == "supermove"){
                } else if (simple == "supermovetime"){
                } else if (simple == "pausemovetime"){
                } else if (simple == "scale"){
                } else if (simple == "sprpriority"){
                } else if (simple == "ontop"){
                } else if (simple == "shadow"){
                } else if (simple == "ownpal"){
                } else if (simple == "removeongethit"){
                } else if (simple == "ignorehitpause"){
                } else if (simple == "trans"){
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment env(stage, guy);
#define evaluateNumber(value, default_) (value != NULL ? value->evaluate(env).toNumber() : default_)
        int id_value = (int) evaluateNumber(id, -1);
        double posX_value = evaluateNumber(posX, 0);
        double posY_value = evaluateNumber(posY, 0);
        double velocityX_value = evaluateNumber(velocityX, 0);
        double velocityY_value = evaluateNumber(velocityY, 0);
        double accelerationX_value = evaluateNumber(accelerationX, 0);
        double accelerationY_value = evaluateNumber(accelerationY, 0);
        int removeTime_value = (int) evaluateNumber(removeTime, -2);
#undef evaluateNumber

        MugenAnimation * animation = NULL;
        if (ownAnimation){
            animation = guy.getAnimation(this->animation);
        } else {
            animation = stage.getFightAnimation(this->animation);
        }

        stage.createExplode(animation, id_value, posX_value + guy.getRX(), posY_value + guy.getRY(), velocityX_value, velocityY_value, accelerationX_value, accelerationY_value);
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
    }
    return "???";
}

StateController * StateController::compile(Ast::Section * section, const string & name, int state, StateController::Type type){
    switch (type){
        case StateController::ChangeAnim : return new ControllerChangeAnim(section, name);
        case StateController::ChangeState : return new ControllerChangeState(section, name);
        case StateController::CtrlSet : return new ControllerCtrlSet(section, name);
        case StateController::PlaySnd : return new ControllerPlaySound(section, name);
        case StateController::VarSet : return new ControllerVarSet(section, name);
        case StateController::VelSet : return new ControllerVelSet(section, name);
        case StateController::HitVelSet : return new ControllerHitVelSet(section, name);
        case StateController::PosAdd : return new ControllerPosAdd(section, name);
        case StateController::PosSet : return new ControllerPosSet(section, name);
        case StateController::VelAdd : return new ControllerVelAdd(section, name);
        case StateController::VelMul : return new ControllerVelMul(section, name);
        case StateController::HitDef : return new ControllerHitDef(section, name);
        case StateController::StateTypeSet : return new ControllerStateTypeSet(section, name);
        case StateController::SuperPause : return new ControllerSuperPause(section, name);
        case StateController::AfterImage : return new ControllerAfterImage(section, name);
        case StateController::AfterImageTime : return new ControllerAfterImageTime(section, name);
        case StateController::AngleAdd : return new ControllerAngleAdd(section, name);
        case StateController::AngleDraw : return new ControllerAngleDraw(section, name);
        case StateController::AngleMul : return new ControllerAngleMul(section, name);
        case StateController::AngleSet : return new ControllerAngleSet(section, name);
        case StateController::AssertSpecial : return new ControllerAssertSpecial(section, name);
        case StateController::AttackDist : return new ControllerAttackDist(section, name);
        case StateController::Null : return new ControllerNull(name);
        case StateController::Turn : return new ControllerTurn(section, name);
        case StateController::VarAdd : return new ControllerVarAdd(section, name);
        case StateController::ForceFeedback : return new ControllerForceFeedback(section, name);
        case StateController::Width : return new ControllerWidth(section, name);
        case StateController::MakeDust : return new ControllerMakeDust(section, name);
        case StateController::FallEnvShake : return new ControllerFallEnvShake(section, name);
        case StateController::Explod : return new ControllerExplod(section, name);
        case StateController::AllPalFX :
        case StateController::AppendToClipboard :
        case StateController::AttackMulSet :
        case StateController::BGPalFX :
        case StateController::BindToParent :
        case StateController::BindToRoot :
        case StateController::BindToTarget :
        case StateController::ChangeAnim2 :
        case StateController::ClearClipboard :
        case StateController::DefenceMulSet :
        case StateController::DestroySelf :
        case StateController::DisplayToClipboard :
        case StateController::EnvColor :
        case StateController::EnvShake :
        case StateController::ExplodBindTime :
        case StateController::GameMakeAnim :
        case StateController::Gravity :
        case StateController::Helper :
        case StateController::HitAdd :
        case StateController::HitBy :
        case StateController::HitFallDamage :
        case StateController::HitFallSet :
        case StateController::HitFallVel :
        case StateController::HitOverride :
        case StateController::LifeAdd :
        case StateController::LifeSet :
        case StateController::ModifyExplod :
        case StateController::MoveHitReset :
        case StateController::NotHitBy :
        case StateController::Offset :
        case StateController::PalFX :
        case StateController::ParentVarAdd :
        case StateController::ParentVarSet :
        case StateController::Pause :
        case StateController::PlayerPush :
        case StateController::PosFreeze :
        case StateController::PowerAdd :
        case StateController::PowerSet :
        case StateController::Projectile :
        case StateController::RemoveExplod :
        case StateController::ReversalDef :
        case StateController::ScreenBound :
        case StateController::SelfState :
        case StateController::SprPriority :
        case StateController::SndPan :
        case StateController::StopSnd :
        case StateController::TargetBind :
        case StateController::TargetDrop :
        case StateController::TargetFacing :
        case StateController::TargetLifeAdd :
        case StateController::TargetPowerAdd :
        case StateController::TargetState :
        case StateController::TargetVelAdd :
        case StateController::TargetVelSet :
        case StateController::Trans :
        case StateController::VarRandom :
        case StateController::VarRangeSet : {
            class DefaultController: public StateController {
            public:
                DefaultController(Ast::Section * section, const string & name):
                    StateController(name, section){
                    }

                virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
                    /* nothing */
                }
            };

            Global::debug(0) << "Warning: unhandled state controller for " << toString(type) << " " << name << endl;
            return new DefaultController(section, name);
        }
        case StateController::Unknown : {
            ostringstream out;
            out << "Unknown state controller for " << state << " " << name;
            throw MugenException(out.str());
            break;
        }
            /*
               case InternalCommand : {
               typedef void (Character::*func)(const MugenStage & stage, const vector<string> & inputs);
               func f = (func) controller.internal;
               (guy.*f)(stage, commands);
               break;
               }
               */
    }

    ostringstream out;
    out << "Unknown state controller for " << state << " " << name << " type (" << type << ")";
    throw MugenException(out.str());
}

}
