# Initial root placement and the fight-call reset

M4dp separates scripted positions from the initial round drawing position. The
beta draws each root at its stage starting X/Y during RoundState 0 and 1, while
CNS continues to read the actual changed coordinates. The current animation
still draws: changing from the idle sprite to the attack sprite is visible at
the starting position. Helpers keep their own displaced drawing positions.
Positive shadow controls follow the root's displayed placement.

At the transition to RoundState 2, the beta restores both roots' actual X/Y to
the stage starts. This happens before the first RoundState-2 CNS pass, including
when a moving owner observes the boundary during Pause. Animation, state time,
counters and markers remain intact. Velocity, facing and Z resets were not
measured and are not inferred by this change.

The shared actor/shadow drawing consumer now uses the starting root X/Y during
those initial phases. Scripted coordinates, collisions and effect producers
continue to use actual coordinates. Match restores actual root X/Y on the first
transition to RoundState 2. No paused sprite cache or phase-clock freeze was
introduced.

## Independent native controls

All original inputs, resources, recorders, configurations, captures and numeric
expectations remain unchanged. The 23 stable programs were reviewed in native-only
form before promotion or production implementation; they pin 276 numeric values.

| Programs | Native distinctions |
| --- | --- |
| `preintropose-*` — 10 | Changed X/Y and animation before Pause at GameTime 1, 4, 29, 30, 31, 40, 100 and 130. Extra owner passes at 2/5 preserve actual changed X and animation while drawing stays at the start. |
| `posecontext-*` — 10 | Explicit intro on P1/both at 31/40/99, helpers displaced to X=-120 at 1/31, positive shadows at 1/100, and the 99/100 boundary. |
| `poseboundary-*` — 2 | The round phase advances on the issuing frame. The next owner pass after Pause at 99 observes RoundState 2 and both roots' starting X values. |
| `posevertical-*` — 1 | Both actual Y values return to zero at that boundary after prior -20/-10 writes. |

Four original-input ledgers and four exact configuration-finalization ledgers
retain sources, full independent parsed/compiled intents, captures, seeds and
resource provenance. The strict world region `[160,140,1120,640]` was selected
from native images before the Go baseline. The newly named comparator checks
exact RGB and original preparation/finalization hashes.

A separate five-program active-play study collected 55 frames and 660 numeric
observations with calibrated Pause/Scroll Lock stepping. All 45 issue/following
worlds equal the no-Pause post-issue world exactly. Both roots complete their
issuing-frame position, animation and marker controllers. These controls disprove
a generic Pause drawing or scheduling correction. They remain private
counterfactual evidence, rather than additional published controller coverage.

## Baseline and revision

The unchanged production baseline has two failing native X/Y reset cases; their
parent also fails. The first derived image comparator expected obsolete
system-effect configuration fields and failed before pixel comparison. Its source
and errors are preserved. A separately named corrected comparator reused the same
completed application captures. No application, native input or expectation was
rerun or changed to correct that metadata error.

The six measured baseline world differences are 79,755 pixels for root/intro/helper
controls, 110,597 for the shadow control, 36,030 for an already-correct placement
with a held FIGHT overlay, and 97,837 for the combined boundary placement/overlay.
All six native/Go pairs were reviewed before implementation.

Revision83510 completed/awaited exit1; the separate elevated 21,476-input/client
guard passes. Core races pass890 results; all23 native cases pass (24 results
including the parent). Root, explicit-intro and helper application worlds now
match exactly, across1,440,000 RGB pixels. The positive shadow is reduced to464
pixels differing by at most one RGB level. The two fight-call worlds differ36,030
pixels because the HUD overlay remains held. All six revision pairs are viewed.
These three strict failures remain unwaived; no sampler or tolerance changed.

Final16701 completed and was awaited with exit1; the separate21,476-input/client
guard passes. New worlds are16/23 exact (69.6%),7,680,000 RGB pixels. The
remaining seven cases are six held HUD observers (one also has468 shadow pixels)
and one pure shadow case of464 pixels/max-one RGB. All seven original early
dust/Game worlds are now exact across3,561,600 pixels. All39 native/Go pairs,13
full-scene sheets and stock3 captures are viewed.

The combined native gate passes481/482 cases:506pass/twofail results including the
earlier negative-start constraint case's parent. Additional match tests pass 16
results. Core890 remains passing with all185 production Go files byte-identical
through the final run. The latest full scoped suite remains historical
M4dl5,028/36.

The final82 gates preserve nine exit1 statuses: native, seven new world comparisons
and stale inventory; the other73 pass. Exact old inventory sources are archived
against Git before regeneration, and the current check passes. Passive audit21612
completed/awaited0:710 original native path checks,2,553 prepared path checks,
943 native/1,640 application binary-resource copies,23 exact configs,42 app
captures, shared binaries, seeds/recomputed RGB/input/PCM/source versions pass.
All45 active-pause counterfactual worlds and768 balanced native events are also
verified. Native authoring context is resolved against the recorded Git version;
native-used inputs stay immutable original paths.

No original native input, expectation, region or comparison tolerance changed. The first
failed derived comparator remains unchanged and excluded from the checkpoint;
the successful separately named comparator is the production verification tool.
Broader reset/afterimage/camera/round variants, held HUD component expiry and
older raster/audio failures remain open. This is bounded evidence, not an overall
engine completion percentage. Continue [HUD lifetime controls](HUD-LIFETIME-PLAN.md)
before implementing those timings.
