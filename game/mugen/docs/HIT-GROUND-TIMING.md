# Ground and guard HitDef timing

M4bg compiles `ground.hittime` and `ground.slidetime`, plus the four guard fields
whose defaults depend on those expressions. Their shared executor feeds the
existing production Character/Match consumers; there is no separate test engine.

## Native rules

| Field | Omitted source |
| --- | --- |
| ground.hittime | 0 |
| ground.slidetime | 0 |
| guard.hittime | ground.hittime |
| guard.slidetime | ground.slidetime |
| guard.ctrltime | guard.slidetime |
| airguard.ctrltime | guard.ctrltime |

Each inherited field re-executes the original source expression, then truncates
runtime floats toward zero and clamps negative results to zero. Bottom becomes
zero. Literal floats reject at load, as do multi-value tuples. Unknown triggers
reject before any state controller can mutate the host.

For example, starting var(10)=7 and var(11)=11, sources
`ground.hittime=(var(10):=var(10)+1)+20` and
`ground.slidetime=(var(11):=var(11)+1)+30` produce ground times 28/42, guard
times 29/43, ground guard control 44 and air guard control 45. Final variables
are 9/15 even when the actual contact is an ordinary hit. These are source
defaults, not copies of previously stored integers. The guard.slidetime default
differs from the manual's guard.hittime wording, agreeing with earlier native
guard-defaults evidence.

Forward/reverse full-field tag programs establish these insertion points in
the previously measured HitDef evaluation order:

- ground.hittime, ground.slidetime: after p2facing, before ground.velocity.
- guard.hittime, guard.slidetime, guard.ctrltime: after guard.velocity,
  before guard.pausetime.
- airguard.ctrltime: after airguard.velocity, before airguard.cornerpush.veloff.

Source declaration order does not change evaluation order. Compilation retains
immutable expression trees and does not write defaults into shared source maps.

## Evidence and verification

Twenty-one original text programs, `hittiming-ground-*`, pin 84 authored input
hashes, 348 numeric observations (including overlapping clipboard pages), and
four native load rejections. Source manifests were saved before native runs;
expected values come only from the original 1.1b1 executable. No new binary
fixtures or copied reference assets are committed. Do not regenerate these
sources or record goldens from Go.

Cases cover omitted defaults, dynamic expressions, repeated source assignments,
ground guard inheritance, air guard control, negative/runtime-float/bottom
values, integer/tuple rejection, and forward/reverse source order. Entry-time
GetHitVar/HitOver values and life/guard/contact observations exercise the shared
Match consumer. Unit tests additionally check shared-host isolation and atomic
compilation errors for all six fields.

Focused integrated races pass all 21 programs plus the 19 prior pause and four
fatal-guard pause controls (Match 29.907s). Engine/evaluator/CNS/combat races pass
1.454s/1.020s/1.013s/1.012s. Three fresh native reproductions (guard defaults, reverse order, float rejection)
pass. Four actual apps/five captures pass source/config/binary/PNG/runtime
provenance audit, including KFM intro and mixed stock fighters. The full scoped
race suite passes: Match 614.394s, Character 11.208s, System 4.601s, storyboard 1.789s, stock/resources 6.528s.
Logs: temp/m4bg-final-validation.log and m4bg-final-full-race.log; provenance:
temp/m4bg-evidence-audit.json. The first disposable app recipe used a stale
pause-family directory before launch; its failure is retained in
temp/m4bg-validation.log, and the corrected final pipeline passes.

Example fresh native replay, using BUILD.md's private environment:

```
python3 game/mugen/tests/reference_probe.py --fixture hittiming-ground-airguard-side-effects --name UNIQUE_NAME --fixture-stage --bank fvar --mugenversion 1.1 --clipboard-delay 0 --pages 0 --seconds 6
```

The expression-routing audit is now 44/81 documented HitDef keys (54.3%), with
37 literal/absent fields. This is source routing, not complete behavior coverage
or whole-engine completion. Remaining timing work includes air.hittime and
down.hittime schemas and their consumers, exact recovery/KO/round clocks, and
broad pause/projectile/helper/foreign/team/multiple-contact combinations.
Six alpha and two spark raster failures remain open. Stop after M4bg's validated
local checkpoint for the user's testing allocation.
