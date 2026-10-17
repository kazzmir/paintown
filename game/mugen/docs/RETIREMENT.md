# Helper retirement and orphaned effects

M4ak compares fourteen helperretire programs: 288 numeric observations, two
intentional load rejections and 56 authored CNS/AIR/CMD/stage hashes. The native
engine, source and recipe are pinned by each JSON. Diagnostic authors/runners
are temp/author-helper-retire*.py, run-helperretire.py and the two case lists.
This checkpoint closes focused retirement gaps, not the entire helper gate.

## Removal and retained relationships

DestroySelf compiles recursive/removeexplods as integer expressions before
executing effects. Both default to zero. Recursive values 1, 2, -1 and Time/3
at Time=3 remove the helper's full descendant tree. A bottom recursive expression
acts as zero. Float literals reject in either field. Default/nonrecursive
removal leaves descendants alive and stops following controllers in that pass.
The registry retires IDs immediately; it never reuses them in the match.

The nested control starts with three helpers. The root sees three at its Time=3
pass, before the later helper's removal. At Time=4 it sees two with default
removal, or zero with recursive removal. PlayerIDExist(58) becomes zero while
IDs 59/60 survive only the nonrecursive case.

An orphan retains the original parent pointer for Parent redirects. In the
moving-parent control, removal at Time=3 still completes that parent's physics
and clock step: later reads see Parent Time=4, Pos X=49, Pos Y=-26, Vel X=2,
StateNo=800, Life=1000 and the original ID 58. This differs from PlayerID(58),
which no longer resolves. ParentVarAdd after removal leaves its value 7 unchanged.
The measured Parent NumHelper and ParentDist values become zero; live child/root
family queries still find the two descendants. These packets distinguish retained
metadata from live registry/parent mutation. They do not establish every trigger
or redirected assignment against retired state.

## Owned and orphaned Explods

Without removeexplods, existing helper-owned Explods survive removal. With the
flag, that helper's Explods disappear. Recursive removal applies the same choice
to descendant effects. Five native scenes isolate all four combinations and a
finite lifetime: nonrecursive preserve/remove, recursive preserve/remove and
recursive preserve with removetime=600.

The scenes use blank root/helper animations and two static authored Explods.
They use the beta 1.1 default postype=none, so their positions are stage-space;
bindtime=-1 alone does not make those scenes owner-bound. Binding release has
separate direct simulation coverage and the native manual's explicit rule;
complete executable binding-release timing still needs its own control.

The world now advances orphaned effects independently of active helper passes.
Their resources, palette, coordinate units, animation, motion and remaining
lifetime survive. Finite orphans expire; permanent ones remain until explicitly
removed or the round resets. Effects requested by live helpers still advance
once per tick. Removal releases actor/Explod anchors that reference retired
helpers. Round reset retires outstanding orphan visuals and drops their owners.

## Validation and limits

Focused native comparisons plus the orphan clock/expiry/binding/reset regression
pass in temp/m4ak-focused-race.log. The engine race test in m4ak-engine-race.log
also checks that an invalid later argument cannot execute an earlier variable
assignment. Pre-fix failures remain in m4ak-before-retirement.log. Final full
suite, window measurements and fresh replay results belong in CHECKPOINTS.md.

Still open: native bound-target release publication, helper sound channels,
retired-state assignment and all remaining relationship queries, helper-owned
projectiles, death/KO removal versus DestroySelf, pause/hitpause/foreign ownership
combinations, per-owner effect limits and full multi-entity combat. The existing
helper configuration/phase evidence is in HELPERS.md.

Six final 1280x720 captures under temp/app-runs/helperretire-scene-*-go-final/
match the native clothing masks exactly. Nonrecursive preserve/remove retain
two/one effects; recursive preserve/remove retain two/zero. Each visible sprite
has 8,379 thresholded white pixels, at the same coordinates in both engines.
The finite recursive orphan shows two sprites at seven seconds and zero at
fourteen. Native late expiry is the separate m4ak-replay-scene-finite capture;
clipboard pages restart the native process, so pages alone are not elapsed-time
samples. Full framebuffer/background/debug/HUD equivalence remains open.

The first app captures exposed hardcoded linear effect filtering (15,304 white
pixels versus native 16,758 for two sprites). Nearest-neighbor sampling fixes
all final masks. Full scoped races pass in temp/m4ak-full-race.log; the final
renderer change additionally passes m4ak-system-final-race.log. Pre-filter
captures remain in the corresponding *-go directories. Final capture hashes,
dimensions/nonblack checks and runtime logs pass; measured masks are recorded
in temp/m4ak-window-comparison.json.

Fresh recursive-negative/orphan-parent replays and the fourteen-second finite
scene reproduce all observed variables/input hashes, under m4ak-replay-*.
