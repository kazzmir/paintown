#ifndef paintown_mugen_character_h
#define paintown_mugen_character_h

#include <fstream>
#include <string>
#include <vector>
#include <deque>
#include <map>
#include "exception.h"

// Implement object_attack
#include "object/object_attack.h"
#include "network/network.h"
#include "util/pointer.h"
#include "input/input-map.h"
#include "animation.h"
#include "util.h"
#include "compiler.h"

namespace Ast{
    class KeyList;
    class Key;
    class Value;
    class Section;
}

/*
namespace Mugen{
namespace Compiler{
    class Value;
}
}
*/

namespace PaintownUtil = Util;

class Bitmap;
class MugenItemContent;
class MugenSprite;
class MugenSound;
class MugenAnimation;
class MugenStage;

namespace Mugen{

class Behavior;

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

namespace Physics{

enum Type{
    None, /* N */
    Air, /* A */
    Stand, /* S */
    Crouch, /* C */
};

}

namespace StateType{
    extern std::string Stand;
    extern std::string Crouch;
    extern std::string Air;
    extern std::string LyingDown;
}

namespace Move{
    extern std::string Attack;
    extern std::string Idle;
    extern std::string Hit;
}

namespace AttackType{
    extern std::string Normal;
    extern std::string Special;
    extern std::string Hyper;

    enum Animation{
        NoAnimation = -1,
        Light = 0,
        Medium = 1,
        Hard = 2,
        Back = 3,
        Up = 4,
        DiagonalUp = 5
    };

    enum Ground{
        None = 0,
        High,
        Low,
        Trip
    };
}

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

namespace PhysicalAttack{
    extern std::string Normal;
    extern std::string Throw;
    extern std::string Projectile;
}

class Character;

struct HitDefinition{
    HitDefinition():
    hitFlag("MAF"),
    animationType(AttackType::Light),
    animationTypeAir(AttackType::NoAnimation),
    animationTypeFall(AttackType::NoAnimation),
    spark(-1),
    guardSpark(-1),
    groundType(AttackType::None),
    airType(AttackType::None),
    groundHitTime(0),
    airHitTime(20),
    yAcceleration(0.35),
    guardVelocity(0),
    airJuggle(0)
    {}

    ~HitDefinition();

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
        std::string state;
        /* AttackType */
        std::string attackType;
        /* PhysicalAttack */
        std::string physics;
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
    std::string hitFlag;

    /* guardflag = hit_flags (string)
     * This determines how P2 may guard the attack. hit_flags is a string containing a combination of the following characters:
     *
     * "H" for "high", "L" for "low" or "A" for air. "M" (mid) is equivalent to saying "HL". If omitted, defaults to an empty string, meaning P2 cannot guard the attack.
     * affectteam = team_type (string)
     * team_type specifies which team's players can be hit by this HitDef. Use B for both teams (all players), E for enemy team (opponents), or F for friendly team (your own team). The default is E.
     */
    std::string guardFlag;

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
        int hit;
        std::string type;
    } priority;

    /* damage = hit_damage, guard_damage (int, int)
     * hit_damage is the damage that P2 takes when hit by P2. The optional guard_damage parameter is the damage taken by P2 if the hit is guarded. Both default to zero if omitted.
     */
    struct Damage{
        Damage(){
        }

        ~Damage();

        PaintownUtil::ClassPointer<Compiler::Value> damage;
        PaintownUtil::ClassPointer<Compiler::Value> guardDamage;
    } damage;

    /* pausetime = p1_pausetime, p2_shaketime (int, int)
     * This is the time that each player will pause on the hit. p1_pausetime is the time to freeze P1, measured in game-ticks. p2_pausetime is the time to make P2 shake before recoiling from the hit. Defaults to 0,0 if omitted.
     */
    struct PauseTime{
        PauseTime():
            player1(0), player2(0){}
        int player1;
        int player2;
    } pause;

    /* guard.pausetime = p1_pausetime, p2_shaketime (int, int)
     * Similar to the "pausetime" parameter, these are the times to pause each player if the hit was guarded. Defaults to the same values as the "pausetime" parameter if omitted.
     */
    struct GuardPauseTime{
        int player1;
        int player2;
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
        int x, y;
    } sparkPosition;

    /* hitsound = snd_grp, snd_item (int, int)
     * This is the sound to play on hit (from common.snd). The included fight.snd lets you choose from 5,0 (light hit sound) through to 5,4 (painful whack). To play a sound from the player's own SND file, precede the first number with an "S". For example, "hitsound = S1,0". Defaults to the value set in the player variables if omitted.
     */
    struct HitSound{
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
            group(0),
            item(0){
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
    PaintownUtil::ClassPointer<Compiler::Value> groundSlideTime;

    /* guard.slidetime = slide_time (int)
     * Same as "ground.slidetime", but this is the value if P2 guards the hit. Defaults to same value as "guard.hittime".
     */
    int guardSlideTime;

    /* ground.hittime = hit_time (int)
     * Time that P2 stays in the hit state after being hit. Adjust this value carefully, to make combos possible. Applicable only to hits that keep P2 on the ground. Defaults to 0 if omitted.
     */
    int groundHitTime;

    /* guard.hittime = hit_time (int)
    * Same as "ground.hittime", but this is the value if P2 guards the hit. Defaults to same value as "ground.hittime".
    */
    int guardGroundHitTime;

    /* air.hittime = hit_time (int)
    * Time that p2 stays in the hit state after being hit in or into the air, before being able to guard again. This parameter has no effect if the "fall" parameter is set to 1. Defaults to 20 if omitted.
    */
    int airHitTime;

    /* guard.ctrltime = ctrl_time (int)
    * This is the time before p2 regains control in the ground guard state. Defaults to the same value as "guard.slidetime" if omitted.
    */
    int guardControlTime;

    /* guard.dist = x_dist (int)
    * This is the x-distance from P1 in which P2 will go into a guard state if P2 is holding the direction away from P1. Defaults to the value in the player variables if omitted. You normally do not need to use this parameter.
    */
    int guardDistance;

    /* yaccel = accel (float)
    * Specifies the y acceleration to impart to P2 if the hit connects. Defaults to .35 in 240p, .7 in 480p, 1.4 in 720p.
    */
    double yAcceleration;

    /* ground.velocity = x_velocity, y_velocity (float, float)
    * Initial velocity to give P2 after being hit, if P2 is on the ground. If y_velocity is not zero, P2 will be knocked into the air. Both values default to 0 if omitted. You can leave out the y_velocity if you want P2 to remain on the ground.
    */
    struct GroundVelocity{
        GroundVelocity():
            x(0), y(0){}
        double x, y;
    } groundVelocity;

    /* guard.velocity = x_velocity (float)
    * Velocity to give P2 if P2 guards the hit on the ground. Defaults to the x_velocity value of the "ground.velocity" parameter if omitted.
    */
    double guardVelocity;

    /* air.velocity = x_velocity, y_velocity (float, float)
    * Initial velocity to give P2 if P2 is hit in the air. Defaults to 0,0 if omitted.
    */
    struct AirVelocity{
        AirVelocity():
            x(0), y(0){}

        double x, y;
    } airVelocity;

    /* airguard.velocity = x_velocity, y_velocity (float float)
    * Velocity to give P2 if P2 guards the hit in the air. Defaults to x_velocity * 1.5, y_velocity / 2, where x_velocity and y_velocity are values of the "air.velocity" parameter.
    */
    struct AirGuardVelocity{
        AirGuardVelocity():
            x(0), y(0){}

        double x, y;
    } airGuardVelocity;

    /* ground.cornerpush.veloff = x_velocity (float)
    * Determines the additional velocity (velocity offset) to impart to the player if he lands a ground hit in the corner. Setting this to a higher value will cause the player to be "pushed back" farther out of the corner. If omitted, default value depends on the attr parameter. If arg1 of attr is "A", default value is 0. Otherwise, defaults to 1.3 * guard.velocity.
    */
    PaintownUtil::ClassPointer<Compiler::Value> groundCornerPushoff;

    /* air.cornerpush.veloff = x_velocity (float)
    * Determines the additional velocity (velocity offset) to impart to the player if he lands a hit to an aerial opponent in the corner. Setting this to a higher value will cause the player to be "pushed back" farther out of the corner. Defaults to ground.cornerpush.veloff if omitted.
    */
    double airCornerPushoff;

    /* down.cornerpush.veloff = x_velocity (float)
    * Determines the additional velocity (velocity offset) to impart to the player if he lands a hit on a downed opponent in the corner. Setting this to a higher value will cause the player to be "pushed back" farther out of the corner. Defaults to ground.cornerpush.veloff if omitted.
    */
    double downCornerPushoff;

    /* guard.cornerpush.veloff = x_velocity (float)
    * Determines the additional velocity (velocity offset) to impart to the player if his hit is guarded in the corner. Setting this to a higher value will cause the player to be "pushed back" farther out of the corner. Defaults to ground.cornerpush.veloff if omitted.
    */
    double guardCornerPushoff;

    /* airguard.cornerpush.veloff = x_velocity (float)
    * Determines the additional velocity (velocity offset) to impart to the player if his hit is guarded in the corner. Setting this to a higher value will cause the player to be "pushed back" farther out of the corner. Defaults to guard.cornerpush.veloff if omitted.
    */
    double airGuardCornerPushoff;

    /* airguard.ctrltime = ctrl_time (int)
    * This is the time before p2 regains control in the air guard state. Defaults to the same value as "guard.ctrltime" if omitted.
    */
    int airGuardControlTime;

    /* air.juggle = juggle_points (int)
    * The amount of additional juggle points the hit requires. Not to be confused with the "juggle" parameter in the StateDef. You typically do not need this parameter, except for HitDefs of projectiles. Defaults to 0 if omitted.
    */
    int airJuggle;

    /* mindist = x_pos, y_pos (int, int)
    * See below.
    * maxdist = x_pos, y_pos (int, int)
    * These let you control the minimum and maximum distance of P2 relative to P1, after P2 has been hit. These parameters are not commonly used. Defaults to no change in P2's position if omitted.
    */
    struct Distance{
        int x, y;
    };

    Distance minimum, maximum;

    /* snap = x_pos, y_pos (int, int)
    * This moves P2 to the specified position relative to P1 if hit. This parameter is not normally used. If you want to snap P2 to a particular position for a throw, it is recommended you use a "TargetBind" controller in P1's throwing state instead. Defaults to no change in P2's position if omitted.
    */
    Distance snap;

    /* p1sprpriority = drawing_priority (int)
    * This is the drawing priority of P1's sprite if the move hits or is guarded by P2. Together with the p2sprpriority parameter, it controls whether or not P1 is drawn in front of or behind P2. The default value is 1.
    */
    PaintownUtil::ClassPointer<Compiler::Value> player1SpritePriority;

    /* p2sprpriority = drawing_priority (int)
    * This is the drawing priority of P2's sprite if the move hits or is guarded by P2. The default value is 0.
    */
    int player2SpritePriority;

    /* p1facing = facing (int)
    * Set to -1 to make P1 turn around if the hit is successful. The default value is no change in where P1 is facing.
    */
    PaintownUtil::ClassPointer<Compiler::Value> player1Facing;

    /* p1getp2facing = facing (int)
    * Set to 1 to have P1 face in the same direction as P2 is facing after the hit connects, and -1 to have P1 face the opposite direction from P2. Defaults to 0 (no change). If nonzero, this parameter takes precedence over p1facing.
    */
    PaintownUtil::ClassPointer<Compiler::Value> player1GetPlayer2Facing;

    /* p2facing = facing (int)
    * Set to 1 to make P2 face the same direction as P1 if the hit is successful, -1 to make P2 face away. The default value is 0, no change in where P2 is facing.
    */
    PaintownUtil::ClassPointer<Compiler::Value> player2Facing;

    /* p1stateno = state_no (int)
    * This is the number of the state to set P1 to if the hit is successful. The state must be an attack state (movetype = A) for at least 1 tick. Used mainly for throws. Defaults to -1, no change.
    */
    PaintownUtil::ClassPointer<Compiler::Value> player1State;

    /* p2stateno = state_no (int)
    * This is the number of the state to set P2 to if the hit is successful. P2 will get P1's state and animation data. P2 will retain P1's states and animation data until P2 is hit, or a SelfState controller is used to return P2 to his own states. The state must be a get-hit state (movetype = H) for at least 1 tick. Used mainly for throws; can also be used for custom hit reactions. Defaults to -1, no change.
    */
    PaintownUtil::ClassPointer<Compiler::Value> player2State;

    /* p2getp1state = bvalue (boolean)
    * Set to 0 to prevent P2 from getting P1's state and animation data, in case you do not want that default behaviour of the "p2stateno" parameter. Defaults to 1 if the "p2stateno" parameter is used. Ignored otherwise.
    */
    PaintownUtil::ClassPointer<Compiler::Value> player2GetPlayer1State;

    /* forcestand = bvalue (boolean)
    * Set to 1 to force P2 to a standing state-type if the hit is successful, and P2 is in a crouching state. Has no effect if P2 is in an air state. Normally defaults to 0, but if the y_velocity of the "ground.velocity" parameter is non-zero, it defaults to 1.
    */
    int forceStand;

    struct Fall{
        Fall():
            yVelocity(0){
            }

        ~Fall();

        /* fall = bvalue (boolean)
         * Set to 1 if you want P2 to go into a "fall" state (where P2 hits the ground without regaining control in the air). Use if you want a move to "knock down" P2. Defaults to 0.
         */
        PaintownUtil::ClassPointer<Compiler::Value> fall;

        /* fall.xvelocity = x_velocity (float)
         * This is the x-velocity that P2 gets when bouncing off the ground in the "fall" state. Defaults to no change if omitted.
         */
        double xVelocity;

        /* fall.yvelocity = y_velocity (float)
         * This is the y-velocity that P2 gets when bouncing off the ground in the "fall" state. Defaults to -4.5 in 240p, -9 in 480p, -18 in 720p.
         */
        double yVelocity;

        /* fall.recover = bvalue (boolean)
         * Set to 0 if you do not want P2 to be able to recover from the "fall" state. Defaults to 1 if omitted (can recover).
         */
        PaintownUtil::ClassPointer<Compiler::Value> recover;

         /* fall.recovertime = recover_time (int)
         * This is the time that must pass before P2 is able to recover from the "fall" state by inputting his recovery command. Does not include the time that P2 is paused for while shaking from the hit. Defaults to 4 if omitted.
         */
        int recoverTime;

        /* fall.damage = damage_amt (int)
         * Indicates the amount of damage to deal when P2 hits the ground out of a falling state. Defaults to 0 if omitted.
         */
        PaintownUtil::ClassPointer<Compiler::Value> damage;

         /* air.fall = bvalue (boolean)
         * Set to 1 if you want P2 to go into a "fall" state (where P2 hits the ground without regaining control in the air) if hit while P2 is in the air. Defaults to the same value as fall.
         */
        int airFall;

         /* forcenofall = bvalue (boolean)
         * Set to 1 to force P2 out of a "fall" state, if he is in one. This parameter has no effect on P2 if he is not in a "fall" state. This parameter is ignored if the "fall" parameter is set to 1. Defaults to 0 if omitted.
         */
        int forceNoFall;
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

struct HitState{
    HitState():
        shakeTime(0),
        hitTime(-1),
        hits(0),
        slideTime(0),
        returnControlTime(0),
        yAcceleration(0),
        yVelocity(0),
        xVelocity(0),
        guarded(false),
        damage(0){
        }

    void update(MugenStage & stage, const Character & guy, bool inAir, const HitDefinition & hit);
    int shakeTime;
    int hitTime;
    
    /* FIXME: handle hits somehow. corresponds to hitcount
     * hitcount: Returns the number of hits taken by the player in current combo. (int)
     */
    int hits;
    int slideTime;
    int returnControlTime;
    double yAcceleration;
    double yVelocity;
    double xVelocity;
    AttackType::Animation animationType;
    AttackType::Ground groundType;
    AttackType::Ground airType;
    bool guarded;
    int damage;

    struct Fall{
        Fall():
            fall(false),
            yVelocity(0){
            }

        bool fall;
        double yVelocity;
    } fall;
};

class StateController;

/* comes from a StateDef */
class State{
public:
    State();

    enum Type{
        Standing,
        Crouching,
        Air,
        LyingDown,
        Unchanged,
    };

    virtual inline void setType(Type t){
        type = t;
    }

    virtual inline void setAnimation(Compiler::Value * animation){
        this->animation = animation;
    }

    virtual inline void setControl(Compiler::Value * control){
        changeControl = true;
        this->control = control;
    }

    virtual void setJuggle(Compiler::Value * juggle);

    virtual void setVelocity(Compiler::Value * x, Compiler::Value * y);
    virtual void setPhysics(Physics::Type p);
    virtual void setPower(Compiler::Value * power);

    virtual inline Compiler::Value * getPower() const {
        return powerAdd;
    }

    virtual inline bool powerChanged() const {
        return changePower;
    }

    virtual void setMoveType(const std::string & type);
                    
    virtual inline void setHitDefPersist(bool what){
        hitDefPersist = what;
    }
    
    virtual inline bool doesHitDefPersist() const {
        return hitDefPersist;
    }

    virtual inline const std::vector<StateController*> & getControllers() const {
        return controllers;
    }

    virtual void addController(StateController * controller);
    virtual void addControllerFront(StateController * controller);

    virtual void transitionTo(const MugenStage & stage, Character & who);

    virtual ~State();

protected:
    Type type;
    Compiler::Value * animation;
    bool changeControl;
    Compiler::Value * control;
    std::vector<StateController*> controllers;
    bool changeVelocity;
    Compiler::Value * velocity_x, * velocity_y;
    bool changePhysics;
    Physics::Type physics;
    bool changePower;
    Compiler::Value * powerAdd;
    std::string moveType;
    Compiler::Value * juggle;
    bool hitDefPersist;
};

class Command;

class Character: public ObjectAttack {
public:
	// Location at dataPath() + "mugen/chars/"
	Character(const Filesystem::AbsolutePath & s );
	// Character(const char * location );
	Character(const Filesystem::AbsolutePath & s, int alliance );
	Character(const Filesystem::AbsolutePath & s, const int x, const int y, int alliance );
	Character(const Character &copy );

	virtual ~Character();
	
	// Convert to paintown character or whatever
	// Do code
	
	virtual void load(int useAct = 1);
	
	virtual void renderSprite(const int x, const int y, const unsigned int group, const unsigned int image, Bitmap *bmp, const int flip=1, const double scalex = 1, const double scaley = 1);
			   
	// Change palettes
	virtual void nextPalette();
	virtual void priorPalette();
	
	virtual inline const std::string & getName() const {
            return name;
        }

        virtual inline const std::string & getAuthor() const {
            return author;
        }
	
	virtual inline const std::string &getDisplayName() const {
            return displayName;
        }
	
	virtual inline unsigned int getCurrentPalette() const {
            return currentPalette;
        }

        virtual inline const std::map<int, MugenAnimation*> & getAnimations() const {
            return animations;
        }

        /*
        virtual inline void setInput(const InputMap<Mugen::Keys> & inputLeft, const InputMap<Mugen::Keys> & inputRight){
            this->inputLeft = inputLeft;
            this->inputRight = inputRight;
        }
        */
	
	virtual inline const std::map<unsigned int, std::map<unsigned int, MugenSound *> >& getSounds() const {
            return sounds;
        }

	virtual inline const std::map<unsigned int, std::map<unsigned int, MugenSound *> >* getCommonSounds() const {
            return commonSounds;
        }

        virtual inline MugenSprite * getSprite(int group, int image){
            return this->sprites[group][image];
        }

        virtual const Bitmap * getCurrentFrame() const;
        MugenAnimation * getCurrentAnimation() const;

        virtual void drawReflection(Bitmap * work, int rel_x, int rel_y, int intensity);
	
	/*! This all the inherited members */
	virtual void act(std::vector<Object*, std::allocator<Object*> >*, World*, std::vector<Object*, std::allocator<Object*> >*);                       
	virtual void draw(Bitmap*, int cameraX, int cameraY);
	virtual void grabbed(Object*);
	virtual void unGrab();
	virtual bool isGrabbed();
	virtual Object* copy();
	virtual const std::string& getAttackName();
	virtual bool collision(ObjectAttack*);
	virtual int getDamage() const;
	virtual bool isCollidable(Object*);
	virtual bool isGettable();
	virtual bool isGrabbable(Object*);
	virtual bool isAttacking();
	virtual int getWidth() const;
        virtual int getBackWidth() const;

        /* absolute X coordinate of the back of the character */
        virtual int getBackX() const;
        /* same thing for the front */
        virtual int getFrontX() const;
	virtual Network::Message getCreateMessage();
	virtual void getAttackCoords(int&, int&);
	virtual double minZDistance() const;
	virtual void attacked(World*, Object*, std::vector<Object*, std::allocator<Object*> >&);

        virtual void changeState(MugenStage & stage, int state, const std::vector<std::string> & inputs);

        virtual void setAnimation(int animation);

        /* true if enemy can hit this */
        virtual bool canBeHit(Character * enemy);
        
        virtual inline int getAnimation() const {
            return this->currentAnimation;
        }

        virtual inline int getCurrentState() const {
            return this->currentState;
        }

        virtual inline std::string getStateType() const {
            return stateType;
        }

        virtual inline void setStateType(const std::string & str){
            stateType = str;
        }

        virtual inline void setXVelocity(double x){
            this->velocity_x = x;
        }

        virtual inline double getXVelocity() const {
            return this->velocity_x;
        }
        
        virtual inline void setYVelocity(double y){
            this->velocity_y = y;
        }
        
        virtual inline double getYVelocity() const {
            return this->velocity_y;
        }

        virtual inline double getWalkBackX() const {
            return walkback;
        }
        
        virtual inline double getWalkForwardX() const {
            return walkfwd;
        }

        virtual inline void setWalkBackX(double x){
            walkback = x;
        }
        
        virtual inline void setWalkForwardX(double x){
            walkfwd = x;
        }
        
        virtual inline double getRunBackX() const {
            return runbackx;
        }

        virtual inline void setRunBackX(double x){
            runbackx = x;
        }

        virtual inline void setRunBackY(double x){
            runbacky = x;
        }

        virtual inline double getRunBackY() const {
            return runbacky;
        }
        
        virtual inline double getRunForwardX() const {
            return runforwardx;
        }

        virtual inline void setRunForwardX(double x){
            runforwardx = x;
        }
        
        virtual inline double getRunForwardY() const {
            return runforwardy;
        }
        
        virtual inline void setRunForwardY(double x){
            runforwardy = x;
        }

        virtual inline double getNeutralJumpingX() const {
            return jumpneux;
        }

        virtual inline void setNeutralJumpingX(double x){
            jumpneux = x;
        }
        
        virtual inline double getNeutralJumpingY() const {
            return jumpneuy;
        }
        
        virtual inline void setNeutralJumpingY(double x){
            jumpneuy = x;
        }

        virtual inline double getYPosition() const {
            return getY();
        }
        
        virtual inline double getPower() const {
            return power;
        }

        virtual void addPower(double d);

        virtual inline bool hasControl() const {
            return has_control;
        }

        virtual inline void setControl(bool b){
            has_control = b;
        }

        virtual inline void setJumpBack(double x){
            jumpback = x;
        }

        virtual inline double getJumpBack() const {
            return jumpback;
        }

        virtual inline void setJumpForward(double x){
            jumpfwd = x;
        }

        virtual inline double getJumpForward() const {
            return jumpfwd;
        }
        
        virtual inline void setRunJumpBack(double x){
            runjumpback = x;
        }

        virtual inline int getRunJumpBack() const {
            return runjumpback;
        }

        virtual inline void setRunJumpForward(double x){
            runjumpfwd = x;
        }

        virtual inline double getRunJumpForward() const {
            return runjumpfwd;
        }

        virtual inline void setAirJumpNeutralX(double x){
            airjumpneux = x;
        }

        virtual inline double getAirJumpNeutralX() const {
            return airjumpneux;
        }
        
        virtual inline void setAirJumpNeutralY(double y){
            airjumpneuy = y;
        }

        virtual inline double getAirJumpNeutralY() const {
            return airjumpneuy;
        }

        virtual inline void setAirJumpBack(double x){
            airjumpback = x;
        }

        virtual inline double getAirJumpBack() const {
            return airjumpback;
        }

        virtual inline void setAirJumpForward(double x){
            airjumpfwd = x;
        }

        virtual inline double getAirJumpForward() const {
            return airjumpfwd;
        }

        virtual inline int getStateTime() const {
            return stateTime;
        }

        virtual inline int getPreviousState() const {
            return previousState;
        }

        virtual inline const std::string & getMoveType() const {
            return moveType;
        }

        virtual inline void setMoveType(const std::string & str){
            moveType = str;
        }

        virtual void resetStateTime();

        virtual void setVariable(int index, Compiler::Value * value);
        virtual void setFloatVariable(int index, Compiler::Value * value);
        virtual void setSystemVariable(int index, Compiler::Value * value);
        virtual Compiler::Value * getVariable(int index) const;
        virtual Compiler::Value * getFloatVariable(int index) const;
        virtual Compiler::Value * getSystemVariable(int index) const;

        virtual inline Physics::Type getCurrentPhysics() const {
            return currentPhysics;
        }

        virtual void setCurrentPhysics(Physics::Type p){
            currentPhysics = p;
        }

        virtual void setGravity(double n){
            gravity = n;
        }

        virtual double getGravity() const {
            return gravity;
        }

        virtual void setLieDownTime(int x){
            lieDownTime = x;
        }

        virtual int getLieDownTime() const {
            return lieDownTime;
        }

        virtual void setStandingFriction(double n){
            standFriction = n;
        }

        virtual double getStandingFriction() const {
            return standFriction;
        }

        virtual bool hasAnimation(int index) const;
	
	virtual inline void toggleDebug(){
	    debug = !debug;
	}

        virtual inline void setHitDef(const HitDefinition * hit){
            this->hit = hit;
        }

        virtual const HitDefinition * getHit() const {
            return this->hit;
        }

        virtual inline int getHeight() const {
            return height;
        }

        virtual inline void setHeight(int h){
            height = h;
        }

        /* `this' hit `enemy' */
        void didHit(Character * enemy, MugenStage & stage);

        /* `enemy' hit `this' with hitdef `hit' */
        void wasHit(MugenStage & stage, Character * enemy, const HitDefinition & hit);

        /* `this' character guarded `enemy' */
        void guarded(Character * enemy, const HitDefinition & hit);

        /* true if the player is holding back */
        bool isBlocking(const HitDefinition & hit);
        /* true if the player was attacked and blocked it */
        bool isGuarding() const;

        virtual const HitState & getHitState() const {
            return hitState;
        }

        const std::vector<MugenArea> getAttackBoxes() const;
        const std::vector<MugenArea> getDefenseBoxes() const;

        /* paused from an attack */
        virtual bool isPaused();
        /* time left in the hitpause */
        virtual int pauseTime() const;

        /* prevent character from being hit, like after a KO */
        virtual void setUnhurtable();

        /* character can be hit */
        virtual void setHurtable();

        bool canTurn() const;
        void doTurn(MugenStage & stage);

        /* recover after falling */
        virtual bool canRecover() const;

        virtual MugenSound * getSound(int group, int item) const;
        virtual MugenSound * getCommonSound(int group, int item) const;

        virtual inline void setJugglePoints(int x){
            airjuggle = x;
        }

        virtual inline int getJugglePoints() const {
            return airjuggle;
        }

        virtual inline void setCurrentJuggle(int j){
            currentJuggle = j;
        }

        virtual inline int getCurrentJuggle() const {
            return currentJuggle;
        }

        virtual inline void setCommonSounds(const std::map< unsigned int, std::map< unsigned int, MugenSound * > > * sounds){
            this->commonSounds = sounds;
        }

        virtual inline void setExtraJumps(int a){
            airjumpnum = a;
        }

        virtual inline int getExtraJumps() const {
            return airjumpnum;
        }

        virtual inline double getAirJumpHeight() const {
            return airjumpheight;
        }

        virtual inline void setAirJumpHeight(double f){
            airjumpheight = f;
        }

        virtual int getCurrentCombo() const;

        virtual inline int getHitCount() const {
            return hitCount;
        }

        virtual inline const std::vector<WinGame> & getWins() const {
            return wins;
        }

        virtual inline void clearWins(){
            wins.clear();
        }

        virtual void addWin(WinGame win);

        virtual inline int getMatchWins() const {
            return matchWins;
        }

        virtual void addMatchWin();

        virtual void resetPlayer();

        virtual inline void setBehavior(Behavior * b){
            behavior = b;
        }

        virtual inline Behavior * getBehavior(){
            return this->behavior;
        }

        virtual inline void setDefaultSpark(int s){
            sparkno = s;
        }

        virtual inline void setDefaultGuardSpark(int s){
            guardsparkno = s;
        }

        virtual inline int getDefaultSpark() const {
            return sparkno;
        }

        virtual inline int getDefaultGuardSpark() const {
            return guardsparkno;
        }

        virtual inline void setRegeneration(bool r){
            this->regenerateHealth = r;
        }
        
        virtual inline int getAttackDistance() const {
	    return this->attackdist;
	}

        virtual void setAfterImage(int time, int length, int timegap, int framegap);
        virtual void setAfterImageTime(int time);

        virtual void updateAngleEffect(double angle);
        virtual double getAngleEffect() const;
        virtual void drawAngleEffect(double angle, bool setAngle, double scaleX, double scaleY);

protected:
    void initialize();

    virtual inline void setCurrentState(int state){
        this->currentState = state;
    }
    
    virtual void loadSelectData();

    virtual void loadCmdFile(const Filesystem::RelativePath & path);
    virtual void loadCnsFile(const Filesystem::RelativePath & path);
    virtual void loadStateFile(const Filesystem::AbsolutePath & base, const std::string & path, bool allowDefinitions, bool allowStates);

    virtual void addCommand(Command * command);

    virtual void setConstant(std::string name, const std::vector<double> & values);
    virtual void setConstant(std::string name, double value);

    virtual std::vector<std::string> doInput(const MugenStage & stage);
    virtual bool doStates(MugenStage & stage, const std::vector<std::string> & active, int state);

    typedef void (internalCommand_t)(MugenStage & stage, const std::vector<std::string> & inputs);

    /*
    void resetJump(const MugenStage & stage, const std::vector<std::string> & inputs);
    void doubleJump(const MugenStage & stage, const std::vector<std::string> & inputs);
    void stopGuarding(const MugenStage & stage, const std::vector<std::string> & inputs);
    */
    internalCommand_t resetJump;
    internalCommand_t doubleJump;
    internalCommand_t stopGuarding;

    virtual void fixAssumptions();
    virtual void parseState(Ast::Section * section);
    virtual void parseStateDefinition(Ast::Section * section);

    // InputMap<Mugen::Keys> & getInput();

protected:

	/* Location is the directory passed in ctor
	This is where the def is loaded and all the relevant files
	are loaded from
	*/
    Filesystem::AbsolutePath location;
	
        Filesystem::AbsolutePath baseDir;
	
	/* These items are taken from character.def file */
	
	/* Base definitions */
	
	// Name of Character
	std::string name;
	// Name of Character to Display why there is two is beyond me, retards
	std::string displayName;
	// Version date (unused)
	std::string versionDate;
	// Version that works with mugen (this isn't mugen)
	std::string mugenVersion;
	// Author 
	std::string author;
	// Palette defaults
	std::vector< unsigned int> palDefaults;
	unsigned int currentPalette;
	
	/* Relevant files */
	
	// Command set file
        Filesystem::RelativePath cmdFile;
	// Constants
	std::string constantsFile;
        /*
	// States
	std::string stateFile;
	// Common States
	std::string commonStateFile;
	// Other state files? I can't find documentation on this, in the meantime we'll wing it
	std::string stFile[12];
        */
	// Sprites
	std::string sffFile;
	// animations
	std::string airFile;
	// Sounds
	std::string sndFile;
	// Palettes max 12
        std::map<int, std::string> palFile;
	
	// Arcade mode ( I don't think we will be using this anytime soon )
	std::string introFile;
	std::string endingFile;
	
	/* Now on to the nitty gritty */
	
	/* Player Data and constants comes from cns file */
	
	/* Section [Data] */
	
	// Life
	int life;
	// Attack
	int attack;
	// Defence
	int defence;
	// How much to bring up defese on fall
	int falldefenseup;
	// How long to lie down when fall
	int lieDownTime;
        /* starting air juggle points */
	int airjuggle;

        /* number of juggle points left */
        int juggleRemaining;

        /* number of juggle points the current move will take */
        int currentJuggle;

	// Default Hit Spark Number for hitdefs ???
	int sparkno;
	// Default guard spark number
	int guardsparkno;
	// Echo on KO (I guess is for the death sound)
	bool koecho;
	// Volume offset on characters sounds
	int volumeoffset;
	// Maybe used in VS mode later
	/* According to the definition: 
	Variables with this index and above will not have their values
	reset to 0 between rounds or matches. There are 60 int variables,
	indexed from 0 to 59, and 40 float variables, indexed from 0 to 39.
	If omitted, then it defaults to 60 and 40 for integer and float
	variables repectively, meaning that none are persistent, i.e. all
	are reset. If you want your variables to persist between matches,
	you need to override state 5900 from common1.cns.
	*/
	int intpersistindex;
	int floatpersistindex;
	
	/* Section [Size] */
	
	// Horizontal Scaling Factor
	int xscale;
	//Vertical scaling factor.
	int yscale;
	//      ;Player width (back, ground)
	int groundback;
	//   ;Player width (front, ground)
	int groundfront;
	//      ;Player width (back, air)
	int airback;
	//     ;Player width (front, air)
	int airfront;
	//  = 60          ;Height of player (for opponent to jump over)
	int height;
	// = 160    ;Default attack distance
	int attackdist;
	//  = 90 ;Default attack distance for projectiles
	int projattackdist;
	//  = 0     ;Set to 1 to scale projectiles too
	bool projdoscale;
	// = -5, -90   ;Approximate position of head
	Mugen::Point headPosition;
	//  = -5, -60    ;Approximate position of midsection
	Mugen::Point midPosition;
	//  = 0     ;Number of pixels to vertically offset the shadow
	int shadowoffset;
	// = 0,0    ;Player drawing offset in pixels (x, y)
	Mugen::Point drawOffset;
	
	/* Section [Velocity] */
	
	//   = 2.4      ;Walk forward
	double walkfwd;
	// = -2.2     ;Walk backward
	double walkback;
	//  = 4.6, 0    ;Run forward (x, y)
	double runforwardx;
	double runforwardy;
	// = -4.5,-3.8 ;Hop backward (x, y)
	double runbackx;
	double runbacky;
	// = 0,-8.4    ;Neutral jumping velocity (x, y)
	double jumpneux;
	double jumpneuy;
	// = -2.55    ;Jump back Speed (x, y)
	double jumpback;
	// = 2.5       ;Jump forward Speed (x, y)
	double jumpfwd;
	// = -2.55,-8.1 ;Running jump speeds (opt)
	double runjumpback;
	// = 4,-8.1      ;.
	double runjumpfwd;
	// = 0,-8.1      ;.
	double airjumpneux;
	double airjumpneuy;
	// Air jump speeds (opt)
	double airjumpback;
	double airjumpfwd;

        double power;
	
	/* Movement */
	
	//  = 1      ;Number of air jumps allowed (opt)
	int airjumpnum;
	//  = 35  ;Minimum distance from ground before you can air jump (opt)
	double airjumpheight;
	// = .44         ;Vertical acceleration
	double yaccel;
	//  = .85  ;Friction coefficient when standing
	double standfriction;
	//  = .82 ;Friction coefficient when crouching
	double crouchfriction;

	/* Sprites */
	std::map< unsigned int, std::map< unsigned int, MugenSprite * > > sprites;
	// Bitmaps of those sprites
	std::map< unsigned int, std::map< unsigned int, Bitmap * > > bitmaps;
	
	/* Animation Lists stored by action number, ie [Begin Action 500] */
	std::map< int, MugenAnimation * > animations;
	
	/* Sounds */
	std::map< unsigned int, std::map< unsigned int, MugenSound * > > sounds;
        /* sounds from the stage */
        const std::map< unsigned int, std::map< unsigned int, MugenSound * > > * commonSounds;
	
	/* Commands, Triggers or whatever else we come up with */
        std::map<std::string, Constant> constants;

        std::vector<Command *> commands;

        std::map<int, State*> states;

        int currentState;
        int previousState;
        int currentAnimation;
	
	// Debug state
	bool debug;

        /*
        InputMap<Mugen::Keys> inputLeft;
        InputMap<Mugen::Keys> inputRight;
        */

        double velocity_x, velocity_y;

        bool has_control;

        /* how much time the player has been in the current state */
        int stateTime;
    
        /* dont delete these in the destructor, the state controller will do that */
        std::map<int, Compiler::Value*> variables;
        std::map<int, Compiler::Value*> floatVariables;
        std::map<int, Compiler::Value*> systemVariables;
        Physics::Type currentPhysics;

        /* yaccel */
        double gravity;

        /* stand.friction */
        double standFriction;

        /* S (stand), C (crouch), A (air), L (lying down) */
        std::string stateType;
        std::string moveType;

        const HitDefinition * hit;

        HitState hitState;
        unsigned int lastTicket;

        int combo;
        // int nextCombo;
        
        int hitCount;

        std::vector<WinGame> wins;

        int matchWins;

        Compiler::Value * internalJumpNumber;

        Behavior * behavior;

        /* true if the player is holding the back button */
        bool blocking;

        //! regenerate health?
        bool regenerateHealth;
        bool regenerating;
        int regenerateTime;
        int regenerateHealthDifference;

        /* used to communicate the need to guard in the engine */
        bool needToGuard;

        /* true if the player is currently guarding an attack */
        bool guarding;

        struct AfterImage{
            AfterImage():
                show(false){
                }

            struct Frame{
                Frame():
                    sprite(NULL),
                    life(0){
                    }

                Frame(MugenFrame * sprite, Effects effects, int life, int x, int y):
                    sprite(sprite),
                    effects(effects),
                    life(life),
                    x(x),
                    y(y){
                    }

                MugenFrame * sprite;
                Effects effects;
                int life;
                int x;
                int y;
            };

            /* true if after images are being shown */
            bool show;
            int currentTime;
            int timegap;
            int framegap;
            int lifetime;
            unsigned int length;

            std::deque<Frame> frames;
        } afterImage;
};

}

#endif
