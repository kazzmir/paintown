# HitDef chains and juggle costs (M4bp)

Target: immutable local M.U.G.E.N 1.1 beta 1. The allocation covers air.juggle,
chainID and nochainID. These add three integer expression schemas, bringing
numeric routing to 58/81 and typed schemas to 67/81 (nine literals remain a
separate count). Fourteen documented keys remain in constraints, palette and
shake. These counts do not establish whole-engine compatibility.

## Native rules and consumers

The three keys reject float literals and malformed/unknown expressions before
state effects. Runtime floats truncate; bottom becomes 0. Defaults are 0 for
extra air juggle cost and -1 for the chain selectors. A missing second nochain
component is -1. Both components evaluate first-to-second, even when the first
is negative. Cross-field order is id, chainID, nochainID, kill/guard.kill/
fall.kill, hitonce, air.juggle, then damage and the previously measured fields.
Forward and reverse declarations yield identical tagged order.

A nonnegative chainID requires a preceding contact from the same attacker with
that ID. Negative values allow any predecessor. An explicit matching chain
wins over a conflicting nochainID. Otherwise nochainID excludes either of its
IDs while the same attacker owns the current hit context; a negative first
component disables the exclusion, including a positive second component.
A guard establishes the predecessor. None does not. An idle contact destination
ends the predecessor/fall context. Later StateTypeSet in an established hit
state retains the separately measured fall metadata.

Ordinary attacks charge the Statedef juggle cost once per move. Defining a new
Statedef juggle parameter starts another cost; a continuation without one
inherits the move. Measured 7-point states allow two contacts from 15 points;
8-point and 16-point states allow only the initial ground contact. The extra
HitDef air.juggle value is evaluated but does not charge ordinary root contacts
in the one-state controls, including 0/8/15/16/-1. Projectiles charge it per
projectile: against an airborne receiver, 0 permits three hits, 8 permits one,
and 16 permits none. Non-falling root/projectile contacts do not spend points. Two such contacts
before a final falling contact retain the initial budget. GetHitVar(chainid)
reports the actual authored ID (7 then 9), including valid ID 0. None leaves
it 0. The beta rejects GetHitVar(hitid), despite the argument-summary typo in
its manual; three independent programs establish the load error. Static
selector validation and the typed chainid binding now follow that behavior.

The projectile consumer required a bounded adapter correction: native projanim,
offset and velocity names, active HitDef/animation, single-contact retirement,
removal time and target credit to the owner. Projectiles now advance without
executing the owner's CNS or starting a second world frame. Full projectile
schemas, cancellation, priority/hit-count interactions, removal animations,
queries, teams, foreign states and pause variants remain a separate gate.

## Evidence

The final 57 hitcombo-* programs pin 660 observations, eight native rejections
and 228 pre-native hashes. Two misplaced-marker order drafts are archived under
ignored temp/m4bp-draft-order, unpromoted. Three duplicate-state drafts were
archived before native execution. Corrected source inputs were frozen as new
programs; native values/tolerances were never changed. Manifests distinguish
34 base, six extra, three clean state, two final order and two component-order
controls. Four additional non-fall, three rejected hitid, two chainid-query and one
zero-ID controls were frozen before native execution.

Integration results, provenance and remaining limits are recorded in HANDOFF.md.
Broad entity/helper/third-party/projectile/reversal/foreign/pause combinations,
all recovery and full combat remain open. Existing strict alpha/spark raster
failures and absolute round clocks remain unwaived.

The enum/combat full scoped races passed before the final lifetime/query
changes (Match 805.949s). Final affected Match replays pass 90.507s, full
Character 11.481s, core races and fresh native/application checks pass. The
final full integration is scheduled after M4bt; source snapshots and diagnostic
failed logs are distinguished in HANDOFF.md.
