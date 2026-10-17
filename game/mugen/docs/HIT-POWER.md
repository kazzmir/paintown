# HitDef damage and power

M4ba implements the measured damage/power expressions, shared helper pool,
configured multipliers and contact publication. Reversal:83eb5bd9 (M4az).
Full1.1 beta1 compatibility remains unfinished. The bounded checkpoint passes
its integrated/native/app/full-race validation.

## Expressions and parameter order

Damage, GetPower and GivePower compile as integer tuples. Omitted damage
components are zero. A bare float literal rejects; computed floats truncate,
bottom becomes zero and redirected tuple components retain their environments.
The source expressions remain immutable across actors sharing a compiled state.

Absent hit-power components derive from the raw hit-damage expression times
the configured attack/get-hit factor. An omitted guard-power component derives
from its hit-power expression divided by two. These are expression defaults,
not cached values. Time3-.1 deals2 damage but defaults to attacker2/receiver1;
using the already truncated damage would incorrectly grant1/1. Tagged defaults
execute the original damage expression five times. Explicit source/reverse order
both produce Damage1/Get2/Give4 with counter5. The150-damage control gives105/90,
rejecting promoted-double multiplication of a float32 .7 factor (which gives104).
The implementation retains the measured VM float32 expression arithmetic.

Twenty-seven tagged components, including the existing controller fields, give
the same native order with forward and reversed source declarations:

| Positions | Parameters |
| --- | --- |
| 1,2 | id, hitonce |
| 3..4 | damage hit/guard |
| 5..6 | getpower hit/guard |
| 7..8 | givepower hit/guard |
| 9..12 | numhits, hitsound group, guardsound group, priority |
| 13..17 | p1stateno, p2stateno, p2getp1state, p1sprpriority, p2sprpriority |
| 18..21 | forcestand, forcenofall, sparkno, guard.sparkno |
| 22..23 | sparkxy X/Y |
| 24..26 | p1facing, p1getp2facing, p2facing |
| 27 | ground.velocity X |

The ten vector fields then retain their M4aw measured order. This corrects the
old cue ordering as well as inserting power. The compiler checks that every
compiled cue has an evaluation route. Full27-field captures use overlapping
pages0/12/15; these are independent native launches with pinned source hashes.
This establishes the current typed fields, not every remaining literal field.

## Transfer, rules and ownership

Transfer happens at accepted contact, before receiver entry and attackerTime4
observations. Hit damage10 defaults to attacker7/receiver6; guarding defaults to
3/3 independently of chip damage. Explicit100/80, guard40/20, single-component
inheritance, negative transfers and0..3000 clipping match the original programs.
HitOverride to MoveTypeI still transfers100/80 although life stays unchanged.
AttackMul.5 changes life loss to5 while power remains7/6; NumHits3 also retains
7/6. A miss grants no power. These tests separate transfer from deferred life
application and combo-credit counting.

Helpers read and change their root's pool. Observe/Set/Add/cap/lower/root-add,
Statedef poweradd and SuperPause poweradd all have original native controls.
GetPower/SetPower/AddPower now select the root; helpers inherit configured rules.
Root HUD counters consume that same stored pool.

PowerRules uses an optional rule set so omitted Match options receive .7/.6,
while explicit zero factors remain zero. Production FightState passes the
loaded config values into Match and recreated round actors. Custom0/.3/.2/
negative factors match native; explicit values ignore default-factor changes.
A1000/1000 control clips both roots to3000 and supports visible application
verification of the production config path. Config-entry omission itself still
needs native measurement; this checkpoint does not prescribe that fallback.

## Signed damage and life bounds

The measured contact damage model adds.5 before integer truncation and applies
life bounds. At injured900, raw-10 heals9 to909; guard-3 heals2 to902. Raw-10
with AttackMul.175 heals1 to901. Full-life controls stay1000. Away-from-zero
rounding and unbounded life application fail these observations. Power defaults
continue to use raw damage, independently of these life adjustments.

## Corpus and validation

There are66 original programs:804 numeric observations, three expected literal-
float rejections and264 CNS/CMD/AIR/stage hashes. All66 integrated programs pass
in the full scoped race suite, temp/m4ba-full-race.log (Match538.364s,
Character12.143s). The first65 also passed the focused replay in46.014s.
Ordered/final pure races pass. Original executable/config and root Go module
integrity checks pass. The full command used --check -race -timeout=20m.

All eight actual app runs/eleven captures pass provenance and runtime/resource
checks in m4ba-final-app-audit.json. Their identical binary is
554c7ba209dd5fe6c8c08ccec8cbf2526429101ff2d697f3ec179679338f6a4e.
Seven strict clothing masks match exactly: six have11,181 white pixels and the
helper-cap scene has16,713. The high-rule native/Go screenshots both show level3
in both HUD counters; the production configuration path reaches the power pool.
Viewed stock7s shows two recovered fighters without lingering effects. Four
fresh native programs/six captures reproduce their original observations,
including full reverse field order, helper SuperPause, negative rules and signed
fractional healing. Pipeline m4ba-validation.log/session88607 completed exit0.

The diagnostic stage exposes a separate rendering gap: native fills uncovered
areas magenta for BGDef.debugbg=1 while Go fills black and does not parse that
field. These app masks establish exact actor coverage, not whole-frame equality.
The stock power-bar AIR alpha/animation phases also remain outside the pixel
comparison. No stock sprites, native binary or window captures are committed;
only authored programs and numerical metadata are stored.

Remaining gates include the rest of HitDef's literal schema, tuple omission/
malformation variants, extreme integer overflow, config omissions, foreign/custom-state combinations,
helper/projectile/team/self-contact/pause/multiple-hit pools, kill/fall/juggle/
recovery and full engine/assets/modes. M4az's six alpha precision gates remain
open; they are not waived by the power checkpoint.
