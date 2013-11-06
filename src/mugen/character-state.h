
#ifndef _serialize_Mugen_cb7dd6790d2fd9ec77e1f7defc9715c3
#define _serialize_Mugen_cb7dd6790d2fd9ec77e1f7defc9715c3

#include "common.h"
#include "compiler.h"
#include "util/token.h"
#include "serialize.h"
#include "util/graphics/color.h"
#include <map>

class Token;

namespace Mugen{


struct HitAttributes{
    HitAttributes(){
        slot = 0;
        standing = false;
        crouching = false;
        aerial = false;
    }

    int slot;
    bool standing;
    bool crouching;
    bool aerial;
    std::vector<AttackType::Attribute > attributes;
};
Token * serialize(const HitAttributes & data);
HitAttributes deserializeHitAttributes(const Token * data);


struct ResourceEffect{
    ResourceEffect(){
        own = false;
        group = 0;
        item = 0;
    }

    bool own;
    int group;
    int item;
};
Token * serialize(const ResourceEffect & data);
ResourceEffect deserializeResourceEffect(const Token * data);


struct HitFlags{
    HitFlags(){
        high = false;
        low = false;
        air = false;
        fall = false;
        down = false;
        getHitState = false;
        notGetHitState = false;
    }

    bool high;
    bool low;
    bool air;
    bool fall;
    bool down;
    bool getHitState;
    bool notGetHitState;
};
Token * serialize(const HitFlags & data);
HitFlags deserializeHitFlags(const Token * data);


struct PauseTime{
    PauseTime(){
        player1 = 0;
        player2 = 0;
    }

    int player1;
    int player2;
};
Token * serialize(const PauseTime & data);
PauseTime deserializePauseTime(const Token * data);


struct Distance{
    Distance(){
        x = 0;
        y = 0;
    }

    int x;
    int y;
};
Token * serialize(const Distance & data);
Distance deserializeDistance(const Token * data);



struct Attribute{
    Attribute():state("X"), attackType("X"), physics("X"){
        
    }

    std::string state;
    std::string attackType;
    std::string physics;
};
Token * serialize(const Attribute & data);
Attribute deserializeAttribute(const Token * data);


struct Priority{
    Priority(){
        hit = 0;
        type = "X";
    }

    int hit;
    std::string type;
};
Token * serialize(const Priority & data);
Priority deserializePriority(const Token * data);


struct Damage{
    Damage(){
        damage = 0;
        guardDamage = 0;
    }

    int damage;
    int guardDamage;
};
Token * serialize(const Damage & data);
Damage deserializeDamage(const Token * data);


struct SparkPosition{
    SparkPosition(){
        x = 0;
        y = 0;
    }

    int x;
    int y;
};
Token * serialize(const SparkPosition & data);
SparkPosition deserializeSparkPosition(const Token * data);


struct GetPower{
    GetPower(){
        hit = 0;
        guarded = 0;
    }

    int hit;
    int guarded;
};
Token * serialize(const GetPower & data);
GetPower deserializeGetPower(const Token * data);


struct GivePower{
    GivePower(){
        hit = 0;
        guarded = 0;
    }

    int hit;
    int guarded;
};
Token * serialize(const GivePower & data);
GivePower deserializeGivePower(const Token * data);


struct GroundVelocity{
    GroundVelocity(){
        x = 0;
        y = 0;
    }

    double x;
    double y;
};
Token * serialize(const GroundVelocity & data);
GroundVelocity deserializeGroundVelocity(const Token * data);


struct AirVelocity{
    AirVelocity(){
        x = 0;
        y = 0;
    }

    double x;
    double y;
};
Token * serialize(const AirVelocity & data);
AirVelocity deserializeAirVelocity(const Token * data);


struct AirGuardVelocity{
    AirGuardVelocity(){
        x = 0;
        y = 0;
    }

    double x;
    double y;
};
Token * serialize(const AirGuardVelocity & data);
AirGuardVelocity deserializeAirGuardVelocity(const Token * data);



struct Shake{
    Shake(){
        time = 0;
    }

    int time;
};
Token * serialize(const Shake & data);
Shake deserializeShake(const Token * data);

struct Fall{
    Fall(){
        fall = 0;
        xVelocity = 0;
        yVelocity = 0;
        changeXVelocity = false;
        recover = false;
        recoverTime = 0;
        damage = 0;
        airFall = 0;
        forceNoFall = 0;
    }

    Shake envShake;
    int fall;
    double xVelocity;
    double yVelocity;
    bool changeXVelocity;
    bool recover;
    int recoverTime;
    int damage;
    int airFall;
    int forceNoFall;
};
Token * serialize(const Fall & data);
Fall deserializeFall(const Token * data);

struct HitDefinition{
    HitDefinition(){
        alive = false;
        animationType = defaultAttackTypeAnimation();
        animationTypeAir = defaultAttackTypeAnimation();
        animationTypeFall = defaultAttackTypeAnimation();
        groundType = defaultAttackTypeGround();
        airType = defaultAttackTypeGround();
        groundSlideTime = 0;
        guardSlideTime = 0;
        groundHitTime = 0;
        guardGroundHitTime = 0;
        airHitTime = 0;
        guardControlTime = 0;
        guardDistance = 0;
        yAcceleration = 0;
        guardVelocity = 0;
        groundCornerPushoff = 0;
        airCornerPushoff = 0;
        downCornerPushoff = 0;
        guardCornerPushoff = 0;
        airGuardCornerPushoff = 0;
        airGuardControlTime = 0;
        airJuggle = 0;
        id = 0;
        chainId = 0;
        player1SpritePriority = 0;
        player2SpritePriority = 0;
        player1Facing = 0;
        player1GetPlayer2Facing = 0;
        player2Facing = 0;
        player1State = 0;
        player2State = 0;
        player2GetPlayer1State = 0;
        forceStand = 0;
    }

    bool alive;
    Attribute attribute;
    HitFlags hitFlag;
    HitFlags guardFlag;
    AttackType::Animation animationType;
    AttackType::Animation animationTypeAir;
    AttackType::Animation animationTypeFall;
    Priority priority;
    Damage damage;
    PauseTime pause;
    PauseTime guardPause;
    ResourceEffect spark;
    ResourceEffect guardSpark;
    SparkPosition sparkPosition;
    ResourceEffect hitSound;
    GetPower getPower;
    GivePower givePower;
    ResourceEffect guardHitSound;
    AttackType::Ground groundType;
    AttackType::Ground airType;
    int groundSlideTime;
    int guardSlideTime;
    int groundHitTime;
    int guardGroundHitTime;
    int airHitTime;
    int guardControlTime;
    int guardDistance;
    double yAcceleration;
    GroundVelocity groundVelocity;
    double guardVelocity;
    AirVelocity airVelocity;
    AirGuardVelocity airGuardVelocity;
    double groundCornerPushoff;
    double airCornerPushoff;
    double downCornerPushoff;
    double guardCornerPushoff;
    double airGuardCornerPushoff;
    int airGuardControlTime;
    int airJuggle;
    int id;
    int chainId;
    Distance minimum;
    Distance maximum;
    Distance snap;
    int player1SpritePriority;
    int player2SpritePriority;
    int player1Facing;
    int player1GetPlayer2Facing;
    int player2Facing;
    int player1State;
    int player2State;
    int player2GetPlayer1State;
    int forceStand;
    Fall fall;
};
Token * serialize(const HitDefinition & data);
HitDefinition deserializeHitDefinition(const Token * data);


struct HitOverride{
    HitOverride(){
        time = 0;
        state = 0;
        forceAir = false;
    }

    int time;
    HitAttributes attributes;
    int state;
    bool forceAir;
};
Token * serialize(const HitOverride & data);
HitOverride deserializeHitOverride(const Token * data);




struct Shake1{
    Shake1(){
        time = 0;
    }

    int time;
};
Token * serialize(const Shake1 & data);
Shake1 deserializeShake1(const Token * data);

struct Fall1{
    Fall1(){
        fall = false;
        recover = false;
        recoverTime = 0;
        xVelocity = 0;
        yVelocity = 0;
        changeXVelocity = false;
        damage = 0;
    }

    Shake1 envShake;
    bool fall;
    bool recover;
    int recoverTime;
    double xVelocity;
    double yVelocity;
    bool changeXVelocity;
    double damage;
};
Token * serialize(const Fall1 & data);
Fall1 deserializeFall1(const Token * data);

struct HitState{
    HitState(){
        shakeTime = 0;
        hitTime = 0;
        hits = 0;
        slideTime = 0;
        returnControlTime = 0;
        recoverTime = 0;
        yAcceleration = 0;
        yVelocity = 0;
        xVelocity = 0;
        animationType = defaultAttackTypeAnimation();
        airType = defaultAttackTypeGround();
        groundType = defaultAttackTypeGround();
        hitType = defaultAttackTypeGround();
        guarded = false;
        damage = 0;
        chainId = 0;
        spritePriority = 0;
        moveContact = 0;
    }

    int shakeTime;
    int hitTime;
    int hits;
    int slideTime;
    int returnControlTime;
    int recoverTime;
    double yAcceleration;
    double yVelocity;
    double xVelocity;
    AttackType::Animation animationType;
    AttackType::Ground airType;
    AttackType::Ground groundType;
    AttackType::Ground hitType;
    bool guarded;
    int damage;
    int chainId;
    int spritePriority;
    Fall1 fall;
    int moveContact;
};
Token * serialize(const HitState & data);
HitState deserializeHitState(const Token * data);



struct HitSound{
    HitSound(){
        own = false;
        group = 0;
        item = 0;
    }

    bool own;
    int group;
    int item;
};
Token * serialize(const HitSound & data);
HitSound deserializeHitSound(const Token * data);

struct ReversalData{
    ReversalData(){
        sparkX = 0;
        sparkY = 0;
        player1State = 0;
        player2State = 0;
        player1Pause = 0;
        player2Pause = 0;
        standing = false;
        crouching = false;
        aerial = false;
    }

    PauseTime pause;
    ResourceEffect spark;
    HitSound hitSound;
    int sparkX;
    int sparkY;
    int player1State;
    int player2State;
    int player1Pause;
    int player2Pause;
    bool standing;
    bool crouching;
    bool aerial;
    std::vector<AttackType::Attribute > attributes;
};
Token * serialize(const ReversalData & data);
ReversalData deserializeReversalData(const Token * data);



struct WidthOverride{
    WidthOverride(){
        enabled = false;
        edgeFront = 0;
        edgeBack = 0;
        playerFront = 0;
        playerBack = 0;
    }

    bool enabled;
    int edgeFront;
    int edgeBack;
    int playerFront;
    int playerBack;
};
Token * serialize(const WidthOverride & data);
WidthOverride deserializeWidthOverride(const Token * data);


struct HitByOverride{
    HitByOverride(){
        standing = false;
        crouching = false;
        aerial = false;
        time = 0;
    }

    bool standing;
    bool crouching;
    bool aerial;
    int time;
    std::vector<AttackType::Attribute > attributes;
};
Token * serialize(const HitByOverride & data);
HitByOverride deserializeHitByOverride(const Token * data);


struct TransOverride{
    TransOverride(){
        enabled = false;
        type = defaultTransType();
        alphaSource = 0;
        alphaDestination = 0;
    }

    bool enabled;
    TransType type;
    int alphaSource;
    int alphaDestination;
};
Token * serialize(const TransOverride & data);
TransOverride deserializeTransOverride(const Token * data);


struct SpecialStuff{
    SpecialStuff(){
        invisible = false;
        intro = false;
    }

    bool invisible;
    bool intro;
};
Token * serialize(const SpecialStuff & data);
SpecialStuff deserializeSpecialStuff(const Token * data);


struct Bind{
    Bind(){
        bound = defaultCharacterId();
        time = 0;
        facing = 0;
        offsetX = 0;
        offsetY = 0;
    }

    CharacterId bound;
    int time;
    int facing;
    double offsetX;
    double offsetY;
};
Token * serialize(const Bind & data);
Bind deserializeBind(const Token * data);


struct CharacterData{
    CharacterData(){
        who = defaultCharacterId();
        enabled = false;
    }

    CharacterId who;
    bool enabled;
};
Token * serialize(const CharacterData & data);
CharacterData deserializeCharacterData(const Token * data);


struct DrawAngleEffect{
    DrawAngleEffect(){
        enabled = false;
        angle = 0;
        scaleX = 0;
        scaleY = 0;
    }

    bool enabled;
    double angle;
    double scaleX;
    double scaleY;
};
Token * serialize(const DrawAngleEffect & data);
DrawAngleEffect deserializeDrawAngleEffect(const Token * data);

struct StateData{
    StateData(){
        juggleRemaining = 0;
        currentJuggle = 0;
        currentState = 0;
        previousState = 0;
        currentAnimation = 0;
        velocity_x = 0;
        velocity_y = 0;
        has_control = false;
        stateTime = 0;
        currentPhysics = defaultPhysicsType();
        combo = 0;
        hitCount = 0;
        blocking = false;
        guarding = false;
        defenseMultiplier = 0;
        attackMultiplier = 0;
        frozen = false;
        reversalActive = false;
        pushPlayer = 0;
        health = 0;
        spritePriority = 0;
        wasHitCounter = 0;
        drawAngle = 0;
        virtualx = 0;
        virtualy = 0;
        virtualz = 0;
        facing = defaultFacing();
        power = 0;
    }

    int juggleRemaining;
    int currentJuggle;
    int currentState;
    int previousState;
    int currentAnimation;
    double velocity_x;
    double velocity_y;
    bool has_control;
    int stateTime;
    std::map<int, RuntimeValue > variables;
    std::map<int, RuntimeValue > floatVariables;
    std::map<int, RuntimeValue > systemVariables;
    Physics::Type currentPhysics;
    std::string stateType;
    std::string moveType;
    HitDefinition hit;
    HitState hitState;
    int combo;
    int hitCount;
    bool blocking;
    bool guarding;
    WidthOverride widthOverride;
    HitByOverride hitByOverride[2];
    double defenseMultiplier;
    double attackMultiplier;
    bool frozen;
    ReversalData reversal;
    bool reversalActive;
    TransOverride transOverride;
    int pushPlayer;
    SpecialStuff special;
    double health;
    Bind bind;
    std::map<int, std::vector<CharacterId > > targets;
    int spritePriority;
    uint32_t wasHitCounter;
    CharacterData characterData;
    double drawAngle;
    DrawAngleEffect drawAngleData;
    std::vector<std::string > active;
    std::map<int, HitOverride > hitOverrides;
    double virtualx;
    double virtualy;
    double virtualz;
    Facing facing;
    double power;
    std::map<std::string, std::string > commandState;
};
Token * serialize(const StateData & data);
StateData deserializeStateData(const Token * data);


struct AnimationState{
    AnimationState(){
        position = 0;
        looped = false;
        started = false;
        ticks = 0;
        virtual_ticks = 0;
    }

    uint32_t position;
    bool looped;
    bool started;
    int ticks;
    int virtual_ticks;
};
Token * serialize(const AnimationState & data);
AnimationState deserializeAnimationState(const Token * data);


struct ScreenBound{
    ScreenBound(){
        enabled = false;
        offScreen = false;
        panX = false;
        panY = false;
    }

    bool enabled;
    bool offScreen;
    bool panX;
    bool panY;
};
Token * serialize(const ScreenBound & data);
ScreenBound deserializeScreenBound(const Token * data);



struct Pause{
    Pause(){
        time = 0;
        buffer = 0;
        moveTime = 0;
        pauseBackground = false;
        who = defaultCharacterId();
    }

    int time;
    int buffer;
    int moveTime;
    bool pauseBackground;
    CharacterId who;
};
Token * serialize(const Pause & data);
Pause deserializePause(const Token * data);


struct Zoom{
    Zoom(){
        enabled = false;
        x = 0;
        y = 0;
        zoomTime = 0;
        zoomOutTime = 0;
        zoom = 0;
        in = false;
        time = 0;
        bindTime = 0;
        deltaX = 0;
        deltaY = 0;
        scaleX = 0;
        scaleY = 0;
        velocityX = 0;
        velocityY = 0;
        accelX = 0;
        accelY = 0;
        superMoveTime = 0;
        pauseMoveTime = 0;
        removeOnGetHit = false;
        hitCount = 0;
        bound = defaultCharacterId();
        owner = defaultCharacterId();
    }

    bool enabled;
    double x;
    double y;
    int zoomTime;
    int zoomOutTime;
    int zoom;
    bool in;
    int time;
    int bindTime;
    int deltaX;
    int deltaY;
    double scaleX;
    double scaleY;
    double velocityX;
    double velocityY;
    double accelX;
    double accelY;
    int superMoveTime;
    int pauseMoveTime;
    bool removeOnGetHit;
    uint32_t hitCount;
    CharacterId bound;
    CharacterId owner;
};
Token * serialize(const Zoom & data);
Zoom deserializeZoom(const Token * data);


struct EnvironmentColor{
    EnvironmentColor(){
        color = defaultGraphicsColor();
        time = 0;
        under = false;
    }

    Graphics::Color color;
    int time;
    bool under;
};
Token * serialize(const EnvironmentColor & data);
EnvironmentColor deserializeEnvironmentColor(const Token * data);


struct SuperPause{
    SuperPause(){
        time = 0;
        positionX = 0;
        positionY = 0;
        soundGroup = 0;
        soundItem = 0;
    }

    int time;
    int positionX;
    int positionY;
    int soundGroup;
    int soundItem;
};
Token * serialize(const SuperPause & data);
SuperPause deserializeSuperPause(const Token * data);

struct StageStateData{
    StageStateData(){
        quake_time = 0;
        cycles = 0;
        inleft = 0;
        inright = 0;
        onLeftSide = 0;
        onRightSide = 0;
        inabove = 0;
        camerax = 0;
        cameray = 0;
        ticker = 0;
        gameRate = 0;
    }

    Pause pause;
    std::map<CharacterId, ScreenBound > screenBound;
    Zoom zoom;
    EnvironmentColor environmentColor;
    SuperPause superPause;
    int quake_time;
    int cycles;
    int inleft;
    int inright;
    int onLeftSide;
    int onRightSide;
    int inabove;
    double camerax;
    double cameray;
    uint32_t ticker;
    double gameRate;
};
Token * serialize(const StageStateData & data);
StageStateData deserializeStageStateData(const Token * data);


struct PlayerData{
    PlayerData(){
        oldx = 0;
        oldy = 0;
        leftTension = false;
        rightTension = false;
        leftSide = false;
        rightSide = false;
        above = false;
        jumped = false;
    }

    double oldx;
    double oldy;
    bool leftTension;
    bool rightTension;
    bool leftSide;
    bool rightSide;
    bool above;
    bool jumped;
};
Token * serialize(const PlayerData & data);
PlayerData deserializePlayerData(const Token * data);

}

#endif

