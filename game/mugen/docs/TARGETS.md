# Target lists, controllers and redirected units

M4y adds 49 original native programs: 744 recorded numeric values and three
required-parameter load rejections. Production Character reproduces every
packet/rejection. Typed TargetDrop, TargetFacing, TargetLifeAdd, TargetPowerAdd,
TargetVelSet and TargetVelAdd now operate on acquired targets. TargetState only
has its required-argument schema here; state ownership and binding remain next.
No full Target/helper/projectile/controller gate is declared complete.

## Acquisition, labels and release

A successful normal or guarded hit acquires the victim when it enters H. An
override remaining I acquires none. The target is visible to the attacker's
following CNS pass, before deferred hit damage changes Life. A target with
EntityID 57 and HitDef ID 1234 gives NumTarget=1, NumTarget(1234)=1 and Target,ID
57. EntityID and HitDef ID remain distinct.

Changing the attacker's state, including to I without hitdefpersist, retains
the victim. Reissuing HitDef with ID 5678 retains the old label until the new
contact; that contact replaces it. The target disappears after its H-to-I
recovery. Attack-first ordering means the attacker still sees it during the
recovery frame, then sees zero the next tick. These traces support the ordered
generation ledger added in M4x.

Omitted HitDef ID is zero, and negative HitDef IDs clamp to zero. This corrects
M4x's provisional -1 default. Negative Target/NumTarget lookup arguments select
all in the measured -1/-2 cases. TargetDrop uses excludeID, not the old port's
exclude spelling. Omitted excludeID drops all; a matching expression 1000+234
retains the single target, while 999 drops it immediately without a new hit.
The typed keepone option and world-local deterministic choice support multiple
records, but native multi-target selection/RNG and full multi-entity contact
acquisition are not verified by these single-victim programs.

## Life, power, facing and velocity

TargetLifeAdd changes Life immediately in the issuing CNS pass. Negative values
use the victim's runtime DefenceMul and temporary super defence divisor; the
caller's AttackMul is not applied in these programs. A later -100 changes 900
to 700 with DefenceMul=2, to 800 with absolute=1, and to 833 with the default
super divisor 1.5. Positive healing ignores that damage scaling: +50 with
DefenceMul=2 gives 950. Life clamps at maximum; kill=0 retains at least one.
Base CNS defence/stat scaling, exact half rounding, pending/frozen damage,
custom-state/guard/KO/revival edges remain separate obligations.

Power changes immediately and clamps to 0..PowerMax. TargetFacing uses caller-
relative signs; zero and omission preserve facing. A flip also negates local
Vel X to preserve world motion. Both velocity controllers use the target's
facing and convert the caller's width units into the victim's. Omitted components
are preserved; an empty controller is a no-op. Set (4.5,-2.25), then add
(1.25,-.75), yields (5.75,-3) in a same-width scene. Mixed 320/1280 scenes verify
physical conversion independently of redirected query conversion below.

Native accepts omitted TargetFacing value, but rejects omitted values for
TargetLifeAdd, TargetPowerAdd and TargetState at loading. Go now distinguishes
those schemas before effects. All other invalid/omitted/version parameter and
evaluation-order cases still need comparison.

## Defence and world-phase observations

A new non-H get-hit sequence clears a prior idle-state DefenceMul adjustment.
A multiplier established while H survives H-to-H state changes and affects a
later hit multiplicatively. Native first hits remain 100, while the second hit
with -2 renewing DefenceMul=2 is 200. This corrects the old port's division by
runtime DefenceMul. The separate temporary SuperPause defence remains a divisor.
AttackMul=2 gives 200 on the first hit.

An explicit contact trace records -3, -2, -1 and positive receiver passes. All
four see state 5000, pending damage 100, Life 1000 and Time 0 on the contact
frame. This confirms contact before all negative/positive passes and damage
after them for this root case. The neutral CMD source is hash-pinned separately.
It does not establish complete helper/projectile/global phase ordering.

## Redirection argument scope and result coordinates

Ordinary eager function arguments evaluate in the originating player, while the
outer trigger runs on the selected destination. Enemy,FVar(Var(0)) uses the
origin's index to read the enemy's variable. Enemy,Abs(Vel X) takes the origin's
Vel X argument. Redirected IfElse evaluates both branches and their assignments
in the origin. Scalar variable results and IDs are not geometrically rescaled.

Cond is a VM special form: its delayed condition/selected branch execute in
the destination context, including destination units. Thus a 320-wide origin
querying a 1280-wide enemy gets 15 from Enemy,Const(size.height), 60 from
Enemy,IfElse(1,Const(size.height),0), and 60 from Enemy,Cond(1,Const(size.height),0).
The eager IfElse's 60 is the origin's constant; Cond's 60 is the destination's
unscaled constant. Cond assignments affect the destination; eager IfElse
assignments affect the origin. Dedicated distinct-variable fixtures remove
the ambiguity of equal stock values.

Direct geometric triggers return caller units: target-local Pos X=280 and
Vel X=8 become 70 and 2 for a 320-wide origin. Direct Const240p/GameWidth and
viewport/body/constant values follow the same outer-trigger conversion;
dimensionless scale/zoom values stay unchanged. Geometric Const results retain
their declared integer/float kinds: converted ground.front=4 divided by integer
3 yields 1. Raw FVar/Var values 640 remain 640. Other redirect forms, fractional
profiles/rounding and complete constant/vector metadata remain open.

The first upward-position draft moved the native camera and is diagnostic only.
The promoted program keeps the camera at zero with a positive, frozen Y under
Physics N. Native eased camera following differs from the current Go camera;
camera easing remains open rather than being absorbed into trigger conversion.

## Validation and next work

Full scoped races pass. Pure engine/evaluator races include the native argument-
scope packet without DISPLAY. Representative fresh native replays cover label
replacement, super-scaled life, mixed velocity, lazy coordinate scope, negative
phases and a required-value rejection. Source/CMD/AIR/executable hashes are pinned.
The original distribution, root modules and desktop settings remain unchanged.

Next: actual TargetState/ChangeAnim2/SelfState state/animation ownership, TargetBind
and BindToTarget/Parent/Root clocks and geometry; then complete helper typed
parameters/negative eligibility, multi-target/neutral/team rules and projectile
world phases. The unresolved bottom-power native defect from BOTTOM.md remains
open. These tests do not promote any whole controller/trigger to Verified.
