# Physical corner push

M4ax uses 51 original `cornerpush-*` programs: 1,224 native numeric observations
and 204 authored CNS/AIR/CMD/stage hashes. Each packet samples twelve attacker
positions at world-frame offsets 0/1/2/3/4/5/6/8/10/15/25/40, plus both players'
velocities, receiver positions/life and final attacker heading. Sampling starts
in attack state 700; negative-state probes explicitly run through hitpause.

## Observed movement and selection

Corner push is a separate world impulse. Vel X remains unchanged while position
moves. Default -5.2 moves the attacker from 130 to 124.800003, 121.160004 and
118.612 over the first three completed steps. Explicit zero disables it;
positive and negative values retain their signs. Ground, air, down, guard and
air-guard contacts select their corresponding already-compiled corner fields.
Explicit child fields override conflicting parent defaults. Air-attribute zero
defaults and an explicit nonzero air-attribute control are separate cases.

The impulse survives ChangeState, TargetDrop and Turn. It adds to ordinary
movement and preserves its world direction across later turns. HitDef P1Facing
selects receiver knockback before the physical attacker turn; the corner impulse
uses the attacker's physical heading at contact. Both left/right deferred-turn
controls pin that distinction. Target facing and knockback direction/speed do
not select the sign or suppress a valid impulse, including zero knockback.

## Common-space proximity and precision

The receiver must be less than five common engine pixels from an effective
left or right bound at contact. This is proximity, rather than prediction of
future knockback. At game width 1280, character width 320, distances 1.249 and
1.251 bracket the cutoff; exactly 1.25 does not qualify. A target two character
pixels from the bound qualifies at game width 640 and does not at width 1280.
Fast knockback from farther away does not acquire a force, nor does an interior
contact when its receiver subsequently reaches the edge. Disabling receiver
ScreenBound prevents the impulse and lets the receiver move beyond that edge.

The selected float32 hit field converts to common units before the independent
double-precision impulse is stored. Each active movement step applies it and
multiplies it by the float32 representation of 0.7, promoted to double. After
that decay, a magnitude below one common pixel stops it. A newly requested small
impulse still gets its first step before this cutoff. Character widths 320,
640 and 1280 and a separate game-width640 control distinguish common units from
character units. The irregular width400, -50.5 control distinguishes initial
float32 common conversion from a double-only or locally stored approximation.

Positions accumulate in double precision. The large -50.5 trace distinguishes
both per-step float32 rounding and a decimal-double 0.7 coefficient from the
native arithmetic. Only the matching model reproduces the small late positions
and final -37.534153; the width400 counterpart ends at -1.28416038.

## Clocks and integration

Attacker hitpause freezes both movement and impulse decay; a pause of three
leaves the first four pre-movement observations at 130. PosFreeze suppresses
ordinary velocity movement while this separate impulse continues to move and
decay. An own-velocity-plus-freeze control distinguishes the two contributions.
Bounds still constrain the resulting position. Round reset creates fresh actors
and clears their impulses. No target-ledger or state-entry reset is added.

Character owns the common-space impulse; contact selects it before incoming
state entry. The normal tick applies it once after CNS and before ordinary
movement, alongside the separately consumed down-hit offset. Screen-bound math
is shared with the existing confinement operation. Projectile-owner corner push
remains a separate missing path in the legacy projectile adapter; it is not
implemented by pushing the projectile itself.

## Evidence and remaining work

The original first captures sampled pre-round spawn positions because the clock
seed lacked StateNo=700. That batch was interrupted and cleaned up; its copies
remain diagnostic under temp. All final base expectations were rerecorded as
native-controlled. The preserved 22-program pre-fix comparator fails 17 cases in
`temp/m4ax-before-push.log`.

Native logs are `m4ax-controlled-native.log`, `m4ax-math-native.log` and
`m4ax-integration-native.log`. All 51 corrected integrated cases pass in
`temp/m4ax-common-push.log`. Full scoped races pass in
`temp/m4ax-full-race.log` (Match439.097 seconds). Six fresh native replays pass
in `temp/m4ax-replays.log`, including precision, exact proximity, game640,
deferred facing, own movement/freeze and the irregular400 profile.

Ten actual 1280x720 captures from one identical binary pass source/binary/config/
PNG/dimension/runtime checks; stock recovery retains two fighters without
persistent dust. The five-mask comparison remains a failing render gate:
stationary proximity-equal matches exactly (11,952 white pixels), while default,
large, hitflip-left and PosFreeze differ by 567/471/933/567 pixels. Their native/
Go white counts are 13,284/13,485, 16,443/16,566, 9,615/9,492 and 13,284/13,485.
The isolated large-impulse fighter has equal horizontal mask bounds but different
sampling; an integer translation does not resolve it. Do not loosen the checker
or change correct simulation coordinates to fit these images. Investigate
fractional sprite rendering next with stationary/moving original marker controls.
Results and the explicit failing assertion are in `temp/m4ax-app-checks.json`
and `temp/m4ax-app-checks.log`. The irregular400 window is inspected but has no
whole-raster claim. This checkpoint closes the measured physics observations,
not that outstanding renderer gate.

Read-only disassembly corroborates the runtime probes for the pinned beta engine
SHA-256 d5508f77f2a86f12358e669f91c64ba5e4a789d4b71262e8a758814384303a4e:
0x4342f0 compares absolute distances with the double constant 5 at 0x4fd908;
0x43ec5d initializes the world impulse using the physical facing;
0x43cc55 multiplies its double velocity by the float field initialized from
0x4fda1c (0.699999988079071); 0x43cc8a compares the decayed magnitude with one.
Disassembly/section tables stay disposable under temp. No executable is patched
or committed, and executable observations remain the regression expectations.

Full projectile ownership, helper/mixed-opponent/foreign/binding/multiple-hit/
incoming-interruption combinations, moving-camera/zoom and all width/stage
combinations remain open. Full HitDef, juggle/recovery/power/KO, assets, renderer
and game modes are still tracked in the compatibility inventory. Reversal:
7af38317 (M4aw). This checkpoint does not declare the port complete.


## Renderer follow-up

M4ay closes the four fractional-position clothing-mask failures recorded above.
Its final six masks, including irregular400, match native exactly; physical
corner-push code is unchanged. See [SPRITE-SAMPLING.md](SPRITE-SAMPLING.md) for
the new original fine-pattern controls, exact RGB/indexed samplers and verified
scope. The separate pre-existing RGB HUD/effect ramp gap remains open for M4az.
