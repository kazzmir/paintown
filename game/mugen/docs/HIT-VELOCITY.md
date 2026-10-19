# Hit velocity, inherited expressions and down-hit offsets

M4aw uses 63 original `hitvelocity-*` programs: 1,020 native numeric observations,
252 authored CNS/AIR/CMD/stage hashes and no rejected final programs. Neutral
states isolate GetHitVar, initial HitVelSet velocity, late velocity, movement,
posture and damage. Tagged assignments distinguish stored values from later
re-evaluation; mixed profiles and independently authored constants isolate units.

## Vectors and inheritance

Ground and air vectors default to zero; omitted air Y never inherits ground Y.
Guard X defaults to ground X and has no Y. Down components default independently
to air components. Air-guard components independently default to air X times 1.5
and air Y times 0.5. A supplied single X preserves the inherited down/air-guard
Y; explicit Y zero replaces it. Computed fractions retain float32 values, bottom
supplies zero and redirects preserve caller/receiver units. Single bottom X also
preserves the omitted Y. Reversing written field order changes none of these.

The receiver snapshots down/air/guard context at contact. Down velocities remain
available after a destination changes the receiver's state type, like the earlier
air-context snapshot. X getters are world-oriented; HitVelSet still converts to
facing-relative velocity. Separate initial variable snapshots and redirected late
velocity checks distinguish raw variable storage from coordinate conversion.

## Default expressions run again

Inherited defaults retain expressions, rather than caching their first results.
With all nine velocity components incrementing one counter, native evaluation is:

| Order | Parameter | Counter values |
| --- | --- | --- |
| 1 | ground.velocity | 1, 2 |
| 2 | ground.cornerpush.veloff default | 3 |
| 3 | air.velocity | 4, 5 |
| 4 | air.cornerpush.veloff default | 6 |
| 5 | down.velocity | 7, 8 |
| 6 | down.cornerpush.veloff default | 9 |
| 7 | guard.velocity | 10 |
| 8 | guard.cornerpush.veloff default | 11 |
| 9 | airguard.velocity | 12, 13 |
| 10 | airguard.cornerpush.veloff default | 14 |

The default corner-push expression uses 1.3 times guard velocity for the measured
S attribute; the A attribute defaults it to zero. Air/down/guard corner defaults
inherit the ground expression, while air-guard corner inherits the guard-corner
expression. Five tagged receiver contexts have identical activation/late tags.
Explicit zero corner fields suppress the extra evaluations; explicit ground or
guard corner expressions replace the relevant inherited chain and run again.

Omitted guard velocity re-evaluates ground X; omitted down/air-guard components
re-evaluate their air expressions. Single components do the same for omitted Y.
Distinct tags preserve both the original stored vector and the final assignment
side effects. Native source-order reversal, explicit-zero and A-attribute
controls rule out an arbitrary repeated-evaluation count.

The typed compiler models these ten fields in one expression-default builder.
Five corner values are compiled, stored and converted with the hit snapshot.
M4ax subsequently implements the bounded [physical corner-push behavior](CORNER-PUSH.md).
This checkpoint's default expressions establish velocity/VM behavior; they do
not promote the whole HitDef controller to verified.

## Down-hit Y offset

A downed hit with nonzero down Y requests a fixed offset of 15 in 240p units,
converted to receiver-local units: 15 at width 320, 60 at width 1280. It is
independent of trip ground-level and ground-back constants. Original trip=7 and
back=8 controls retain the same offset; the normal constant remains observable
separately. An explicit zero down Y requests no offset.

GetHitVar(yoff) exposes the pending offset as a float. Division controls yield
5/16=0.3125 and 5/61=0.0819672123. Authored Time=0 PosSet and the rest of CNS run
before the offset applies; movement follows. At Time=1, the 320-wide -.5 velocity
case reports Y=14.5, while the 1280-wide -2 case reports 58. A standing destination
retains the request. The offset is consumed once: both Time=1 and late redirected
yoff are zero. The shared coordinate adapter scales this float field.

## Evidence and limits

The preserved initial 36-program comparator fails 22 cases in
`temp/m4aw-before-velocity.log`. Native data is in `m4aw-native-first.log`,
`m4aw-phase-native.log`, `m4aw-final-native.log`, `m4aw-constants-native.log` and
`m4aw-offset-native.log`. The initial minimal constant controls omitted the
required Velocity section and were rejected as invalid probes; repaired original
numeric prefixes were recorded separately. Those diagnostic copies are retained
under temp; neither rejection is a final compatibility expectation.

All 63 corrected integrated comparisons pass in `temp/m4aw-offset-velocity.log`.
Pure CNS/engine/evaluator races pass in `temp/m4aw-final-pure-race.log`.
The full scoped race suite passes in `temp/m4aw-full-race.log` (Match 395.933
seconds). Six fresh native replays pass in `temp/m4aw-replays.log`, covering
default tags, an inherited corner expression, a single-component default, true
mixed profiles, altered constants and the offset type/consumption. Ten actual
1280x720 captures from one identical binary pass both-player source/bank,
stage/binary/config/PNG/dimension/runtime checks. Five stable white masks match
exactly: 11,181, 14,991 and three at 8,379 pixels. The moving mixed-profile window
has provenance/runtime checks without an unstable mask claim. Stock throw
recovery retains two fighters without persistent dust. Results are
`temp/m4aw-app-checks.json` and `temp/m4aw-app-checks.log`. Background/HUD/debug
and whole-frame renderer parity remain open.

Wider corner-push, paused/frozen/bound/foreign/helper/projectile combinations,
full down/bounce/recovery/juggle/KO behavior, arbitrary parameter grammar and
cross-field order outside this group, remaining GetHitVar fields and complete
engine/asset/mode compatibility remain open. Reversal: 31795b0c (M4av).
