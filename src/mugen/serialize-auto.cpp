
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



Token * serialize(const Attribute & data){
    Token * out = new Token();
    *out << "Attribute";
    *out->newToken() << "state" << serialize(data.state);
    *out->newToken() << "attackType" << serialize(data.attackType);
    *out->newToken() << "physics" << serialize(data.physics);

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



Token * serialize(const Shake & data){
    Token * out = new Token();
    *out << "Shake";
    if (data.time != 0){
        *out->newToken() << "time" << data.time;
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




Token * serialize(const Shake1 & data){
    Token * out = new Token();
    *out << "Shake1";
    if (data.time != 0){
        *out->newToken() << "time" << data.time;
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


Token * serialize(const CharacterData & data){
    Token * out = new Token();
    *out << "CharacterData";
    *out->newToken() << "who" << serialize(data.who);
    if (data.enabled != false){
        *out->newToken() << "enabled" << data.enabled;
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
        *t1->newToken() << it->first << serialize(it->second);
    }

    Token * t2 = out->newToken();
    *t2 << "floatVariables";
    for (std::map<int, RuntimeValue >::const_iterator it = data.floatVariables.begin(); it != data.floatVariables.end(); it++){
        *t2->newToken() << it->first << serialize(it->second);
    }

    Token * t3 = out->newToken();
    *t3 << "systemVariables";
    for (std::map<int, RuntimeValue >::const_iterator it = data.systemVariables.begin(); it != data.systemVariables.end(); it++){
        *t3->newToken() << it->first << serialize(it->second);
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
        *t4->newToken() << it->first << serialize(it->second);
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
        *t6->newToken() << it->first << serialize(it->second);
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

}

