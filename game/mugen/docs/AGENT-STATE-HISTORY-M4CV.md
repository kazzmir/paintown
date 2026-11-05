# Historical M4cv working notes

Superseded snapshots, including disproven early hypotheses. Not active instructions.

## AGENTS snapshots

# MUGEN Go port — active instructions

## Authorization and scope

The user selected **complete remaining effects/controllers (item3)** on
2026-11-04 and repeatedly authorized continuation. Continue autonomously through
ITEM-3-PLAN.md with documented local checkpoints and commits. Do not stop after
two checkpoints. Stop after item3 or a concrete issue needing user input. Item4
(modes/teams/AI/BGM, assets, remaining triggers and final renderer/font/platform
conformance) remains unallocated. Earlier allocation stop conditions are history.

Author changes only under `game/mugen/`. No pushes, binaries, subagents or goals.
Preserve unrelated root work and incoming upstream font/module changes. Commit
source/text only; audit staged paths, UTF-8 content and absence of binary data.
The tracked `mugen.sh` is a Bash launcher. Reference resources and build outputs
remain ignored. New engine files may require explicit `git add -f` because of a
historical root ignore rule; never force-add the whole tree or temp assets.

Target Elecbyte **M.U.G.E.N1.1 beta1**, using immutable `temp/mugen-1.1b1/docs/`
and its stock data as primary reference. C++ `src/mugen/` is incomplete prior art.
Use the scoped mugen.mod/mugen.sum overlay and readonly module flags. Verify
`sha256sum -c game/mugen/temp/upstream-post-merge-module.sha256`.

## Immediate state — read HANDOFF.md

Latest committed checkpoints: **M4cu b8654fca3** (EnvColor),
**M4ct a048effa3**, native actor/owned-effect
ordering. All16 new strict crops and four original M4cs overlaps pass unchanged
goldens; no remaining M4cs ordering waiver. Earlier M4cq12 fractional/nonquarter
raster failures remain unwaived separately.

**M4cv EnvShake is ACTIVE: native43-program batch starting.**
Temp/prep-m4cv.py executed ONCE after M4cu commit and an idle guard, freezing43
programs/173 original+probe hashes in m4cv-native-inputs/metadata.json. Run via
run-m4cv-native.py; await its exec session, then a separate client guard BEFORE
Go/test/probe/fixture edits or another runtime. No EnvShake Go implementation or
JSON promotion yet. Only MD/ignored independent prep during the frozen run.
Inspect an ALL-CASE native-only table of traces/defaults/order/types/profile/
pause/helper behavior BEFORE implementation. Do not infer standalone behavior
from the earlier HitDef/FallEnvShake consumer or manual alone. Then typed schema,
actual consumer, focused/native/render/regression/stock/audit, local commit and
full integration at this environment boundary. Continue ALL remaining item3.

M4cu b8654fca3 acceptance:76 native programs/68 accepted/816 values/eight rejects/
305 hashes;68 exact466,400-pixel crops (31,715,200 pixels)/2,516 RGB samples.
Core and189 native races123.968s, final System9.365s, ten regressions and visually
inspected stock win/rematch/win82 balanced events/1,972,229 PCM frames pass. All
final source/native/resource/config/binary/PNG/audio/module/inventory guards
pass;402 staged UTF-8 source/text paths audited, no binaries/outside changes or
push. All sessions awaited. Audit:m4cu-final-evidence-audit.json.
Current71 typed/18 fallback/6 stubs/9 partial/14 bounded/60 route-only is a source
audit, not engine completion. Latest full remains M4cs3,857/36;12 M4cq raster
counterexamples remain unwaived. See HANDOFF and current topic notes.

## Native and validation workflow

1. Freeze separately named original fixtures/recipes before native observation.
   **Never edit/delete/move original native inputs, captures or goldens**, including
   failed diagnostics. Never relax comparisons/tolerances to make a test pass.
2. Inspect an **all-case native-only consumer/layer table before implementation**.
   Do not infer uninspected matrix branches from manual prose. Native beta quirks
   can differ from its manual. Check actual controller kinds per state.
3. Implement typed schemas/defaults/errors, evaluation order and real production
   consumers. Unknown controllers/invalid triggers must not silently succeed.
   Keep file/state/controller/expression context in diagnostics.
4. Test counterexamples/dependencies first. Collect every strict RGB mismatch
   in a batch, retain each public probe's failure status, and fail the complete
   matrix on any mismatch. Never describe a partially passing matrix as exact.
5. Use focused races/apps during revisions and full scoped races at meaningful
   integration boundaries and final allocation closure. Run required checks;
   broaden/repeat only for changes/failures/unresolved concerns. Full races use
   `--check -race -p 1 -count=1 -timeout=40m`.
6. Freeze source/test/probe/fixture/module inputs for each runtime run. **Native,
   actual-app and display-dependent Go runs are strictly serial.** Await every
   exec session, then a separate client guard before edits/another runtime.
   Never edit frozen inputs or the active driver while a run is executing.
7. Record findings, failures, exact limits, tests and remaining work at each local
   checkpoint. Update README, COMPATIBILITY, COVERAGE and HANDOFF as relevant.
   Full stock selection/fight/results/rematch and required private audio remain
   acceptance gates. Source routing and behavioral evidence are separate counts.

## Tools and private environment

Use `rg` / `rg --files --no-ignore` to discover source paths before reading them;
root ignores hide some engine files. Disposable Go packages/builds belong under
`temp/_tmp` (underscore), preventing recursive package discovery. No ordinary Go
experiments anywhere else under temp. Keep the Go module readonly.

Private runtime/Git wrapper from repository root:
`python3 game/mugen/temp/round-task-env.py ...` (elevated tool execution uses the
already authorized wrapper prefix). It checks clients and sets private display,
audio, Go cache/_tmp, scoped modules, Wine32 shims and local dev headers/libs.
Xvfb:97/Pulse are task-owned; metadata is temp/item3-private-servers.json. Check
selective process output before inferring a server died; full listings truncate.
Do not print private audio cookies. Do not restart/kill unrelated user services.

Byte-identical completed private Go app binaries and stock SFF/SND/font copies
may share storage with their paths/content/hash/permissions preserved; no native
resources, screenshots or expectations are candidates. Prior record:
temp/private-go-asset-sharing.json. Keep all diagnostics and evidence intact.

## Remaining allocation and historical record

EnvShake; AfterImage/AfterImageTime and projectile trails/shadows; remaining
Explod creation/ownership/remapping/random/angle/trans/shadow/capacity; all
AssertSpecial consumers; GameMakeAnim/MakeDust; clipboard formatting; force
feedback and actual target/device boundary; residual typed/literal schemas,
HitAdd HUD, StateTypeSet and FVar aliases. `-nojoy` alone cannot prove a hardware
limitation. VarSet literal7.9 must reject natively; fix conflicting old unit
inputs to7.9+0.0 expressions without changing native goldens.

Read current HANDOFF and topic notes. Old detailed status is preserved in
`docs/AGENT-STATE-HISTORY-M4CU.md` for historical lookup only; its old stop
conditions are superseded. No engine-completion percentage is currently justified.


Native batch session65107 remains active (first43 programs/173 hashes). Early
observations:missing time ACCEPTED (no shake);order1234 time/ampl/phase/freq even
zero/negative;negative frequencies preserved, unlike HitDef;default amplitude
raw-4 author units (profiles640=>404,1280=>804). Pause/SuperPause traces need
compare against their explicit+4 amplitudes, not default-4:GameTime advances3
extra ticks and wave phase advances globally. Do not misread matching sign-flipped
traces as a frozen shake clock. Source/JSON promotion has NOT begun.
Temp/prep-m4cv-extra.py is prepared ONLY:11 controls discriminate runtime amp
fractions at phase90, asymmetric half rounding, phase0.5 withamp90, zero/negative
minimum-frequency behavior withamp100000, and profile runtime negatives. The
original phase30/runtime4.75 case cannot distinguish truncation from retaining
fractions; preserve it and add separate controls after65107 drains/idle guard.
Then inspect all native cases before implementation. No Go/test/probe/fixture
edits during active runs, no expected-data/tolerance changes.

M4cv first43 native batch65107 FINISHED/awaited and separate idle guard passed.
Six native rejections;missing-time ACCEPTED with zero shake. Nonpositive standalone
requests CANCEL an earlier pending shake (replace0/neg1), unlike HitDef's ignore
policy. Ordertime/ampl/phase/freq1234 eveninactive. Defaultamplraw-4author-local,
negative frequency unbounded. Do not clamp it likeHitDef. Extra14 controls/57
hashes now frozenONCE via prep-m4cv-extra.py: runtime/half/phase/frequency/profile
counterexamples plus same-frame time1/2 and active-cancel exposure. Total57
programs/229 unique original+probe hashes. New native batch run-m4cv-extra.py is
starting; await its execsession then separate guard BEFORE Go/test/probe/fixture
edits/other runtime. No EnvShake implementation or JSON promotion. All-case
native-only table required before implementation; originals/tolerances immutable.

M4cv extra14 batch21338 FINISHED/awaited; separate idle guard passed. Important
corrections before implementation:runtime amplitude fractions persist;half+2.5
roundsScreenPos197 andhalf-2.5=>202. Zero frequency behaves0.001, negativefrequency
phase0/high amplitude indicates ABS frequency, not signed retention; phase90
alone cannot distinguish signs. Existing active shake survives a time0 request,
although a zero request overwrites an earlier same-frame queued positive. Source
no implementation yet. Same-frame time2 QueryVar6 remains200 thennextpass196;
time1 no shake. Clock11 new controls/45 hashes frozenONCE:active1/negative/queued
zero/negative/missingtime, negative default-phase/signed-phase and four EnvColor/
AllPalFX composition scenes. Total68 programs/273 hashes. Native run via
run-m4cv-clock.py starting; await thenseparate guard BEFORE Go/test/probe/fixture
edits/other runtime. All-case table-m4cv-native.py prepared for68, NOTEXECUTED.
Native original/PNG/JSON/tolerance immutability remains. No JSON promotion yet.

M4cv clock11 batch49307 FINISHED/awaited; separate idle guard passed. Active1
replaces current shake at END frame (Var6sameframe196,nextpass200). Zero/negative/
missing clears earlier same-tickqueued request but leaves CURRENT active shake
unchanged. Missing time therefore default0. Negativefreq normalizedABS BEFORE
phase default (freq-90 omittedphase starts90). Negativephase retained. ExtraFreq0
withamp100000 matches.001; phase0negativefreq showsABS, correcting earlier signed
assumption. Native normal/debug world shaking DOES expose BLACK viewport gaps
(no unshakenpink fill); do not change that. PositiveEnvColor overlays stayfixed
includinggapareas. EnvColor+All viewportgap composition needs two final controls.
Layer2/9hashes frozenONCE, run-m4cv-layer.py starts. Total70 programs/281 original+
probe hashes. Await its session+separate guard before Go/tests/probes/fixtures/
other runtime. NoGo/JSONpromotion yet. Then execute table-m4cv-native.py ALL70
and inspect entire table before implementation. All earlier assumptions above
are superseded by these discriminating controls; originals remain untouched.

M4cv layer2 batch82206 FINISHED/awaited; separate guard passed. Native viewport
hole colors measured:normalnoAll black;AllFX-only hole138,225,255 whileinside
99,90,248;negativeEnv+All hole/inside138,225,255;positiveEnv fixed12,34,56 across
hole/inside;positiveEnv+All fixed59,35,111. Existing whole-world shake is correct
for normal/debugfill (black holes); add fixed viewport backdrop only for positive
EnvColor and rawblack+finalAll hole coverage. Do NOT unshake debugpink. NoGo yet.
Two final tiny-frequency±0.0001/amp100000 controls frozenONCE (9hashes), test
minimum magnitude vszero-only special case. Total72 programs/289 source+probe
hashes; run-m4cv-min.py starts. Await new execsession then separate guard before
source/Go/tests/probes/fixture edits/other runtime. Execute table-m4cv-native.py
ALL72 before implementation. JSON promotion not begun; originals immutable.

M4cv all native72 batches65107/21338/49307/82206/64771 FINISHED/awaited; separate
client guards passed. All-case native-only table EXECUTED and all72 inspected
(m4cv-native-behavior-table.json),289 original/probe hashes. JSON promotion
completed:66 accepted/792 numeric values/six rejects;45 fixed393,040-pixel RGB
crops/1,755 samples,21 dynamic-phase diagnostics without static goldens. Exact
zero frequency=>.001; NONZERO tiny±.0001 survive magnitude normalization. Normalize
abs before omitted-phase90 threshold. Defaulttime0/defaultampraw-4author units.
Negative/zero/missing clearsPENDINGonly, preservesCURRENT;time1queues0and cancels
CURRENTatENDframe. Same-frame getters seeoldshake. Runtimeamplfractions retained.
Go implementation+tests/probe WRITTEN. Newtyped compiler, Character scaling,
RequestEnvironmentShake clearsqueuedthenusesexistingRequestShake; normalHitDef
RequestShake unchanged. Renderer fixed viewport backdrop covers positiveEnvColor
and rawblack+finalAll gaps; normal/debug shakingstill exposesblack. No unshaken
pink. cns/hit_shake.go max(.001,freq) stillneedsseparate native negative/tiny audit;
doNOT silently changepriorHitDefgoldens or infer standalone equivalence.
Now first verification run-m4cv-checks.py starts:core, nativeShake/EnvColor/global/
ordering and45 staticactualapps, compositioncases first. Sourcefreeze
m4cv1-check-inputs.json; await newexecsession+separate idle guard BEFORE edits/
other runtime. Only MD/independentignoredprep duringfrozenrun. Thenregression,
stock/privatePCM/audit, FULLenvironment integration boundary, localcommit and
continueALLitem3. No push/binaries/agents/goals/outside authoring.

M4cv first verification session4295 ACTIVE. Core PASS Engine1.439s/Character
11.293s/System9.417s. Native targeted Shake/EnvColor/global/ordering and45 actual
static apps still running; composition first. Sourcefreeze:m4cv1-check-inputs.json.
No source/probe/test/fixture edits or another runtime until4295 drains+separate
idle guard. Full integration script run-m4cv-full.py prepared ONLY, not executed;
uses40m package deadline as actual M4cs full script did, with expanded corpus.
No full-suite claim until it actually passes and frozen inputs verify.

CURRENT M4cv revision: first runtime4295 FINISHED/awaited, separate guard passed.
All controller/native checks PASS;45 app matrix38 exact/seven640/1280profile
failures. Profiles expose existing framebuffer interpolation plus HUD AIR blend
omission. Separate native viewport calibration capture exists (cropped947px;
clipboard extraction incomplete); retain as diagnostic, not promoted evidence.
Its separately frozen4sources/probe are unchanged. Idle guard passed. HUD AIR
blend consumer implemented with shared ApplyBlend; no sampler/golden changes.
Revision2 session34597 ACTIVE: System race and all45apps, retaining every strict
failure. m4cv2-check-inputs.json frozen. No Go/test/probe/fixture edits or new
runtime until34597 drains and separate client guard. First three profiles now
have only one-channel HUD precision errors (old opacity error fixed), world
crops still252/876diffs. Renderer conformance gate remains UNWAIVED. Do not claim
45exact. Next regressions/full integration/stock/privateaudio/audit/localcommit,
then continue allitem3. run-m4cv-regressions.py prepared ONLY; source edits must
precede any future runtime source freeze. Native originals remain immutable.

## HANDOFF snapshots

# Current handoff — M4cv active

The user authorized continuing **all item3 effects/controllers**, with local
checkpoint commits and documentation; no two-checkpoint stop remains. Item4
is unallocated. No pushes, binaries, agents, goals or authoring outside
`game/mugen/`. Current constraints are in ../AGENTS.md. Historical working notes
are archived in AGENT-STATE-HISTORY-M4CU.md; they are not current instructions.

## Immediate state

**M4cu is COMMITTED b8654fca3; M4cv EnvShake native batch starts now.**
Temp/prep-m4cv.py executed ONCE after commit/idle guard, freezing43 original
programs and173 source/probe hashes. Native driver:temp/run-m4cv-native.py;
manifests:m4cv-native-inputs/metadata.json. Await its exec session, then run a
separate idle-client guard before any Go/test/probe/fixture edits or another
runtime. No EnvShake Go implementation or JSON promotion yet. Only Markdown/
ignored independent preparation can change while frozen runtime is active.

43 cases cover six-frame ScreenPosY traces;required time and errors, float
schema/runtime conversion, active/zero/negative evaluation order, default-phase
frequency evaluation count, replacement/cancellation, Pause/SuperPause,640/1280
profiles with explicit/default amplitudes and helpers. First produce and inspect
an ALL-CASE native-only behavior table, then implement. Previous HitDef shake
consumer is a candidate dependency, not evidence of standalone equivalence.
Native originals/captures/goldens/tolerances stay immutable. After focused/native/
actual-render/regression/stock/audit acceptance, full environment integration,
local commit and continue ALLitem3. Current source inventory unchanged.

M4cu final68 m4cu4 applications pass31,715,200 exact RGB pixels/2,516 samples.
Ten old ordering/palette regressions and visually inspected stock win/rematch/win
pass,82 balanced events/1,972,229 private PCM frames. Final audit:
temp/m4cu-final-evidence-audit.json. All source/native/resource/binary/config/PNG/
audio/module/inventory guards pass.402 staged UTF-8 source/text files; no binaries/
outside changes/push. Sessions57289/44293/46817/23233/99214/67266 were awaited;
separate client guards pass. Failed m4cu1/2/3 diagnostics/native expectations
remain unchanged. Last binary:ba9e4b130873405e7ad44bfb4138489c79310a7c24c0eb6c199c6b4a89cf5d31.

## EnvColor implementation/evidence

Read ENV-COLOR.md.76 native programs:68 accepted,816 numeric values,eight load
rejects,305 unique source/probe hashes,2,516 RGB samples. Native manifests:
`m4cu-{native,extra,boundary}-inputs/metadata.json`; captures:`m4cu-envcolor-*`.
All three native batches87036/13521/70876 were awaited with separate guards.
Originals/captures/goldens remain immutable. `m4cu-native-layer-table.json`
inspects every accepted native consumer before the final layer revision.

Typed `engine/env_color.go` evaluates RGB -> time -> under (tag23415); defaults
white/time1/under0. Unmasked uint32 packing r|g<<8|b<<16 propagates carry/sign
bits, not clamping. Integer literals reject floats, runtime conversion truncates,
missing redirects become zero. Global lifetime continues through CNSPause;
issuing frame remains visible; zero cancels and negatives persist.

Base stage is suppressed for nonzero time. Negative durations draw no quad,
retain stage foreground/actors/HUD and expose raw black (only final AllPalFX).
Positive under draws the GPU color before actors, retaining stage foreground;
positive front draws it after stage foreground and before foreground effects.
Ontop remains above it. Positive quads receive source+final AllPalFX passes;
BGPalFX does not affect them. Positive EnvColor covers the sampled HUD for either
under flag. HUD Draw still runs offscreen to preserve lazy clocks/resources;
final HUD fitting is masked instead. No full HUD-layer/round/format conformance.

Passing core:Engine1.450/Character11.338/System9.430s.189 selected native programs
pass123.968s. Only system/fight_state.go changed after that core/native run;
final audit verifies controller/timeline/replay identity explicitly. Subsequent
System races pass; final9.365s. Preserve diagnostic m4cu1 (world exact/HUD fail),
m4cu2 (positive-under foreground fail),m4cu3 (66/68 exact; two negative foreground
fails). All original comparisons are unchanged. M4cu4 tests all five foreground
cases first, then remaining accepted programs on the same build.

## Latest committed / integration boundary

M4ct **a048effa3**: interleaved native actor/owned-effect priority ordering.
16 native programs/192 values/65 original+probe hashes;16 exact151,800-pixel
crops (2,428,800 pixels), all four unchanged M4cs overlap crops fixed. Core,
99 effect/global and14 actor-order native races, four palette/raster regression
apps and visually inspected stock win/rematch/win82 balanced events/1,965,839
PCM frames pass. Audit:m4ct-final-evidence-audit.json. Staged97 UTF-8 source/text
paths; no binaries/outside changes or push. Read EFFECT-ORDERING.md.

Latest **full** integration:M4cs cac8c6957,3,857 results/36 packages (25 tested,
11 compiled-only),Match1606.364s,total1691.569s,14,164 frozen inputs. It predates
M4ct/M4cu; do not relabel it as a current full run. M4cs palette/GPU evidence is
in GLOBAL-PALETTE-FX.md; its four ordering failures are now closed by M4ct.
The12 M4cq fractional/nonquarter raster counterexamples remain unwaived separately
(ANGLES-OFFSET.md). Current source audit:71 typed/18 fallback,6 stubs,9 partial,
14 bounded,60 route-only; no family variant-complete or overall engine percentage.

## After the EnvShake checkpoint

M4cv is the current allocation step described above; its43 originals are frozen.
Complete the environment integration with a fresh full scoped race suite after
focused semantics and actual drawing pass. Preserve all originals and diagnostics.

Then continue remaining creation/palette/random/shadow/capacity/afterimage/
special/debug/dust/haptics/schema consumers in ITEM-3-PLAN.md. Native VarSet
literal-float rejection and alias/version gaps are explicitly queued. Broader
round/foreign/pause/hidden-HUD/system-spark combinations remain interaction gates.

## Private host and storage

Use `temp/round-task-env.py` via the authorized elevated prefix. DISPLAY:97,
private Xauthority/software GL/Pulse/ALSA, scoped modules readonly, Go cache and
`temp/_tmp`, local Wine32 shims and eight dev-header packages are configured.
Servers metadata:item3-private-servers.json. No private cookie output. Runtime
and source mutation stay serial. Module guard:upstream-post-merge-module.sha256.

Completed private Go app assets share43,003 identical paths (about5.06GB unique
inode storage saved), preserving bytes/hash/permissions and leaving native files,
captures and goldens untouched. Record:private-go-asset-sharing.json. Do not
rerun that script without a new output-name preflight; its original log exists.
Private app binaries are also shared by equal verified hashes. No binaries are
tracked. Preserve unrelated root files/modules/untracked work.


Native batch session65107 remains active (first43 programs/173 hashes). Early
observations:missing time ACCEPTED (no shake);order1234 time/ampl/phase/freq even
zero/negative;negative frequencies preserved, unlike HitDef;default amplitude
raw-4 author units (profiles640=>404,1280=>804). Pause/SuperPause traces need
compare against their explicit+4 amplitudes, not default-4:GameTime advances3
extra ticks and wave phase advances globally. Do not misread matching sign-flipped
traces as a frozen shake clock. Source/JSON promotion has NOT begun.
Temp/prep-m4cv-extra.py is prepared ONLY:11 controls discriminate runtime amp
fractions at phase90, asymmetric half rounding, phase0.5 withamp90, zero/negative
minimum-frequency behavior withamp100000, and profile runtime negatives. The
original phase30/runtime4.75 case cannot distinguish truncation from retaining
fractions; preserve it and add separate controls after65107 drains/idle guard.
Then inspect all native cases before implementation. No Go/test/probe/fixture
edits during active runs, no expected-data/tolerance changes.

M4cv first43 native batch65107 FINISHED/awaited and separate idle guard passed.
Six native rejections;missing-time ACCEPTED with zero shake. Nonpositive standalone
requests CANCEL an earlier pending shake (replace0/neg1), unlike HitDef's ignore
policy. Ordertime/ampl/phase/freq1234 eveninactive. Defaultamplraw-4author-local,
negative frequency unbounded. Do not clamp it likeHitDef. Extra14 controls/57
hashes now frozenONCE via prep-m4cv-extra.py: runtime/half/phase/frequency/profile
counterexamples plus same-frame time1/2 and active-cancel exposure. Total57
programs/229 unique original+probe hashes. New native batch run-m4cv-extra.py is
starting; await its execsession then separate guard BEFORE Go/test/probe/fixture
edits/other runtime. No EnvShake implementation or JSON promotion. All-case
native-only table required before implementation; originals/tolerances immutable.

M4cv extra14 batch21338 FINISHED/awaited; separate idle guard passed. Important
corrections before implementation:runtime amplitude fractions persist;half+2.5
roundsScreenPos197 andhalf-2.5=>202. Zero frequency behaves0.001, negativefrequency
phase0/high amplitude indicates ABS frequency, not signed retention; phase90
alone cannot distinguish signs. Existing active shake survives a time0 request,
although a zero request overwrites an earlier same-frame queued positive. Source
no implementation yet. Same-frame time2 QueryVar6 remains200 thennextpass196;
time1 no shake. Clock11 new controls/45 hashes frozenONCE:active1/negative/queued
zero/negative/missingtime, negative default-phase/signed-phase and four EnvColor/
AllPalFX composition scenes. Total68 programs/273 hashes. Native run via
run-m4cv-clock.py starting; await thenseparate guard BEFORE Go/test/probe/fixture
edits/other runtime. All-case table-m4cv-native.py prepared for68, NOTEXECUTED.
Native original/PNG/JSON/tolerance immutability remains. No JSON promotion yet.

M4cv clock11 batch49307 FINISHED/awaited; separate idle guard passed. Active1
replaces current shake at END frame (Var6sameframe196,nextpass200). Zero/negative/
missing clears earlier same-tickqueued request but leaves CURRENT active shake
unchanged. Missing time therefore default0. Negativefreq normalizedABS BEFORE
phase default (freq-90 omittedphase starts90). Negativephase retained. ExtraFreq0
withamp100000 matches.001; phase0negativefreq showsABS, correcting earlier signed
assumption. Native normal/debug world shaking DOES expose BLACK viewport gaps
(no unshakenpink fill); do not change that. PositiveEnvColor overlays stayfixed
includinggapareas. EnvColor+All viewportgap composition needs two final controls.
Layer2/9hashes frozenONCE, run-m4cv-layer.py starts. Total70 programs/281 original+
probe hashes. Await its session+separate guard before Go/tests/probes/fixtures/
other runtime. NoGo/JSONpromotion yet. Then execute table-m4cv-native.py ALL70
and inspect entire table before implementation. All earlier assumptions above
are superseded by these discriminating controls; originals remain untouched.

M4cv layer2 batch82206 FINISHED/awaited; separate guard passed. Native viewport
hole colors measured:normalnoAll black;AllFX-only hole138,225,255 whileinside
99,90,248;negativeEnv+All hole/inside138,225,255;positiveEnv fixed12,34,56 across
hole/inside;positiveEnv+All fixed59,35,111. Existing whole-world shake is correct
for normal/debugfill (black holes); add fixed viewport backdrop only for positive
EnvColor and rawblack+finalAll hole coverage. Do NOT unshake debugpink. NoGo yet.
Two final tiny-frequency±0.0001/amp100000 controls frozenONCE (9hashes), test
minimum magnitude vszero-only special case. Total72 programs/289 source+probe
hashes; run-m4cv-min.py starts. Await new execsession then separate guard before
source/Go/tests/probes/fixture edits/other runtime. Execute table-m4cv-native.py
ALL72 before implementation. JSON promotion not begun; originals immutable.

M4cv all native72 batches65107/21338/49307/82206/64771 FINISHED/awaited; separate
client guards passed. All-case native-only table EXECUTED and all72 inspected
(m4cv-native-behavior-table.json),289 original/probe hashes. JSON promotion
completed:66 accepted/792 numeric values/six rejects;45 fixed393,040-pixel RGB
crops/1,755 samples,21 dynamic-phase diagnostics without static goldens. Exact
zero frequency=>.001; NONZERO tiny±.0001 survive magnitude normalization. Normalize
abs before omitted-phase90 threshold. Defaulttime0/defaultampraw-4author units.
Negative/zero/missing clearsPENDINGonly, preservesCURRENT;time1queues0and cancels
CURRENTatENDframe. Same-frame getters seeoldshake. Runtimeamplfractions retained.
Go implementation+tests/probe WRITTEN. Newtyped compiler, Character scaling,
RequestEnvironmentShake clearsqueuedthenusesexistingRequestShake; normalHitDef
RequestShake unchanged. Renderer fixed viewport backdrop covers positiveEnvColor
and rawblack+finalAll gaps; normal/debug shakingstill exposesblack. No unshaken
pink. cns/hit_shake.go max(.001,freq) stillneedsseparate native negative/tiny audit;
doNOT silently changepriorHitDefgoldens or infer standalone equivalence.
Now first verification run-m4cv-checks.py starts:core, nativeShake/EnvColor/global/
ordering and45 staticactualapps, compositioncases first. Sourcefreeze
m4cv1-check-inputs.json; await newexecsession+separate idle guard BEFORE edits/
other runtime. Only MD/independentignoredprep duringfrozenrun. Thenregression,
stock/privatePCM/audit, FULLenvironment integration boundary, localcommit and
continueALLitem3. No push/binaries/agents/goals/outside authoring.

M4cv first verification session4295 ACTIVE. Core PASS Engine1.439s/Character
11.293s/System9.417s. Native targeted Shake/EnvColor/global/ordering and45 actual
static apps still running; composition first. Sourcefreeze:m4cv1-check-inputs.json.
No source/probe/test/fixture edits or another runtime until4295 drains+separate
idle guard. Full integration script run-m4cv-full.py prepared ONLY, not executed;
uses40m package deadline as actual M4cs full script did, with expanded corpus.
No full-suite claim until it actually passes and frozen inputs verify.

Latest M4cv update: revision2 session34597 ACTIVE after first matrix finished and
separate idle guard. See final AGENTS paragraph for exact current state; it
supersedes earlier active-session/pre-implementation text. First matrix native
replays PASS;38/45 app crops exact/seven profile render failures. HUD AIR alpha
consumer added after native-only pixel diagnosis. Revision2 System+45apps frozen
m4cv2-check-inputs.json; await and guard before edits/runtime. Retain ALL failures;
no changed goldens/tolerances. Regression and full scripts prepared ONLY.
