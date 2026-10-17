# Stock throw composition and contact frames

M4ad exercises the supplied KFM throw states and AIR without copying them into
source fixtures. Original wrappers place two characters and observe the stock
800 -> 810/820 -> 821 -> 5100 -> 5110/5120 -> 0 path. These wrappers
declare mugenversion 1.1; complete legacy-version behavior remains separate. Files overrides load
kfm.cns, common1.cns and kfm.air from a disposable distribution. The native
recipe and Go replay pin their actual paths/hashes in file_sha256, alongside
the authored wrapper/opponent hashes. Original binaries, art and state files
remain unchanged and untracked.

## Stock sequence

The ordinary throw acquires the victim, preserves its standing animation until
820's ChangeAnim2, and renews its TargetBind offsets through each stock AIR
phase. Entry to 810 sees target state 820/previous 5000 and the old animation;
one tick later the target uses action 820 and reports isbound=1. Initial body
push separates axes to -16/+16; the first bind places the victim at X=12.

At throw Time=40 the victim still has Life=1000. The stock element-11 controllers
apply TargetLifeAdd(-78) and TargetState(821); the following observation sees
Life=922, X=-66,Y=-50, velocity approximately (2.8,-6.6). Releasing the one-tick
bind then produces X=-68.8,Y=-56.2 with Y velocity -6.2. The victim reaches its
own landing/get-up states, returns to 0 with previous state 5120, and disappears
from the attacker's target list.

The forward-branch wrapper sets the stock throw's var(2) after its normal
Time=0 input sample. This deliberately exercises the stock Turn/PosAdd/
TargetFacing branch without claiming command-input compatibility. Its final
bound X=71 and first released X=73.8 distinguish the changed facing and offsets.
Damage, landing and recovery still follow the stock body. Other body-edge,
Width, camera and simultaneous-attack/priority effects remain separate gates.

A TargetDrop at throw Time=4 exercises 820's actual !GetHitVar(isbound) fallback.
The victim returns through SelfState(5050), lands and gets up without losing
life. It remains at X=42. This checks a released target; an attacker receiving a
new hit must still be compared independently because native also forces its
bound targets into fall states.

## First grab and collision pose

The first throw-relative trace could match even if the grab happened late.
An independent GameTime stamp measures 800 entry to 810 entry. Native takes one
tick; the first Go run took 22. It missed the one-frame Clsn1 in action 800 and
only contacted when that action wrapped, just before its exit controller.
Throw-relative positions alone did not expose that delay.

The root contact phase now uses the preceding completed simulation pose, saved
after CNS/physics and before the animation clock advances. It does not use the
next element selected for the following CNS/display pass. The direct
CheckCollision API still evaluates current elements. Frozen-clock updates also
publish pose changes made by ignorehitpause controllers. Complete pause/entity/
projectile ordering remains unverified by these root tests.

Four independent minimal programs distinguish first/second-element attack
windows and first/second-element hurtbox windows. Their one-tick elements keep
this correction separate from stock throw code and from constant hitbox tests.
Existing native contact, guard, pause, spark, sound and ownership packets must
continue passing before accepting the shared phase change. Shared late body
push still has separate frame/vertical/dynamic-width obligations.

## Omitted fall X velocity

GetHitVar(fall.xvel) exposes a numeric sentinel when fall.xvelocity was omitted;
that trigger result remains unchanged. HitFallVel must preserve the current X
velocity for that omission. The old controller assigned the sentinel as a real
velocity, driving the released victim to the screen edge. Native's TargetDrop
escape stays at X=42 with velocity zero. An explicit fall X value continues to
be applied. Other HitFallVel flags, repeat/paused calls and full fall schemas
remain separate controller obligations.

## Validation scope

The four stock programs pin 132 observations; the four minimal window programs
pin another 96. The stock replay runs 240 world ticks with original state/AIR
hashes checked. The pure simulation does not load sound banks, so stock PlaySnd
controllers report unavailable resources there; the production smoke separately
loads the actual character/common assets. Full scoped races pass in temp/m4ad-full-first-race.log, including all prior
contact/guard/pause/sound/spark/ownership regressions. Four fresh native replays match start/drop and independent first/second windows,
including original Files hashes. Production window checks are recorded with the
checkpoint. Full throws, HitDef hit flags/facing/priority,
dynamic Width, interrupted bindings, all modes and complete compatibility are
not declared verified.


The normal, forward and TargetDrop variants also run through real 1280x720 Go
windows at temp/app-runs/stock-throw-{base,forward,drop}-go/. Captures at 2/3/4/7
seconds show the stock lift in both directions, released fall and final recovery.
Original sprite and sound banks load; no runtime errors appear. These are owned
window/asset smoke checks, not framebuffer or complete audio parity.


The final normal/forward captures expose a remaining visual bug: common landing
Explods using anim=F(expression) fall through the legacy literal parser to own
action 0 and infinite lifetime. They leave extra standing KFM sprites after
recovery. State/health/target observations pass, but a clean visual throw is not
yet accepted. Next connect typed Explod resource lookup and lifetime to the shared
effect scene, then repeat these captures. The dropped variant and the scripted
state tests do not excuse the normal/forward rendering defect.
