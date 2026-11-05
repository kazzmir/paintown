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

## M4ci validation

The generator passed write and comparison modes, with all 89 names accounted
for and source/manual SHA-256 evidence. Root go.mod/go.sum guards pass. No runtime
Go code changed in this source-only checkpoint, so no full race/native rerun is
needed here. The last integration is df59235b: 3,169 passing results across 36
packages. Native/app/display Go work for upcoming features remains serial.
