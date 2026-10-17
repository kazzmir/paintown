# HitDef target and team selection

M4ao compares 28 original native programs: 300 numeric observations, three
load rejections and 112 CNS/AIR/CMD/stage hashes. The source of truth is the
supplied 1.1 beta executable and its HitDef documentation.

## Single-target definitions

Native one-target controls distinguish ordinary NA omission/zero from explicit
one, negative and computed values. NT, ST and HT default to one; explicit zero
allows multiple victims. A literal 1.5 rejects the controller during loading,
while Time+1.5 is accepted and converted at execution. Bottom (1/0) becomes
zero in this argument. The compiled path validates before invoking the host
and preserves immutable source maps.

A one-target definition consumes its first accepted contact, including a guard,
and cannot affect the next overlapping helper on a later tick. Guarding leaves
the root at Life=997, the helper at 1000 and MoveHit=0/MoveContact=70; an ordinary
multiple-target definition also hits the helper at birth+2. The per-definition
entity guard retains consumption until a new HitDef, independently of contact
counters. A replacement one-target ID 8 hits the root again (Life=980), drops
the old helper target and leaves NumTarget=1, NumTarget(7)=0 and Target ID=56.

## Teams and ordering

E (default) selects opponents, F selects friendly entities and B selects both.
The attacker is excluded. Both is accepted by its initial letter; Z and quoted
"B" reject loading. Root-origin cases distinguish enemy helper 58 from friendly
helper 59. Helper-origin cases separately check friendly and enemy roots.
Enemy/NumEnemy queries remain root-only and independent of this combat list.

Candidate order is roots before helpers. Among roots, enemies precede friends;
helpers follow creation order regardless of team. A both-root helper attack
selects enemy root 57 before friendly root 56, including with hitonce=1.
A delayed mixed control instead selects friendly root 56 before enemy helper
59; its delay removes newborn-helper activation as an explanation. A further
control moves both roots outside attack range and creates friendly helper 59
before enemy helper 60. The one-target attack hits 59. These independent cases
distinguish root priority, root team ordering and helper creation order.

## Evidence and limits

Original sources/goldens: tests/testdata/reference/hitselect-*.
The first fifteen-program pre-fix comparison fails twelve cases in
temp/m4ao-before-selection.log. The first integrated comparison passes 22 of 24
programs and isolates bottom conversion and root ordering failures in
m4ao-first-selection-race.log. Headless CNS/engine/evaluator races pass in
m4ao-pure-race.log. All 28 programs pass in m4ao-second-selection-race.log. The
full suite passes in m4ao-full-race.log (Match 167.428 seconds). Four fresh native
replays pass: guard-once, replace-drop, helper-pair-team-order and invalid-team
under temp/reference-runs/m4ao-replay-*.

Seven real 1280x720 captures under temp/app-runs/hitselect-helper-order-go/
(3/5/7 seconds) and hitselect-stock-throw-go/ (2/3/4/7) pass provenance,
dimensions, nonblack and runtime/resource checks. Stock recovery retains two
fighters without permanent dust clones. The overlapping helper scene's white
clothing masks contain 22,362 native versus 22,626 Go pixels and differ at
12,036 pixels. The foreground facing also differs at overlapping actors,
pointing to a tied sprite-order problem that needs independent controls.
Existing late clipped placement, native debug magenta versus Go black,
background filtering and HUD differences remain open. These are smoke checks,
not framebuffer parity. Measurements: temp/m4ao-app-comparison.json.

This is not complete HitDef compatibility. Priority, cancellation, reversal,
override/foreign/pause combinations, all hitflags/attributes, binding release
on discarded targets, complete collision geometry, projectiles, team modes and
renderer sampling remain separate obligations. Legacy projectile adapters
retain their prior collision ownership restrictions and are not promoted by
root/helper team evidence.
