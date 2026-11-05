# Controller routing and consumer audit — M4ci

Rechecked 2026-11-04 at df59235b before item 3 implementation. The reproducible
source audit is `python3 game/mugen/tests/controller_inventory.py`; `--write`
regenerates CONTROLLER-BINDINGS.json after a reviewed source change. It compares
the immutable local beta manual's 89 names with actual dispatcher/compiler
routes and named consumer gaps. Registration and typed parsing are separate
from observable behavior; no family is marked variant-complete.

## M4ci baseline findings

- 87/89 manual names are registered. ExplodBindTime and ModifyExplod are absent.
- 59 families have a typed compiler route; 28 use fallback handlers. Eight of
  those fallbacks are parameterless operations, so fallback alone is not a bug.
- 14 families call stubs: AfterImage/AfterImageTime, EnvShake/EnvColor, Trans,
  BindToParent/Root/Target, PalFX/AllPalFX/BGPalFX, ForceFeedback,
  GameMakeAnim/MakeDust.
- Five angle/offset families update fields that player rendering never reads.
- Seven explicitly partial consumers cover Explod, Projectile trails/shadows,
  AssertSpecial, debug text, HitAdd's displayed combo field and StateTypeSet's
  literal fallback. These findings plus the two missing names give 28 families
  with concrete outstanding schema/consumer obligations. Other rows still need
  parameter/variant closure; they are not automatically accepted.
- Only eight literal AssertSpecial consumers were found: intro, noautoturn,
  noguard, nojugglecheck, nokoslow, nokosnd, roundnotover and timerfreeze.
- FVarSet/FVarAdd/FVarRangeSet are extra registered aliases outside the 89-row
  manual. Their beta/version acceptance needs explicit native controls.

`SetHitFallDamage` still has a stub comment, but compiled HitFallSet uses the
measured typed fall-data path instead. Counting that comment as a new missing
production controller would be wrong. The old COMPATIBILITY.md baseline table
is historical and contains many already-fixed rows; use this audit and topic
notes for current allocation decisions.

## Remaining work estimate and boundaries

Item 3 comprises roughly ten substantial groups, often requiring multiple local
checkpoints: Explod mutation/binding/full fields; actor binding; palette effects;
shake/color; transforms/transparency; afterimages/projectile effects; special
flags; dust/system animations; debug/haptics; and residual schema/consumer gaps.
ITEM-3-PLAN.md records order and gates. This is not a reliable time/token estimate
or an overall completion percentage. Closed-source native discovery can change
batch boundaries.

After item 3, item 4 still includes modes/teams/AI, BGM, asset/format/error-matrix
validation, remaining stage/mode/name triggers and final renderer/font/platform
conformance. The playable one-on-one engine and 120/127 trigger routes do not
establish that those larger workstreams are nearly complete.

## M4cj update

ExplodBindTime now has a typed route and bounded native consumer; see
EXPLOD-BINDING.md. Counts are 88/89 registered, 60 typed and 28 fallback, with
only ModifyExplod absent. Fourteen stubs and five drawing fields remain. Eight
partial families now include a newly observed VarSet literal-float schema gap;
the seven M4ci rows were not exhaustive. One bounded native row is distinct from
60 route-only rows. No row is variant-complete. These are source/audit counts,
not an engine completion percentage.

## M4ck update

ModifyExplod now has a typed route and a partial native consumer; see
MODIFY-EXPLOD.md. Counts are89 registered,61 typed and28 fallback, with no missing
dispatcher names. Fourteen stubs, five unconsumed actor draw families and nine
partial families remain. The native mutation corpus and drawing consumers do not
close shared alpha/shadow/priority/pause or final sampling gates. All89 rows remain
variant-incomplete. Registering the last name is not a full coverage claim.

## M4ci validation

The generator passed write and comparison modes, with all 89 names accounted
for and source/manual SHA-256 evidence. Root go.mod/go.sum guards pass. No runtime
Go code changed in this source-only checkpoint, so no full race/native rerun is
needed here. The last integration is df59235b: 3,169 passing results across 36
packages. Native/app/display Go work for upcoming features remains serial.

## M4cl update

BindToParent/Root have typed schemas and bounded native production constraints.
The reproducible audit now counts89 registered/63 typed/26 fallback,12 stubs,
five unconsumed actor draw families, nine partial families, three bounded-native
and60 route-only families. All rows remain variant-incomplete. Read SELF-BINDING.md.

## M4cm update

BindToTarget now has a typed schema and measured combat consumer. The audit
counts89 registered/64 typed/25 fallback,11 stubs, five unconsumed actor draw
families, nine partial families, four bounded-native and60 route-only families.
All rows remain variant-incomplete. Read TARGET-SELF-BINDING.md.

## M4cp update

Trans has a typed native schema and per-actor rendering consumer. AIR blends
also reach actor drawing. Counts are89 registered/65 typed/24 fallback,10 stubs,
five unconsumed angle/offset families, nine partial rows, five bounded-native
and60 route-only families. All rows remain variant-incomplete. Read TRANS.md.

## M4cq implementation

AngleSet/Add/Mul/Draw and Offset now have bounded native drawing consumers.
Counts are89 registered/67 typed/22 fallback,10 stubs, zero unread actor drawing
families, nine partial rows, ten bounded-native and60 route-only families.
All rows remain variant-incomplete;12 raster counterexamples remain unwaived.
Read ANGLES-OFFSET.md. Final full integration passes3,703 results/36 packages with no failures or
individual skips; all13,467 frozen inputs verify.

## M4cr implementation

PalFX now has a typed schema, palette-owned clocks/helper rules and unclamped
production rendering. Counts are89 registered/68 typed/21 fallback,9 stubs,
zero unread actor drawing families,9 partial,11 bounded-native and60 route-only.
All48 palette apps have exact native RGB crops; the wider global/contact/
creation/rendering matrix remains open. All rows stay variant-incomplete.
Read PALETTE-FX.md; registrations do not establish an engine percentage.

## M4cs implementation

AllPalFX/BGPalFX have typed schemas, global clocks and measured production
palette consumers. Counts are89 registered/70 typed/19 fallback,7 stubs,
zero unread actor drawing families,9 partial,13 bounded-native and60 route-only.
All rows remain variant-incomplete.83 native programs and71 actual scenes
establish palette/stage behavior; four actor overlap crops expose the pending
owned-Explod priority consumer. They remain UNWAIVED. Read GLOBAL-PALETTE-FX.md.
This is not full engine/controller/renderer conformance or an engine percentage.

## M4ct update

Actor and ordinary owned effects share measured sprite ordering; all16 new
strict crops and four unchanged M4cs overlap crops match native. The audit now
includes character/actor_order.go and records the bounded priority consumer on
Explod/ModifyExplod. Counts remain70 typed/19 fallback/7 stubs/9 partial/13 bounded/
60 route-only; no family is variant-complete. Read EFFECT-ORDERING.md.

## M4cu update

EnvColor has a typed compiler and measured global lifetime/drawing consumer.
All76 native programs and68 strict application scenes establish packing, errors,
evaluation order, positive/negative durations and layer/palette behavior; see
ENV-COLOR.md. Audit counts are71 typed/18 fallback,6 stubs,9 partial,14 bounded,
60 route-only.71/89 (79.8%) is parameter routing, not behavioral coverage. All
families remain variant-incomplete. Source audit now includes engine/env_color.go.
