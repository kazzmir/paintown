# AssertSpecial — guard and walk controls

M4di compares automatic guard entry, scripted guard poses, attacker unguardable,
walking and the state-file dependency exposed by actual keyboard input. The
supplied M.U.G.E.N 1.1 beta 1 executable is the behavioral authority.

| Native controls | Programs | Numeric values | Observed behavior |
| --- | ---: | ---: | --- |
| Scripted standing/crouching/air guard | 15 | 180 | Posture-disable flags do not cancel an authored guard pose. Attacker unguardable changes guard to hit; defender unguardable does not. |
| Owned keyboard guard entry and walking | 13 | 156 | Each posture flag blocks its own automatic guard entry. Unrelated flags preserve entry. Nowalk blocks forward and backward walk entry. |
| Actual-input/render observers | 16 | 192 | Separate native EnvColor observers expose guard/hit and allowed/blocked walk outcomes. |
| Function-style Command syntax | 1 rejection | 0 | `Command("holdback")` rejects at load; canonical `Command = "holdback"` works. |
| Common-file/undefined-state dependency | 6 | 72 | Blank/omitted stcommon load no common file. Explicit common loads its body. Missing ordinary states warn and continue. |
| Total | 51 | 600 | 50 accepted programs and one expected load rejection. |

All complete source/header/controller/parameter/trigger/AIR/CMD/stage/resource
intents were checked before native execution. Every numeric, contact, posture,
event, color and full-scene row was inspected before one-time promotion. Native
source files, recorders, configurations, captured images and expected values
remain immutable. Five disjoint native ledgers and config-finalization records
preserve the recipes. The earlier missing-Data and invalid-Command preparation
failures remain available as diagnostics; only the latter expression rejection
is promoted.

## Production behavior

`nostandguard`, `nocrouchguard` and `noairguard` now gate automatic guard entry
for the matching current posture. They do not change contact classification
inside an authored guard state. `unguardable` is read from the actual attacker
at contact, including shared juggle/contact selection paths; no unmeasured
inheritance of a parent/root flag is introduced. `nowalk` gates entering the
forward/backward walk state. Already-walking and broader helper/projectile/pause
variants still need separate native controls.

Reference expression validation rejects function-style Command with source
context. The generic expression parser retains its separate extension boundary.
Replay adapters translate physical arrow names in frozen input recipes into the
facing-relative directions expected by Character.InputSource. Original recipes
are unchanged. The application recorder replays physical keys to its owned
window and records the exact recipe, hash, schedule, process/window and cleanup.
The original recorder is retained under temp before instrumentation changes.

## Common-file and missing-state correction

The first revised Go matrix matched all numeric guard/walk replays but failed
the crouch-disable application scene. Go silently imported common state 12;
native loaded no common file, warned about undefined state 12, retained crouch
attributes, and continued receiving input. This changed the posture at the point
where the guard-entry flag was checked.

Six independent native controls distinguish loading a common state body from
merely reaching its number. Blank and omitted stcommon skip a custom common1.cns
with a distinctive state-777 marker. Naming that file explicitly loads it,
selects crouch/action9992 and executes marker777. Undefined internal destinations
0, 12, 777 and 999 install the destination number, retain standing/control0/
action9990, advance Time to40 and continue negative states. The prior positive
controller counter remains1. These are measured warnings, not fatal errors.

The loader now skips blank/omitted common declarations while retaining explicit
resolution/compilation errors. Internal missing-state entry follows the existing
external missing-state continuation model. A missing builtin crouch exit may be
requested again while the retained crouch attributes still require it. Malformed
defined programs still stop execution. The earlier unit assumption that every
undefined destination must be fatal was replaced with a malformed-program error
case; the six native programs test actual missing-state behavior.

This supersedes the blank/omitted common-file inference in M3d/M2f and
REFERENCE.md. Their original values remain unchanged: reaching state0 did not
prove that common state0's body had loaded. Transition ctrl/anim overrides,
negative destinations and wider foreign/missing-state variants are not newly
claimed by this six-case study.

## Validation

All **51 native programs pass**: 50 accepted, one expected load rejection and
600 numeric observations. The final 16 application world crops are exact across
**8,140,800 RGB pixels**. All native/Go pairs and the older application scenes
were visually reviewed. The original baseline had three image failures; the
first behavior revision retained one crouch failure, resolved only after the
independent common-file/undefined-state study. Those failures and preparation
errors remain preserved in temp and the [handoff](HANDOFF.md).

The full scoped race suite passes **4,870 results across 36 packages** (25 tested,
11 compiled-only), with no individual test skips or race reports. Match completes
in 2,286.677 seconds. Five older palette/order applications, four shake masks and
stock win/rematch/win pass. Stock captures at 21/26/45 seconds were reviewed;
82 balanced key events and 1,971,685 private stereo PCM frames pass their checks.
The new fixture application inputs contain 24 balanced key transitions.
All 22 final gates and outer session 34865 complete/await with exit zero.

A separate elevated guard verifies 19,190 final, 19,189 revision and 19,187 nested
application frozen paths (57,566 hash checks), with no live clients. The passive
auditor verifies 19,192 unique final inputs, 3,000 immutable original inputs,
all 51 exact native configurations, 41 canonical resources, 2,091 native resource
copies and all 109 manifest files. Records: temp/m4di-final-evidence-audit.json,
temp/m4di-final-visual-review.json and temp/m4di-final1-separate-guard.json.
Validated production binary SHA256:
`50bb38a3e2121ba500250289075dc21a65081ae07456fa39ee64d4adaa4fecbd`.
Only source/text is included in the local checkpoint.

New functional render scope is world[160,140,1120,670], chosen from native-only
images before Go baseline/promotion. Native debug-warning glyphs are excluded
from this new observer region, and full original images are retained. Existing
HUD regions, the35 prior HUD failures and all older strict renderer obligations
remain unchanged and unwaived. No sampling code or tolerance changed.

The eight separate noko observations remain unpromoted: life0/Alive1 was observed
under an active assertion, but late enable/expiry/frozen-owner/death and round
lifetime still require independent controls. No guessed life-one clamp was
implemented. Other special-flag consumers, system animations/dust, clipboard,
residual controller schemas and renderer obligations remain item3 work. Item4
(broader modes/AI/assets/BGM/final conformance) remains unallocated. Routing
remains75/89 typed(84.3%); this is not behavioral or whole-engine coverage.
