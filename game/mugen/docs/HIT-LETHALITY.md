# Lethal contacts and fall damage

The target is the supplied Elecbyte 1.1 beta 1 binary, not the incomplete C++
engine or an interpretation of the descriptive controller manual. Original
`hitkill-*` programs record integer/float variable snapshots against that binary.
Source hashes and literal-float rejection records accompany the observations.

## Damage before state entry

Contact damage is bounded before the receiver's get-hit state executes. With
five life and ten raw damage, `GetHitVar(damage)` is five while entry `Life` is
still five. Setting `kill=0` bounds the effective amount to four. The same rule
uses `guard.kill` when the contact initially qualifies as guarded. Each flag
defaults to true; explicit zero and bottom become false, nonzero integers become
true. Literal floats are rejected; computed values truncate before conversion.

The cap does not change raw damage-derived power: the example still gives seven
attacker power and six receiver power. Applying the pending damage uses bounded
life arithmetic; a receiver that sets its own life to zero during entry stays
dead even with `kill=0`.

A fatal guarded contact retains guard damage and `guard.kill` selection, then
publishes an ordinary hit reaction. It uses hit power, `MoveHit`, an unguarded
get-hit context and the ordinary hit destination. It also forces a fall. A
control with hit damage one and chip ten kills five life; reversing the pair
leaves four life and the guarded reaction. `kill=0` alone does not protect
against fatal chip when `guard.kill` is omitted.

## Fall damage in beta 1

`GetHitVar(fall.kill)` returns a canonical integer boolean and defaults to one,
including before the first contact. `GetHitVar(fall)` returns zero outside an H
move type even if the stored fall flag is set.

Native `HitFallDamage` consumes the stored `fall.damage` once. It applies the
raw signed integer amount without attack or defence scaling. Calling it again
does not repeat the damage; negative damage heals within life bounds. For
example, a one-damage contact followed by fall damage six changes life from
100 to 99 to 93. An attack multiplier of two changes this to 100, 98, 92.

The measured beta controller also applies this amount when the receiver is not
falling, when its override enters an I state, and when `fall.kill=0`. These cases
are explicit controls, not inferred protection from the manual. This controller
does not itself force a fall when its damage kills the receiver.

## Expression evaluation

Typed scalar handling covers `kill`, `guard.kill`, `fall.kill` and `fall.damage`.
A complete tagged program checks their position among the existing HitDef
expression fields. Source declaration order does not change evaluation order:

```
id, kill, guard.kill, fall.kill, hitonce,
damage, getpower, givepower, numhits,
hitsound, guardsound, priority,
p1stateno, p2stateno, p2getp1state, p1sprpriority, p2sprpriority,
forcestand, forcenofall, fall.damage,
sparkno, guard.sparkno, sparkxy,
p1facing, p1getp2facing, p2facing,
then the measured velocity/corner-push fields
```

## Capture protocol and remaining work

The final corpus contains 90 programs, 1,080 numeric observations, four expected
literal-float rejections and 360 authored source hashes. Thirty-eight programs
isolate KO velocity, including a lying receiver excluded by the hit flags and
three explicit D-flag contacts.

In receiver-local 240p units, a fatal standing/crouching contact with zero Y
first multiplies X by float32 `.66`. All nonzero X then gain 2.5 with their
original sign; zero X remains zero. Airborne contacts skip the `.66` step.
Nonpositive Y loses two and is capped above at -6 for ground contacts or -3
for air contacts. Positive Y stays unchanged. All constants scale by receiver
local width/320, after converting the attack vector to receiver units. Downed
contacts retain their original down vector. Both getters and entry HitVelSet
observe the adjusted vector; KO does not independently change a scripted S
destination to A. Mixed 640-to-320 and 320-to-640 controls distinguish source
conversion from receiver-local constants.

The airborne/downed probes also exposed ordinary controller parameter
precedence: native keeps the first repeated parameter, whereas the port kept
the last. Explicit duplicate damage, kill and PowerSet value controls confirm
the rule. The CNS collector now preserves the first ordinary parameter while
retaining every repeated trigger condition. Duplicate controller metadata and
Statedef parameter precedence are outside these controls.

KO slowdown makes an absolute tick snapshot unreliable at a short wall-clock
deadline. The accepted programs latch a positive-state snapshot at relative
`Time>=8`; a dedicated sentinel proves that the snapshot actually ran. The
clipboard controller executes in the same positive state. Empty-state and
unreached-tick pilots remain diagnostic files below `temp/` and are excluded
from the corpus. Startup/clipboard failures are not accepted as native load
rejections.

Relative CNS observations do not prove absolute KO timing. KO motion controls
sample the stored and applied velocities at entry and the following tick,
including different character coordinate profiles. Position freezes after the
first three motion steps allow a separate deterministic actor pose comparison.
Whole-frame rendering, KO slowdown duration, round transition timing,
fatal-guard sound/effect/pause combinations, and broad
team/projectile/foreign-state/multiple-contact combinations remain gates.

## Checkpoint validation

All90 integrated native replays pass with the race detector in
`temp/m4bb-integrated.log` (Match61.287s). Pure CNS/engine/combat/evaluator races
pass in `m4bb-precedence-pure.log`. The first expanded replay exposed seven
last-parameter-wins failures; the ordinary-parameter precedence fix closes them
without changing accepted source or numeric goldens.

Thirteen actual app runs produce sixteen captures from one binary,
`d354737083b645fc513029e69f441268af50869ed14c3dbed067b2d5048185bb`.
Prepared player/stage/fight inputs, binary/config/capture hashes, dimensions and
runtime logs pass the audit in `m4bb-final-app-audit.json`. Twelve strict pose
masks match exactly: 8,728 to 13,923 garment pixels per scene. The mask selects
native garment RGB247,239,231 in box160,180..1120,650; grayscale announcement
text is outside this actor-color check. The original airborne native window
had a K.O. overlay covering1,180 of these pixels. Its failure is retained in
`*-occluded` diagnostics. An unchanged-source native replay at8s verifies its
pinned numeric observations and matches all13,923 pose pixels. No source,
numeric golden or pixel tolerance was changed. The stock throw7s capture was
inspected: two recovered fighters and no persistent effect clones.

Nine fresh native programs reproduce ten numeric captures and one expected
literal-float rejection, including full-field order, fall outside H/falling,
fatal guard, mixed coordinates and the unobscured airborne pose. See
`m4bb-replays.log` and `m4bb-replay-ko-air-up-unoccluded.log`. Final input audit
checks all360 source hashes. Original reference executable and root module
hashes remain intact. Full scoped races pass in m4bb-full-race.log
(Match618.564s, Character11.012s, System5.027s).

Reversal is M4ba3fa397a9. Absolute KO/announcement timing, stage debug fill,
M4az's six alpha crops and complete engine/asset/mode compatibility remain open.
