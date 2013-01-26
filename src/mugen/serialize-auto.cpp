
#include "character-state.h"

namespace Mugen{


Token * serialize(const HitAttributes & data){
    Token * out = new Token();
    *out << "HitAttributes";
    if (data.slot != 0){
        *out->newToken() << "slot" << data.slot;
    }
    if (data.standing != false){
        *out->newToken() << "standing" << data.standing;
    }
    if (data.crouching != false){
        *out->newToken() << "crouching" << data.crouching;
    }
    if (data.aerial != false){
        *out->newToken() << "aerial" << data.aerial;
    }

    Token * t1 = out->newToken();
    *t1 << "attributes";
    for (std::vector<AttackType::Attribute >::const_iterator it = data.attributes.begin(); it != data.attributes.end(); it++){
        *t1 << serialize(*it);
    }
    
    return out;
}

HitAttributes deserializeHitAttributes(const Token * data){
    HitAttributes out;
    const Token * use = NULL;
    use = data->findToken("_/slot");
    if (use != NULL){
        use->view() >> out.slot;
    }
    use = data->findToken("_/standing");
    if (use != NULL){
        use->view() >> out.standing;
    }
    use = data->findToken("_/crouching");
    if (use != NULL){
        use->view() >> out.crouching;
    }
    use = data->findToken("_/aerial");
    if (use != NULL){
        use->view() >> out.aerial;
    }
    use = data->findToken("_/attributes");
    if (use != NULL){
        for (TokenView view = use->view(); view.hasMore(); /**/){
            out.attributes.push_back(deserializeAttackTypeAttribute(view.next()));
        }

    }

    return out;
}


Token * serialize(const ResourceEffect & data){
    Token * out = new Token();
    *out << "ResourceEffect";
    if (data.own != false){
        *out->newToken() << "own" << data.own;
    }
    if (data.group != 0){
        *out->newToken() << "group" << data.group;
    }
    if (data.item != 0){
        *out->newToken() << "item" << data.item;
    }

    return out;
}

ResourceEffect deserializeResourceEffect(const Token * data){
    ResourceEffect out;
    const Token * use = NULL;
    use = data->findToken("_/own");
    if (use != NULL){
        use->view() >> out.own;
    }
    use = data->findToken("_/group");
    if (use != NULL){
        use->view() >> out.group;
    }
    use = data->findToken("_/item");
    if (use != NULL){
        use->view() >> out.item;
    }

    return out;
}


Token * serialize(const HitFlags & data){
    Token * out = new Token();
    *out << "HitFlags";
    if (data.high != false){
        *out->newToken() << "high" << data.high;
    }
    if (data.low != false){
        *out->newToken() << "low" << data.low;
    }
    if (data.air != false){
        *out->newToken() << "air" << data.air;
    }
    if (data.fall != false){
        *out->newToken() << "fall" << data.fall;
    }
    if (data.down != false){
        *out->newToken() << "down" << data.down;
    }
    if (data.getHitState != false){
        *out->newToken() << "getHitState" << data.getHitState;
    }
    if (data.notGetHitState != false){
        *out->newToken() << "notGetHitState" << data.notGetHitState;
    }

    return out;
}

HitFlags deserializeHitFlags(const Token * data){
    HitFlags out;
    const Token * use = NULL;
    use = data->findToken("_/high");
    if (use != NULL){
        use->view() >> out.high;
    }
    use = data->findToken("_/low");
    if (use != NULL){
        use->view() >> out.low;
    }
    use = data->findToken("_/air");
    if (use != NULL){
        use->view() >> out.air;
    }
    use = data->findToken("_/fall");
    if (use != NULL){
        use->view() >> out.fall;
    }
    use = data->findToken("_/down");
    if (use != NULL){
        use->view() >> out.down;
    }
    use = data->findToken("_/getHitState");
    if (use != NULL){
        use->view() >> out.getHitState;
    }
    use = data->findToken("_/notGetHitState");
    if (use != NULL){
        use->view() >> out.notGetHitState;
    }

    return out;
}


Token * serialize(const PauseTime & data){
    Token * out = new Token();
    *out << "PauseTime";
    if (data.player1 != 0){
        *out->newToken() << "player1" << data.player1;
    }
    if (data.player2 != 0){
        *out->newToken() << "player2" << data.player2;
    }

    return out;
}

PauseTime deserializePauseTime(const Token * data){
    PauseTime out;
    const Token * use = NULL;
    use = data->findToken("_/player1");
    if (use != NULL){
        use->view() >> out.player1;
    }
    use = data->findToken("_/player2");
    if (use != NULL){
        use->view() >> out.player2;
    }

    return out;
}


Token * serialize(const Distance & data){
    Token * out = new Token();
    *out << "Distance";
    if (data.x != 0){
        *out->newToken() << "x" << data.x;
    }
    if (data.y != 0){
        *out->newToken() << "y" << data.y;
    }

    return out;
}

Distance deserializeDistance(const Token * data){
    Distance out;
    const Token * use = NULL;
    use = data->findToken("_/x");
    if (use != NULL){
        use->view() >> out.x;
    }
    use = data->findToken("_/y");
    if (use != NULL){
        use->view() >> out.y;
    }

    return out;
}



Token * serialize(const Attribute & data){
    Token * out = new Token();
    *out << "Attribute";
*out->newToken() << "state" << serialize(data.state);
*out->newToken() << "attackType" << serialize(data.attackType);
*out->newToken() << "physics" << serialize(data.physics);

    return out;
}

Attribute deserializeAttribute(const Token * data){
    Attribute out;
    const Token * use = NULL;
    use = data->findToken("_/state");
    if (use != NULL){
        use->view() >> out.state;
    }
    use = data->findToken("_/attackType");
    if (use != NULL){
        use->view() >> out.attackType;
    }
    use = data->findToken("_/physics");
    if (use != NULL){
        use->view() >> out.physics;
    }

    return out;
}


Token * serialize(const Priority & data){
    Token * out = new Token();
    *out << "Priority";
    if (data.hit != 0){
        *out->newToken() << "hit" << data.hit;
    }
*out->newToken() << "type" << serialize(data.type);

    return out;
}

Priority deserializePriority(const Token * data){
    Priority out;
    const Token * use = NULL;
    use = data->findToken("_/hit");
    if (use != NULL){
        use->view() >> out.hit;
    }
    use = data->findToken("_/type");
    if (use != NULL){
        use->view() >> out.type;
    }

    return out;
}


Token * serialize(const Damage & data){
    Token * out = new Token();
    *out << "Damage";
    if (data.damage != 0){
        *out->newToken() << "damage" << data.damage;
    }
    if (data.guardDamage != 0){
        *out->newToken() << "guardDamage" << data.guardDamage;
    }

    return out;
}

Damage deserializeDamage(const Token * data){
    Damage out;
    const Token * use = NULL;
    use = data->findToken("_/damage");
    if (use != NULL){
        use->view() >> out.damage;
    }
    use = data->findToken("_/guardDamage");
    if (use != NULL){
        use->view() >> out.guardDamage;
    }

    return out;
}


Token * serialize(const SparkPosition & data){
    Token * out = new Token();
    *out << "SparkPosition";
    if (data.x != 0){
        *out->newToken() << "x" << data.x;
    }
    if (data.y != 0){
        *out->newToken() << "y" << data.y;
    }

    return out;
}

SparkPosition deserializeSparkPosition(const Token * data){
    SparkPosition out;
    const Token * use = NULL;
    use = data->findToken("_/x");
    if (use != NULL){
        use->view() >> out.x;
    }
    use = data->findToken("_/y");
    if (use != NULL){
        use->view() >> out.y;
    }

    return out;
}


Token * serialize(const GetPower & data){
    Token * out = new Token();
    *out << "GetPower";
    if (data.hit != 0){
        *out->newToken() << "hit" << data.hit;
    }
    if (data.guarded != 0){
        *out->newToken() << "guarded" << data.guarded;
    }

    return out;
}

GetPower deserializeGetPower(const Token * data){
    GetPower out;
    const Token * use = NULL;
    use = data->findToken("_/hit");
    if (use != NULL){
        use->view() >> out.hit;
    }
    use = data->findToken("_/guarded");
    if (use != NULL){
        use->view() >> out.guarded;
    }

    return out;
}


Token * serialize(const GivePower & data){
    Token * out = new Token();
    *out << "GivePower";
    if (data.hit != 0){
        *out->newToken() << "hit" << data.hit;
    }
    if (data.guarded != 0){
        *out->newToken() << "guarded" << data.guarded;
    }

    return out;
}

GivePower deserializeGivePower(const Token * data){
    GivePower out;
    const Token * use = NULL;
    use = data->findToken("_/hit");
    if (use != NULL){
        use->view() >> out.hit;
    }
    use = data->findToken("_/guarded");
    if (use != NULL){
        use->view() >> out.guarded;
    }

    return out;
}


Token * serialize(const GroundVelocity & data){
    Token * out = new Token();
    *out << "GroundVelocity";
    if (data.x != 0){
        *out->newToken() << "x" << data.x;
    }
    if (data.y != 0){
        *out->newToken() << "y" << data.y;
    }

    return out;
}

GroundVelocity deserializeGroundVelocity(const Token * data){
    GroundVelocity out;
    const Token * use = NULL;
    use = data->findToken("_/x");
    if (use != NULL){
        use->view() >> out.x;
    }
    use = data->findToken("_/y");
    if (use != NULL){
        use->view() >> out.y;
    }

    return out;
}


Token * serialize(const AirVelocity & data){
    Token * out = new Token();
    *out << "AirVelocity";
    if (data.x != 0){
        *out->newToken() << "x" << data.x;
    }
    if (data.y != 0){
        *out->newToken() << "y" << data.y;
    }

    return out;
}

AirVelocity deserializeAirVelocity(const Token * data){
    AirVelocity out;
    const Token * use = NULL;
    use = data->findToken("_/x");
    if (use != NULL){
        use->view() >> out.x;
    }
    use = data->findToken("_/y");
    if (use != NULL){
        use->view() >> out.y;
    }

    return out;
}


Token * serialize(const AirGuardVelocity & data){
    Token * out = new Token();
    *out << "AirGuardVelocity";
    if (data.x != 0){
        *out->newToken() << "x" << data.x;
    }
    if (data.y != 0){
        *out->newToken() << "y" << data.y;
    }

    return out;
}

AirGuardVelocity deserializeAirGuardVelocity(const Token * data){
    AirGuardVelocity out;
    const Token * use = NULL;
    use = data->findToken("_/x");
    if (use != NULL){
        use->view() >> out.x;
    }
    use = data->findToken("_/y");
    if (use != NULL){
        use->view() >> out.y;
    }

    return out;
}



Token * serialize(const Shake & data){
    Token * out = new Token();
    *out << "Shake";
    if (data.time != 0){
        *out->newToken() << "time" << data.time;
    }

    return out;
}

Shake deserializeShake(const Token * data){
    Shake out;
    const Token * use = NULL;
    use = data->findToken("_/time");
    if (use != NULL){
        use->view() >> out.time;
    }

    return out;
}

Token * serialize(const Fall & data){
    Token * out = new Token();
    *out << "Fall";
    *out->newToken() << "envShake" << serialize(data.envShake);
    if (data.fall != 0){
        *out->newToken() << "fall" << data.fall;
    }
    if (data.xVelocity != 0){
        *out->newToken() << "xVelocity" << data.xVelocity;
    }
    if (data.yVelocity != 0){
        *out->newToken() << "yVelocity" << data.yVelocity;
    }
    if (data.changeXVelocity != false){
        *out->newToken() << "changeXVelocity" << data.changeXVelocity;
    }
    if (data.recover != false){
        *out->newToken() << "recover" << data.recover;
    }
    if (data.recoverTime != 0){
        *out->newToken() << "recoverTime" << data.recoverTime;
    }
    if (data.damage != 0){
        *out->newToken() << "damage" << data.damage;
    }
    if (data.airFall != 0){
        *out->newToken() << "airFall" << data.airFall;
    }
    if (data.forceNoFall != 0){
        *out->newToken() << "forceNoFall" << data.forceNoFall;
    }

    return out;
}

Fall deserializeFall(const Token * data){
    Fall out;
    const Token * use = NULL;
    use = data->findToken("_/envShake");
    if (use != NULL){
        const Token * child;
        use->view() >> child;
        out.envShake = deserializeShake(child);
    }
    use = data->findToken("_/fall");
    if (use != NULL){
        use->view() >> out.fall;
    }
    use = data->findToken("_/xVelocity");
    if (use != NULL){
        use->view() >> out.xVelocity;
    }
    use = data->findToken("_/yVelocity");
    if (use != NULL){
        use->view() >> out.yVelocity;
    }
    use = data->findToken("_/changeXVelocity");
    if (use != NULL){
        use->view() >> out.changeXVelocity;
    }
    use = data->findToken("_/recover");
    if (use != NULL){
        use->view() >> out.recover;
    }
    use = data->findToken("_/recoverTime");
    if (use != NULL){
        use->view() >> out.recoverTime;
    }
    use = data->findToken("_/damage");
    if (use != NULL){
        use->view() >> out.damage;
    }
    use = data->findToken("_/airFall");
    if (use != NULL){
        use->view() >> out.airFall;
    }
    use = data->findToken("_/forceNoFall");
    if (use != NULL){
        use->view() >> out.forceNoFall;
    }

    return out;
}

Token * serialize(const HitDefinition & data){
    Token * out = new Token();
    *out << "HitDefinition";
    if (data.alive != false){
        *out->newToken() << "alive" << data.alive;
    }
    *out->newToken() << "attribute" << serialize(data.attribute);
*out->newToken() << "hitFlag" << serialize(data.hitFlag);
*out->newToken() << "guardFlag" << serialize(data.guardFlag);
*out->newToken() << "animationType" << serialize(data.animationType);
*out->newToken() << "animationTypeAir" << serialize(data.animationTypeAir);
*out->newToken() << "animationTypeFall" << serialize(data.animationTypeFall);
    *out->newToken() << "priority" << serialize(data.priority);
    *out->newToken() << "damage" << serialize(data.damage);
*out->newToken() << "pause" << serialize(data.pause);
*out->newToken() << "guardPause" << serialize(data.guardPause);
*out->newToken() << "spark" << serialize(data.spark);
*out->newToken() << "guardSpark" << serialize(data.guardSpark);
    *out->newToken() << "sparkPosition" << serialize(data.sparkPosition);
*out->newToken() << "hitSound" << serialize(data.hitSound);
    *out->newToken() << "getPower" << serialize(data.getPower);
    *out->newToken() << "givePower" << serialize(data.givePower);
*out->newToken() << "guardHitSound" << serialize(data.guardHitSound);
*out->newToken() << "groundType" << serialize(data.groundType);
*out->newToken() << "airType" << serialize(data.airType);
    if (data.groundSlideTime != 0){
        *out->newToken() << "groundSlideTime" << data.groundSlideTime;
    }
    if (data.guardSlideTime != 0){
        *out->newToken() << "guardSlideTime" << data.guardSlideTime;
    }
    if (data.groundHitTime != 0){
        *out->newToken() << "groundHitTime" << data.groundHitTime;
    }
    if (data.guardGroundHitTime != 0){
        *out->newToken() << "guardGroundHitTime" << data.guardGroundHitTime;
    }
    if (data.airHitTime != 0){
        *out->newToken() << "airHitTime" << data.airHitTime;
    }
    if (data.guardControlTime != 0){
        *out->newToken() << "guardControlTime" << data.guardControlTime;
    }
    if (data.guardDistance != 0){
        *out->newToken() << "guardDistance" << data.guardDistance;
    }
    if (data.yAcceleration != 0){
        *out->newToken() << "yAcceleration" << data.yAcceleration;
    }
    *out->newToken() << "groundVelocity" << serialize(data.groundVelocity);
    if (data.guardVelocity != 0){
        *out->newToken() << "guardVelocity" << data.guardVelocity;
    }
    *out->newToken() << "airVelocity" << serialize(data.airVelocity);
    *out->newToken() << "airGuardVelocity" << serialize(data.airGuardVelocity);
    if (data.groundCornerPushoff != 0){
        *out->newToken() << "groundCornerPushoff" << data.groundCornerPushoff;
    }
    if (data.airCornerPushoff != 0){
        *out->newToken() << "airCornerPushoff" << data.airCornerPushoff;
    }
    if (data.downCornerPushoff != 0){
        *out->newToken() << "downCornerPushoff" << data.downCornerPushoff;
    }
    if (data.guardCornerPushoff != 0){
        *out->newToken() << "guardCornerPushoff" << data.guardCornerPushoff;
    }
    if (data.airGuardCornerPushoff != 0){
        *out->newToken() << "airGuardCornerPushoff" << data.airGuardCornerPushoff;
    }
    if (data.airGuardControlTime != 0){
        *out->newToken() << "airGuardControlTime" << data.airGuardControlTime;
    }
    if (data.airJuggle != 0){
        *out->newToken() << "airJuggle" << data.airJuggle;
    }
    if (data.id != 0){
        *out->newToken() << "id" << data.id;
    }
    if (data.chainId != 0){
        *out->newToken() << "chainId" << data.chainId;
    }
*out->newToken() << "minimum" << serialize(data.minimum);
*out->newToken() << "maximum" << serialize(data.maximum);
*out->newToken() << "snap" << serialize(data.snap);
    if (data.player1SpritePriority != 0){
        *out->newToken() << "player1SpritePriority" << data.player1SpritePriority;
    }
    if (data.player2SpritePriority != 0){
        *out->newToken() << "player2SpritePriority" << data.player2SpritePriority;
    }
    if (data.player1Facing != 0){
        *out->newToken() << "player1Facing" << data.player1Facing;
    }
    if (data.player1GetPlayer2Facing != 0){
        *out->newToken() << "player1GetPlayer2Facing" << data.player1GetPlayer2Facing;
    }
    if (data.player2Facing != 0){
        *out->newToken() << "player2Facing" << data.player2Facing;
    }
    if (data.player1State != 0){
        *out->newToken() << "player1State" << data.player1State;
    }
    if (data.player2State != 0){
        *out->newToken() << "player2State" << data.player2State;
    }
    if (data.player2GetPlayer1State != 0){
        *out->newToken() << "player2GetPlayer1State" << data.player2GetPlayer1State;
    }
    if (data.forceStand != 0){
        *out->newToken() << "forceStand" << data.forceStand;
    }
    *out->newToken() << "fall" << serialize(data.fall);

    return out;
}

HitDefinition deserializeHitDefinition(const Token * data){
    HitDefinition out;
    const Token * use = NULL;
    use = data->findToken("_/alive");
    if (use != NULL){
        use->view() >> out.alive;
    }
    use = data->findToken("_/attribute");
    if (use != NULL){
        const Token * child;
        use->view() >> child;
        out.attribute = deserializeAttribute(child);
    }
    use = data->findToken("_/hitFlag");
    if (use != NULL){
        out.hitFlag = deserializeHitFlags(use);
    }
    use = data->findToken("_/guardFlag");
    if (use != NULL){
        out.guardFlag = deserializeHitFlags(use);
    }
    use = data->findToken("_/animationType");
    if (use != NULL){
        out.animationType = deserializeAttackTypeAnimation(use);
    }
    use = data->findToken("_/animationTypeAir");
    if (use != NULL){
        out.animationTypeAir = deserializeAttackTypeAnimation(use);
    }
    use = data->findToken("_/animationTypeFall");
    if (use != NULL){
        out.animationTypeFall = deserializeAttackTypeAnimation(use);
    }
    use = data->findToken("_/priority");
    if (use != NULL){
        const Token * child;
        use->view() >> child;
        out.priority = deserializePriority(child);
    }
    use = data->findToken("_/damage");
    if (use != NULL){
        const Token * child;
        use->view() >> child;
        out.damage = deserializeDamage(child);
    }
    use = data->findToken("_/pause");
    if (use != NULL){
        out.pause = deserializePauseTime(use);
    }
    use = data->findToken("_/guardPause");
    if (use != NULL){
        out.guardPause = deserializePauseTime(use);
    }
    use = data->findToken("_/spark");
    if (use != NULL){
        out.spark = deserializeResourceEffect(use);
    }
    use = data->findToken("_/guardSpark");
    if (use != NULL){
        out.guardSpark = deserializeResourceEffect(use);
    }
    use = data->findToken("_/sparkPosition");
    if (use != NULL){
        const Token * child;
        use->view() >> child;
        out.sparkPosition = deserializeSparkPosition(child);
    }
    use = data->findToken("_/hitSound");
    if (use != NULL){
        out.hitSound = deserializeResourceEffect(use);
    }
    use = data->findToken("_/getPower");
    if (use != NULL){
        const Token * child;
        use->view() >> child;
        out.getPower = deserializeGetPower(child);
    }
    use = data->findToken("_/givePower");
    if (use != NULL){
        const Token * child;
        use->view() >> child;
        out.givePower = deserializeGivePower(child);
    }
    use = data->findToken("_/guardHitSound");
    if (use != NULL){
        out.guardHitSound = deserializeResourceEffect(use);
    }
    use = data->findToken("_/groundType");
    if (use != NULL){
        out.groundType = deserializeAttackTypeGround(use);
    }
    use = data->findToken("_/airType");
    if (use != NULL){
        out.airType = deserializeAttackTypeGround(use);
    }
    use = data->findToken("_/groundSlideTime");
    if (use != NULL){
        use->view() >> out.groundSlideTime;
    }
    use = data->findToken("_/guardSlideTime");
    if (use != NULL){
        use->view() >> out.guardSlideTime;
    }
    use = data->findToken("_/groundHitTime");
    if (use != NULL){
        use->view() >> out.groundHitTime;
    }
    use = data->findToken("_/guardGroundHitTime");
    if (use != NULL){
        use->view() >> out.guardGroundHitTime;
    }
    use = data->findToken("_/airHitTime");
    if (use != NULL){
        use->view() >> out.airHitTime;
    }
    use = data->findToken("_/guardControlTime");
    if (use != NULL){
        use->view() >> out.guardControlTime;
    }
    use = data->findToken("_/guardDistance");
    if (use != NULL){
        use->view() >> out.guardDistance;
    }
    use = data->findToken("_/yAcceleration");
    if (use != NULL){
        use->view() >> out.yAcceleration;
    }
    use = data->findToken("_/groundVelocity");
    if (use != NULL){
        const Token * child;
        use->view() >> child;
        out.groundVelocity = deserializeGroundVelocity(child);
    }
    use = data->findToken("_/guardVelocity");
    if (use != NULL){
        use->view() >> out.guardVelocity;
    }
    use = data->findToken("_/airVelocity");
    if (use != NULL){
        const Token * child;
        use->view() >> child;
        out.airVelocity = deserializeAirVelocity(child);
    }
    use = data->findToken("_/airGuardVelocity");
    if (use != NULL){
        const Token * child;
        use->view() >> child;
        out.airGuardVelocity = deserializeAirGuardVelocity(child);
    }
    use = data->findToken("_/groundCornerPushoff");
    if (use != NULL){
        use->view() >> out.groundCornerPushoff;
    }
    use = data->findToken("_/airCornerPushoff");
    if (use != NULL){
        use->view() >> out.airCornerPushoff;
    }
    use = data->findToken("_/downCornerPushoff");
    if (use != NULL){
        use->view() >> out.downCornerPushoff;
    }
    use = data->findToken("_/guardCornerPushoff");
    if (use != NULL){
        use->view() >> out.guardCornerPushoff;
    }
    use = data->findToken("_/airGuardCornerPushoff");
    if (use != NULL){
        use->view() >> out.airGuardCornerPushoff;
    }
    use = data->findToken("_/airGuardControlTime");
    if (use != NULL){
        use->view() >> out.airGuardControlTime;
    }
    use = data->findToken("_/airJuggle");
    if (use != NULL){
        use->view() >> out.airJuggle;
    }
    use = data->findToken("_/id");
    if (use != NULL){
        use->view() >> out.id;
    }
    use = data->findToken("_/chainId");
    if (use != NULL){
        use->view() >> out.chainId;
    }
    use = data->findToken("_/minimum");
    if (use != NULL){
        out.minimum = deserializeDistance(use);
    }
    use = data->findToken("_/maximum");
    if (use != NULL){
        out.maximum = deserializeDistance(use);
    }
    use = data->findToken("_/snap");
    if (use != NULL){
        out.snap = deserializeDistance(use);
    }
    use = data->findToken("_/player1SpritePriority");
    if (use != NULL){
        use->view() >> out.player1SpritePriority;
    }
    use = data->findToken("_/player2SpritePriority");
    if (use != NULL){
        use->view() >> out.player2SpritePriority;
    }
    use = data->findToken("_/player1Facing");
    if (use != NULL){
        use->view() >> out.player1Facing;
    }
    use = data->findToken("_/player1GetPlayer2Facing");
    if (use != NULL){
        use->view() >> out.player1GetPlayer2Facing;
    }
    use = data->findToken("_/player2Facing");
    if (use != NULL){
        use->view() >> out.player2Facing;
    }
    use = data->findToken("_/player1State");
    if (use != NULL){
        use->view() >> out.player1State;
    }
    use = data->findToken("_/player2State");
    if (use != NULL){
        use->view() >> out.player2State;
    }
    use = data->findToken("_/player2GetPlayer1State");
    if (use != NULL){
        use->view() >> out.player2GetPlayer1State;
    }
    use = data->findToken("_/forceStand");
    if (use != NULL){
        use->view() >> out.forceStand;
    }
    use = data->findToken("_/fall");
    if (use != NULL){
        const Token * child;
        use->view() >> child;
        out.fall = deserializeFall(child);
    }

    return out;
}


Token * serialize(const HitOverride & data){
    Token * out = new Token();
    *out << "HitOverride";
    if (data.time != 0){
        *out->newToken() << "time" << data.time;
    }
*out->newToken() << "attributes" << serialize(data.attributes);
    if (data.state != 0){
        *out->newToken() << "state" << data.state;
    }
    if (data.forceAir != false){
        *out->newToken() << "forceAir" << data.forceAir;
    }

    return out;
}

HitOverride deserializeHitOverride(const Token * data){
    HitOverride out;
    const Token * use = NULL;
    use = data->findToken("_/time");
    if (use != NULL){
        use->view() >> out.time;
    }
    use = data->findToken("_/attributes");
    if (use != NULL){
        out.attributes = deserializeHitAttributes(use);
    }
    use = data->findToken("_/state");
    if (use != NULL){
        use->view() >> out.state;
    }
    use = data->findToken("_/forceAir");
    if (use != NULL){
        use->view() >> out.forceAir;
    }

    return out;
}




Token * serialize(const Shake1 & data){
    Token * out = new Token();
    *out << "Shake1";
    if (data.time != 0){
        *out->newToken() << "time" << data.time;
    }

    return out;
}

Shake1 deserializeShake1(const Token * data){
    Shake1 out;
    const Token * use = NULL;
    use = data->findToken("_/time");
    if (use != NULL){
        use->view() >> out.time;
    }

    return out;
}

Token * serialize(const Fall1 & data){
    Token * out = new Token();
    *out << "Fall1";
    *out->newToken() << "envShake" << serialize(data.envShake);
    if (data.fall != false){
        *out->newToken() << "fall" << data.fall;
    }
    if (data.recover != false){
        *out->newToken() << "recover" << data.recover;
    }
    if (data.recoverTime != 0){
        *out->newToken() << "recoverTime" << data.recoverTime;
    }
    if (data.xVelocity != 0){
        *out->newToken() << "xVelocity" << data.xVelocity;
    }
    if (data.yVelocity != 0){
        *out->newToken() << "yVelocity" << data.yVelocity;
    }
    if (data.changeXVelocity != false){
        *out->newToken() << "changeXVelocity" << data.changeXVelocity;
    }
    if (data.damage != 0){
        *out->newToken() << "damage" << data.damage;
    }

    return out;
}

Fall1 deserializeFall1(const Token * data){
    Fall1 out;
    const Token * use = NULL;
    use = data->findToken("_/envShake");
    if (use != NULL){
        const Token * child;
        use->view() >> child;
        out.envShake = deserializeShake1(child);
    }
    use = data->findToken("_/fall");
    if (use != NULL){
        use->view() >> out.fall;
    }
    use = data->findToken("_/recover");
    if (use != NULL){
        use->view() >> out.recover;
    }
    use = data->findToken("_/recoverTime");
    if (use != NULL){
        use->view() >> out.recoverTime;
    }
    use = data->findToken("_/xVelocity");
    if (use != NULL){
        use->view() >> out.xVelocity;
    }
    use = data->findToken("_/yVelocity");
    if (use != NULL){
        use->view() >> out.yVelocity;
    }
    use = data->findToken("_/changeXVelocity");
    if (use != NULL){
        use->view() >> out.changeXVelocity;
    }
    use = data->findToken("_/damage");
    if (use != NULL){
        use->view() >> out.damage;
    }

    return out;
}

Token * serialize(const HitState & data){
    Token * out = new Token();
    *out << "HitState";
    if (data.shakeTime != 0){
        *out->newToken() << "shakeTime" << data.shakeTime;
    }
    if (data.hitTime != 0){
        *out->newToken() << "hitTime" << data.hitTime;
    }
    if (data.hits != 0){
        *out->newToken() << "hits" << data.hits;
    }
    if (data.slideTime != 0){
        *out->newToken() << "slideTime" << data.slideTime;
    }
    if (data.returnControlTime != 0){
        *out->newToken() << "returnControlTime" << data.returnControlTime;
    }
    if (data.recoverTime != 0){
        *out->newToken() << "recoverTime" << data.recoverTime;
    }
    if (data.yAcceleration != 0){
        *out->newToken() << "yAcceleration" << data.yAcceleration;
    }
    if (data.yVelocity != 0){
        *out->newToken() << "yVelocity" << data.yVelocity;
    }
    if (data.xVelocity != 0){
        *out->newToken() << "xVelocity" << data.xVelocity;
    }
*out->newToken() << "animationType" << serialize(data.animationType);
*out->newToken() << "airType" << serialize(data.airType);
*out->newToken() << "groundType" << serialize(data.groundType);
*out->newToken() << "hitType" << serialize(data.hitType);
    if (data.guarded != false){
        *out->newToken() << "guarded" << data.guarded;
    }
    if (data.damage != 0){
        *out->newToken() << "damage" << data.damage;
    }
    if (data.chainId != 0){
        *out->newToken() << "chainId" << data.chainId;
    }
    if (data.spritePriority != 0){
        *out->newToken() << "spritePriority" << data.spritePriority;
    }
    *out->newToken() << "fall" << serialize(data.fall);
    if (data.moveContact != 0){
        *out->newToken() << "moveContact" << data.moveContact;
    }

    return out;
}

HitState deserializeHitState(const Token * data){
    HitState out;
    const Token * use = NULL;
    use = data->findToken("_/shakeTime");
    if (use != NULL){
        use->view() >> out.shakeTime;
    }
    use = data->findToken("_/hitTime");
    if (use != NULL){
        use->view() >> out.hitTime;
    }
    use = data->findToken("_/hits");
    if (use != NULL){
        use->view() >> out.hits;
    }
    use = data->findToken("_/slideTime");
    if (use != NULL){
        use->view() >> out.slideTime;
    }
    use = data->findToken("_/returnControlTime");
    if (use != NULL){
        use->view() >> out.returnControlTime;
    }
    use = data->findToken("_/recoverTime");
    if (use != NULL){
        use->view() >> out.recoverTime;
    }
    use = data->findToken("_/yAcceleration");
    if (use != NULL){
        use->view() >> out.yAcceleration;
    }
    use = data->findToken("_/yVelocity");
    if (use != NULL){
        use->view() >> out.yVelocity;
    }
    use = data->findToken("_/xVelocity");
    if (use != NULL){
        use->view() >> out.xVelocity;
    }
    use = data->findToken("_/animationType");
    if (use != NULL){
        out.animationType = deserializeAttackTypeAnimation(use);
    }
    use = data->findToken("_/airType");
    if (use != NULL){
        out.airType = deserializeAttackTypeGround(use);
    }
    use = data->findToken("_/groundType");
    if (use != NULL){
        out.groundType = deserializeAttackTypeGround(use);
    }
    use = data->findToken("_/hitType");
    if (use != NULL){
        out.hitType = deserializeAttackTypeGround(use);
    }
    use = data->findToken("_/guarded");
    if (use != NULL){
        use->view() >> out.guarded;
    }
    use = data->findToken("_/damage");
    if (use != NULL){
        use->view() >> out.damage;
    }
    use = data->findToken("_/chainId");
    if (use != NULL){
        use->view() >> out.chainId;
    }
    use = data->findToken("_/spritePriority");
    if (use != NULL){
        use->view() >> out.spritePriority;
    }
    use = data->findToken("_/fall");
    if (use != NULL){
        const Token * child;
        use->view() >> child;
        out.fall = deserializeFall1(child);
    }
    use = data->findToken("_/moveContact");
    if (use != NULL){
        use->view() >> out.moveContact;
    }

    return out;
}



Token * serialize(const HitSound & data){
    Token * out = new Token();
    *out << "HitSound";
    if (data.own != false){
        *out->newToken() << "own" << data.own;
    }
    if (data.group != 0){
        *out->newToken() << "group" << data.group;
    }
    if (data.item != 0){
        *out->newToken() << "item" << data.item;
    }

    return out;
}

HitSound deserializeHitSound(const Token * data){
    HitSound out;
    const Token * use = NULL;
    use = data->findToken("_/own");
    if (use != NULL){
        use->view() >> out.own;
    }
    use = data->findToken("_/group");
    if (use != NULL){
        use->view() >> out.group;
    }
    use = data->findToken("_/item");
    if (use != NULL){
        use->view() >> out.item;
    }

    return out;
}

Token * serialize(const ReversalData & data){
    Token * out = new Token();
    *out << "ReversalData";
*out->newToken() << "pause" << serialize(data.pause);
*out->newToken() << "spark" << serialize(data.spark);
    *out->newToken() << "hitSound" << serialize(data.hitSound);
    if (data.sparkX != 0){
        *out->newToken() << "sparkX" << data.sparkX;
    }
    if (data.sparkY != 0){
        *out->newToken() << "sparkY" << data.sparkY;
    }
    if (data.player1State != 0){
        *out->newToken() << "player1State" << data.player1State;
    }
    if (data.player2State != 0){
        *out->newToken() << "player2State" << data.player2State;
    }
    if (data.player1Pause != 0){
        *out->newToken() << "player1Pause" << data.player1Pause;
    }
    if (data.player2Pause != 0){
        *out->newToken() << "player2Pause" << data.player2Pause;
    }
    if (data.standing != false){
        *out->newToken() << "standing" << data.standing;
    }
    if (data.crouching != false){
        *out->newToken() << "crouching" << data.crouching;
    }
    if (data.aerial != false){
        *out->newToken() << "aerial" << data.aerial;
    }

    Token * t1 = out->newToken();
    *t1 << "attributes";
    for (std::vector<AttackType::Attribute >::const_iterator it = data.attributes.begin(); it != data.attributes.end(); it++){
        *t1 << serialize(*it);
    }
    
    return out;
}

ReversalData deserializeReversalData(const Token * data){
    ReversalData out;
    const Token * use = NULL;
    use = data->findToken("_/pause");
    if (use != NULL){
        out.pause = deserializePauseTime(use);
    }
    use = data->findToken("_/spark");
    if (use != NULL){
        out.spark = deserializeResourceEffect(use);
    }
    use = data->findToken("_/hitSound");
    if (use != NULL){
        const Token * child;
        use->view() >> child;
        out.hitSound = deserializeHitSound(child);
    }
    use = data->findToken("_/sparkX");
    if (use != NULL){
        use->view() >> out.sparkX;
    }
    use = data->findToken("_/sparkY");
    if (use != NULL){
        use->view() >> out.sparkY;
    }
    use = data->findToken("_/player1State");
    if (use != NULL){
        use->view() >> out.player1State;
    }
    use = data->findToken("_/player2State");
    if (use != NULL){
        use->view() >> out.player2State;
    }
    use = data->findToken("_/player1Pause");
    if (use != NULL){
        use->view() >> out.player1Pause;
    }
    use = data->findToken("_/player2Pause");
    if (use != NULL){
        use->view() >> out.player2Pause;
    }
    use = data->findToken("_/standing");
    if (use != NULL){
        use->view() >> out.standing;
    }
    use = data->findToken("_/crouching");
    if (use != NULL){
        use->view() >> out.crouching;
    }
    use = data->findToken("_/aerial");
    if (use != NULL){
        use->view() >> out.aerial;
    }
    use = data->findToken("_/attributes");
    if (use != NULL){
        for (TokenView view = use->view(); view.hasMore(); /**/){
            out.attributes.push_back(deserializeAttackTypeAttribute(view.next()));
        }

    }

    return out;
}



Token * serialize(const WidthOverride & data){
    Token * out = new Token();
    *out << "WidthOverride";
    if (data.enabled != false){
        *out->newToken() << "enabled" << data.enabled;
    }
    if (data.edgeFront != 0){
        *out->newToken() << "edgeFront" << data.edgeFront;
    }
    if (data.edgeBack != 0){
        *out->newToken() << "edgeBack" << data.edgeBack;
    }
    if (data.playerFront != 0){
        *out->newToken() << "playerFront" << data.playerFront;
    }
    if (data.playerBack != 0){
        *out->newToken() << "playerBack" << data.playerBack;
    }

    return out;
}

WidthOverride deserializeWidthOverride(const Token * data){
    WidthOverride out;
    const Token * use = NULL;
    use = data->findToken("_/enabled");
    if (use != NULL){
        use->view() >> out.enabled;
    }
    use = data->findToken("_/edgeFront");
    if (use != NULL){
        use->view() >> out.edgeFront;
    }
    use = data->findToken("_/edgeBack");
    if (use != NULL){
        use->view() >> out.edgeBack;
    }
    use = data->findToken("_/playerFront");
    if (use != NULL){
        use->view() >> out.playerFront;
    }
    use = data->findToken("_/playerBack");
    if (use != NULL){
        use->view() >> out.playerBack;
    }

    return out;
}


Token * serialize(const HitByOverride & data){
    Token * out = new Token();
    *out << "HitByOverride";
    if (data.standing != false){
        *out->newToken() << "standing" << data.standing;
    }
    if (data.crouching != false){
        *out->newToken() << "crouching" << data.crouching;
    }
    if (data.aerial != false){
        *out->newToken() << "aerial" << data.aerial;
    }
    if (data.time != 0){
        *out->newToken() << "time" << data.time;
    }

    Token * t1 = out->newToken();
    *t1 << "attributes";
    for (std::vector<AttackType::Attribute >::const_iterator it = data.attributes.begin(); it != data.attributes.end(); it++){
        *t1 << serialize(*it);
    }
    
    return out;
}

HitByOverride deserializeHitByOverride(const Token * data){
    HitByOverride out;
    const Token * use = NULL;
    use = data->findToken("_/standing");
    if (use != NULL){
        use->view() >> out.standing;
    }
    use = data->findToken("_/crouching");
    if (use != NULL){
        use->view() >> out.crouching;
    }
    use = data->findToken("_/aerial");
    if (use != NULL){
        use->view() >> out.aerial;
    }
    use = data->findToken("_/time");
    if (use != NULL){
        use->view() >> out.time;
    }
    use = data->findToken("_/attributes");
    if (use != NULL){
        for (TokenView view = use->view(); view.hasMore(); /**/){
            out.attributes.push_back(deserializeAttackTypeAttribute(view.next()));
        }

    }

    return out;
}


Token * serialize(const TransOverride & data){
    Token * out = new Token();
    *out << "TransOverride";
    if (data.enabled != false){
        *out->newToken() << "enabled" << data.enabled;
    }
*out->newToken() << "type" << serialize(data.type);
    if (data.alphaSource != 0){
        *out->newToken() << "alphaSource" << data.alphaSource;
    }
    if (data.alphaDestination != 0){
        *out->newToken() << "alphaDestination" << data.alphaDestination;
    }

    return out;
}

TransOverride deserializeTransOverride(const Token * data){
    TransOverride out;
    const Token * use = NULL;
    use = data->findToken("_/enabled");
    if (use != NULL){
        use->view() >> out.enabled;
    }
    use = data->findToken("_/type");
    if (use != NULL){
        out.type = deserializeTransType(use);
    }
    use = data->findToken("_/alphaSource");
    if (use != NULL){
        use->view() >> out.alphaSource;
    }
    use = data->findToken("_/alphaDestination");
    if (use != NULL){
        use->view() >> out.alphaDestination;
    }

    return out;
}


Token * serialize(const SpecialStuff & data){
    Token * out = new Token();
    *out << "SpecialStuff";
    if (data.invisible != false){
        *out->newToken() << "invisible" << data.invisible;
    }
    if (data.intro != false){
        *out->newToken() << "intro" << data.intro;
    }

    return out;
}

SpecialStuff deserializeSpecialStuff(const Token * data){
    SpecialStuff out;
    const Token * use = NULL;
    use = data->findToken("_/invisible");
    if (use != NULL){
        use->view() >> out.invisible;
    }
    use = data->findToken("_/intro");
    if (use != NULL){
        use->view() >> out.intro;
    }

    return out;
}


Token * serialize(const Bind & data){
    Token * out = new Token();
    *out << "Bind";
*out->newToken() << "bound" << serialize(data.bound);
    if (data.time != 0){
        *out->newToken() << "time" << data.time;
    }
    if (data.facing != 0){
        *out->newToken() << "facing" << data.facing;
    }
    if (data.offsetX != 0){
        *out->newToken() << "offsetX" << data.offsetX;
    }
    if (data.offsetY != 0){
        *out->newToken() << "offsetY" << data.offsetY;
    }

    return out;
}

Bind deserializeBind(const Token * data){
    Bind out;
    const Token * use = NULL;
    use = data->findToken("_/bound");
    if (use != NULL){
        out.bound = deserializeCharacterId(use);
    }
    use = data->findToken("_/time");
    if (use != NULL){
        use->view() >> out.time;
    }
    use = data->findToken("_/facing");
    if (use != NULL){
        use->view() >> out.facing;
    }
    use = data->findToken("_/offsetX");
    if (use != NULL){
        use->view() >> out.offsetX;
    }
    use = data->findToken("_/offsetY");
    if (use != NULL){
        use->view() >> out.offsetY;
    }

    return out;
}


Token * serialize(const CharacterData & data){
    Token * out = new Token();
    *out << "CharacterData";
*out->newToken() << "who" << serialize(data.who);
    if (data.enabled != false){
        *out->newToken() << "enabled" << data.enabled;
    }

    return out;
}

CharacterData deserializeCharacterData(const Token * data){
    CharacterData out;
    const Token * use = NULL;
    use = data->findToken("_/who");
    if (use != NULL){
        out.who = deserializeCharacterId(use);
    }
    use = data->findToken("_/enabled");
    if (use != NULL){
        use->view() >> out.enabled;
    }

    return out;
}


Token * serialize(const DrawAngleEffect & data){
    Token * out = new Token();
    *out << "DrawAngleEffect";
    if (data.enabled != false){
        *out->newToken() << "enabled" << data.enabled;
    }
    if (data.angle != 0){
        *out->newToken() << "angle" << data.angle;
    }
    if (data.scaleX != 0){
        *out->newToken() << "scaleX" << data.scaleX;
    }
    if (data.scaleY != 0){
        *out->newToken() << "scaleY" << data.scaleY;
    }

    return out;
}

DrawAngleEffect deserializeDrawAngleEffect(const Token * data){
    DrawAngleEffect out;
    const Token * use = NULL;
    use = data->findToken("_/enabled");
    if (use != NULL){
        use->view() >> out.enabled;
    }
    use = data->findToken("_/angle");
    if (use != NULL){
        use->view() >> out.angle;
    }
    use = data->findToken("_/scaleX");
    if (use != NULL){
        use->view() >> out.scaleX;
    }
    use = data->findToken("_/scaleY");
    if (use != NULL){
        use->view() >> out.scaleY;
    }

    return out;
}

Token * serialize(const StateData & data){
    Token * out = new Token();
    *out << "StateData";
    if (data.juggleRemaining != 0){
        *out->newToken() << "juggleRemaining" << data.juggleRemaining;
    }
    if (data.currentJuggle != 0){
        *out->newToken() << "currentJuggle" << data.currentJuggle;
    }
    if (data.currentState != 0){
        *out->newToken() << "currentState" << data.currentState;
    }
    if (data.previousState != 0){
        *out->newToken() << "previousState" << data.previousState;
    }
    if (data.currentAnimation != 0){
        *out->newToken() << "currentAnimation" << data.currentAnimation;
    }
    if (data.velocity_x != 0){
        *out->newToken() << "velocity_x" << data.velocity_x;
    }
    if (data.velocity_y != 0){
        *out->newToken() << "velocity_y" << data.velocity_y;
    }
    if (data.has_control != false){
        *out->newToken() << "has_control" << data.has_control;
    }
    if (data.stateTime != 0){
        *out->newToken() << "stateTime" << data.stateTime;
    }

    Token * t1 = out->newToken();
    *t1 << "variables";
    for (std::map<int, RuntimeValue >::const_iterator it = data.variables.begin(); it != data.variables.end(); it++){
        *t1->newToken() << serialize(it->first) << serialize(it->second);
    }
    
    Token * t2 = out->newToken();
    *t2 << "floatVariables";
    for (std::map<int, RuntimeValue >::const_iterator it = data.floatVariables.begin(); it != data.floatVariables.end(); it++){
        *t2->newToken() << serialize(it->first) << serialize(it->second);
    }
    
    Token * t3 = out->newToken();
    *t3 << "systemVariables";
    for (std::map<int, RuntimeValue >::const_iterator it = data.systemVariables.begin(); it != data.systemVariables.end(); it++){
        *t3->newToken() << serialize(it->first) << serialize(it->second);
    }
    *out->newToken() << "currentPhysics" << serialize(data.currentPhysics);
*out->newToken() << "stateType" << serialize(data.stateType);
*out->newToken() << "moveType" << serialize(data.moveType);
*out->newToken() << "hit" << serialize(data.hit);
*out->newToken() << "hitState" << serialize(data.hitState);
    if (data.combo != 0){
        *out->newToken() << "combo" << data.combo;
    }
    if (data.hitCount != 0){
        *out->newToken() << "hitCount" << data.hitCount;
    }
    if (data.blocking != false){
        *out->newToken() << "blocking" << data.blocking;
    }
    if (data.guarding != false){
        *out->newToken() << "guarding" << data.guarding;
    }
    *out->newToken() << "widthOverride" << serialize(data.widthOverride);
    Token * token_hitByOverride = out->newToken();
    *token_hitByOverride << "hitByOverride";
    *token_hitByOverride->newToken() << 0 << serialize(data.hitByOverride[0]);
    *token_hitByOverride->newToken() << 1 << serialize(data.hitByOverride[1]);
    if (data.defenseMultiplier != 0){
        *out->newToken() << "defenseMultiplier" << data.defenseMultiplier;
    }
    if (data.attackMultiplier != 0){
        *out->newToken() << "attackMultiplier" << data.attackMultiplier;
    }
    if (data.frozen != false){
        *out->newToken() << "frozen" << data.frozen;
    }
*out->newToken() << "reversal" << serialize(data.reversal);
    if (data.reversalActive != false){
        *out->newToken() << "reversalActive" << data.reversalActive;
    }
    *out->newToken() << "transOverride" << serialize(data.transOverride);
    if (data.pushPlayer != 0){
        *out->newToken() << "pushPlayer" << data.pushPlayer;
    }
    *out->newToken() << "special" << serialize(data.special);
    if (data.health != 0){
        *out->newToken() << "health" << data.health;
    }
    *out->newToken() << "bind" << serialize(data.bind);

    Token * t4 = out->newToken();
    *t4 << "targets";
    for (std::map<int, std::vector<CharacterId > >::const_iterator it = data.targets.begin(); it != data.targets.end(); it++){
        *t4->newToken() << serialize(it->first) << serialize(it->second);
    }
        if (data.spritePriority != 0){
        *out->newToken() << "spritePriority" << data.spritePriority;
    }
    if (data.wasHitCounter != 0){
        *out->newToken() << "wasHitCounter" << data.wasHitCounter;
    }
    *out->newToken() << "characterData" << serialize(data.characterData);
    if (data.drawAngle != 0){
        *out->newToken() << "drawAngle" << data.drawAngle;
    }
    *out->newToken() << "drawAngleData" << serialize(data.drawAngleData);

    Token * t5 = out->newToken();
    *t5 << "active";
    for (std::vector<std::string >::const_iterator it = data.active.begin(); it != data.active.end(); it++){
        *t5 << serialize(*it);
    }
    
    Token * t6 = out->newToken();
    *t6 << "hitOverrides";
    for (std::map<int, HitOverride >::const_iterator it = data.hitOverrides.begin(); it != data.hitOverrides.end(); it++){
        *t6->newToken() << serialize(it->first) << serialize(it->second);
    }
        if (data.virtualx != 0){
        *out->newToken() << "virtualx" << data.virtualx;
    }
    if (data.virtualy != 0){
        *out->newToken() << "virtualy" << data.virtualy;
    }
    if (data.virtualz != 0){
        *out->newToken() << "virtualz" << data.virtualz;
    }
*out->newToken() << "facing" << serialize(data.facing);
    if (data.power != 0){
        *out->newToken() << "power" << data.power;
    }

    return out;
}

StateData deserializeStateData(const Token * data){
    StateData out;
    const Token * use = NULL;
    use = data->findToken("_/juggleRemaining");
    if (use != NULL){
        use->view() >> out.juggleRemaining;
    }
    use = data->findToken("_/currentJuggle");
    if (use != NULL){
        use->view() >> out.currentJuggle;
    }
    use = data->findToken("_/currentState");
    if (use != NULL){
        use->view() >> out.currentState;
    }
    use = data->findToken("_/previousState");
    if (use != NULL){
        use->view() >> out.previousState;
    }
    use = data->findToken("_/currentAnimation");
    if (use != NULL){
        use->view() >> out.currentAnimation;
    }
    use = data->findToken("_/velocity_x");
    if (use != NULL){
        use->view() >> out.velocity_x;
    }
    use = data->findToken("_/velocity_y");
    if (use != NULL){
        use->view() >> out.velocity_y;
    }
    use = data->findToken("_/has_control");
    if (use != NULL){
        use->view() >> out.has_control;
    }
    use = data->findToken("_/stateTime");
    if (use != NULL){
        use->view() >> out.stateTime;
    }
    use = data->findToken("_/variables");
    if (use != NULL){
        
    }
    use = data->findToken("_/floatVariables");
    if (use != NULL){
        
    }
    use = data->findToken("_/systemVariables");
    if (use != NULL){
        
    }
    use = data->findToken("_/currentPhysics");
    if (use != NULL){
        out.currentPhysics = deserializePhysicsType(use);
    }
    use = data->findToken("_/stateType");
    if (use != NULL){
        use->view() >> out.stateType;
    }
    use = data->findToken("_/moveType");
    if (use != NULL){
        use->view() >> out.moveType;
    }
    use = data->findToken("_/hit");
    if (use != NULL){
        out.hit = deserializeHitDefinition(use);
    }
    use = data->findToken("_/hitState");
    if (use != NULL){
        out.hitState = deserializeHitState(use);
    }
    use = data->findToken("_/combo");
    if (use != NULL){
        use->view() >> out.combo;
    }
    use = data->findToken("_/hitCount");
    if (use != NULL){
        use->view() >> out.hitCount;
    }
    use = data->findToken("_/blocking");
    if (use != NULL){
        use->view() >> out.blocking;
    }
    use = data->findToken("_/guarding");
    if (use != NULL){
        use->view() >> out.guarding;
    }
    use = data->findToken("_/widthOverride");
    if (use != NULL){
        const Token * child;
        use->view() >> child;
        out.widthOverride = deserializeWidthOverride(child);
    }
    use = data->findToken("_/hitByOverride");
    if (use != NULL){
        
    }
    use = data->findToken("_/defenseMultiplier");
    if (use != NULL){
        use->view() >> out.defenseMultiplier;
    }
    use = data->findToken("_/attackMultiplier");
    if (use != NULL){
        use->view() >> out.attackMultiplier;
    }
    use = data->findToken("_/frozen");
    if (use != NULL){
        use->view() >> out.frozen;
    }
    use = data->findToken("_/reversal");
    if (use != NULL){
        out.reversal = deserializeReversalData(use);
    }
    use = data->findToken("_/reversalActive");
    if (use != NULL){
        use->view() >> out.reversalActive;
    }
    use = data->findToken("_/transOverride");
    if (use != NULL){
        const Token * child;
        use->view() >> child;
        out.transOverride = deserializeTransOverride(child);
    }
    use = data->findToken("_/pushPlayer");
    if (use != NULL){
        use->view() >> out.pushPlayer;
    }
    use = data->findToken("_/special");
    if (use != NULL){
        const Token * child;
        use->view() >> child;
        out.special = deserializeSpecialStuff(child);
    }
    use = data->findToken("_/health");
    if (use != NULL){
        use->view() >> out.health;
    }
    use = data->findToken("_/bind");
    if (use != NULL){
        const Token * child;
        use->view() >> child;
        out.bind = deserializeBind(child);
    }
    use = data->findToken("_/targets");
    if (use != NULL){
        
    }
    use = data->findToken("_/spritePriority");
    if (use != NULL){
        use->view() >> out.spritePriority;
    }
    use = data->findToken("_/wasHitCounter");
    if (use != NULL){
        use->view() >> out.wasHitCounter;
    }
    use = data->findToken("_/characterData");
    if (use != NULL){
        const Token * child;
        use->view() >> child;
        out.characterData = deserializeCharacterData(child);
    }
    use = data->findToken("_/drawAngle");
    if (use != NULL){
        use->view() >> out.drawAngle;
    }
    use = data->findToken("_/drawAngleData");
    if (use != NULL){
        const Token * child;
        use->view() >> child;
        out.drawAngleData = deserializeDrawAngleEffect(child);
    }
    use = data->findToken("_/active");
    if (use != NULL){
        for (TokenView view = use->view(); view.hasMore(); /**/){
            std::string element;
            view >> element;
            out.active.push_back(element);
                    
        }

    }
    use = data->findToken("_/hitOverrides");
    if (use != NULL){
        
    }
    use = data->findToken("_/virtualx");
    if (use != NULL){
        use->view() >> out.virtualx;
    }
    use = data->findToken("_/virtualy");
    if (use != NULL){
        use->view() >> out.virtualy;
    }
    use = data->findToken("_/virtualz");
    if (use != NULL){
        use->view() >> out.virtualz;
    }
    use = data->findToken("_/facing");
    if (use != NULL){
        out.facing = deserializeFacing(use);
    }
    use = data->findToken("_/power");
    if (use != NULL){
        use->view() >> out.power;
    }

    return out;
}


Token * serialize(const AnimationState & data){
    Token * out = new Token();
    *out << "AnimationState";
    if (data.position != 0){
        *out->newToken() << "position" << data.position;
    }
    if (data.looped != false){
        *out->newToken() << "looped" << data.looped;
    }
    if (data.started != false){
        *out->newToken() << "started" << data.started;
    }
    if (data.ticks != 0){
        *out->newToken() << "ticks" << data.ticks;
    }
    if (data.virtual_ticks != 0){
        *out->newToken() << "virtual_ticks" << data.virtual_ticks;
    }

    return out;
}

AnimationState deserializeAnimationState(const Token * data){
    AnimationState out;
    const Token * use = NULL;
    use = data->findToken("_/position");
    if (use != NULL){
        use->view() >> out.position;
    }
    use = data->findToken("_/looped");
    if (use != NULL){
        use->view() >> out.looped;
    }
    use = data->findToken("_/started");
    if (use != NULL){
        use->view() >> out.started;
    }
    use = data->findToken("_/ticks");
    if (use != NULL){
        use->view() >> out.ticks;
    }
    use = data->findToken("_/virtual_ticks");
    if (use != NULL){
        use->view() >> out.virtual_ticks;
    }

    return out;
}


Token * serialize(const ScreenBound & data){
    Token * out = new Token();
    *out << "ScreenBound";
    if (data.enabled != false){
        *out->newToken() << "enabled" << data.enabled;
    }
    if (data.offScreen != false){
        *out->newToken() << "offScreen" << data.offScreen;
    }
    if (data.panX != false){
        *out->newToken() << "panX" << data.panX;
    }
    if (data.panY != false){
        *out->newToken() << "panY" << data.panY;
    }

    return out;
}

ScreenBound deserializeScreenBound(const Token * data){
    ScreenBound out;
    const Token * use = NULL;
    use = data->findToken("_/enabled");
    if (use != NULL){
        use->view() >> out.enabled;
    }
    use = data->findToken("_/offScreen");
    if (use != NULL){
        use->view() >> out.offScreen;
    }
    use = data->findToken("_/panX");
    if (use != NULL){
        use->view() >> out.panX;
    }
    use = data->findToken("_/panY");
    if (use != NULL){
        use->view() >> out.panY;
    }

    return out;
}



Token * serialize(const Pause & data){
    Token * out = new Token();
    *out << "Pause";
    if (data.time != 0){
        *out->newToken() << "time" << data.time;
    }
    if (data.buffer != 0){
        *out->newToken() << "buffer" << data.buffer;
    }
    if (data.moveTime != 0){
        *out->newToken() << "moveTime" << data.moveTime;
    }
    if (data.pauseBackground != false){
        *out->newToken() << "pauseBackground" << data.pauseBackground;
    }
*out->newToken() << "who" << serialize(data.who);

    return out;
}

Pause deserializePause(const Token * data){
    Pause out;
    const Token * use = NULL;
    use = data->findToken("_/time");
    if (use != NULL){
        use->view() >> out.time;
    }
    use = data->findToken("_/buffer");
    if (use != NULL){
        use->view() >> out.buffer;
    }
    use = data->findToken("_/moveTime");
    if (use != NULL){
        use->view() >> out.moveTime;
    }
    use = data->findToken("_/pauseBackground");
    if (use != NULL){
        use->view() >> out.pauseBackground;
    }
    use = data->findToken("_/who");
    if (use != NULL){
        out.who = deserializeCharacterId(use);
    }

    return out;
}


Token * serialize(const Zoom & data){
    Token * out = new Token();
    *out << "Zoom";
    if (data.enabled != false){
        *out->newToken() << "enabled" << data.enabled;
    }
    if (data.x != 0){
        *out->newToken() << "x" << data.x;
    }
    if (data.y != 0){
        *out->newToken() << "y" << data.y;
    }
    if (data.zoomTime != 0){
        *out->newToken() << "zoomTime" << data.zoomTime;
    }
    if (data.zoomOutTime != 0){
        *out->newToken() << "zoomOutTime" << data.zoomOutTime;
    }
    if (data.zoom != 0){
        *out->newToken() << "zoom" << data.zoom;
    }
    if (data.in != false){
        *out->newToken() << "in" << data.in;
    }
    if (data.time != 0){
        *out->newToken() << "time" << data.time;
    }
    if (data.bindTime != 0){
        *out->newToken() << "bindTime" << data.bindTime;
    }
    if (data.deltaX != 0){
        *out->newToken() << "deltaX" << data.deltaX;
    }
    if (data.deltaY != 0){
        *out->newToken() << "deltaY" << data.deltaY;
    }
    if (data.scaleX != 0){
        *out->newToken() << "scaleX" << data.scaleX;
    }
    if (data.scaleY != 0){
        *out->newToken() << "scaleY" << data.scaleY;
    }
    if (data.velocityX != 0){
        *out->newToken() << "velocityX" << data.velocityX;
    }
    if (data.velocityY != 0){
        *out->newToken() << "velocityY" << data.velocityY;
    }
    if (data.accelX != 0){
        *out->newToken() << "accelX" << data.accelX;
    }
    if (data.accelY != 0){
        *out->newToken() << "accelY" << data.accelY;
    }
    if (data.superMoveTime != 0){
        *out->newToken() << "superMoveTime" << data.superMoveTime;
    }
    if (data.pauseMoveTime != 0){
        *out->newToken() << "pauseMoveTime" << data.pauseMoveTime;
    }
    if (data.removeOnGetHit != false){
        *out->newToken() << "removeOnGetHit" << data.removeOnGetHit;
    }
    if (data.hitCount != 0){
        *out->newToken() << "hitCount" << data.hitCount;
    }
*out->newToken() << "bound" << serialize(data.bound);
*out->newToken() << "owner" << serialize(data.owner);

    return out;
}

Zoom deserializeZoom(const Token * data){
    Zoom out;
    const Token * use = NULL;
    use = data->findToken("_/enabled");
    if (use != NULL){
        use->view() >> out.enabled;
    }
    use = data->findToken("_/x");
    if (use != NULL){
        use->view() >> out.x;
    }
    use = data->findToken("_/y");
    if (use != NULL){
        use->view() >> out.y;
    }
    use = data->findToken("_/zoomTime");
    if (use != NULL){
        use->view() >> out.zoomTime;
    }
    use = data->findToken("_/zoomOutTime");
    if (use != NULL){
        use->view() >> out.zoomOutTime;
    }
    use = data->findToken("_/zoom");
    if (use != NULL){
        use->view() >> out.zoom;
    }
    use = data->findToken("_/in");
    if (use != NULL){
        use->view() >> out.in;
    }
    use = data->findToken("_/time");
    if (use != NULL){
        use->view() >> out.time;
    }
    use = data->findToken("_/bindTime");
    if (use != NULL){
        use->view() >> out.bindTime;
    }
    use = data->findToken("_/deltaX");
    if (use != NULL){
        use->view() >> out.deltaX;
    }
    use = data->findToken("_/deltaY");
    if (use != NULL){
        use->view() >> out.deltaY;
    }
    use = data->findToken("_/scaleX");
    if (use != NULL){
        use->view() >> out.scaleX;
    }
    use = data->findToken("_/scaleY");
    if (use != NULL){
        use->view() >> out.scaleY;
    }
    use = data->findToken("_/velocityX");
    if (use != NULL){
        use->view() >> out.velocityX;
    }
    use = data->findToken("_/velocityY");
    if (use != NULL){
        use->view() >> out.velocityY;
    }
    use = data->findToken("_/accelX");
    if (use != NULL){
        use->view() >> out.accelX;
    }
    use = data->findToken("_/accelY");
    if (use != NULL){
        use->view() >> out.accelY;
    }
    use = data->findToken("_/superMoveTime");
    if (use != NULL){
        use->view() >> out.superMoveTime;
    }
    use = data->findToken("_/pauseMoveTime");
    if (use != NULL){
        use->view() >> out.pauseMoveTime;
    }
    use = data->findToken("_/removeOnGetHit");
    if (use != NULL){
        use->view() >> out.removeOnGetHit;
    }
    use = data->findToken("_/hitCount");
    if (use != NULL){
        use->view() >> out.hitCount;
    }
    use = data->findToken("_/bound");
    if (use != NULL){
        out.bound = deserializeCharacterId(use);
    }
    use = data->findToken("_/owner");
    if (use != NULL){
        out.owner = deserializeCharacterId(use);
    }

    return out;
}


Token * serialize(const EnvironmentColor & data){
    Token * out = new Token();
    *out << "EnvironmentColor";
*out->newToken() << "color" << serialize(data.color);
    if (data.time != 0){
        *out->newToken() << "time" << data.time;
    }
    if (data.under != false){
        *out->newToken() << "under" << data.under;
    }

    return out;
}

EnvironmentColor deserializeEnvironmentColor(const Token * data){
    EnvironmentColor out;
    const Token * use = NULL;
    use = data->findToken("_/color");
    if (use != NULL){
        out.color = deserializeGraphicsColor(use);
    }
    use = data->findToken("_/time");
    if (use != NULL){
        use->view() >> out.time;
    }
    use = data->findToken("_/under");
    if (use != NULL){
        use->view() >> out.under;
    }

    return out;
}


Token * serialize(const SuperPause & data){
    Token * out = new Token();
    *out << "SuperPause";
    if (data.time != 0){
        *out->newToken() << "time" << data.time;
    }
    if (data.positionX != 0){
        *out->newToken() << "positionX" << data.positionX;
    }
    if (data.positionY != 0){
        *out->newToken() << "positionY" << data.positionY;
    }
    if (data.soundGroup != 0){
        *out->newToken() << "soundGroup" << data.soundGroup;
    }
    if (data.soundItem != 0){
        *out->newToken() << "soundItem" << data.soundItem;
    }

    return out;
}

SuperPause deserializeSuperPause(const Token * data){
    SuperPause out;
    const Token * use = NULL;
    use = data->findToken("_/time");
    if (use != NULL){
        use->view() >> out.time;
    }
    use = data->findToken("_/positionX");
    if (use != NULL){
        use->view() >> out.positionX;
    }
    use = data->findToken("_/positionY");
    if (use != NULL){
        use->view() >> out.positionY;
    }
    use = data->findToken("_/soundGroup");
    if (use != NULL){
        use->view() >> out.soundGroup;
    }
    use = data->findToken("_/soundItem");
    if (use != NULL){
        use->view() >> out.soundItem;
    }

    return out;
}

Token * serialize(const StageStateData & data){
    Token * out = new Token();
    *out << "StageStateData";
    *out->newToken() << "pause" << serialize(data.pause);

    Token * t1 = out->newToken();
    *t1 << "screenBound";
    for (std::map<CharacterId, ScreenBound >::const_iterator it = data.screenBound.begin(); it != data.screenBound.end(); it++){
        *t1->newToken() << serialize(it->first) << serialize(it->second);
    }
        *out->newToken() << "zoom" << serialize(data.zoom);
    *out->newToken() << "environmentColor" << serialize(data.environmentColor);
    *out->newToken() << "superPause" << serialize(data.superPause);
    if (data.quake_time != 0){
        *out->newToken() << "quake_time" << data.quake_time;
    }
    if (data.cycles != 0){
        *out->newToken() << "cycles" << data.cycles;
    }
    if (data.inleft != 0){
        *out->newToken() << "inleft" << data.inleft;
    }
    if (data.inright != 0){
        *out->newToken() << "inright" << data.inright;
    }
    if (data.onLeftSide != 0){
        *out->newToken() << "onLeftSide" << data.onLeftSide;
    }
    if (data.onRightSide != 0){
        *out->newToken() << "onRightSide" << data.onRightSide;
    }
    if (data.inabove != 0){
        *out->newToken() << "inabove" << data.inabove;
    }
    if (data.camerax != 0){
        *out->newToken() << "camerax" << data.camerax;
    }
    if (data.cameray != 0){
        *out->newToken() << "cameray" << data.cameray;
    }
    if (data.ticker != 0){
        *out->newToken() << "ticker" << data.ticker;
    }

    return out;
}

StageStateData deserializeStageStateData(const Token * data){
    StageStateData out;
    const Token * use = NULL;
    use = data->findToken("_/pause");
    if (use != NULL){
        const Token * child;
        use->view() >> child;
        out.pause = deserializePause(child);
    }
    use = data->findToken("_/screenBound");
    if (use != NULL){
        
    }
    use = data->findToken("_/zoom");
    if (use != NULL){
        const Token * child;
        use->view() >> child;
        out.zoom = deserializeZoom(child);
    }
    use = data->findToken("_/environmentColor");
    if (use != NULL){
        const Token * child;
        use->view() >> child;
        out.environmentColor = deserializeEnvironmentColor(child);
    }
    use = data->findToken("_/superPause");
    if (use != NULL){
        const Token * child;
        use->view() >> child;
        out.superPause = deserializeSuperPause(child);
    }
    use = data->findToken("_/quake_time");
    if (use != NULL){
        use->view() >> out.quake_time;
    }
    use = data->findToken("_/cycles");
    if (use != NULL){
        use->view() >> out.cycles;
    }
    use = data->findToken("_/inleft");
    if (use != NULL){
        use->view() >> out.inleft;
    }
    use = data->findToken("_/inright");
    if (use != NULL){
        use->view() >> out.inright;
    }
    use = data->findToken("_/onLeftSide");
    if (use != NULL){
        use->view() >> out.onLeftSide;
    }
    use = data->findToken("_/onRightSide");
    if (use != NULL){
        use->view() >> out.onRightSide;
    }
    use = data->findToken("_/inabove");
    if (use != NULL){
        use->view() >> out.inabove;
    }
    use = data->findToken("_/camerax");
    if (use != NULL){
        use->view() >> out.camerax;
    }
    use = data->findToken("_/cameray");
    if (use != NULL){
        use->view() >> out.cameray;
    }
    use = data->findToken("_/ticker");
    if (use != NULL){
        use->view() >> out.ticker;
    }

    return out;
}

}

