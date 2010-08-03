#ifndef _paintown_mugen_state_controller_h
#define _paintown_mugen_state_controller_h

#include <map>
#include <vector>
#include <string>

namespace Ast{
    class Section;
}

class MugenStage;

namespace Mugen{

namespace Compiler{
    class Value;
}

class Character;

/* comes from a State */
class StateController{
protected:
    /*
    class CompiledController{
    public:
        CompiledController();
        virtual ~CompiledController();
        virtual void execute(MugenStage & stage, Character & guy, const std::vector<std::string> & commands) = 0;
    };
    */

public:
    StateController(const std::string & name);
    StateController(const std::string & name, Ast::Section * section);

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
    };
    
    static StateController * compile(Ast::Section * section, const std::string & name, int state, Type type);

    bool canTrigger(const MugenStage & stage, const Character & character, const std::vector<std::string> & commands) const;
    virtual void activate(MugenStage & stage, Character & who, const std::vector<std::string> & commands) const = 0;

    virtual inline void setType(Type type){
        this->type = type;
    }

    virtual inline Type getType() const {
        return this->type;
    }

    virtual inline const std::string & getName() const {
        return name;
    }

    virtual void addTriggerAll(Compiler::Value * trigger);
    virtual void addTrigger(int number, Compiler::Value * trigger);

    virtual inline bool getDebug() const {
        return false;
        // return debug;
    }

    virtual ~StateController();

protected:

    bool canTrigger(const MugenStage & stage, const Character & character, const std::vector<Compiler::Value*> & expressions, const std::vector<std::string> & commands) const;
    bool canTrigger(const MugenStage & stage, const Character & character, const Compiler::Value * expression, const std::vector<std::string> & commands) const;
    std::vector<int> sortTriggers() const;

protected:
    Type type;
    std::string name;
    
    std::map<int, std::vector<Compiler::Value*> > triggers;
};

}

#endif
