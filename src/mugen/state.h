#ifndef _paintown_12f1db6ef8192d13ebc286e90a11ca44
#define _paintown_12f1db6ef8192d13ebc286e90a11ca44

namespace Mugen{

/* Our animation definitions */
enum AnimationType{
    Standing = 0,          /* Standing */
    StandTurning = 5, //        Stand turning                Must have finite looptime
    CrouchTurning = 6,  //       Crouch turning               Must have finite looptime
    StandToCrouch = 10, //       Stand to crouch              Finite looptime
    Crouching = 11,//        Crouching
    CrouchToStand = 12, //       Crouch to stand              Finite looptime
    WalkingForwards = 20, //       Walking forwards
    WalkingBackwards = 21, //       Walking backwards
    JumpStart = 40,//        Jump start (on ground)       Shown just before player leaves ground
    JumpNeutralUp = 41, //Jump neutral (upwards)       Shown when player is going up
    JumpForwardsUp = 42, //Jump forwards (upwards)      Shown when player is going up-towards
    JumpBackUp = 43, //Jump back (upwards)          Shown when player is going up-back
    JumpNeutralDown = 44, //  opt   Jump neutral (downwards)     Activated when Y-velocity > -2
    AirJumpStart = 45, //  opt   Jump fwd (downwards)         Same as above
    JumpBackDown = 46, //  opt   Jump back (downwards)        Same as above
    JumpLanding = 47, //Jump landing                 Shown as player lands on ground
    RunForwards = 100, //       Run fwd/hop forward
    HopBack = 105, //       Hop back	
    StartGuardStand = 120, //       Start guarding (stand), //       Finite looptime
    StartGuardCrouch = 121, //       Start guarding (crouch)      Finite looptime
    StartGuardAir = 122, //       Start guarding (air), //         Finite looptime
    GuardStand = 130, //       Guard (stand)
    GuardCrouch = 131, //       Guard (crouch)
    GuardAir = 132, //       Guard (air)
    StopGuardStand = 140, //       Stop guarding (stand)        Finite looptime
    StopGuardCrouch = 141, //       Stop guarding (crouch)       Finite looptime
    StopGuardAir = 142, //       Stop guarding (air)          Finite looptime
    GuardHitStand = 150, //       Guarding a hit (stand)       Finite looptime
    GuardHitCrouch = 151, //       Guarding a hit (crouch)      Finite looptime
    GuardHitAir = 152, //       Guarding a hit (air)         No loop
    Lose = 170, // opt   Lose                         (See Note 1)
    Draw = 175, // opt   Time Over drawgame           (See Note 1)
    Win = 180, // opt   Win                          No loop (181-189 for multiple) (See Note 1)
    Intro = 190, // opt   Intro                        No loop (See Note 1)
    Taunt = 195, // opt   Taunt                        Finite looptime (See Note 1)
    HitHighLight = 5000, //       Stand/Air Hit high (light)   Looptime around 10-20
    HitHighMedium = 5001, //      Stand/Air Hit high (medium)    "     (See Note 2)
    HitHighHard = 5002, //       Stand/Air Hit high (hard), //       "
    StandRecoverHighLight = 5005, //       Stand Recover high (light)   No loop (See Note 3)
    StandRecoverHighMedium = 5006, //       Stand Recover high (medium)    "
    StandRecoverHighHard = 5007, //       Stand Recover high (hard), //       "
    HitLowLight = 5010, //       Stand/Air Hit low (light)    Looptime around 10-20
    HitLowMedium = 5011, //       Stand/Air Hit low (medium)     "
    HitLowHard = 5012, //       Stand/Air Hit low (hard), //        "
    StandRecoverLowLight = 5015, //       Stand Recover low (light)    No loop
    StandRecoverLowMedium = 5016, //       Stand Recover low (medium)     "
    StandRecoverLowHard = 5017, //       Stand Recover low (hard), //        "
    CrouchHitLight = 5020, //       Crouch Hit (light), //            Looptime around 10-20
    CrouchHitMedium = 5021, //       Crouch Hit (medium), //       , //       "
    CrouchHitHard = 5022, //       Crouch Hit (hard), //       , //         "
    CrouchRecoverLight = 5025, //       Crouch Recover (light), //        No loop
    CrouchRecoverMedium = 5026, //       Crouch Recover (medium), //         "
    CrouchRecoverHard = 5027, //       Crouch Recover (hard), //           "
    HitBack = 5030, //       Stand/Air Hit back, //            Looptime around 10-20
    HitTransition = 5035, // opt  Stand/Air Hit transition     Looptime around 5-15 (See Note 3)
    AirRecover = 5040, //       Air Recover, //       , //       , //       No loop
    AirFall = 5050, //       Air Fall, //       , //       , //          No loop
    AirFallComingDown = 5060, // opt  Air Fall (coming down), //        No loop
    Tripped = 5070, //       Tripped
    LiedownHitStaydown = 5080, //       LieDown Hit (stay down), //       (See Note 4)
    LiedownHitIntoair = 5090, //       LieDown Hit (hit up into air)
    HitGroundFromFall = 5100, //       Hitting ground from fall     Looptime around 3
    BounceIntoAir = 5160, //       Bounce into air
    HitGroundFromBounce = 5170, //       Hit ground from bounce, //        Looptime around 3 or 4
    Liedown = 5110, //       LieDown
    GetUpFromLiedown = 5120, //       Get up from LieDown
    LiedeadFirstRounds = 5140, // opt  LieDead (first rounds)
    LiedeadFinalRound = 5150, // opt  LieDead (final round)
    FallRecoveryNearGround = 5200, //       Fall-recovery near ground
    FallRecoveryMidair = 5210, //       Fall-recovery in mid-air
    Dizzy = 5300, //       Dizzy
    Continue = 5500, // opt  "Continue?" screen           If omitted, plays dizzy anim
    YesContinue = 5510, // opt  "Yes" to "Continue"          If omitted, plays first win anim (not yet implemented)
    NoContinue = 5520, // opt  "No" to "Continue"           (not yet implemented)
    AirFallHitUp = 5051, // opt  Air fall -- hit up
    FallFromHitUp = 5061, // opt  Coming down from hit up
    LiedownHitStaydown2 = 5081, // opt  LieDown Hit (stay down) A
    BounceFromGroundIntoAir = 5101, // opt  Bounce from ground into air  Looptime around 3
    BounceIntoAir2 = 5161, // opt  Bounce into air
    HitGroundFromBounce2 = 5171, // opt  Hit ground from bounce       Looptime around 3 or 4
    Liedown2 = 5111, // opt  LieDown
    GetUpFromLiedown2 = 5121, // opt  Get up from LieDown
    LiedeadFirstRounds2 = 5151, // opt  LieDead (first rounds)
    LiedeadFinalRound2 = 5156, // opt  LieDead (final round)
    AirFallHitUp2 = 5052, // opt  Air fall -- hit up
    FallFromHitUp2 = 5062, // opt  Coming down from hit up
    LiedownHitStaydown3 = 5082, // opt  LieDown Hit (stay down)
    BounceFromGroundIntoAir2 = 5102, // opt  Bounce from ground into air  Looptime around 3
    BounceIntoAir3 = 5162, // opt  Bounce into air
    HitGroundFromBounce3 = 5172, // opt  Hit ground from bounce       Looptime around 3 or 4
    Liedown3 = 5112, // opt  LieDown
    GetUpFromLiedown3 = 5122, // opt  Get up from LieDown
    LiedeadFirstRounds3 = 5152, // opt  LieDead (first rounds)
    LiedeadFinalRound3 = 5157, // opt  LieDead (final round)
    Initialize = 5900,
    Unknown = 99999999
};

}

#endif
