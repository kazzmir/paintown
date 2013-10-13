
#include "character-state.h"

namespace Mugen{


Token * serialize(const HitAttributes & data){
    Token * out = new Token();
    *out << "HitAttributes";
   *out->newToken() << "slot" << data.slot;
   *out->newToken() << "standing" << data.standing;
   *out->newToken() << "crouching" << data.crouching;
   *out->newToken() << "aerial" << data.aerial;

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
   *out->newToken() << "own" << data.own;
   *out->newToken() << "group" << data.group;
   *out->newToken() << "item" << data.item;

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
   *out->newToken() << "high" << data.high;
   *out->newToken() << "low" << data.low;
   *out->newToken() << "air" << data.air;
   *out->newToken() << "fall" << data.fall;
   *out->newToken() << "down" << data.down;
   *out->newToken() << "getHitState" << data.getHitState;
   *out->newToken() << "notGetHitState" << data.notGetHitState;

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
   *out->newToken() << "player1" << data.player1;
   *out->newToken() << "player2" << data.player2;

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
   *out->newToken() << "x" << data.x;
   *out->newToken() << "y" << data.y;

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
   *out->newToken() << "state" << data.state;   *out->newToken() << "attackType" << data.attackType;   *out->newToken() << "physics" << data.physics;
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
   *out->newToken() << "hit" << data.hit;
   *out->newToken() << "type" << data.type;
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
   *out->newToken() << "damage" << data.damage;
   *out->newToken() << "guardDamage" << data.guardDamage;

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
   *out->newToken() << "x" << data.x;
   *out->newToken() << "y" << data.y;

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
   *out->newToken() << "hit" << data.hit;
   *out->newToken() << "guarded" << data.guarded;

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
   *out->newToken() << "hit" << data.hit;
   *out->newToken() << "guarded" << data.guarded;

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
   *out->newToken() << "x" << data.x;
   *out->newToken() << "y" << data.y;

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
   *out->newToken() << "x" << data.x;
   *out->newToken() << "y" << data.y;

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
   *out->newToken() << "x" << data.x;
   *out->newToken() << "y" << data.y;

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
   *out->newToken() << "time" << data.time;

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
   *out->newToken() << "fall" << data.fall;
   *out->newToken() << "xVelocity" << data.xVelocity;
   *out->newToken() << "yVelocity" << data.yVelocity;
   *out->newToken() << "changeXVelocity" << data.changeXVelocity;
   *out->newToken() << "recover" << data.recover;
   *out->newToken() << "recoverTime" << data.recoverTime;
   *out->newToken() << "damage" << data.damage;
   *out->newToken() << "airFall" << data.airFall;
   *out->newToken() << "forceNoFall" << data.forceNoFall;

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
   *out->newToken() << "alive" << data.alive;
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
   *out->newToken() << "groundSlideTime" << data.groundSlideTime;
   *out->newToken() << "guardSlideTime" << data.guardSlideTime;
   *out->newToken() << "groundHitTime" << data.groundHitTime;
   *out->newToken() << "guardGroundHitTime" << data.guardGroundHitTime;
   *out->newToken() << "airHitTime" << data.airHitTime;
   *out->newToken() << "guardControlTime" << data.guardControlTime;
   *out->newToken() << "guardDistance" << data.guardDistance;
   *out->newToken() << "yAcceleration" << data.yAcceleration;
    *out->newToken() << "groundVelocity" << serialize(data.groundVelocity);
   *out->newToken() << "guardVelocity" << data.guardVelocity;
    *out->newToken() << "airVelocity" << serialize(data.airVelocity);
    *out->newToken() << "airGuardVelocity" << serialize(data.airGuardVelocity);
   *out->newToken() << "groundCornerPushoff" << data.groundCornerPushoff;
   *out->newToken() << "airCornerPushoff" << data.airCornerPushoff;
   *out->newToken() << "downCornerPushoff" << data.downCornerPushoff;
   *out->newToken() << "guardCornerPushoff" << data.guardCornerPushoff;
   *out->newToken() << "airGuardCornerPushoff" << data.airGuardCornerPushoff;
   *out->newToken() << "airGuardControlTime" << data.airGuardControlTime;
   *out->newToken() << "airJuggle" << data.airJuggle;
   *out->newToken() << "id" << data.id;
   *out->newToken() << "chainId" << data.chainId;
    *out->newToken() << "minimum" << serialize(data.minimum);
    *out->newToken() << "maximum" << serialize(data.maximum);
    *out->newToken() << "snap" << serialize(data.snap);
   *out->newToken() << "player1SpritePriority" << data.player1SpritePriority;
   *out->newToken() << "player2SpritePriority" << data.player2SpritePriority;
   *out->newToken() << "player1Facing" << data.player1Facing;
   *out->newToken() << "player1GetPlayer2Facing" << data.player1GetPlayer2Facing;
   *out->newToken() << "player2Facing" << data.player2Facing;
   *out->newToken() << "player1State" << data.player1State;
   *out->newToken() << "player2State" << data.player2State;
   *out->newToken() << "player2GetPlayer1State" << data.player2GetPlayer1State;
   *out->newToken() << "forceStand" << data.forceStand;
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
    use = data->findToken("_/hitFlag/HitFlags");
    if (use != NULL){
        out.hitFlag = deserializeHitFlags(use);
    }
    use = data->findToken("_/guardFlag/HitFlags");
    if (use != NULL){
        out.guardFlag = deserializeHitFlags(use);
    }
    use = data->findToken("_/animationType/AttackTypeAnimation");
    if (use != NULL){
        out.animationType = deserializeAttackTypeAnimation(use);
    }
    use = data->findToken("_/animationTypeAir/AttackTypeAnimation");
    if (use != NULL){
        out.animationTypeAir = deserializeAttackTypeAnimation(use);
    }
    use = data->findToken("_/animationTypeFall/AttackTypeAnimation");
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
    use = data->findToken("_/pause/PauseTime");
    if (use != NULL){
        out.pause = deserializePauseTime(use);
    }
    use = data->findToken("_/guardPause/PauseTime");
    if (use != NULL){
        out.guardPause = deserializePauseTime(use);
    }
    use = data->findToken("_/spark/ResourceEffect");
    if (use != NULL){
        out.spark = deserializeResourceEffect(use);
    }
    use = data->findToken("_/guardSpark/ResourceEffect");
    if (use != NULL){
        out.guardSpark = deserializeResourceEffect(use);
    }
    use = data->findToken("_/sparkPosition");
    if (use != NULL){
        const Token * child;
        use->view() >> child;
        out.sparkPosition = deserializeSparkPosition(child);
    }
    use = data->findToken("_/hitSound/ResourceEffect");
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
    use = data->findToken("_/guardHitSound/ResourceEffect");
    if (use != NULL){
        out.guardHitSound = deserializeResourceEffect(use);
    }
    use = data->findToken("_/groundType/AttackTypeGround");
    if (use != NULL){
        out.groundType = deserializeAttackTypeGround(use);
    }
    use = data->findToken("_/airType/AttackTypeGround");
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
    use = data->findToken("_/minimum/Distance");
    if (use != NULL){
        out.minimum = deserializeDistance(use);
    }
    use = data->findToken("_/maximum/Distance");
    if (use != NULL){
        out.maximum = deserializeDistance(use);
    }
    use = data->findToken("_/snap/Distance");
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
   *out->newToken() << "time" << data.time;
    *out->newToken() << "attributes" << serialize(data.attributes);
   *out->newToken() << "state" << data.state;
   *out->newToken() << "forceAir" << data.forceAir;

    return out;
}

HitOverride deserializeHitOverride(const Token * data){
    HitOverride out;
    const Token * use = NULL;
    use = data->findToken("_/time");
    if (use != NULL){
        use->view() >> out.time;
    }
    use = data->findToken("_/attributes/HitAttributes");
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
   *out->newToken() << "time" << data.time;

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
   *out->newToken() << "fall" << data.fall;
   *out->newToken() << "recover" << data.recover;
   *out->newToken() << "recoverTime" << data.recoverTime;
   *out->newToken() << "xVelocity" << data.xVelocity;
   *out->newToken() << "yVelocity" << data.yVelocity;
   *out->newToken() << "changeXVelocity" << data.changeXVelocity;
   *out->newToken() << "damage" << data.damage;

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
   *out->newToken() << "shakeTime" << data.shakeTime;
   *out->newToken() << "hitTime" << data.hitTime;
   *out->newToken() << "hits" << data.hits;
   *out->newToken() << "slideTime" << data.slideTime;
   *out->newToken() << "returnControlTime" << data.returnControlTime;
   *out->newToken() << "recoverTime" << data.recoverTime;
   *out->newToken() << "yAcceleration" << data.yAcceleration;
   *out->newToken() << "yVelocity" << data.yVelocity;
   *out->newToken() << "xVelocity" << data.xVelocity;
    *out->newToken() << "animationType" << serialize(data.animationType);
    *out->newToken() << "airType" << serialize(data.airType);
    *out->newToken() << "groundType" << serialize(data.groundType);
    *out->newToken() << "hitType" << serialize(data.hitType);
   *out->newToken() << "guarded" << data.guarded;
   *out->newToken() << "damage" << data.damage;
   *out->newToken() << "chainId" << data.chainId;
   *out->newToken() << "spritePriority" << data.spritePriority;
    *out->newToken() << "fall" << serialize(data.fall);
   *out->newToken() << "moveContact" << data.moveContact;

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
    use = data->findToken("_/animationType/AttackTypeAnimation");
    if (use != NULL){
        out.animationType = deserializeAttackTypeAnimation(use);
    }
    use = data->findToken("_/airType/AttackTypeGround");
    if (use != NULL){
        out.airType = deserializeAttackTypeGround(use);
    }
    use = data->findToken("_/groundType/AttackTypeGround");
    if (use != NULL){
        out.groundType = deserializeAttackTypeGround(use);
    }
    use = data->findToken("_/hitType/AttackTypeGround");
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
   *out->newToken() << "own" << data.own;
   *out->newToken() << "group" << data.group;
   *out->newToken() << "item" << data.item;

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
   *out->newToken() << "sparkX" << data.sparkX;
   *out->newToken() << "sparkY" << data.sparkY;
   *out->newToken() << "player1State" << data.player1State;
   *out->newToken() << "player2State" << data.player2State;
   *out->newToken() << "player1Pause" << data.player1Pause;
   *out->newToken() << "player2Pause" << data.player2Pause;
   *out->newToken() << "standing" << data.standing;
   *out->newToken() << "crouching" << data.crouching;
   *out->newToken() << "aerial" << data.aerial;

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
    use = data->findToken("_/pause/PauseTime");
    if (use != NULL){
        out.pause = deserializePauseTime(use);
    }
    use = data->findToken("_/spark/ResourceEffect");
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
   *out->newToken() << "enabled" << data.enabled;
   *out->newToken() << "edgeFront" << data.edgeFront;
   *out->newToken() << "edgeBack" << data.edgeBack;
   *out->newToken() << "playerFront" << data.playerFront;
   *out->newToken() << "playerBack" << data.playerBack;

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
   *out->newToken() << "standing" << data.standing;
   *out->newToken() << "crouching" << data.crouching;
   *out->newToken() << "aerial" << data.aerial;
   *out->newToken() << "time" << data.time;

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
   *out->newToken() << "enabled" << data.enabled;
    *out->newToken() << "type" << serialize(data.type);
   *out->newToken() << "alphaSource" << data.alphaSource;
   *out->newToken() << "alphaDestination" << data.alphaDestination;

    return out;
}

TransOverride deserializeTransOverride(const Token * data){
    TransOverride out;
    const Token * use = NULL;
    use = data->findToken("_/enabled");
    if (use != NULL){
        use->view() >> out.enabled;
    }
    use = data->findToken("_/type/TransType");
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
   *out->newToken() << "invisible" << data.invisible;
   *out->newToken() << "intro" << data.intro;

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
   *out->newToken() << "time" << data.time;
   *out->newToken() << "facing" << data.facing;
   *out->newToken() << "offsetX" << data.offsetX;
   *out->newToken() << "offsetY" << data.offsetY;

    return out;
}

Bind deserializeBind(const Token * data){
    Bind out;
    const Token * use = NULL;
    use = data->findToken("_/bound/CharacterId");
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
   *out->newToken() << "enabled" << data.enabled;

    return out;
}

CharacterData deserializeCharacterData(const Token * data){
    CharacterData out;
    const Token * use = NULL;
    use = data->findToken("_/who/CharacterId");
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
   *out->newToken() << "enabled" << data.enabled;
   *out->newToken() << "angle" << data.angle;
   *out->newToken() << "scaleX" << data.scaleX;
   *out->newToken() << "scaleY" << data.scaleY;

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
   *out->newToken() << "juggleRemaining" << data.juggleRemaining;
   *out->newToken() << "currentJuggle" << data.currentJuggle;
   *out->newToken() << "currentState" << data.currentState;
   *out->newToken() << "previousState" << data.previousState;
   *out->newToken() << "currentAnimation" << data.currentAnimation;
   *out->newToken() << "velocity_x" << data.velocity_x;
   *out->newToken() << "velocity_y" << data.velocity_y;
   *out->newToken() << "has_control" << data.has_control;
   *out->newToken() << "stateTime" << data.stateTime;

    Token * t1 = out->newToken();
    *t1 << "variables";
    for (std::map<int, RuntimeValue >::const_iterator it = data.variables.begin(); it != data.variables.end(); it++){
        *t1->newToken() << "e" << serialize(it->first) << serialize(it->second);
    }
    
    Token * t2 = out->newToken();
    *t2 << "floatVariables";
    for (std::map<int, RuntimeValue >::const_iterator it = data.floatVariables.begin(); it != data.floatVariables.end(); it++){
        *t2->newToken() << "e" << serialize(it->first) << serialize(it->second);
    }
    
    Token * t3 = out->newToken();
    *t3 << "systemVariables";
    for (std::map<int, RuntimeValue >::const_iterator it = data.systemVariables.begin(); it != data.systemVariables.end(); it++){
        *t3->newToken() << "e" << serialize(it->first) << serialize(it->second);
    }
        *out->newToken() << "currentPhysics" << serialize(data.currentPhysics);
   *out->newToken() << "stateType" << data.stateType;   *out->newToken() << "moveType" << data.moveType;    *out->newToken() << "hit" << serialize(data.hit);
    *out->newToken() << "hitState" << serialize(data.hitState);
   *out->newToken() << "combo" << data.combo;
   *out->newToken() << "hitCount" << data.hitCount;
   *out->newToken() << "blocking" << data.blocking;
   *out->newToken() << "guarding" << data.guarding;
    *out->newToken() << "widthOverride" << serialize(data.widthOverride);
    Token * token_hitByOverride = out->newToken();
    *token_hitByOverride << "hitByOverride";
    *token_hitByOverride->newToken() << 0 << serialize(data.hitByOverride[0]);
    *token_hitByOverride->newToken() << 1 << serialize(data.hitByOverride[1]);
   *out->newToken() << "defenseMultiplier" << data.defenseMultiplier;
   *out->newToken() << "attackMultiplier" << data.attackMultiplier;
   *out->newToken() << "frozen" << data.frozen;
    *out->newToken() << "reversal" << serialize(data.reversal);
   *out->newToken() << "reversalActive" << data.reversalActive;
    *out->newToken() << "transOverride" << serialize(data.transOverride);
   *out->newToken() << "pushPlayer" << data.pushPlayer;
    *out->newToken() << "special" << serialize(data.special);
   *out->newToken() << "health" << data.health;
    *out->newToken() << "bind" << serialize(data.bind);

    Token * t4 = out->newToken();
    *t4 << "targets";
    for (std::map<int, std::vector<CharacterId > >::const_iterator it = data.targets.begin(); it != data.targets.end(); it++){
        *t4->newToken() << "e" << serialize(it->first) << serialize(it->second);
    }
       *out->newToken() << "spritePriority" << data.spritePriority;
   *out->newToken() << "wasHitCounter" << data.wasHitCounter;
    *out->newToken() << "characterData" << serialize(data.characterData);
   *out->newToken() << "drawAngle" << data.drawAngle;
    *out->newToken() << "drawAngleData" << serialize(data.drawAngleData);

    Token * t5 = out->newToken();
    *t5 << "active";
    for (std::vector<std::string >::const_iterator it = data.active.begin(); it != data.active.end(); it++){
        *t5 << serialize(*it);
    }
    
    Token * t6 = out->newToken();
    *t6 << "hitOverrides";
    for (std::map<int, HitOverride >::const_iterator it = data.hitOverrides.begin(); it != data.hitOverrides.end(); it++){
        *t6->newToken() << "e" << serialize(it->first) << serialize(it->second);
    }
       *out->newToken() << "virtualx" << data.virtualx;
   *out->newToken() << "virtualy" << data.virtualy;
   *out->newToken() << "virtualz" << data.virtualz;
    *out->newToken() << "facing" << serialize(data.facing);
   *out->newToken() << "power" << data.power;

    Token * t7 = out->newToken();
    *t7 << "commandState";
    for (std::map<std::string, std::string >::const_iterator it = data.commandState.begin(); it != data.commandState.end(); it++){
        *t7->newToken() << "e" << serialize(it->first) << serialize(it->second);
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
        for (TokenView view = use->view(); view.hasMore(); /**/){
            const Token * entry = view.next();
            int valueKey;
            deserialize_int(valueKey, entry->getToken(1));
            RuntimeValue valueValue;
            deserialize_RuntimeValue(valueValue, entry->getToken(2));
            out.variables[valueKey] = valueValue;
        }

    }
    use = data->findToken("_/floatVariables");
    if (use != NULL){
        for (TokenView view = use->view(); view.hasMore(); /**/){
            const Token * entry = view.next();
            int valueKey;
            deserialize_int(valueKey, entry->getToken(1));
            RuntimeValue valueValue;
            deserialize_RuntimeValue(valueValue, entry->getToken(2));
            out.floatVariables[valueKey] = valueValue;
        }

    }
    use = data->findToken("_/systemVariables");
    if (use != NULL){
        for (TokenView view = use->view(); view.hasMore(); /**/){
            const Token * entry = view.next();
            int valueKey;
            deserialize_int(valueKey, entry->getToken(1));
            RuntimeValue valueValue;
            deserialize_RuntimeValue(valueValue, entry->getToken(2));
            out.systemVariables[valueKey] = valueValue;
        }

    }
    use = data->findToken("_/currentPhysics/PhysicsType");
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
    use = data->findToken("_/hit/HitDefinition");
    if (use != NULL){
        out.hit = deserializeHitDefinition(use);
    }
    use = data->findToken("_/hitState/HitState");
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
    use = data->findToken("_/reversal/ReversalData");
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
        for (TokenView view = use->view(); view.hasMore(); /**/){
            const Token * entry = view.next();
            int valueKey;
            deserialize_int(valueKey, entry->getToken(1));
            std::vector<CharacterId> valueValue;
            deserialize_stdvectorCharacterId(valueValue, entry->getToken(2));
            out.targets[valueKey] = valueValue;
        }

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
        for (TokenView view = use->view(); view.hasMore(); /**/){
            const Token * entry = view.next();
            int valueKey;
            deserialize_int(valueKey, entry->getToken(1));
            HitOverride valueValue;
            deserialize_HitOverride(valueValue, entry->getToken(2));
            out.hitOverrides[valueKey] = valueValue;
        }

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
    use = data->findToken("_/facing/Facing");
    if (use != NULL){
        out.facing = deserializeFacing(use);
    }
    use = data->findToken("_/power");
    if (use != NULL){
        use->view() >> out.power;
    }
    use = data->findToken("_/commandState");
    if (use != NULL){
        for (TokenView view = use->view(); view.hasMore(); /**/){
            const Token * entry = view.next();
            std::string valueKey;
            deserialize_stdstring(valueKey, entry->getToken(1));
            std::string valueValue;
            deserialize_stdstring(valueValue, entry->getToken(2));
            out.commandState[valueKey] = valueValue;
        }

    }

    return out;
}


Token * serialize(const AnimationState & data){
    Token * out = new Token();
    *out << "AnimationState";
   *out->newToken() << "position" << data.position;
   *out->newToken() << "looped" << data.looped;
   *out->newToken() << "started" << data.started;
   *out->newToken() << "ticks" << data.ticks;
   *out->newToken() << "virtual_ticks" << data.virtual_ticks;

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
   *out->newToken() << "enabled" << data.enabled;
   *out->newToken() << "offScreen" << data.offScreen;
   *out->newToken() << "panX" << data.panX;
   *out->newToken() << "panY" << data.panY;

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
   *out->newToken() << "time" << data.time;
   *out->newToken() << "buffer" << data.buffer;
   *out->newToken() << "moveTime" << data.moveTime;
   *out->newToken() << "pauseBackground" << data.pauseBackground;
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
    use = data->findToken("_/who/CharacterId");
    if (use != NULL){
        out.who = deserializeCharacterId(use);
    }

    return out;
}


Token * serialize(const Zoom & data){
    Token * out = new Token();
    *out << "Zoom";
   *out->newToken() << "enabled" << data.enabled;
   *out->newToken() << "x" << data.x;
   *out->newToken() << "y" << data.y;
   *out->newToken() << "zoomTime" << data.zoomTime;
   *out->newToken() << "zoomOutTime" << data.zoomOutTime;
   *out->newToken() << "zoom" << data.zoom;
   *out->newToken() << "in" << data.in;
   *out->newToken() << "time" << data.time;
   *out->newToken() << "bindTime" << data.bindTime;
   *out->newToken() << "deltaX" << data.deltaX;
   *out->newToken() << "deltaY" << data.deltaY;
   *out->newToken() << "scaleX" << data.scaleX;
   *out->newToken() << "scaleY" << data.scaleY;
   *out->newToken() << "velocityX" << data.velocityX;
   *out->newToken() << "velocityY" << data.velocityY;
   *out->newToken() << "accelX" << data.accelX;
   *out->newToken() << "accelY" << data.accelY;
   *out->newToken() << "superMoveTime" << data.superMoveTime;
   *out->newToken() << "pauseMoveTime" << data.pauseMoveTime;
   *out->newToken() << "removeOnGetHit" << data.removeOnGetHit;
   *out->newToken() << "hitCount" << data.hitCount;
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
    use = data->findToken("_/bound/CharacterId");
    if (use != NULL){
        out.bound = deserializeCharacterId(use);
    }
    use = data->findToken("_/owner/CharacterId");
    if (use != NULL){
        out.owner = deserializeCharacterId(use);
    }

    return out;
}


Token * serialize(const EnvironmentColor & data){
    Token * out = new Token();
    *out << "EnvironmentColor";
    *out->newToken() << "color" << serialize(data.color);
   *out->newToken() << "time" << data.time;
   *out->newToken() << "under" << data.under;

    return out;
}

EnvironmentColor deserializeEnvironmentColor(const Token * data){
    EnvironmentColor out;
    const Token * use = NULL;
    use = data->findToken("_/color/GraphicsColor");
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
   *out->newToken() << "time" << data.time;
   *out->newToken() << "positionX" << data.positionX;
   *out->newToken() << "positionY" << data.positionY;
   *out->newToken() << "soundGroup" << data.soundGroup;
   *out->newToken() << "soundItem" << data.soundItem;

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
        *t1->newToken() << "e" << serialize(it->first) << serialize(it->second);
    }
        *out->newToken() << "zoom" << serialize(data.zoom);
    *out->newToken() << "environmentColor" << serialize(data.environmentColor);
    *out->newToken() << "superPause" << serialize(data.superPause);
   *out->newToken() << "quake_time" << data.quake_time;
   *out->newToken() << "cycles" << data.cycles;
   *out->newToken() << "inleft" << data.inleft;
   *out->newToken() << "inright" << data.inright;
   *out->newToken() << "onLeftSide" << data.onLeftSide;
   *out->newToken() << "onRightSide" << data.onRightSide;
   *out->newToken() << "inabove" << data.inabove;
   *out->newToken() << "camerax" << data.camerax;
   *out->newToken() << "cameray" << data.cameray;
   *out->newToken() << "ticker" << data.ticker;

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
        for (TokenView view = use->view(); view.hasMore(); /**/){
            const Token * entry = view.next();
            CharacterId valueKey;
            deserialize_CharacterId(valueKey, entry->getToken(1));
            ScreenBound valueValue;
            deserialize_ScreenBound(valueValue, entry->getToken(2));
            out.screenBound[valueKey] = valueValue;
        }

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


Token * serialize(const PlayerData & data){
    Token * out = new Token();
    *out << "PlayerData";
   *out->newToken() << "oldx" << data.oldx;
   *out->newToken() << "oldy" << data.oldy;
   *out->newToken() << "leftTension" << data.leftTension;
   *out->newToken() << "rightTension" << data.rightTension;
   *out->newToken() << "leftSide" << data.leftSide;
   *out->newToken() << "rightSide" << data.rightSide;
   *out->newToken() << "above" << data.above;
   *out->newToken() << "jumped" << data.jumped;

    return out;
}

PlayerData deserializePlayerData(const Token * data){
    PlayerData out;
    const Token * use = NULL;
    use = data->findToken("_/oldx");
    if (use != NULL){
        use->view() >> out.oldx;
    }
    use = data->findToken("_/oldy");
    if (use != NULL){
        use->view() >> out.oldy;
    }
    use = data->findToken("_/leftTension");
    if (use != NULL){
        use->view() >> out.leftTension;
    }
    use = data->findToken("_/rightTension");
    if (use != NULL){
        use->view() >> out.rightTension;
    }
    use = data->findToken("_/leftSide");
    if (use != NULL){
        use->view() >> out.leftSide;
    }
    use = data->findToken("_/rightSide");
    if (use != NULL){
        use->view() >> out.rightSide;
    }
    use = data->findToken("_/above");
    if (use != NULL){
        use->view() >> out.above;
    }
    use = data->findToken("_/jumped");
    if (use != NULL){
        use->view() >> out.jumped;
    }

    return out;
}

}

