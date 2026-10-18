# Trigger runtime routing audit (M4bz)

Target: the immutable local M.U.G.E.N 1.1 beta 1 trigger manual.
Run `python3 game/mugen/tests/trigger_inventory.py` from the repository root.
After reviewing an intentional routing change, `--write` regenerates
[TRIGGER-BINDINGS.json](TRIGGER-BINDINGS.json). The tool compares the manual's
127 section names with the recognized vocabulary and inspects named production
functions; parser/validation switches are excluded. Explicit conditional routes
include Command, Const variants, IfElse/Cond, Log, GetHitVar and HitDefAttr.

109/127 families (85.8%) have a candidate runtime route; 18 have none. This is a
source-routing percentage, not behavioral coverage. A route can return bottom
in an unsupported context, implement only some variants, or call partial code.
No row is marked variant-complete. The original compatibility table is historical.

| Unbound families | Dependency / next obligation |
| --- | --- |
| HitPauseTime, Random | Scalar/state context and random-stream semantics, including pauses and redirection. |
| MoveReversed | Actual reversal acceptance and contact bookkeeping. |
| NumProjID, ProjCancelTime, ProjContact, ProjContactTime, ProjGuarded, ProjGuardedTime, ProjHit, ProjHitTime | Projectile identity, lifecycle and owner credit; legacy indexed comparison forms. |
| IsHomeTeam, TeamMode, P3Name, P4Name | Match mode, teams and available-player context. |
| TopEdge, BottomEdge, StageVar | Vertical/zoom camera and typed stage selectors. |

The manual documents 36 distinct GetHitVar selectors (including selectors listed
in Details but omitted from its short argument list). 29/36 have both typed
runtime and Character routes (80.6%); seven are missing: `fallcount`, `hitcount`,
`hitid`, `recovertime`, `xveladd`, `yveladd`, `zoff`. The existing `air.animtype`
route is an additional accepted selector, outside this denominator. M4ca measures
combat metadata/counters; M4cb separately measures the liedown `recovertime`
consumer and actual recovery routing. Eligibility alone does not prove routing.

Remaining variant obligations include selector argument/error/type checking;
vector axes (recognition currently admits generic X/Y/Z more broadly than the
runtime); win/lose suffixes; numbered/legacy projectile forms; name comparison
and missing opponent behavior; redirect/foreign/helper ownership; pause/state
lifetime; and all Const/StageVar/GetHitVar selectors. MatchNo=1 is bounded to
one-on-one Versus; horizontal edges do not establish vertical camera support.
Math/type/bottom and numeric conformance remain governed by their native controls.

Validation: the generator was run in write and comparison modes and the scoped
evaluator tests pass. No Go runtime changed in M4bz, so no new native/application
capture or full race rerun is required for this source-only checkpoint. The
M4by final integration remains the last full suite. Do not average this inventory
with key registration, stock compilation, test counts or whole-engine coverage.
