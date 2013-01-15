
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


Token * serialize(const ResourceEffect & data){
    Token * out = new Token();
    *out << "ResourceEffect";
    *out->newToken() << "own" << data.own;
    *out->newToken() << "group" << data.group;
    *out->newToken() << "item" << data.item;

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


Token * serialize(const PauseTime & data){
    Token * out = new Token();
    *out << "PauseTime";
    *out->newToken() << "player1" << data.player1;
    *out->newToken() << "player2" << data.player2;

    return out;
}


Token * serialize(const Distance & data){
    Token * out = new Token();
    *out << "Distance";
    *out->newToken() << "x" << data.x;
    *out->newToken() << "y" << data.y;

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
    *out->newToken() << "hit" << data.hit;
    *out->newToken() << "type" << serialize(data.type);

    return out;
}


Token * serialize(const Damage & data){
    Token * out = new Token();
    *out << "Damage";
    *out->newToken() << "damage" << data.damage;
    *out->newToken() << "guardDamage" << data.guardDamage;

    return out;
}


Token * serialize(const SparkPosition & data){
    Token * out = new Token();
    *out << "SparkPosition";
    *out->newToken() << "x" << data.x;
    *out->newToken() << "y" << data.y;

    return out;
}


Token * serialize(const GetPower & data){
    Token * out = new Token();
    *out << "GetPower";
    *out->newToken() << "hit" << data.hit;
    *out->newToken() << "guarded" << data.guarded;

    return out;
}


Token * serialize(const GivePower & data){
    Token * out = new Token();
    *out << "GivePower";
    *out->newToken() << "hit" << data.hit;
    *out->newToken() << "guarded" << data.guarded;

    return out;
}


Token * serialize(const GroundVelocity & data){
    Token * out = new Token();
    *out << "GroundVelocity";
    *out->newToken() << "x" << data.x;
    *out->newToken() << "y" << data.y;

    return out;
}


Token * serialize(const AirVelocity & data){
    Token * out = new Token();
    *out << "AirVelocity";
    *out->newToken() << "x" << data.x;
    *out->newToken() << "y" << data.y;

    return out;
}


Token * serialize(const AirGuardVelocity & data){
    Token * out = new Token();
    *out << "AirGuardVelocity";
    *out->newToken() << "x" << data.x;
    *out->newToken() << "y" << data.y;

    return out;
}



Token * serialize(const Shake & data){
    Token * out = new Token();
    *out << "Shake";
    *out->newToken() << "time" << data.time;

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


Token * serialize(const HitOverride & data){
    Token * out = new Token();
    *out << "HitOverride";
    *out->newToken() << "time" << data.time;
    *out->newToken() << "attributes" << serialize(data.attributes);
    *out->newToken() << "state" << data.state;
    *out->newToken() << "forceAir" << data.forceAir;

    return out;
}




Token * serialize(const Shake1 & data){
    Token * out = new Token();
    *out << "Shake1";
    *out->newToken() << "time" << data.time;

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



Token * serialize(const HitSound & data){
    Token * out = new Token();
    *out << "HitSound";
    *out->newToken() << "own" << data.own;
    *out->newToken() << "group" << data.group;
    *out->newToken() << "item" << data.item;

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


Token * serialize(const TransOverride & data){
    Token * out = new Token();
    *out << "TransOverride";
    *out->newToken() << "enabled" << data.enabled;
    *out->newToken() << "type" << serialize(data.type);
    *out->newToken() << "alphaSource" << data.alphaSource;
    *out->newToken() << "alphaDestination" << data.alphaDestination;

    return out;
}


Token * serialize(const SpecialStuff & data){
    Token * out = new Token();
    *out << "SpecialStuff";
    *out->newToken() << "invisible" << data.invisible;
    *out->newToken() << "intro" << data.intro;

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


Token * serialize(const CharacterData & data){
    Token * out = new Token();
    *out << "CharacterData";
    *out->newToken() << "who" << serialize(data.who);
    *out->newToken() << "enabled" << data.enabled;

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
        *t4->newToken() << it->first << serialize(it->second);
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
        *t6->newToken() << it->first << serialize(it->second);
    }
    *out->newToken() << "virtualx" << data.virtualx;
    *out->newToken() << "virtualy" << data.virtualy;
    *out->newToken() << "virtualz" << data.virtualz;
    *out->newToken() << "facing" << serialize(data.facing);
    *out->newToken() << "power" << data.power;

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

}

