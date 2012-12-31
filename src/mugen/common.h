#ifndef _paintown_mugen_common_h
#define _paintown_mugen_common_h

/* FIXME: we only need this header to get Graphics::Bitmap::Filter but we can't
 * just forward declare Filter because Bitmap is a class not a namespace.
 * Move Filter to the Graphics namespace so we can forward declare it here
 * and get rid of this include.
 */
#include "util/graphics/bitmap.h"

namespace Mugen{

class Stage;
class Character;

enum Facing{
    FacingLeft, FacingRight
};

/* Wrapper class for a character id (int) */
class CharacterId{
private:
    int id;

public:
    CharacterId();
    CharacterId(const CharacterId & copy);
    explicit CharacterId(int value);
    virtual ~CharacterId();
    bool operator==(const CharacterId &) const;
    bool operator!=(const CharacterId &) const;
    bool operator<(const CharacterId &) const;

    /* Used to pass the raw integer value to mugen script */
    int intValue() const;
};


namespace AttackType{
    extern std::string Normal;
    extern std::string Special;
    extern std::string Hyper;

    enum Attribute{
        NoAttribute,
        NormalAttack,
        NormalThrow,
        NormalProjectile,
        SpecialAttack,
        SpecialThrow,
        SpecialProjectile,
        HyperAttack,
        HyperThrow,
        HyperProjectile
    };

    enum Animation{
        NoAnimation = -1,
        Light = 0,
        Medium = 1,
        Hard = 2,
        Back = 3,
        Up = 4,
        DiagonalUp = 5
    };

    /* Badly named.. */
    enum Ground{
        None = 0,
        High,
        Low,
        Trip
    };
}

struct ResourceEffect{
    ResourceEffect():
        own(false),
        group(-1),
        item(-1){
        }

    bool own;
    int group;
    int item;
};

/* subset of HitDefinition stuff, just for reversals */
struct ReversalData{
    ReversalData():
        sparkX(0),
        sparkY(0),
        player1State(-1),
        player2State(-1),
        player1Pause(0),
        player2Pause(0),
        standing(true),
        crouching(true),
        aerial(true){
        }

    struct PauseTime{
        PauseTime():
            player1(0),
            player2(0){
            }

        int player1;
        int player2;
    } pause;

    ResourceEffect spark;

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

    int sparkX, sparkY;

    int player1State;
    int player2State;

    int player1Pause;
    int player2Pause;

    bool standing;
    bool crouching;
    bool aerial;
    std::vector<AttackType::Attribute> attributes;
};

struct HitDefinition{
    HitDefinition():
    alive(false),
    animationType(AttackType::Light),
    animationTypeAir(AttackType::NoAnimation),
    animationTypeFall(AttackType::NoAnimation),
    groundType(AttackType::None),
    airType(AttackType::None),
    groundHitTime(0),
    airHitTime(20),
    yAcceleration(0.35),
    guardVelocity(0),
    airJuggle(0),
    id(0),
    chainId(-1)
    {}

    bool alive;

    void enable(){
        alive = true;
    }

    bool isEnabled() const {
        return alive;
    }

    void disable(){
        alive = false;
    }

    struct Attribute{
        /* StateType */
        std::string state;
        /* AttackType */
        std::string attackType;
        /* PhysicalAttack */
        std::string physics;
    } attribute;

    struct HitFlags{
        HitFlags():
        high(true),
        low(true),
        air(true),
        fall(true),
        down(false),
        getHitState(false),
        notGetHitState(false){
        }

        void reset(){
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
   
    HitFlags hitFlag;
    HitFlags guardFlag;

    AttackType::Animation animationType;
    AttackType::Animation animationTypeAir;
    AttackType::Animation animationTypeFall;

    struct Priority{
        int hit;
        std::string type;
    } priority;

    struct Damage{
        Damage(){
        }

        int damage;
        int guardDamage;
    } damage;

    struct PauseTime{
        PauseTime():
            player1(0), player2(0){}
        int player1;
        int player2;
    } pause, guardPause;
    
    ResourceEffect spark;
    ResourceEffect guardSpark;
    
    struct SparkPosition{
        int x, y;
    } sparkPosition;

    ResourceEffect hitSound;

    struct GetPower{
        GetPower():
            hit(0),
            guarded(0){
            }
        int hit;
        int guarded;
    } getPower;

    struct GivePower{
        GivePower():
            hit(0),
            guarded(0){
            }
        int hit;
        int guarded;
    } givePower;

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

    struct GroundVelocity{
        GroundVelocity():
            x(0), y(0){}
        double x, y;
    } groundVelocity;

    double guardVelocity;

    struct AirVelocity{
        AirVelocity():
            x(0), y(0){}

        double x, y;
    } airVelocity;

    struct AirGuardVelocity{
        AirGuardVelocity():
            x(0), y(0){}

        double x, y;
    } airGuardVelocity;

    double groundCornerPushoff;
    double airCornerPushoff;
    double downCornerPushoff;
    double guardCornerPushoff;
    double airGuardCornerPushoff;
    int airGuardControlTime;
    int airJuggle;

    int id;
    /* FIXME: handle the chainid when a hitdef is activated */
    int chainId;

    struct Distance{
        int x, y;
    };

    Distance minimum, maximum;
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

    struct Fall{
        Fall():
            yVelocity(0){
            }

        struct Shake{
            int time;
        } envShake;

        int fall;
        double xVelocity;
        double yVelocity;
        bool changeXVelocity;
        bool recover;
        int recoverTime;
        int damage;
        int airFall;
        int forceNoFall;
    } fall;
};

enum TransType{
    None = 0,
    Add,
    Add1,
    Sub,
    AddAlpha,
    Translucent,
    /* used by the Trans controller to specify the trans should not change */
    Default
};

class Effects{
    public:
	Effects();
        Effects(const Effects & copy);
	const Effects &operator=(const Effects &e);
	virtual ~Effects();

        /* adds the mask from the parameter to this object */
        Effects operator+(const Effects &) const;

        /* adds the translucency */
        Effects operator+(const TransType & translucent) const;

        /* what kind of translucency to use */
	TransType trans;

        /* translucency values */
	int alphaSource;
	int alphaDest;

        /* if the masking color should be shown or not, true is not shown */
	bool mask;

        /* horizontal flip. true for flip, false for no flip */
	bool facing;

        /* vertical flip */
	bool vfacing;

        /* scale horizontally */
	double scalex;

        /* scale vertically */
	double scaley;

        double rotation;

        /* clipping. -1 means dont alter clipping */
        struct Dimension{
            Dimension():x1(-1), x2(-1), y1(-1), y2(-1){}

            int x1, x2;
            int y1, y2;
        } dimension;

        Graphics::Bitmap::Filter * filter;
};

struct HitAttributes{
    HitAttributes():
        slot(-1),
        standing(false),
        crouching(false),
        aerial(false){
        }

    HitAttributes(const HitAttributes & copy):
        slot(copy.slot),
        standing(copy.standing),
        crouching(copy.crouching),
        aerial(copy.aerial),
        attributes(copy.attributes){
        }

    int slot;
    bool standing;
    bool crouching;
    bool aerial;

    std::vector<AttackType::Attribute> attributes;
};

struct Constant{
    enum ConstantType{
        None,
        Double,
        ListOfDouble,
    };

    Constant():
    type(None){
    }

    Constant(double value):
    type(Double),
    double_(value){
    }

    Constant(std::vector<double> doubles):
    type(ListOfDouble),
    doubles(doubles){
    }

    ConstantType type;

    double double_;
    std::vector<double> doubles;
};

struct HitState{
    HitState():
        shakeTime(0),
        hitTime(-1),
        hits(0),
        slideTime(0),
        returnControlTime(0),
        recoverTime(0),
        yAcceleration(0),
        yVelocity(0),
        xVelocity(0),
        guarded(false),
        damage(0),
        chainId(-1),
        spritePriority(0),
        moveContact(0){
        }

    void update(Mugen::Stage & stage, const Character & guy, bool inAir, const HitDefinition & hit);
    int shakeTime;
    int hitTime;
    
    /* FIXME: handle hits somehow. corresponds to hitcount
     * hitcount: Returns the number of hits taken by the player in current combo. (int)
     */
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

    struct Fall{
        Fall():
            fall(false),
            recover(true),
            recoverTime(0),
            xVelocity(0),
            yVelocity(0),
            changeXVelocity(false),
            damage(0){
            }

        struct Shake{
            int time;
        } envShake;

        bool fall;
        bool recover;
        int recoverTime;
        double xVelocity;
        double yVelocity;
        bool changeXVelocity;
        double damage;
    } fall;

    int moveContact;
};

struct WinGame{
    /* TODO: add an explanation for each win type that describes how to
     * achieve that state.
     */
    enum WinType{
        Normal,
        Special,
        Hyper,
        NormalThrow,
        Cheese,
        TimeOver,
        Suicide,
        Teammate,
        /* Overlayed */
        /* is this needed now that the `perfect' bool exists? */
        Perfect,
    };

    WinGame():
    type(Normal),
    perfect(false){
    }

    WinType type;
    bool perfect;

};

}

#endif
