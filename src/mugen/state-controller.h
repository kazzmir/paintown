#ifndef _paintown_mugen_state_controller_h
#define _paintown_mugen_state_controller_h

#include <map>
#include <vector>
#include <string>
#include "util/pointer.h"

namespace Ast{
    class Section;
    class AttributeSimple;
}

namespace Mugen{

namespace Compiler{
    class Value;
}

class Environment;
class Character;
class Stage;

/* comes from a State */
class StateController{
protected:
    /*
    class CompiledController{
    public:
        CompiledController();
        virtual ~CompiledController();
        virtual void execute(Mugen::Stage & stage, Character & guy, const std::vector<std::string> & commands) = 0;
    };
    */

public:
    StateController(const std::string & name, int state);
    StateController(const std::string & name, int state, Ast::Section * section);
    StateController(const StateController & you);

    /* from scrtls.html or more recently
     * http://elecbyte.com/wiki/index.php/Category:State_Controllers
     */
    enum Type{
        AfterImage,
        AfterImageTime,
        AllPalFX,
        AngleAdd,
        AngleDraw,
        AngleMul,
        AngleSet,
        AppendToClipboard,
        AssertSpecial,
        AttackDist,
        AttackMulSet,
        BGPalFX,
        BindToParent,
        BindToRoot,
        BindToTarget,
        ChangeAnim,
        ChangeAnim2,
        ChangeState,
        ClearClipboard,
        CtrlSet,
        DefenceMulSet,
        DestroySelf,
        DisplayToClipboard,
        EnvColor,
        EnvShake,
        Explod,
        ExplodBindTime,
        ForceFeedback,
        FallEnvShake,
        GameMakeAnim,
        Gravity,
        Helper,
        HitAdd,
        HitBy,
        HitDef,
        HitFallDamage,
        HitFallSet,
        HitFallVel,
        HitOverride,
        HitVelSet,
        LifeAdd,
        LifeSet,
        MakeDust,
        ModifyExplod,
        MoveHitReset,
        NotHitBy,
        Null,
        Offset,
        PalFX,
        ParentVarAdd,
        ParentVarSet,
        Pause,
        PlayerPush,
        PlaySnd,
        PosAdd,
        PosFreeze,
        PosSet,
        PowerAdd,
        PowerSet,
        Projectile,
        RemoveExplod,
        ReversalDef,
        ScreenBound,
        SelfState,
        SprPriority,
        StateTypeSet,
        SndPan,
        StopSnd,
        SuperPause,
        TargetBind,
        TargetDrop,
        TargetFacing,
        TargetLifeAdd,
        TargetPowerAdd,
        TargetState,
        TargetVelAdd,
        TargetVelSet,
        Trans,
        Turn,
        VarAdd,
        VarRandom,
        VarRangeSet,
        VarSet,
        VelAdd,
        VelMul,
        VelSet,
        Width,
        /* Default type value. This value means the `type' attribute has not been set */
        Unknown,
        /* Used as a point to break gdb in */
        Debug
    };
    
    static StateController * compile(Ast::Section * section, const std::string & name, int state, Type type);

    bool canTrigger(const Mugen::Stage & stage, const Character & character, const std::vector<std::string> & commands) const;
    virtual void activate(Mugen::Stage & stage, Character & who, const std::vector<std::string> & commands) const = 0;

    static bool handled(const Ast::AttributeSimple & simple);

    virtual StateController * deepCopy() const = 0;

    virtual inline void setType(Type type){
        this->type = type;
    }

    virtual inline Type getType() const {
        return this->type;
    }

    virtual inline int getState() const {
        return state;
    }

    virtual inline const std::string & getName() const {
        return name;
    }

    virtual void addTriggerAll(Compiler::Value * trigger);
    virtual void addTrigger(int number, Compiler::Value * trigger);

    virtual inline bool getDebug() const {
        return debug;
    }

    virtual inline void setDebug(bool what){
        debug = what;
    }

    virtual void resetPersistent();
    virtual bool persistentOk();

    virtual bool ignoreHitPause(const Environment & environment) const;

    virtual ~StateController();

protected:

    bool canTrigger(const Mugen::Stage & stage, const Character & character, const std::vector<Compiler::Value*> & expressions, const std::vector<std::string> & commands) const;
    bool canTrigger(const Mugen::Stage & stage, const Character & character, const Compiler::Value * expression, const std::vector<std::string> & commands) const;
    std::vector<int> sortTriggers() const;

protected:
    Type type;
    std::string name;
    bool debug;
    
    std::map<int, std::vector<Compiler::Value*> > triggers;

    /* persistent value set in the controller */
    int persistent;
    /* countdown of persistent level */
    int currentPersistent;

    ::Util::ClassPointer<Compiler::Value> ignoreHitPauseValue;

    /* state number */
    int state;

    ::Util::ClassPointer<Compiler::Value> spritePriority;
};

}

#endif
