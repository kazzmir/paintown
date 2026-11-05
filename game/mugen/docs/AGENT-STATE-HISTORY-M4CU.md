# Historical agent state through M4cu

This is an immutable documentation snapshot of older working notes, including
superseded allocations, stop conditions and intermediate failed revisions.
It is **not active authorization or current instructions**. Use ../AGENTS.md and
HANDOFF.md for the current allocation and runtime state. Native originals and
observations remain separately preserved; this archive changes no evidence.

## Previous AGENTS.md

# MUGEN Go port

## Active allocation — item 3

On 2026-11-04 the user selected "Complete remaining effects/controllers (item 3)".
Continue autonomously through this allocation with documented local checkpoints
and commits. Items 1–2 completed at df59235b are historical; their stop condition
is superseded for item 3 only. Item 4 (modes/AI/BGM, asset validation and final
renderer/font/platform conformance) remains unallocated. Keep dependent runtime
consumers in item 3 and record exact variant/evidence limits instead of counting
registrations as implemented behavior. Do not stop after two checkpoints.

Start with M4ci: reproducible current controller/schema/consumer inventory;
then close ExplodBindTime/ModifyExplod and the Explod surface, actor binding,
palette effects, EnvShake/EnvColor, draw transforms/transparency, afterimages
(including dependent projectile effects), AssertSpecial consumers, dust/system
animations, debug formatting and force-feedback commands. Audit nominally typed
controllers for dead/partial consumers too. Split/reorder coherent batches when
native findings require it. Each feature checkpoint needs frozen native originals,
focused tests/actual production consumption, findings/limits and a local commit.
Use full scoped races at meaningful integration boundaries and final allocation
closure, not every small revision. Preserve original inputs/captures and reference
binary. Private native/app/display-dependent Go runs stay serial. GOTMPDIR must
use an underscore-prefixed directory under temp/ so interrupted builds cannot
be discovered as packages. Stop after item 3 or a concrete issue needing input.

No pushes, binaries, agents, goals or authored changes outside game/mugen/.

M4cr is committed as c2e32e933. Read PALETTE-FX.md:53 native programs/576
values/five rejections/812 colors/213 hashes;48 exact apps5,216,400 pixels,504
GPU colors/224 CPU colors,19 HitDef apps228 colors. Core/native races, stock
win/rematch/win82 owned events/1,978,304 private PCM frames and final provenance/
module/inventory/staged-text audits pass. No binaries or outside changes.

M4cs is COMMITTED cac8c6957. Read GLOBAL-PALETTE-FX.md.83 native programs/
948 values/four rejects/333 hashes;10,352 GPU samples;71 app palette/stage checks
pass and67 actor crops exact, FOUR owned-Explod overlap crops UNWAIVED126 pixels
each. Overall app RGB FALSE. Final full3,857 results/36 packages25 tested+11
compiled-only, Match1606.364s/total1691.569s,14,164 frozen inputs.10 regressions,
stock win/rematch/win82 owned events/1,941,379 PCM, all final source/native/artifact/
module/inventory/staged452-text-file guards pass. No binaries/push/outside changes.
M4cr c2e32e933. Latest full integration M4cs; earlier M4cq raster12 remain open.

M4cu full third matrix11997 FINISHED/awaited, strict FALSE:66/68 exact; only
negative-duration foreground-under0/1 fail330,880 pixels each. All originals,
JSON/captures/tolerances unchanged. Separate idle guard passed. Entire native
68-scene layer table is now temp/m4cu-native-layer-table.json (native-only).
Correct rule: stage FG always draws; positive front quad then covers it;
positive under quad and all negative durations retain FG. Only base BG suppressed
whenTime!=0. Revised fight renderer moves front quad afterFG and before unowned
foreground cues, retaining their old relative FG order outside front EnvColor.
HUD Draw now always consumed for lazy clocks/resources, but positive EnvColor
masks final HUD fitting. No native expectation edits. New final m4cu4 matrix
starts via run-m4cu4-checks.py, prioritizing all5 FG controls then remaining68
accepted scenes. RerunSystem races; engine/character/189 native source inputs
remain unchanged. Frozen source m4cu4-check-inputs.json; no Go/test/probe/fixture
edits until session drains+separate guard. Then10 regressions+stock/privatePCM,
finalaudit/docs/local commit; continueALLitem3. Prep-m4cv.py notexecuted. No push/
binaries/agents/goals/outside authoring. M4ct a048effa3. Native input hashes305.

Workflow correction for future checkpoints: after freezing native captures,
produce and inspect an ALL-CASE native-only table of each consumer/layer before
implementation. Do not infer uninspected matrix branches from manual wording.
Run dependency/counterexample cases first, retain strict comparisons and gather
all RGB mismatches in a batch. Preserve every failed diagnostic/original.

M4cu second matrix9553 completed/awaited; separate idle guard passed. First26
scenes passed, then positive-under1 FOREGROUND case failed330,880 world pixels:
native preserves stage foreground for positive under1 despite the manual.
Positive under0 and all negative durations suppress it. Second diagnostic is
preserved unchanged. Fight renderer condition corrected; m4cu3 fresh all68 apps
plus System races now starts via run-m4cu3-checks.py. Native/code fixtures/
comparisons untouched. Collector keeps public probe nonzero failures and reports
all mismatches at the end, avoiding stopping at the first independent pixel
failure; no tolerance/expectation change. Final audit checks first core/native
inputs unchanged except fight renderer, plus new final-source manifest.
Await m4cu3 runtime and separate guard BEFORE edits/next runtime. Finish gates,
local checkpoint then ALLitem3 (EnvShake preparation not executed).

M4cu first validation68255 completed and separate guard passed. Core and189
native programs PASS (Match123.968s). First baseline/bottom full466,400-pixel
crops and samples pass; first boundary-blue crop has ZERO world RGB differences
but9 HUD samples fail: native positive EnvColor covers HUD even under1. Native
76 JSONs/captures unchanged. First run preserved diagnostic. System HUD fix
written and inventory refreshed; final m4cu2 run now starts via
run-m4cu2-checks.py. New input snapshot m4cu2-check-inputs.json; rerun System only
and all68 apps. Core/native source inputs unchanged except fight renderer;
final audit checks that explicitly instead of claiming a fresh full rerun.
Await final runtime+separate client guard before edits/other runtime. No frozen
Go/test/probe/fixture edits. Finish regression/stock/audit/docs/commit thenALLitem3.

M4cu implementation is WRITTEN; verification RUNNING session68255 via
run-m4cu-checks.py. All76 native controls promoted unchanged:68 accepted/816
values/eight rejects/305 unique hashes/2,516 native RGB points. Batch87036/13521/
70876 all awaited and separate idle guards passed. Native packing is uint32
r | g<<8 | b<<16 WITHOUT component masks; boundaries identify sign propagation
and channel carry, not clamping. RGB/time/under order23415; time1 default, negative
time stage suppression/black/no quad, positive time RGB overlay, AllPalFX first+
final pass, BG unaffected, ordinary/ontop/actor/projectile/foreground consumers.
Engine1.450s/Character11.338s/System9.430s core races PASS. Native189-program and
68 strict466,400-pixel apps pending. m4cu1-check-inputs.json freezes source/test/
probe/fixture/module; NO edits while session68255 is active. Await+separate guard
before edits/next runtime. Only MD/ignored private prep allowed during runs.
Continue final regression/stock/audio/audits/docs/local commit, then EnvShake and
ALL item3. prep-m4cv.py is prepared ONLY, not executed. M4ct a048effa3. No agents/
goals/push/binaries/outside authoring; native originals/captures/goldens immutable.

M4cu native first45(87036) and supplemental23(13521) batches completed/awaited,
with separate idle guards. Boundary8 controls/33 hashes are now frozen ONCE and
start via run-m4cu-boundary.py. Total76 programs/305 unique source+probe hashes.
No EnvColor Go changes or JSON promotion yet. Read ENV-COLOR.md and immutable
observations. RGB->time->under order23415; finite live overlays, negative-time
black stage/visible actors, global expiry through CNSPause, positive All/BG and
layer controls are measured. Channel-range conversion needs boundary controls.
Await new native batch and separate client guard BEFORE any Go/test/probe/fixture
edits/runtime. Original inputs/captures/goldens immutable, no tolerance changes.
Temp/promote-m4cu.py prepared for76; temp/prep-m4cv.py is prepared ONLY (not
executed): future EnvShake schemas and six-frame ScreenPosY traces. ContinueALL
item3 after checkpoints. M4ct a048effa3; no agents/goals/push/binaries/outside scope.

M4cu EnvColor is ACTIVE. temp/prep-m4cu.py executed ONCE after M4ct commit,
freezing45 EnvColor controls/181 original+probe hashes. First45 batch87036 completed/awaited; separate idle guard passed. Native
supplement23 cases/93 hashes starts via temp/run-m4cu-extra.py, after separately
named long-positive/live-negative controls were frozen ONCE. Total68 programs/
273 unique source/probe hashes. No Go implementation or JSON promotion yet. Do not edit Go/tests/probes/fixtures during frozen
native runs; await, then a separate idle-client guard before implementation or
another runtime. Original sources/captures/goldens stay immutable. Include
schema/evaluation, global lifetime, over/under, Explod/ontop/Projectile/foreground,
helper ownership and All/BG composition. Continue ALL item3 after this checkpoint.
Completed Go app asset copies now share identical immutable bytes (43,003 paths,
approximately5.06GB unique inode bytes released); paths/hashes preserved and
native/captures/goldens untouched. Log:private-go-asset-sharing.json. No push,
binaries, agents, goals or authored changes outside game/mugen/.

M4ct is COMMITTED a048effa3; all acceptance gates pass. Read docs/EFFECT-ORDERING.md
(or EFFECT-ORDERING.md from docs).16 native programs/192 readings/65 original+
probe hashes;16 exact151,800-pixel crops and all FOUR unchanged M4cs overlaps
fixed exactly. Core Character11.320/System9.465/Effect1.011 races,99 effect/global
native programs67.354s and14 actor-order programs10.476s pass. Four regression
apps and visually inspected stock win/rematch/win82 balanced inputs/1,965,839
private PCM frames pass. All source/native/resource/binary/config/PNG/audio/
module/inventory guards pass; no clients. Audit:m4ct-final-evidence-audit.json.
Latest full M4cs3,857/36 remains;12 M4cq raster cases unwaived. Continue ALL item3 after the local source/text checkpoint. Next guarded preparation temp/prep-m4cu.py is NOT
EXECUTED: EnvColor schemas/clocks/layer matrix. No original/capture/golden changes,
no tolerance relaxation. No item4/agents/goals/push/binaries/outside authoring.
Private Xvfb/Pulse are healthy; serial runtime and frozen-source policy remains.

M4cq is934bd9203, latest full3,703 passes/36 packages (25 tested+11 compiled-only),
13,467 frozen inputs. Twelve raster counterexamples remain unwaived. Continue
ALL item3, then stop only at its completion or concrete issue needing input.
No agents/goals/push/binaries/outside authoring. Private wrapper/servers as below.

M4cq consumes native angle/scale/Offset state and drawing. Read ANGLES-OFFSET.md.
62 promoted programs/696 readings/four controller rejections;282 original/probe/
stock-constant/empty-world hashes remain immutable. Seven incomplete-Data drafts
are unpromoted; seven replacements capture successfully before a post-loop
provenance-name KeyError, corrected by metadata iteration and a separate audit.
All originals/diagnostics are preserved. Original62 JSON wider crop goldens stay
unchanged; separate empty-native/Go-calibrated actor crops are native-only.

Final58 actual apps:46 exact RGB crops/21,454,400 pixels and54 exact white masks.
12 fractional/nonquarter raster counterexamples remain UNWAIVED; strict matrix
correctly exits1. Do not call all58 RGB-conformant. Positive/mirrored quarter
turns are exact after dominant-axis addressing and screen-space flipped bias.
Inverse projection was worse/reverted. New read-only drawing_render_probe.py
checks source/resources/binary/config/PNG/crop hashes and preserves diagnostic
regions via --original-region. Leading-BOM loading and _tmp launcher are fixed.

FINAL FULL SCOPED RACES PASS3,703 results/36 packages (25 tested/11 compiled-only),
no failures/individual skips; Match1490.670s, total1547.931s. All13,467 frozen
inputs verify. Stock win/rematch/win is visually inspected (contact sheet plus
originals),82 balanced owned input events/1,995,181 private PCM frames. All282
native/full/app/module/inventory/provenance guards pass. Final audit:
temp/m4cq-final-evidence-audit.json records application conformance FALSE for the
12 cases. Sessions21196(full)/20967(stock) awaited; separate client checks empty.
Commit the bounded M4cq checkpoint, then continue ALL item3. Engine/drawing.go
needs explicit git add -f (historical ignore); source/text only, no push/binaries.

M4cr now follows as recorded in the leading active block. Continue
palette/shared effects, environment/afterimages, remaining creation/dependencies
and residual controllers. Source67 typed/22 fallback/10 stubs/zero unread actor
draw families/nine partial rows is not whole-engine coverage. Item4 remains
unallocated; full raster/font/platform conformance stays explicitly bounded.
M4cp acfdbb1dc/M4co33098c939; latest completed full suite is M4cq. No agents/goals/
out-of-scope authoring. Private host: item3-private-servers.json, round-task-env.py.
All native/app/display-dependent Go stays serial; await then separately check
clients. Never edit Go/tests/probes/fixtures during a frozen run. Preserve native
originals/captures/goldens and all failed diagnostics. No tolerance changes.

M4cp consumes actor AIR transparency and typed Trans overrides. Read TRANS.md:
56 promoted programs/492 readings/15 rejections/630 colors,233 original/probe
hashes including two preserved unpublished-transition diagnostics. All41 actual
apps match every RGB pixel in108,675-pixel crops (4,455,675 pixels total).
Core/GPU races and56-program Match races29.023s pass. Stock win/rematch/win is
visually inspected;82 balanced events/1,943,231 private PCM frames and all607
focused/app frozen source hashes/module/inventory/artifact guards pass. Final
audit: temp/m4cp-final-evidence-audit.json. All sessions awaited/no clients.
Commit M4cp, then continue angle/scale/Offset and remaining item3. Next private
author script is temp/author-m4cq.py; it is prepared, not executed or frozen.
Counts65 typed/24 fallback/10 stubs, five unconsumed draw families/nine partial
rows; no overall completion claim. Latest full integration remains M4cl.
M4co is33098c939. No pushes, agents, binaries or out-of-scope authoring.

M4co closes the bounded native alpha precision gaps. Read ALPHA-NORMALIZATION.md:
540 actual-GPU samples /1,620 exact RGB component comparisons,26 frozen actual
apps (270 indexed readings, two exact indexed masks,16 exact RGB crops including
six previous failures, five actor regression crops). GPU/core races pass. Stock
win/rematch/win is visually inspected;82 balanced events/1,987,918 private PCM
frames and all frozen hashes/module/inventory/app guards pass. Final audit:
temp/m4co-final-evidence-audit.json. Sessions awaited/no clients. M4co is33098c939;
continue Trans and remaining item3; latest full integration remains M4cl.
Render now has display-dependent GPU tests. No RGBA/platform/full-frame parity
claim follows. Preserve failed diagnostic logs and immutable native originals.

M4cn implements Explod entity binding, lazy animation availability, retained
retargets, motion orientation and owned age-limit clocks.62 programs/708 values/
three rejections/49 calibrated positions/249 unique hashes. Read
EXPLOD-ATTACHMENT.md. Focused236-program Match races126.003s and core races pass;
17 final actual-app masks are exact. The first mixed-profile app used the wrong
opponent definition and is diagnostic; m4cn2 corrected it, with both definition
hashes verified. No runtime/source/golden changed for that correction. Stock
win/rematch/win/82 balanced events/private PCM and all1,480 focused/app/native/
module/inventory guards pass. Sessions awaited/no clients. M4cn is18ff83fbd;
continue remaining creation/shared effects under item3. Next address shared
alpha precision/Trans and palette dependencies, including unresolved ownpal/
remap/random/angle/shadow/capacity fields. Counts64 typed/25 fallback/11 stubs;
latest full integration remains M4cl. Preserve all originals/failed diagnostics.

M4cl is c3e810d40. M4cm implements BindToTarget with73 native programs/792
readings/seven rejections/293 unique hashes. Read TARGET-SELF-BINDING.md:
immediate caller position, reverse combat constraint, retained/sentinel offsets,
game-pixel truncation and tested profiles. Focused194-program Match races125.315s
and core/System/CNS/Character races pass; legacy target glob was narrowed after
its discovery-only failure. Three exact white-pixel masks below HUD, stock
win/rematch/win/82 owned events/private PCM,1,095 final hashes and provenance/
module/inventory guards pass. All sessions awaited; no clients remain. M4cm is
committed as b72baadb9; continue remaining Explod/shared consumers and all item3. Latest full
integration remains M4cl.64 typed/25 fallback/11 stubs; no full parity claim.

M4ck is3d44c4ba. M4cl implements BindToParent/Root before remaining Explod creation
fields; BindToTarget follows with combat selection. Read SELF-BINDING.md:84
programs/936 values/six rejections/336 original hashes plus probe;14 friction
traces establish a fixed standing cutoff1/ignored threshold fields, float32
coefficients with retained velocity precision. Main+legacy helper/contact Match
races80.996s and two exact actual-window masks pass. Core/System/CNS races and
final full scoped races pass3,435 results/36 packages (Match1352.013s). Stock
win/rematch/win,82 balanced owned events/private PCM and all12,141 frozen
source/fixture hashes plus337 unique native hashes pass. No clients remain.
M4cl is committed as c3e810d40; continue BindToTarget. Native captures awaited; all
private native/app/display Go stays serial. Preserve failed logs and originals.
Source routing now63 typed/26 fallback,12 stubs. Continue item3; no new stop or
item4 allocation. Full integration after the dependent binding/Explod boundary.

M4ck implements measured ModifyExplod mutation and 2D/3D drawing. Read
MODIFY-EXPLOD.md:143 programs/1,164 readings/46 rejections plus three alpha grids/
36 readings,584 authored originals. Main Match/grid and full core/System races
pass.18 windows retain nine exact geometry masks and explicit seven rotation/
two alpha discrepancies. Shared alpha precision must be revisited with Trans;
do not claim full renderer/color parity or change goldens/tolerances to pass it.
Uniform-tint experiment failed and was reverted. Stock replay/82 events/private
PCM, all585 unique native hashes and guards pass. All89 names now register;
61 typed routes,14 stubs and partial consumers remain. Continue the remaining
Explod creation fields and dependent consumers, then other item3 groups.

M4ci is c9066e64. M4cj completes the bounded ExplodBindTime slice: 28 native
programs/288 readings/four rejections/28 calibrated positions, eight exact
actual-app masks, focused Match and full Engine/Effect/Character races pass.
All frozen inputs/application provenance/module/inventory guards pass. Read
EXPLOD-BINDING.md and HANDOFF.md. Continue ModifyExplod and full Explod dependencies
before the remaining item 3 groups. Source inventory now88 registered/60 typed;
VarSet literal-float validation is a newly found residual gap. Preserve its
unpromoted setup diagnostic unchanged. Private host restore uses temp Wine32
shims/dev headers; app harness temporary builds use `_tmp`. No clients remain.


## Current authorization

Implementation is authorized. The user approved `docs/PLAN.md` and authorized
local commits at each milestone or coherent checkpoint. Record findings,
validation, and remaining work with each checkpoint. Stage/commit only changes
under `game/mugen/`; preserve unrelated work and never push without instruction.

The user authorized completion of all 27 remaining documented HitDef keys
from the M4bm audit, with local checkpoint commits and continuous iteration
until complete or blocked. No two-checkpoint stop remains. Planned batches:
M4bn guard.dist (1), M4bo literal enums/flags (9), M4bp juggle/chains (3),
M4bq minimum/maximum/snap constraints (3), M4br hit palette effects (3),
M4bs contact shake (4), M4bt fall shake (4). Adjust boundaries if native
findings require it; document dependencies and actual behavioral limits.
Numeric expression routing and literal schema coverage are separate counts:
there are 54 expression keys initially, 18 remaining numeric and nine literal
keys. Do not force literal fields through numeric evaluation to report 81/81.
Acceptance for each batch requires typed schemas/defaults/errors, demonstrated
runtime consumers or measured beta limitations, frozen native controls,
focused regression checks and updated handoff/ledger. Use full scoped races
at the enum/combat integration boundary and final integration, broadening only
for failures or unresolved concerns. Commit each coherent batch locally;
continue automatically. Never push or add binaries. Other engine/mode/asset/
renderer gates remain outside this HitDef allocation unless a key depends on
them. All seven batches M4bn through M4bt passed their gates. The 27-key allocation
is complete: 72 numeric schemas plus nine literal schemas cover all 81 documented
HitDef keys, with bounded native consumers. The final full scoped race suite
passes (Match 942.741s), including post-M4bp corrections. All final native/app/
render/provenance checks pass. See FALL-SHAKE.md, HITDEF-KEYS.json and HANDOFF.md.
Whole-engine parity and broad interaction coverage remain unfinished. The
subsequent round-lifecycle allocation below is also complete. The current
trigger/clock allocation follows it. Prior checkpoints are 8779869a and 24b3ebd0; read HITFALLSET.md,
DOWN-BOUNCE.md and COVERAGE.md, including the corrected idle-override case.
Read `README.md`,
`docs/COVERAGE.md`, `docs/PROGRESS-ASSESSMENT.md` and the current handoff.

## Previous round-lifecycle allocation (complete)

The user approved three more checkpoints after M4bt (2026-10-18):
M4bu KO/time-over clocks (native slowdown, timer freezing, pause interactions,
control removal and win/lose entry); M4bv results/reset (double KO/draws, wins,
match completion, persistence and entity/effect cleanup); M4bw complete-match
validation (actual inputs/damage, multiple rounds, results/rematch, sustained
runs). Proceed through all three with local commits and documentation; stop
only at completion or a concrete issue requiring user input. Keep one-on-one
scope; other modes remain separate. The 27-key allocation above is historical.
Freeze original fixtures/recipes before native captures and never edit them
once observed. Use focused checks while developing and a full scoped race
suite at the final integration boundary. Private native/app runs stay serial.

M4bu is committed as 7fae8ec4; M4bv as ddb48b18. M4bw completes eight
full-life stock matches / 18 rounds through logical/keyboard input, including
natural KOs, time draws and repeated rematches. An owned-window 2%-life replay
also reaches two actual win/rematch screens. The final full scoped race suite
passes: 2,707 test/subtest results, 24 tested and 12 compiled-only packages;
Match 968.597s, total 1,037.599s. Source/config/binary/PNG/private-PCM audits and
root module checks pass; no clients/sessions remain. M4bw is committed as
218e3106. See docs/COMPLETE-MATCHES.md, docs/ROUND-RESULTS.md and docs/HANDOFF.md. Legacy HitVel binding, engine-wide
GameTime handoff and other documented engine gates remain open. Typed HitDef
coverage stays 81/81; no whole-engine completion percentage is justified.
No pushes or binaries.

## Previous trigger/clock allocation (complete)

The user validated M4bw and authorized proceeding. M4bx measures and binds
legacy HitVel X/Y, preserving the three existing counterexamples and adding
frozen native controls for units, heading, hit/guard/air/down contexts and
redirection. M4by separately wires cumulative GameTime through production
menus, matches and rematches. Each checkpoint requires focused regression
checks, updated behavioral limits/evidence and a local source/text commit.
Continue through these two bounded checkpoints unless user input is needed.
Earlier completed allocations are historical. No pushes or new binaries.

M4bx is committed as e1299891: 15 controls / 180 values / 60 frozen inputs, plus three
unchanged projectile counterexamples / 180 integers. Focused Match races
227.327s, Character/System/core races, two fresh native runs and a four-capture
stock input/results/rematch replay pass. Source/PNG/private-PCM/module/native
audits pass. See LEGACY-HITVEL.md and HANDOFF.md. M4by final integration below
also covers this checkpoint. Preserve all native originals and diagnostics.

M4by adds process-owned GameTime, native Config.GameSpeed + 5*Options.GameSpeed
rate, typed TicksPerSecond and bounded Versus MatchNo. Six programs/84 native
values/29 authored input hashes plus System/native-program/keyboard/wrap checks
pass. Final scoped integration PASSED against 408 frozen hashes: 2,735 test/
subtest results, 24 tested and 12 compiled-only packages; Match 956.321s, total
1,028.682s. Fresh native clock and four-PNG stock win/rematch/win with private
PCM pass source/input/config/PNG/executable/audio/root-module audits. All
sessions were awaited; no clients remain. M4by is committed as 0414d54e and was pushed by the user. This allocation is
complete; the current allocation below supersedes its stop condition. Keep renderer/mode/asset/remaining binding gates explicit; no
overall engine completion percentage is justified. No push or binaries.

## Latest binding/recovery allocation (complete)

The user authorized the next few checkpoints after 0414d54e (2026-10-18).
Proceed through three bounded checkpoints: M4bz audits all 127 trigger families
against actual runtime routes, records selector/variant gaps and adds a
reproducible inventory; M4ca implements missing GetHitVar combat metadata and
combo/impact counters with frozen native controls; M4cb implements the remaining
liedown recovery timer/input consumer and corrects recovery routing as supported
by native observations. Split dependent selectors between M4ca/M4cb if needed;
document the reason and keep the original inputs/goldens immutable. Each batch
gets focused checks, evidence/limits and a local source/text commit. Use a full
scoped race run at the final integration boundary, not at every small revision.
M4bz is bd018c65; M4ca is b258eaf3. M4cb completes this group. Final full scoped
races PASS: 2,774 results, 24 tested and 12 compiled-only packages, no individual
skips/failures; Match 1,009.698s, total 1,083.286s. All 470 frozen hashes, fresh
native reproductions, four-PNG stock win/rematch/win, owned events, private PCM
and root-module audits pass. All sessions were awaited; no clients remain.
Read LIEDOWN-RECOVERY.md and HANDOFF.md. Stop for user testing/new allocation;
no further implementation is active. Historical continuation instructions below
applied only until these three checkpoints completed.
Continue automatically through these three; stop at completion or an issue
requiring user input. No pushes, binaries, changes outside game/mugen or agents.

## Current scalar/projectile/reversal allocation

The user tested/pushed d251549d and on 2026-10-19 authorized items 1–2 of the
assessment, with checkpoints and local commits throughout. Proceed automatically
through HitPauseTime, Random/VarRandom, projectile lifecycle/cancellation/contact
queries, then ReversalDef/MoveReversed. Suggested coherent checkpoints: M4cc
HitPauseTime; M4cd random stream/controller; M4ce projectile schema/spawn/motion/
lifetime/ownership; M4cf multi-hit/removal/cancellation; M4cg projectile query
families and legacy forms; M4ch reversals. Split or reorder dependent slices if
native evidence requires it and document the actual boundary. Each gets frozen
native originals, focused checks, evidence/limits and a local source/text commit.
Use full scoped races at meaningful integration boundaries, including final
integration; do not repeat the entire suite for every small correction. Complete
these items without asking for routine approval. Stop after their final checkpoint
or a concrete issue needing user input. Items 3–4 (other controller/effect stubs,
assets/modes/AI/BGM and final renderer/font conformance) await later allocation;
record dependencies rather than silently expanding into those workstreams.
Keep projectile/reversal-specific consumers in scope, with honest variant limits.
No pushes, binaries, sub-agents or authored changes outside game/mugen/.

M4cc is committed as 750f0076. M4cd completes Random/VarRandom: 21 native
programs, 192 values, five rejections and 84 frozen inputs. Core/Match/57-chain
races, fresh seeded sequence/helper reproductions, four-PNG stock win/rematch/
win, 82 balanced owned events/private PCM, 436 frozen hashes and module audits
pass. Read RANDOM.md/HANDOFF.md. Continue M4ce automatically; 42 projspawn-*
programs / 168 hashes are frozen but not yet observed and not in M4cd's commit.
Move basic NumProjID counting into M4ce as needed for native lifetime controls;
keep the remaining query forms in M4cg. No clients/sessions remain at M4cd.

M4ce completes typed projectile setup/motion/lifetime/bounds/root ownership:
104 native programs, 1,192 distinct slots, nine load rejections, 416 authored
hashes and 12 sprite calibrations. Match races PASS 135.338s, core races pass;
fresh tagged native, three actual projectile apps/exact body masks, four-PNG
stock win/rematch/win with 82 owned events/private PCM, 819 frozen hashes and
module audits pass. Read PROJECTILES.md and HANDOFF.md. Continue M4cf contact/
multi-hit/removal/cancellation, including capacity/config and helper birth
ordering, then M4cg queries and M4ch reversals. No sessions/clients remain at
M4ce; no further implementation has started. Full suite at final integration.

M4ce is 4b3ab498. M4cf completes the coherent contact/cancellation/capacity
slice: 91 native programs, 1,118 distinct slots, 364 authored hashes; integrated
295-control Match races PASS 197.707s, end/stock/clock and full core checks pass.
Six actual phase masks, fresh native attrition, stock win/rematch/win with owned
input/private PCM, all 755 hashes and module/provenance audits pass. Read
PROJECTILE-CONTACTS.md. No sessions/clients remain at this checkpoint. Continue
a dependent coordinate checkpoint using eight captured projspace-* and three
unobserved/frozen projspace-draw-* programs; they are excluded from this commit.
Then M4cg event/legacy queries and M4ch reversals. Full suite at final integration.

M4cf contacts is40d1b25b. The dependent coordinate checkpoint completes11
programs/132 values/44 authored hashes and three exact actual-app masks. Full
core and selected Match races192.297s, fresh native payload, stock replay/PCM,
all355 frozen hashes and module guards pass. Read PROJECTILE-COORDINATES.md.
All sessions awaited/no clients. Continue M4cg with37 frozen/unobserved projevent
originals (148 hashes in m4cg-native-inputs/m4cg-extra-inputs.json), then M4ch.
Do not stage those originals in the coordinate commit. Final full scoped races
need a larger package timeout for corpus growth. No pushes/binaries/agents.

M4cf coordinate is2e6195de. M4cg completes51 event query programs/456 values/
13 rejections/204 original hashes. Core/selected Match races88.019s, fresh history,
stock replay/input/PCM and all556 hashes/module guards pass. Read
PROJECTILE-QUERIES.md/HANDOFF.md. No clients/sessions remain. Continue M4ch with
20 frozen unobserved controls in m4ch-final-native-inputs.json, then implement
reversals and final full scoped races30m. Preserve11 invalid drafts in place;
review rejected deleting them, so corrected -valid originals are separate. Do not
stage invalid drafts. Stop after item2; items3–4 remain unallocated.

M4cg is2899cd00. M4ch remains uncommitted. Read current HANDOFF/REVERSALS:
90 promoted programs,948 numeric+84 early clipboard readings, four rejections,
404 hashes/101 originals. Ten genuine Projectile controls prove the beta does
not reverse NA/NP/SP projectiles with idle/attacking owners. Mislabelled older
HitDef-based projectile controls are retained as ordinary diagnostics; their
projectile conclusions are withdrawn. First full run92446 was stopped/awaited
and is not acceptance. Do not reuse its source snapshot as final validation.
Latest corrected replay87 programs60.057s; ten genuine PS+type assertion8.223s.
Actual app3 and fresh full2/post2 gates remain before M4ch commit. Never edit
Go/tests/probes/fixtures during a frozen running validation. Stop after item2;
items3–4 await allocation. No agents/push/binaries/out-of-scope edits.

M4ch final gates PASS: 90 promoted programs / 948 numeric + 84 early clipboard
readings / four rejections / 404 original hashes; seven exact final app masks.
Full3 races: 3,169 results, 24 tested + 12 compiled-only packages, Match1228.212s,
total1298.980s, no failures/skips. All10,831 source/fixture/module and797 app hashes,
fresh genuine PS/spark, stock win/rematch/win/82 events/private PCM and101-run
provenance audits pass. No clients remain. First two full attempts are diagnostic,
not acceptance. Preserve originals in place and ignored build artifacts under
underscore names. Stop after this commit for user testing/3–4 decision.

## Scope

All project changes for this effort belong under `game/mugen/`. Do not modify
the root Go module, other engines, or existing data trees. Keep working notes,
compatibility evidence, and handoffs in `docs/`.
The explicitly requested upstream merge adds font requirements to root go.mod;
preserve those incoming changes. Continue authoring port changes only here.

The user explicitly prohibits adding binaries to the repository. Commit source,
text fixtures and documentation only; keep compiled executables, generated
binary artifacts and reference resources in ignored `temp/`. The tracked `mugen.sh`
file is a text Bash launcher. Existing binary test resources are historical;
do not add more binary files. Verify staged content before each local commit.

## Compatibility target

Target Elecbyte M.U.G.E.N 1.1 beta 1. The local reference distribution is
`temp/mugen-1.1b1/`: use its `docs/`, `data/`, `chars/`, and `stages/` as the
primary specification and regression corpus. The C++ implementation in
`src/mugen/` is useful prior art, but is incomplete and is not the specification.
Do not modify the reference distribution. Do not commit its binaries/assets.

## Implementation and verification

- On resumption, refresh a concise current behavior-coverage ledger from source,
  existing tests and topic documents before estimating broader work. The old
  inventory status columns remain baseline evidence. Keep percentage metrics
  dated, scoped and linked to their numerator/denominator sources.
- Work in bounded feature batches with explicit acceptance and stopping points.
  Use focused checks during development and full regression suites at meaningful
  integration boundaries; repeat broad checks only for a concrete reason.
  Record native evidence, unresolved differences and the next scope at each
  local checkpoint. Stop at the agreed boundary unless more batches are
  explicitly authorized; token-budget decisions belong to the user.
- A registered controller or a successfully parsed file does not demonstrate
  behavioral compatibility. Track partial and missing behavior honestly.
- Prefer deterministic, display-independent tests for expression and state
  semantics. Add corpus tests for the bundled stock content and malformed files.
- Errors must identify the file/state/controller/expression. Never treat an
  unknown controller or invalid trigger as a successful no-op.
- Exercise the complete selection-to-fight path as well as test applications.
- Run native captures, app smoke captures and audio probes sequentially on the
  private display/audio server. Overlapping windows can invalidate OpenGL
  captures, and other audio output invalidates tone provenance. Pure tests
  without DISPLAY may run alongside those probes.
- Run Go with `GOFLAGS=-mod=readonly` so the root module stays unchanged. Build
  artifacts and disposable tools belong in `temp/`, which is not source.
- Use the launcher or `-modfile=game/mugen/mugen.mod` for the scoped dependency
  overlay; see `docs/BUILD.md`. Keep it synchronized when dependencies change.
- Use non-Go extensions for disposable source snapshots. The recursive check
  also discovers ordinary packages below `temp/`; ignored files still compile.
- Full race checks use `--check -race -timeout=30m`; the growing native Match
  corpus approaches Go's default ten-minute package deadline.
- Update `docs/COMPATIBILITY.md` and `docs/HANDOFF.md` when behavior or verified
  coverage changes. Do not call the port complete without tests and reference
  evidence covering the advertised compatibility.

## Previous HANDOFF.md

# Current handoff

## Active item 3 allocation — 2026-11-04

The user selected complete remaining effects/controllers, not a two-checkpoint
stop. Continue through ITEM-3-PLAN.md with local commits and no agents/push/
binaries/out-of-scope edits. Earlier item 1–2 stop conditions are historical.
Item 4 still needs a separate allocation. M4ci (c9066e64) establishes
CONTROLLER-BINDINGS.md and its reproducible JSON/source audit. M4cj implements
ExplodBindTime with 28 native programs / 288 readings / four load rejections /
28 isolated effect positions and 112 pinned inputs. Read EXPLOD-BINDING.md.
Next continue the remaining Explod fields and shared controller/effect consumers.
Keep frozen native originals and
actual controller-kind assertions. Native/app/display Go runs stay serial.

M4cs COMMITTED cac8c6957 (M4cr c2e32e933). GLOBAL-PALETTE-FX.md records83
native programs/948 values/four rejects/333 hashes,10,352 GPU samples,71 exact
stage crops/2,795 colors and67 exact actor crops. FOUR owned-Explod overlaps
remain UNWAIVED126 pixels each, overall app RGB FALSE. Full integration PASS
3,857 results/36 packages25 tested+11 compiled-only, Match1606.364s, total1691.569s,
14,164 frozen inputs.10 regressions/visually inspected stock win/rematch/win82
owned inputs/1,941,379 PCM and all final source/native/artifact/module/inventory/
staged452-text-file checks pass. No binaries/push/outside changes. M4cs full
supersedes older full baselines;12 M4cq raster cases remain unwaived separately.

M4cu full third matrix11997 FINISHED/awaited, strict FALSE:66/68 exact; only
negative-duration foreground-under0/1 fail330,880 pixels each. All originals,
JSON/captures/tolerances unchanged. Separate idle guard passed. Entire native
68-scene layer table is now temp/m4cu-native-layer-table.json (native-only).
Correct rule: stage FG always draws; positive front quad then covers it;
positive under quad and all negative durations retain FG. Only base BG suppressed
whenTime!=0. Revised fight renderer moves front quad afterFG and before unowned
foreground cues, retaining their old relative FG order outside front EnvColor.
HUD Draw now always consumed for lazy clocks/resources, but positive EnvColor
masks final HUD fitting. No native expectation edits. New final m4cu4 matrix
starts via run-m4cu4-checks.py, prioritizing all5 FG controls then remaining68
accepted scenes. RerunSystem races; engine/character/189 native source inputs
remain unchanged. Frozen source m4cu4-check-inputs.json; no Go/test/probe/fixture
edits until session drains+separate guard. Then10 regressions+stock/privatePCM,
finalaudit/docs/local commit; continueALLitem3. Prep-m4cv.py notexecuted. No push/
binaries/agents/goals/outside authoring. M4ct a048effa3. Native input hashes305.

Workflow correction for future checkpoints: after freezing native captures,
produce and inspect an ALL-CASE native-only table of each consumer/layer before
implementation. Do not infer uninspected matrix branches from manual wording.
Run dependency/counterexample cases first, retain strict comparisons and gather
all RGB mismatches in a batch. Preserve every failed diagnostic/original.

M4cu second matrix9553 completed/awaited; separate idle guard passed. First26
scenes passed, then positive-under1 FOREGROUND case failed330,880 world pixels:
native preserves stage foreground for positive under1 despite the manual.
Positive under0 and all negative durations suppress it. Second diagnostic is
preserved unchanged. Fight renderer condition corrected; m4cu3 fresh all68 apps
plus System races now starts via run-m4cu3-checks.py. Native/code fixtures/
comparisons untouched. Collector keeps public probe nonzero failures and reports
all mismatches at the end, avoiding stopping at the first independent pixel
failure; no tolerance/expectation change. Final audit checks first core/native
inputs unchanged except fight renderer, plus new final-source manifest.
Await m4cu3 runtime and separate guard BEFORE edits/next runtime. Finish gates,
local checkpoint then ALLitem3 (EnvShake preparation not executed).

M4cu first validation68255 completed and separate guard passed. Core and189
native programs PASS (Match123.968s). First baseline/bottom full466,400-pixel
crops and samples pass; first boundary-blue crop has ZERO world RGB differences
but9 HUD samples fail: native positive EnvColor covers HUD even under1. Native
76 JSONs/captures unchanged. First run preserved diagnostic. System HUD fix
written and inventory refreshed; final m4cu2 run now starts via
run-m4cu2-checks.py. New input snapshot m4cu2-check-inputs.json; rerun System only
and all68 apps. Core/native source inputs unchanged except fight renderer;
final audit checks that explicitly instead of claiming a fresh full rerun.
Await final runtime+separate client guard before edits/other runtime. No frozen
Go/test/probe/fixture edits. Finish regression/stock/audit/docs/commit thenALLitem3.

M4cu implementation is WRITTEN; verification RUNNING session68255 via
run-m4cu-checks.py. All76 native controls promoted unchanged:68 accepted/816
values/eight rejects/305 unique hashes/2,516 native RGB points. Batch87036/13521/
70876 all awaited and separate idle guards passed. Native packing is uint32
r | g<<8 | b<<16 WITHOUT component masks; boundaries identify sign propagation
and channel carry, not clamping. RGB/time/under order23415; time1 default, negative
time stage suppression/black/no quad, positive time RGB overlay, AllPalFX first+
final pass, BG unaffected, ordinary/ontop/actor/projectile/foreground consumers.
Engine1.450s/Character11.338s/System9.430s core races PASS. Native189-program and
68 strict466,400-pixel apps pending. m4cu1-check-inputs.json freezes source/test/
probe/fixture/module; NO edits while session68255 is active. Await+separate guard
before edits/next runtime. Only MD/ignored private prep allowed during runs.
Continue final regression/stock/audio/audits/docs/local commit, then EnvShake and
ALL item3. prep-m4cv.py is prepared ONLY, not executed. M4ct a048effa3. No agents/
goals/push/binaries/outside authoring; native originals/captures/goldens immutable.

M4cu native first45(87036) and supplemental23(13521) batches completed/awaited,
with separate idle guards. Boundary8 controls/33 hashes are now frozen ONCE and
start via run-m4cu-boundary.py. Total76 programs/305 unique source+probe hashes.
No EnvColor Go changes or JSON promotion yet. Read ENV-COLOR.md and immutable
observations. RGB->time->under order23415; finite live overlays, negative-time
black stage/visible actors, global expiry through CNSPause, positive All/BG and
layer controls are measured. Channel-range conversion needs boundary controls.
Await new native batch and separate client guard BEFORE any Go/test/probe/fixture
edits/runtime. Original inputs/captures/goldens immutable, no tolerance changes.
Temp/promote-m4cu.py prepared for76; temp/prep-m4cv.py is prepared ONLY (not
executed): future EnvShake schemas and six-frame ScreenPosY traces. ContinueALL
item3 after checkpoints. M4ct a048effa3; no agents/goals/push/binaries/outside scope.

M4cu EnvColor is ACTIVE. temp/prep-m4cu.py executed ONCE after M4ct commit,
freezing45 EnvColor controls/181 original+probe hashes. First45 batch87036 completed/awaited; separate idle guard passed. Native
supplement23 cases/93 hashes starts via temp/run-m4cu-extra.py, after separately
named long-positive/live-negative controls were frozen ONCE. Total68 programs/
273 unique source/probe hashes. No Go implementation or JSON promotion yet. Do not edit Go/tests/probes/fixtures during frozen
native runs; await, then a separate idle-client guard before implementation or
another runtime. Original sources/captures/goldens stay immutable. Include
schema/evaluation, global lifetime, over/under, Explod/ontop/Projectile/foreground,
helper ownership and All/BG composition. Continue ALL item3 after this checkpoint.
Completed Go app asset copies now share identical immutable bytes (43,003 paths,
approximately5.06GB unique inode bytes released); paths/hashes preserved and
native/captures/goldens untouched. Log:private-go-asset-sharing.json. No push,
binaries, agents, goals or authored changes outside game/mugen/.

M4ct is COMMITTED a048effa3; all acceptance gates pass. Read docs/EFFECT-ORDERING.md
(or EFFECT-ORDERING.md from docs).16 native programs/192 readings/65 original+
probe hashes;16 exact151,800-pixel crops and all FOUR unchanged M4cs overlaps
fixed exactly. Core Character11.320/System9.465/Effect1.011 races,99 effect/global
native programs67.354s and14 actor-order programs10.476s pass. Four regression
apps and visually inspected stock win/rematch/win82 balanced inputs/1,965,839
private PCM frames pass. All source/native/resource/binary/config/PNG/audio/
module/inventory guards pass; no clients. Audit:m4ct-final-evidence-audit.json.
Latest full M4cs3,857/36 remains;12 M4cq raster cases unwaived. Continue ALL item3 after the local source/text checkpoint. Next guarded preparation temp/prep-m4cu.py is NOT
EXECUTED: EnvColor schemas/clocks/layer matrix. No original/capture/golden changes,
no tolerance relaxation. No item4/agents/goals/push/binaries/outside authoring.
Private Xvfb/Pulse are healthy; serial runtime and frozen-source policy remains.

M4cq implements bounded native angle/scale/Offset schemas/state/production
drawing. Read ANGLES-OFFSET.md for arithmetic, lifetime, coordinates, native
setup corrections and all raster limits.62 promoted programs pin696 readings/
four controller rejections.282 unique hashes include all source/probe/stock-
constant/empty-world inputs. Seven minimal-Data drafts remain unpromoted;
seven complete-constant replacements capture, followed by a driver metadata
KeyError on provenance kfm.cns. Iteration is corrected and the separate constants
audit verifies all seven without recapture. No original was changed or deleted.

Final apps m4cq4 use binary6015e0612326d1694408d8a27617511d1080792a4a3a43dc887e4e64ad36ec43.
46 of58 RGB crops match exactly (21,454,400 pixels);54 white masks are exact.
Twelve fractional/nonquarter raster counterexamples remain UNWAIVED. The strict
58-case runner exited1; this is not full application RGB conformance. See the
exact counts/maxima in ANGLES-OFFSET.md and m4cq4-app-audit.json. Positive and
mirrored quarter turns are exact after dominant-axis addressing and screen-space
flipped bias. The36-case GPU experiment reproduces old production and isolates
this correction. Worse inverse projection was reverted; m4cq1/2/3 are diagnostics.

Original62 JSON goldens/wider diagnostic regions remain immutable. Their broad
region includes native debug text and viewport-border sampling. A separate empty
native world/matching Go app calibrate ROI200,140..1080,670. Separate
actor-drawing-crops.json pins58 native-only crop hashes plus original JSON/PNG
hashes and calibration. No allowance changed. Public drawing_render_probe.py
passes quarters and correctly fails fractional RGB; --original-region retains
the broader diagnostic. The native resources also expose a BOM after leading
blank lines; Go now accepts it before the first section. Launcher uses _tmp.

FINAL FULL SCOPED RACES PASS3,703 test/subtest results:25 tested+11 compiled-only
packages, no failures/individual skips; Match1490.670s, summed1547.931s.36-package
preflight and all13,467 frozen inputs pass. Artifacts are m4cq-full-{packages.json,
inputs.json,race.jsonl,race.stderr.log}. The run integrates M4cm/M4cn/M4co/M4cp
and final M4cq sampling/BOM changes. It supersedes M4cl as completed full baseline.

Stock21/26/45s is visually inspected win/rematch/win (m4cq-stock-review.png and
original captures).82 balanced owned input events and1,995,181 private PCM frames,
all282 native hashes/full/app/resources/binary/config/PNG/PCM/module/inventory
hashes verify. Final audit is temp/m4cq-final-evidence-audit.json; it explicitly
records overall_application_rgb_conformance_passed=false for the12 known cases.
Full21196/stock20967 sessions awaited; separate client check empty. Only MD
changed after final passing source/app runs. Commit source/text under game/mugen/;
engine/drawing.go needs git add -f. No push, binaries, agents or goals.

Continue ALL item3 after the bounded checkpoint. The leading active block records M4cr native progress. Then remaining palette/global,
environment/afterimage/creation/dependent and residual controllers. Counts67 typed/
22 fallback/10 stubs/zero unread actor draw families/nine partial rows are not
an engine percentage. Item4 remains unallocated. M4cp acfdbb1dc/M4co33098c939.
Current private servers/wrapper: item3-private-servers.json / round-task-env.py.
Never overlap native/app/display Go or edit frozen Go/test/probe/fixture inputs.
All builds use temp/_tmp; preserve immutable originals and failed diagnostics.

M4cp implements typed Trans and the missing actor AIR transparency consumer.
Read TRANS.md.56 promoted native programs pin492 numeric readings/15 rejections/
630 native RGB samples.233 unique hashes preserve all originals/probe, including
two stale-publication transition diagnostics. A separate published target-state
snapshot corrects the setup without changing either original. All41 accepted
programs have exact actual-app RGB crops:108,675 pixels each/4,455,675 total.
Both source and destination alpha evaluate even in modes ignoring their values;
default preserves earlier overrides, and actor ownership is independent of
palette sharing. Issuing-frame state transition and frozen drawing are measured.

Core races Engine1.426s/Character11.134s/System9.376s/Render1.666s/Effect1.011s
pass;56-program Match races29.023s pass. Production apps use m4cp1 (five initial
cases) and m4cp2 (the remaining36); no source change occurred between them.
Final stock21/26/45s are visually inspected win/rematch/win, with82 balanced owned
input events and1,943,231 private PCM frames. All607 focused/app source hashes,
233 originals/probe hashes, source/config/binary/PNG/audio/module/inventory guards
pass. Final audit: temp/m4cp-final-evidence-audit.json. All sessions awaited/no
clients. Only source/text under game/mugen/ is authored/committed; no push.

Continue all item3: angle/scale/Offset next (private author-m4cq.py is prepared
but has not been executed), then palette/environment/afterimage/dependent
creation and other controller consumers. Counts65 typed/24 fallback/10 stubs,
five unconsumed actor draw families/nine partial rows. Wider RGBA/hitpause/
foreign-state/palette composition interactions remain explicit gates. Latest
full integration remains M4cl; use a full suite at the next drawing boundary.
Item4 remains unallocated. M4co is33098c939.

M4co closes the two M4ck indexed alpha failures and six old M4az RGB alpha crop
failures without changing native goldens, geometry or tolerances. Read
ALPHA-NORMALIZATION.md. Byte recovery/reciprocal normalization and distinct
complementary/independent blend paths reach both samplers and owned effects.
The GPU test passes540 samples/1,620 RGB components exactly (races1.809s).
System/Character/Background/Effect races9.239s/10.991s/1.034s/1.010s pass.
All26 final applications pass:270 indexed RGB readings, two exact8,379-pixel
masks,16 byte-exact RGB consumer crops and five exact actor regression crops.
Failed incomplete candidates remain diagnostic; native source/images are frozen.

Final stock21/26/45s are visually verified win/rematch/win.82 balanced owned
input events,1,987,918 private PCM frames, binary/config/PNG/audio/source/module/
inventory hashes and separate empty-client checks pass. Final audit:
temp/m4co-final-evidence-audit.json; app audit temp/m4co4-app-audit.json.
All sessions were awaited. Latest full integration remains M4cl (3,435 results,
24 tested+12 compiled-only packages); render now adds real display-dependent GPU
tests, so do not reuse those package categories for the next full suite.
Continue Trans/actor AIR transparency and the remaining item3 consumers. No
full renderer/alpha-channel/platform parity or item4 allocation is claimed.

M4cn implements the bounded Explod attachment/evaluation/motion/clock slice.
Read EXPLOD-ATTACHMENT.md:62 programs/708 readings/three rejections,49 exact
whole-pose calibrated root positions,248 originals plus probe hash. Native
manifests/metadata are m4cn-{native,extra,policy,clock,retarget}-*.json. All
captures and originals are immutable. Negative/default anim falls back to0;
missing resources skip later geometry. ID-1 binds caller (P2/helpers too), failed
retargets retain anchors, screen warns/ignores explicit binding while preserving
legacy behavior, bound displacement is suppressed while acceleration continues,
and owned move allowances are age limits. Source counts64 typed/25 fallback/
11 stubs; creation palette/random/angle/trans/shadow/capacity remain partial.

Engine/Effect races1.425s/1.010s;236-program Match races126.003s;
Character/System/CNS11.146s/9.246s/1.012s all pass.17 final actual-window masks
are exact, including mixed profiles and legacy screen behavior. Initial m4cn1
P2profile app accidentally launched KFM at320 instead of the native probeopponent
at1280; it is preserved diagnostic. m4cn2-explodattach-p2local1280 uses the correct
definition; final audit checks both selected DEF hashes for every accepted app.
No Go source or native observation changed. RGB/full-frame conformance remains
separate from these exact white masks. Corrected profile and stock21/26/45s were
visually inspected. Stock has82 balanced owned events and2,002,050 private PCM
frames. All249 native/1,480 focused/app/source/config/binary/PNG/PCM/module/
inventory guards pass. Final audit: temp/m4cn-final-evidence-audit.json.
All sessions awaited/no clients. Latest full suite remains M4cl.

Continue item3 without a new stop: shared alpha precision/Trans and palette
consumers, then remaining creation/dependent effects. Read-only supporting
native disassembly is temp/item3-reference-disassembly.txt; it corroborates,
not replaces, the native controls. Do not infer facing from a white-pixel count:
fractional sampling can produce the same count as an integer mirrored pose.
M4co now validates and consumes alpha normalization as documented above. The
prior uniform-tint tweak failed and remains reverted. Ordinary scene-driven cue move clocks retain their earlier
policy pending their own matrix; the new age policy applies only to owned
Explods. No whole-engine percentage or item4 allocation follows.

M4cl is c3e810d40. M4cm is b72baadb9 and adds BindToTarget with73 frozen programs/792 readings/
seven rejections/292 authored hashes plus the probe. Read TARGET-SELF-BINDING.md.
Its consumer moves the caller immediately and stores a reverse target constraint;
missing axes reuse retained acquisition offsets. Fractional/profile controls
pin game-pixel truncation. Match races pass125.315s across194 native programs.
Engine/System/CNS races pass1.434s/9.260s/1.012s; Character passes11.183s after
fixing an old target*.json glob to select only its original target-/targets-
corpora. The initial failure was discovery, not a changed native observation.
Three actual-window actor masks are exact below the HUD (8,379/7,455/3,474 pixels);
whole-region RGB differences remain, so this does not claim full framebuffer
conformance. All native originals/captures remain immutable. Current inventory
is64 typed/25 fallback/11 stubs. Final stock win/rematch/win is visually verified;82 balanced owned events and
1,971,901 private PCM frames,293 native/1,095 final source hashes and all app/
config/binary/PNG/audio/module/inventory guards pass. Sessions were awaited; a
separate client check is empty. Final audit is temp/m4cm-final-evidence-audit.json.
The latest full suite remains M4cl. Continue remaining Explod/shared effects.

M4cj is 6faff3f1. M4ck implements typed ModifyExplod, selected per-object mutation
and production scale/angle/perspective/transparency consumers. Its 143 controls
pin 1,164 values and 46 rejections; three alpha grids add 36 values and 27 alpha
pairings. There are 584 authored original hashes plus the probe hash. Read
MODIFY-EXPLOD.md, including the preserved duplicate-parameter drafts. Source
routes are now89/89 registered and61 typed, with14 stubs/nine partial consumers/
five unconsumed actor draw families. This is not full controller coverage.

The main mutation/prior-binding Match races pass85.278s; alpha grid/intent races
pass3.171s. Full Engine/Effect/Character/System races pass1.430s/1.011s/11.129s/
9.318s; Render compiles. Eighteen application windows demonstrate nine exact
nonrotated masks, seven explicitly nonexact rotation masks and two alpha colors
one unit below native. See the topic table; no tolerance/golden was changed.
The strict app audit intentionally reports the alpha failures. A uniform-tint
experiment did not improve them and was reverted. Shared alpha precision remains
for Trans; shadow/priority/pause/ownership/full Explod fields remain active item3
dependencies. Final platform sampling conformance remains item4.

Native manifests are temp/m4ck-{native,supp,sequence,check,policy,alpha}-inputs.json.
Native runs use m4ck-*; accepted geometry-window evidence uses m4ck3-*.
Preserve originals, failed builds/shader runs and the reverted m4ck4 experiment.
Native/app/display Go work remains serial; await sessions and check clients
separately. Full scoped races follow the Explod integration boundary.

M4ck final stock replay passes four PNG/config/binary/private-PCM checks with82
balanced owned key events,1,959,250 PCM frames and nonzero audio. All six native
manifests (590 total entries/585 unique hashes) and module/inventory guards pass.
Application runtime source inputs match the18-window run; the only subsequent
source changes add alpha-grid tests in two test files. Stock final source builds
with those tests present. All sessions were awaited; no clients remain.

M4ck is3d44c4ba. M4cl is c3e810d40 and implements BindToParent/BindToRoot constraints before the
remaining Explod creation fields; BindToTarget follows with combat selection.
This reorders dependencies, not allocation.84 programs/936 values/six rejections
and336 authored hashes pin schema, phase, facing, lifetime, retirement, pauses,
physics and14 friction traces. Read SELF-BINDING.md. The main+legacy helper/contact
Match races pass80.996s across121 native programs; two actual windows match every
native white pixel (7,935 each). Core/System/CNS races pass. Final full scoped races also pass3,435 results,
24 tested+12 compiled-only packages, no failures/individual skips; Match1352.013s
and summed package time1408.227s. Stock win/rematch/win at21/26/45s was visually
inspected;82 balanced owned events and2,010,533 private PCM frames pass. All337
unique native hashes,12,141 frozen full inputs, app source/config/binary/PNG/PCM
and root-module/inventory guards pass. All sessions awaited; no clients remain.
Final audit is temp/m4cl-final-evidence-audit.json. BindToTarget follows.

All native capture sessions were awaited. New self constraints share an exclusive
slot with combat binding but do not overwrite velocity or report combat isbound.
Standing physics uses the measured fixed cutoff1; the beta ignores documented
threshold fields. Coefficients use float32 precision while stored velocities
retain precision between multiplications. The earlier per-step velocity-rounding
attempt failed and is preserved; no native goldens/tolerances changed.
Manifests are m4cl-{native,extra,friction,friction2}-inputs.json; metadata is
separate. Application snapshot/audit names begin m4cl1. Preserve all originals.

The old private display/audio servers were absent. New task-owned Xvfb :97 and
private Pulse processes are recorded in temp/item3-private-servers.json; all
credentials/runtime logs remain under temp/. The display was verified with
xdpyinfo. Use round-task-env.py (GOTMPDIR=temp/_tmp), preserving module guards.
Task-local Wine32 shims and development headers restore the upgraded host without
changing system packages. The latest full suite is M4cl; use focused checks until the next meaningful
integration boundary.

M4cj acceptance: Match races 17.690s; Engine/Effect/Character races
1.448s/1.011s/11.333s. Eight final actual-app masks match every native white
pixel exactly, including left-facing and two-object cases; a two-object PNG was
visually inspected. All application source/config/binary/PNG/prepared-input
hashes, both native manifests (101 + 16 entries), root module guards and the
controller inventory pass. Artifacts: temp/m4cj3-app-{inputs,results,audit}.json,
temp/m4cj-native-mask-audit.json and m4cj-{match2,core}-race.log. All sessions were
awaited and a separate client check is empty. No push/binaries. Continue
ModifyExplod, then full Explod dependencies and the remaining item 3 groups.
The immutable VarSet setup diagnostic remains ignored in place and its schema
gap is explicitly open; valid binding replacement uses a separate name.


## Items 1–2 complete — paused for user testing/allocation

The user authorized assessment items 1–2 with local checkpoint commits. All
allocated work is complete; do not start items 3–4 without new authorization.
No agents, goals, running sessions or native/app/test clients remain. No push.
All authored changes remain under game/mugen/; no binaries are added.

Local checkpoints: M4cc HitPauseTime `750f0076`; M4cd shared Random/VarRandom
`ded53de8`; M4ce projectile setup/motion `4b3ab498`; M4cf contacts/cancellation/
capacity `40d1b25b`; foreign-state projectile coordinates `2e6195de`; M4cg event
queries `2899cd00`; M4ch reversals is the final checkpoint in this working tree.

Read REVERSALS.md and PROJECTILE-QUERIES.md. Source routing is 120/127 (94.5%),
with seven mode/stage/name families unbound. This is a source-route metric;
whole-engine compatibility and complete variant coverage remain unfinished.
All 34 beta-accepted GetHitVar selectors retain routes. Typed HitDef coverage
remains 81/81 keys (72 numeric and nine literal schemas), with bounded consumers.

M4ch adds typed required reversal masks/full HitDef reading, character Clsn1
reversal selection/consumption/persistence, MoveReversed, counters/targets,
GetPower, pauses/custom states and sparks. The beta leaves actual projectiles
unaffected in ten genuine NA/NP/SP controls with idle/attacking owners. A parsed
controller assertion prevents substitution of HitDef in those fixtures. Earlier
mislabelled projectile probes actually used HitDef; their projectile conclusions
were withdrawn and originals preserved. Seven renderer probes have 84 early
clipboard readings, separated from 948 final-variable readings. Ninety promoted
programs include four genuine load rejections; 404 hashes pin 101 originals,
including 11 preserved invalid drafts. See the topic note for all limitations.

Final full scoped races PASS: 3,169 test/subtest results, 24 tested and 12
compiled-only packages (36 full identities), no failures or individual skips.
Match 1,228.212s; total 1,298.980s. The command was:
`bash game/mugen/mugen.sh --check -race -p 1 -count=1 -timeout=30m -json`.
Only full3 is acceptance. Earlier interrupted attempts are retained diagnostics.
The private GOTMPDIR now uses temp/_tmp, preventing abandoned CGO build files
from becoming packages; the full preflight verifies 36 intended packages.

All final gates pass. Fresh genuine projectile and combined-spark captures
reproduce all original numbers and exact 7,935-pixel masks. Stock captures show
win/rematch/win at 21/26/45 seconds, visually inspected. All 82 owned key events
balance; private PCM, source/config/binary/PNG/input/audio provenance, all 10,831
full-suite hashes, 797 application hashes and root module guards pass. Native
prepared definitions/config/commands/eight resources pass all 101 authored runs.
Sessions 36806 and 12654 were awaited; a separate check finds no clients.
Final audit: temp/m4ch3-final-evidence-audit.json; post logs/results begin
m4ch3-post; final stock app is app-runs/m4ch2-stock-input. No originals, numerical
goldens or tolerances changed. No binaries or changes outside game/mugen/ are
committed. The allocated items 1–2 are complete. Stop before items 3–4.

Validation artifacts are ignored: m4ch3-final-inputs.json,
m4ch3-full-scoped-race{.jsonl,.stderr.log,-result.json}, m4ch3-full.log,
m4ch3-post-results.json, m4ch-native-audit.json, m4ch-provenance-audit.json,
m4ch3-phase-app-audit.json and m4ch3-final-evidence-audit.json. Fresh native runs
are reference-runs/m4ch2-fresh-{projectile,spark}. All seven actual app3 masks
match: four reversal sparks and three surviving genuine projectiles, each 7,935
pixels. The initial indexed-sampler mismatch is preserved; the effect path now
uses the existing measured sampler and retains blending.

Next decision: items 3–4 cover remaining controller/effect stubs and asset/mode/
AI/BGM/final renderer/font conformance. Mutual/multiple-contact and team/foreign/
resource/size/zoom variants remain honest gates; no whole-engine percentage or
full beta parity is claimed. Do not infer a new implementation allocation from
this list. The current checkpoint is ready for the user's play test and push.

All prior allocations below are historical; their stop condition is superseded
by the new user authorization. No agents/goals. No authored changes outside
game/mugen/. Preserve unrelated untracked files and root module baselines.

## Completed binding/recovery allocation: M4bz → M4ca → M4cb

All three authorized checkpoints are complete. M4bz is bd018c65; M4ca is
b258eaf3; M4cb is the final local checkpoint. Ready for the user's play test.
No agents/goals/sessions or native/app/test clients remain. Do not start another
feature batch without a new allocation. No push or binaries. All authored
changes remain under game/mugen/ and root module guards pass.

Read LIEDOWN-RECOVERY.md, GETHIT-METADATA.md and TRIGGER-BINDINGS.md. All 34
beta-accepted documented GetHitVar selectors now have routes; manual-only hitid
and zoff reject like the target. The 109/127 family-route count stays unchanged,
with 18 families and broader variants open. No whole-engine percentage follows.

M4ca: ten accepted native programs / 120 values and three rejections / 52 frozen
inputs. Received hit count counts contacts, fall count publishes after 5100's
first CNS pass, and KO-add fields are typed zero in measured target controls.
Focused Match races PASS 47.322s; Character/evaluator/engine races PASS
12.195/1.021/1.494s. Fresh native combo and all 348 source/probe/fixture/module
hashes/root guards passed at its checkpoint. M4cb integration supersedes that
source snapshot while preserving all native originals and goldens.

M4cb: 21 accepted programs / 252 scalar observations / 95 authored hashes.
Ten timers pin positive/zero/negative, state-number/type, I reset, death and
Pause behavior. Five normalized input programs check exact quarter reductions
and 36 balanced owned events. Six authored route controls add 12 events and
remove the old global recovery shortcut. CanRecover retains eligible fall data
after leaving H. The original omitted-required-constant rejection and raw
wall-time-dependent input programs remain ignored diagnostics, never acceptance.
The asset/schema gate for required Data constants remains explicit.

Focused Match races PASS 102.887s; Character/evaluator/engine/CNS races PASS
12.697/1.022/1.497/1.013s. Final full scoped races PASS: 2,774 test/subtest
results, 24 tested and 12 compiled-only packages, no individual skips/failures.
Match 1,009.698s; Character 12.063s; System 9.897s; Stage 1.040s; engine 1.488s;
evaluator 1.025s; CNS 1.013s; stock/resources 6.871s; full command 1,083.286s.
Command: bash game/mugen/mugen.sh --check -race -p 1 -count=1 -timeout=20m -json.
Exec session 78886 was awaited before any final native/application work.

All 470 frozen runtime/test/probe/fixture/module hashes pass after validation.
Fresh native Pause-timer, normalized taps and far-air recovery reproductions
pass exact originals (13.079 / 9.475 / 8.100s). The final stock input app passes
in 46.927s: four PNGs at 3/21/26/45s, with win/rematch/win visually inspected.
All 82 app events target one owned PID/window and release their keys. Private
PCM: 1,920,581 stereo frames, peak 32767, RMS 1656.66. Application binary SHA256:
ff752c91ba6ef1ab418f19c8b35082d01be4a8421e423e8c70ecbb6e973fb7d0.

The final audit passes all 34 programs / 372 scalar observations / three load
rejections, original and prepared CNS/CMD/AIR/stage/definition/config/executable,
exact PNG glyph/value/hash, frozen recipes, balanced events, app binary/PCM and
root modules. Only native FirstRun startup persistence and line endings are
normalized. No original, numeric golden or tolerance changed. All exec sessions
were awaited; a separate process check confirms no native/app/test clients.

Artifacts: temp/m4cb-final-inputs.json, m4cb-full-scoped-race{.jsonl,-result.json,
.stderr.log}, m4cb-final-focused-match.log, m4cb-final-core.log,
m4cb-post-validation.json, m4cb-final-audit.log, m4cb-final-evidence-audit.json,
m4cb-evidence-audit.json and m4cb-app-evidence-audit.json. Fresh runs:
reference-runs/m4cb-fresh-timer-pause, recovery-input-runs/m4cb-fresh-input-taps,
recovery-input-runs/m4cb-fresh-route-far, app-runs/m4cb-stock-input. Historical
native/diagnostic sources remain in reference-runs/m4ca-*, m4cb-*,
recovery-input-runs/m4cb-* and m4cb-raw-input-diagnostics.

Next suggested slices: remaining scalar/random bindings (HitPauseTime, Random),
then projectile lifecycle/query and reversal/entity work. Broader input/recovery,
required asset/constant schemas, modes/AI/BGM and strict renderer/font/platform
gates remain open. This completed group does not authorize all remaining work.
The prior trigger/clock allocation below is historical.

## Completed trigger/clock allocation

M4bx is committed as `e1299891`; M4by completes this allocation. Its final
scoped race suite PASSED and exec session 20080 was awaited: 2,735 passing
test/subtest results, 24 tested and 12 compiled-only packages, no individual
skips/failures. Match 956.321s; Character 11.667s; System 9.760s; Stage 1.039s;
engine 1.459s; stock/resources 6.677s; complete command 1,028.682s.
The final manifest is temp/m4by-final-inputs.json, 408 hashes. The command was
`bash game/mugen/mugen.sh --check -race -p 1 -count=1 -timeout=20m -json`;
runner/log/result names start m4by-full-scoped-race. All hashes pass after final
verification. No agents/goals/sessions or native/app/test clients remain.

M4by native evidence: engine-gametime runs two ordinary Versus entries in one
process, with first/current CNS 751/928 then 1655/1834. Five clockrate programs
pin Config/Options combinations 30/0=>30,75/0=>75,60/4=>80,30/4=>50,75/-3=>60,
including integer division/modulo and MatchNo=1. Six programs / 84 values /
29 frozen authored inputs. Engine owns GameTime across menus, matches, results
and rematches; timelines retain fresh pause/effect ownership. Tick rate now
drives both Ebiten and typed TicksPerSecond; MatchNo is bounded to Versus.
Read ENGINE-CLOCK.md. All native inputs/goldens stay unchanged.

Focused System races pass 10.119s. The native CNS program replays all 24
integers with real Engine title updates and production handoff, without setting
Match origins. Full-life keyboard flow checks real CNS reads across two rounds,
23 finished ticks, fresh rematch, 150 updates and exit/menu waiting; signed
wrap is covered. First diagnostics were an overwritten stock-5900 witness and
then unbound TicksPerSecond/MatchNo plus an assumed first-match blank tick;
both are retained, not acceptance. The initial standalone flow correction
passes 7.366s. M4by does not add a first-match blank initialization pass.

Post-race native reproduction passes, with first-CNS origins 755 and 1663 and
the same exact clock relationships. Four stock-input PNGs show win/rematch/win
at 21/26/45s, inspected alongside the fresh native reentry image. All 82 events
target one owned PID/window and release their keys. Private PCM has 1,930,666
stereo frames, peak 32767, RMS 1657.64. Binary SHA256:
e2f35feffbdee7f3ff0f3dfda0b039f46a669bb2aae690c5b7a0b22978db2c25.

Artifacts: temp/m4by-post-validation.json, m4by-final-{native,stock-app}.log,
reference-runs/m4by-fresh-engine-clock, app-runs/m4by-stock-input,
m4by-app-evidence-audit.json and m4by-final-evidence-audit.json. The final audit
checks every pre-native/final source hash, prepared source/config/bootstrap,
PNG glyph/value/hash, executable, owned events, PCM and root modules. It
preserves the config BOM and reverses only native FirstRun persistence and
line endings. No original input, golden or tolerance was changed.

M4by was committed locally and pushed by the user. No further implementation/validation remains in this bounded
allocation. Never push or add binaries. The next assessment priorities are a
complete remaining-runtime-binding inventory, then recovery/input and entity/
mode/audio slices; existing strict renderer/font/platform gates remain open.

The M4bx record below is historical and verified at its own commit.

The user validated M4bw (`218e3106`) and authorized proceeding through M4bx
legacy HitVel and M4by cumulative engine GameTime, with separate local commits.
Both checkpoints are verified. No push/binaries/agents.

M4bx: 15 new native controls / 180 values / 60 frozen CNS/CMD/AIR/stage inputs.
The three unchanged original projectile counterexamples also pass all 180
integers. HitVel X/Y returns game-space floats, current-facing X and signed Y;
guard/air/down, retained data, Enemy/foreign contexts and 400/700 fractional
profiles are measured. Canonical game-space hit storage and the HitVelSet
transfer retain precision without a receiver-local round trip. General physical
coordinate conformance and remaining trigger fields are separate gates. Read
LEGACY-HITVEL.md before extending this path.

Final Match races pass 227.327s (new controls, original counterexamples,
HitVelocity/ContactFacing/ForcedStanding/HitLethality/HitLiteral). Character
11.610s, System 7.072s, engine/evaluator/CNS/combat 1.453/1.022/1.012/1.011s.
Two fresh native reproductions pass. The rendered stock-input replay shows
two terminal wins and an intervening fresh fight, with four PNGs / 82 owned
window events; inspected 21/26/45s captures. Private PCM: 1,943,525 stereo
frames, peak 32767, RMS 1655.65. Binary SHA256:
68bf49d22cae070d5102cea6cfcc9da4dccbc2ad193b9f896adedc8fe8ebb166.

Artifacts: temp/m4bx-final4-inputs.json (357 source/fixture/probe/module hashes),
m4bx-final-validation.json, m4bx-evidence-audit.json, m4bx-app-evidence-audit.json,
m4bx-final-{core,combat,legacyhitvel-fraction,legacyhitvel-ground-mutated-valid,
stock-app,evidence}.log and app-runs/m4bx-stock-input. All sessions were awaited;
no UI/test clients remain. Root module/native hashes and provenance pass.
Three earlier failing Go runs and original cached/rejected fixture preparations
remain diagnostic, never acceptance. Do not rerun author scripts or change
observed originals. New inert-controller replacements have distinct filenames.

The round-lifecycle allocation below is historical and complete.

Updated 2026-10-18. The user authorized completion of all 27 HitDef keys
remaining after24b3ebd0, with local checkpoint commits and continuous iteration
until complete or blocked. No two-checkpoint pause remains. All authored work
stays under game/mugen/. Never push or add binaries. Current completed batches:
M4bn guard.dist, M4bo nine literal schemas, M4bp chains/juggle, M4bq
constraints, M4br palette, M4bs contact shake and M4bt stored fall shake.
The 27-key allocation and final integration are complete. See the current
assessment for remaining original-target work. The subsequent round-lifecycle
allocation below is also complete.
Read HITDEF-REMAINING.md, COVERAGE.md and the relevant topic documents.

## Previous round-lifecycle allocation (complete)

The user approved M4bu clocks, M4bv results/reset and M4bw complete input-driven
matches after bbba565e. Continue automatically with local commits, no push or
binaries. M4bu is committed as 7fae8ec4; M4bv as ddb48b18.
M4bw completes this authorized allocation. No agents or goal
are active. Read ROUND-LIFECYCLE.md and ROUND-RESULTS.md.

M4bv: 26 promoted native controls / 1,560 integers / 104 pre-native hashes.
Sixteen ordinary Versus cases cover win limits, draws, time wins, MatchOver,
perfect outcomes and rolling roundnotover deadlines. Seven recovering late
projectile cases pin provisional-win-to-DKO and the damage cutoff. Two self-KO
cases pin Alive latching and 5030 entry. One reset case pins persistent power,
integer/float ranges, entity/effect cleanup, root IDs and stale helper handles.
Stage resetBG is implemented and tested against the bundled specification.

Three additional preserved counterexamples expose an unbound legacy HitVel Y
trigger: native -24 versus Go bottom/0, with all other 59 integers agreeing per
case. They do not establish a physical-knockback difference. See the text inputs
and goldens in tests/testdata/reference/known-divergences. The existing trigger/
projectile gate stays open. Do not silently promote registration or key counts.

Final focused races pass: Match 71.953s includes all 26 new controls, the 29 old
clock controls, the corrected idle HitOverride regression and stock KO/time/
pause/sweep/reset checks. Full Character/System/Stage 11.504/4.694/1.039s; core
engine/evaluator/combat/CNS 1.452/1.020/1.011/1.012s. The first integrated run
failed one unnecessary fall flag; the exact old golden passes after its removal.
The independent HitVel diagnostic fails exactly Var(22) and is not acceptance.

Three fresh native reproductions and three apps/six captures pass provenance;
all three final application captures were inspected. Private stock PCM is
nonzero (300,249 stereo frames, peak 32768). Source manifest:
temp/m4bv-final-production-inputs.json, 277 hashes. Audit:
temp/m4bv-final-evidence-audit.json. Binary:
f12ca932c50bdc7e4a2b845a26f21b47f0d5038fc1dc74e6e25b9d14a31418a7.
Native/root-module hashes pass. All sessions were awaited; no UI clients remain.

Native originals remain frozen. The manifests m4bv-prepared-{versus,edges,
perfect-witness,holds,wins-valid,persistence-v2,lifeko,lifeko-latch,
projectile-recover,final-boundaries}.json contain the promoted input hashes.
The last manifest also contains archived malformed wins drafts. Do not rerun
author scripts. m4bv-unpromoted-diagnostics preserves 29 Quick-VS/missing-spawn/
held-hit/rejected-max programs; other initial range, empty-helper and malformed
first-field inputs have separate archive directories and logs. Preparation's
header-separator fix retains 43 earlier preparation hashes; all 1,740 old clock
values and 420 initial Versus values retain exact glyph decoding. FirstRun and
line-ending startup changes are audited; fresh menu origins can differ, but
only explicit timestamp slots are aligned and all event durations stay exact.

M4bv is committed as ddb48b18. M4bw test allocation is verified in
final integration: match/complete_match_test.go completes eight matches
and 18 rounds with full life and actual walk/punch damage, both winners,
alternating wins, time draws and three repeated rematches. system/flow_test.go
now wins two rounds through the configured keyboard instead of injecting life
zero. Maximum pre-over is 70 combat passes / 45 time-draw passes; no stock KO
uses the 900-pass fallback. Focused Match cause checks pass 16.153s, the initial
full keyboard flow passes 6.351s including race overhead.

The rendered input replay uses the optional app_smoke.py --stock-input flag,
2% life and one required win in a disposable copy. Accepted run:
temp/app-runs/m4bw-stock-input-one-hit. Viewed captures show terminal win/rematch
at 21s, a fresh fight at 26s and a second terminal win at 45s. Four PNGs, 82 events
on one owned PID/window and private PCM pass source/config/fight/binary/output
audit. No pointer/focus/global input operations. Initial held-forward and later
20%-life runs remain diagnostic; they did not demonstrate complete matches.
Final source manifest temp/m4bw-final3-production-inputs.json pins 278 files;
all 154 production Go files are unchanged from M4bv. App audit:
temp/m4bw-app-evidence-audit.json. Binary:
8bd3896ddb04b376a4e80ca510a15b51d0320b3ce4b77fce643e32970775f034.
PCM: 1,953,229 frames, peak 32767, RMS 1647.98.

The final full scoped race suite PASSED and exec session 53987 was awaited.
There are 2,707 passing test/subtest results, 24 tested and 12 compiled-only
packages, no skipped individual tests or failures. Match 968.597s, Character
11.496s, System 7.234s, Stage 1.041s, engine 1.460s and stock/resource tests
6.738s. The complete command takes 1,037.599s. The progress monitor shortened
package names, conflating production/test-app character and stage packages;
final counts use all 36 full identities. Earlier 34-package progress figures
were monitor summaries, not the final package count.

Logs/results: temp/m4bw-full-scoped-race.jsonl, its stderr/result files and
m4bw-final-evidence-audit.json. All 278 source hashes and root module checks pass
after the run. App audit/visual review and private PCM pass. No UI/test clients
or sessions remain. Finalize documentation and commit M4bw locally, then report
the completed allocation for user testing; never push or add binaries. No more
validation is needed unless source changes or an unresolved failure requires it.

Current explicit gates include unbound legacy HitVel and engine-wide GameTime
origin/handoff across menus/rematches: native GameTime includes menus, while
new isolated Matches own local timelines. Origin-aligned scalar replays and
fresh-object reset assertions do not prove that engine integration. Other
original controller/recovery/projectile/team/mode/AI/BGM/asset/font/render gates
remain unwaived. See COMPLETE-MATCHES.md and PROGRESS-ASSESSMENT.md.

## M4bq verified checkpoint

Predecessor/reversal b13c22d5.39 programs/588 observations/seven rejections/156
frozen hashes. Topic HIT-CONSTRAINTS.md describes strict schemas, the undocumented
third snap duration, repeated source/default order, deferred offsets, immediate
binding, profiles/facing and entry-write/contact-axis phase. Numeric routing
61/81 plus nine literals =70/81 typed keys (86.4%); eleven effect keys remain.

All39 Match replays pass23.971s. Related binding-retirement/contact-facing/
down-bounce/stock-sweep regressions pass68.873s. Full Character11.508s and core
engine/evaluator/CNS/combat1.457/1.021/1.013/1.014s pass. Two fresh native
reproductions and three apps/four captures pass source/config/binary/PNG/runtime
provenance in m4bq-evidence-audit.json. Stock and indefinite-snap captures were
visually inspected. The stock preview was losslessly reencoded because the
image viewer showed black for a valid repeated PNG; its decoded RGB pixels
were verified identical. Captured originals/goldens are untouched.
run-m4bq-validation.py exited0. Nine Go hashes are frozen in
m4bq-production-inputs.json. Application SHA256:
125738f2800b6207e00036e0814ff47749f97df1471a273d897caefac32866a6.
Manifests: m4bq-prepared-{inputs,extra,routing,offset-profile,entry-routing}.json.
Native values/tolerances never changed; do not rerun authors. No full suite at
this batch: final scoped integration follows M4bt. Stage only M4bq families;
continue automatically, no push or binaries.

## M4br verified checkpoint

Predecessor/reversal572f2eb6.64 numeric + nine literals =73/81 typed keys
(90.1%); eight shake keys remain.24 hitpalette-* programs pin324 observations,
three native rejections and96 pre-native source hashes. Manifests:
m4br-prepared-inputs.json, m4br-prepared-lifetime.json and
m4br-prepared-negative.json.19 static accepted programs have12 native-only
color samples and a zero portrait-delta measurement each. Numeric values,
source inputs, existing body points/colors and tolerances were never changed.
Original body metrics remain in m4br-body-color-expectations-before-portrait.json.

HIT-PALETTE.md describes strict triples/coercion/order/defaults, signed
add-then-multiply byte rounding, time0/<-1 preservation and time-1 indefinite
behavior, guard/None exclusion, shared palette clocks, image transforms,
revision/cache/clone behavior and separate body/portrait rendering. Standalone
PalFX/AllPalFX/BGPalFX remain stubs, not promoted by these HitDef consumers.

Numeric Match24 races pass16.085s; engine/evaluator/CNS/combat/palette races
1.469/1.020/1.012/1.011/8.570s pass. The initial shared-clock test missed its
idle Null controller; test-only correction passed full Character11.422s.
First20 apps matched all body colors, but visual review discovered that Go
also tinted the HUD portrait while native did not. That earlier body-only run
is NOT final proof. Its artifacts/audit are retained under original *-m4br
names. The corrected State.MappedImage/Player HUD path and separate cache roles
are covered by a regression and nineteen native-relative portrait comparisons.

Final full Character races pass11.454s; palette recheck8.602s passes. Two fresh
native reproductions and20 rebuilt apps/21 captures pass provenance. All19
programs match228 exact body colors and retain unchanged portraits. Stock and
representative palette captures were visually inspected (lossless PNG previews
work around the image-viewer repeated-PNG display bug; original pixels/goldens
are unchanged). Final source/config/binary/PNG/scalar audit:
temp/m4br-final-evidence-audit.json.14 source hashes (13 Go plus Python) are
frozen in m4br-final-production-inputs.json. Application SHA256:
dc3b2b1da89c8d6b42a9f51f76a397f15f87c144aae3518f418376b81b859e92.
run-m4br-portrait-validation.py exited0. New apps/reports use *-m4br-final;
original buggy captures remain diagnostics. Root module/executable checksums
pass. Final full scoped races follow M4bt. Commit only M4br family; continue.

## M4bs verified checkpoint

Predecessor/reversal e1833f2a. Numeric routing68/81 + nine literals =77/81
 typed keys (95.1%); four stored-fall keys remain. HIT-SHAKE.md records the
strict literal/runtime types, float amplitude, default/order rules, asymmetric
base320 rounding, issuing-frame duration, shared clock, profile conversion,
ScreenPosY, world-only drawing and round reset. Standalone EnvShake is still
unfinished. No native source, numeric value or tolerance changed.

26 programs/348 observations/three load rejections/104 pre-native hashes pass.
All26 native Match races pass17.581s; engine/evaluator/CNS/combat races
1.497/1.020/1.012/1.011s pass. Full Character races11.362s and related Match
regressions17.542s pass. Two fresh native reproductions pass, including the
mixed half-rounding profile. An initial fresh replay omitted its pinned
localcoord recipe and was rejected by the provenance gate; the corrected new
run passed. Failed recipe artifacts remain diagnostic; no golden changed.

Seventeen native-only rigid render measurements each pin7,935 garment pixels.
All17 rebuilt production app comparisons match exact shapes/displacements.
Including stock,18 apps/19 captures pass input/config/binary/PNG/runtime
provenance. Stock and positive-shake captures were visually inspected; the
stock preview was losslessly reencoded with identical RGB pixels to avoid the
image viewer's repeated-PNG problem. Native/Go images remain ignored.

Manifests: m4bs-prepared-inputs.json, m4bs-prepared-lifetime.json,
m4bs-prepared-runtime-profile.json. Production154 hashes (all153 production
Go files plus the effect probe) are frozen in m4bs-production-inputs.json.
Final audit: m4bs-evidence-audit.json. Application SHA256:
c9f19f9cb7ac6a41a2656f864dd94be7b08b9f1934f3e51203916b80bd93aad8.
run-m4bs-validation-remaining.py exited0; prior successful test/native stages
are in m4bs-validation.log. Root module checksums pass. The contact family was committed as ead79c52;
M4bt final integration is recorded below. No push or new binary.

## M4bt verified final checkpoint

Predecessor/reversal ead79c52. All 27 allocated keys are complete. Numeric
routing 72/81 plus nine literals gives 81/81 typed keys (100%). The source audit
against the immutable local manual is docs/HITDEF-KEYS.json. FALL-SHAKE.md
records the four schemas, contact-before-fall tagged order, receiver-local
payload, integer time/amplitude and floating frequency/phase getters, guard
zeroing, positive-duration consumption, metadata retention and fall=0 consumer.
Standalone EnvShake remains unfinished. Native inputs/goldens are unchanged.

22 programs pin 336 observations, three load rejections and 88 pre-native hashes.
Manifests: m4bt-prepared-inputs.json, m4bt-prepared-lifetime.json and
m4bt-prepared-joint.json. All 48 contact/fall Match races pass in 32.032s. Core
engine/evaluator/CNS/combat races pass in 1.477/1.020/1.013/1.011s. A receiver-local
fractional-amplitude controller regression and typed getter tests pass.

The FINAL full scoped race suite PASSED with -p 1 -count=1 -timeout=20m:
Match 942.741s, Character 11.918s, engine 1.466s, evaluator 1.022s, CNS 1.013s,
combat 1.012s, palette 8.341s, System 4.808s, storyboard 1.792s, stock/resources
6.936s. No test was skipped. This covers post-M4bp lifetime/query corrections
and all later batches, including real stock mixed sweeps, KO/reset and
pause/reset scenarios. Log: temp/m4bt-full-race.log. Prior full runs remain
historically distinguished. Existing external alpha/spark failures are unwaived.

All 269 source/test/probe hashes are frozen in m4bt-production-inputs.json and
remain unchanged. Native runtime and fall=0 fresh reproductions pass. All 16
native-only rigid render comparisons match the exact displacement/shape of
7,935 garment pixels each. Including stock, 17 apps/18 captures pass prepared
input/config/binary/PNG/runtime provenance. Representative fall and stock
captures were visually inspected. The stock preview was losslessly reencoded
with identical RGB pixels to avoid the image-viewer repeated-PNG problem.
Original captures and goldens remain unchanged and ignored.

Runner run-m4bt-validation.py exited0; all stages are in m4bt-validation.log.
Final audit: temp/m4bt-evidence-audit.json. Application SHA256:
1503cd01ddaefef8881484f34b5058880c7d3d3419168f28ff80b2c37a6fd43f.
Root module and immutable native executable hashes pass. No GUI client remains.

The full seven-batch allocation has 248 native programs, 3,144 numeric
observations, 36 expected load rejections and 992 pre-native source hashes.
It includes 19 palette color/portrait, 17 contact-shake and 16 fall-shake render
comparisons. This completes the scoped 27-key allocation, not whole-engine
compatibility. PROGRESS-ASSESSMENT.md and COVERAGE.md preserve remaining
recovery/round/entity/controller/mode/audio/asset/renderer gates. No next code
batch is active. All checkpoint commits are local; no push or new binary.

## M4bp verified checkpoint

Predecessor/reversal ff7a3296. Numeric routing58/81 (71.6%), plus nine literal
schemas =67/81 typed keys (82.7%). Fourteen documented keys remain. This is not
whole-engine completion. HIT-CHAINS-JUGGLE.md describes the precise scope.

Adds strict integer chainID/nochainID/air.juggle schemas, measured order and
coercion, prior-contact ownership/ID, guard/None/idle context, per-move and
per-projectile juggle costs, non-fall budget preservation and the typed chainid
query. The beta rejects GetHitVar(hitid), despite its manual's argument-summary
typo. GetHitVar now validates literal selector names before state effects.
An authored idle contact destination clears fall; later StateTypeSet retains
an existing flag. A narrow single-contact projectile adapter supplies the
necessary air.juggle consumer: native names, active animation/HitDef, removal
time, owner target credit and movement without another CNS/world update.
Full projectile schemas, cancellation, removal animations, queries and broader
entity/foreign/pause combinations remain open.

57 hitcombo-* programs pin660 observations, eight load rejections and228
pre-native source hashes. Manifests under ignored temp/:
- m4bp-prepared-inputs.json: use34 non-order rows.
- m4bp-prepared-extra.json: use six non-states rows.
- m4bp-prepared-final-states.json: three clean state-juggle controls.
- m4bp-prepared-final-order.json: two independent corrected order controls.
- m4bp-prepared-nochain-eval.json: two component-order controls.
- m4bp-prepared-nofall.json: four lifetime controls.
- m4bp-prepared-hitid.json: three native selector rejections.
- m4bp-prepared-zero-id.json: one valid zero-ID chain.
- m4bp-prepared-chainid-query.json: two query controls.

Do not rerun author scripts or derive expected values from Go. Two misplaced
snapshot-marker drafts remain archived/unpromoted in temp/m4bp-draft-order.
Three duplicate-state drafts were archived BEFORE native execution under
temp/m4bp-draft-states. Native JSON values/tolerances were never changed.

The enum/combat full scoped race suite PASSED before the final lifetime/query
extension: Match805.949s, Character11.753s, System4.790s, storyboard1.834s,
stock/resources6.734s; engine/evaluator/CNS/combat1.449/1.021/1.012/1.011s.
Log temp/m4bp-full-race.log; runner run-m4bp-validation.py exited0. This run
also covers the earlier M4bm idle-override correction. Eight source hashes
are retained in m4bp-production-inputs.json; this is the earlier source snapshot,
not the final tree.

Final lifetime/query correction gates PASS:144 Match replays90.507s (57 chain,
65 literal,22 HitFallSet), full Character11.481s and core races
1.445/1.021/1.013/1.012s. The first lifetime replay caught the missing typed
chainid dispatch; a later core test used a host without CharacterEnvironment
and was moved to a proper evaluator fixture. Diagnostic logs remain
m4bp-lifetime-first-* and m4bp-testhost-*. No production change followed the
passing Match/Character checks; only that test fixture changed. Completion
runner run-m4bp-complete-validation.py exited0, including core tests, two fresh
native reproductions and two rebuilt apps/three captures. Eleven final Go
hashes are pinned in m4bp-final-production-inputs.json. Final audit:
m4bp-final-evidence-audit.json. Application SHA256:
a97c3fd5eb5abc5e9b7405e767f314b632dda17a077e050116c732b22638085f.
The final stock7s capture was visually inspected and is byte-identical to the
previously inspected capture979e93044acd31accda05ba0439be525bb20ea39f8c9b4709e0e40ed399e1049.
Root module and immutable native executable checksums pass. Final full scoped
races follow M4bt, covering the lifetime/query changes and remaining fields.
Do not relabel the earlier full run as a final-tree run.

## Frozen inputs

All palette/contact/fall source families have independent native captures.
Never regenerate them or derive expectations from Go. All seven batches through M4bt are
checkpointed; all source families are immutable. Only scalar source/text fixtures are
committed. The effect probe can record from native only and pins source,
prepared character/opponent/stage/HUD resources, binaries, profile and PNGs.

## Earlier checkpoints and remaining gates

M4bn7d30ae85: guard.dist strict integer; omitted/negative range preservation,
zero, facing/edge/height/profile and state-reset consumers.15 native programs,
204 observations/four rejections/60 hashes. Focused Match41.153s, full Character
12.156s, core/native/apps passed. GUARD-DISTANCE.md.

M4bo ff7a3296: nine literal schemas, stored contact animation, None contacts,
HitDefAttr and trip defence entry.65 programs/684 observations/eight rejections/
260 hashes. Focused Match55.619s; broader selection/flags/forced-standing/stock
sweeps97.325s, Character11.772s, core/native/apps passed. HIT-LITERALS.md.

Prior checkpoints24b3ebd0 downed-hit bounce and8779869a HitFallSet are recorded
in DOWN-BOUNCE.md and HITFALLSET.md. M4bm's full run had one diagnosed idle-
override failure; its targeted correction passed and M4bp's later full suite
now passes that integration. Preserve historical failed logs as diagnostics.

Unwaived: six M4az RGB alpha precision cases (maximum1), two M4bd spark raster
failures (276 native core pixels versus288 Go, maximum8), absolute KO/round
clocks, complete recovery/liedown/mashing/input routes, broad helpers/foreign/
projectile/reversal/team/pause combinations, modes/AI/BGM and full assets/fonts/
camera/effects. Absent Windows fonts still use a visible portable fallback;
no native glyph/metric parity is claimed. The full engine is not complete.

## Scope and environment

Branch go-rewrite-mugen; local commits only, preserve unrelated root files.
No agents, skills or goal tool active. Sandbox bwrap startup is broken; scoped
require_escalated commands pass automatic review. No approval blocker.
Do not touch the host display/audio/keyboard/focus.

Private display DISPLAY=:97,
XAUTHORITY=/home/juvinious/projects/paintown/game/mugen/temp/virtual-display/Xauthority,
LIBGL_ALWAYS_SOFTWARE=1. Xvfb PID2146490. Private Pulse PID1393643:
PULSE_SERVER=unix:/home/juvinious/projects/paintown/game/mugen/temp/audio-reference/native;
PULSE_COOKIE points to that directory's cookie; ALSA_CONFIG_PATH to its alsa.conf.
Never print cookies. Native/app/audio/GUI tests run sequentially. Await the prior
session, then separately check ps -eo pid,stat,comm for mugen.exe/mugen-go and
match/character/system/tests/storyboard test clients. Pure engine/evaluator/
CNS/combat tests without DISPLAY may run alongside native captures.

Build caches: GOCACHE=.../game/mugen/temp/go-build,
GOTMPDIR=.../game/mugen/temp/tmp, GOPROXY=off,
GOFLAGS='-modfile=/home/juvinious/projects/paintown/game/mugen/mugen.mod -mod=readonly'.
Full suite: game/mugen/mugen.sh --check -race -p 1 -count=1 -timeout=20m.
Use non-Go suffixes for disposable source snapshots: recursive tests discover
ignored .go files too. Root checksum baseline:
temp/upstream-post-merge-module.sha256. Native executable SHA256:
d5508f77f2a86f12358e669f91c64ba5e4a789d4b71262e8a758814384303a4e.
