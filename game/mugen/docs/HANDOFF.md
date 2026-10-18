# Current handoff

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

Finalize the local M4by source/text commit, then report both checkpoints for
user testing. No further implementation/validation remains in this bounded
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
Full suite: game/mugen/mugen.sh --check -race -p1 -count=1 -timeout=20m.
Use non-Go suffixes for disposable source snapshots: recursive tests discover
ignored .go files too. Root checksum baseline:
temp/upstream-post-merge-module.sha256. Native executable SHA256:
d5508f77f2a86f12358e669f91c64ba5e4a789d4b71262e8a758814384303a4e.
