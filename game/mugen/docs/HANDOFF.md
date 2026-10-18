# Current handoff

Updated 2026-10-18. The user authorized completion of all 27 HitDef keys
remaining after 24b3ebd0, with local checkpoint commits and continuous work
until complete or blocked. M4bn guard.dist is complete; M4bo literal enum/flag
probes are active. Read HITDEF-REMAINING.md, GUARD-DISTANCE.md and COVERAGE.md.
Continue the authorized batches without waiting for review. No push or binaries;
all authored work stays under game/mugen/. Full suites follow the enum/combat
and final integration boundaries, with focused checks per checkpoint.

## M4bn verified checkpoint

Predecessor/reversal 24b3ebd0. guard.dist is a strict int32 expression. Native
omitted/negative updates preserve the current range, zero disables and state
entry resets to configured attack distance. Facing, strict edge, vertical
independence, mixed coordinates and tagged order are measured. Routing is
55/81 (67.9%); 26 keys remain in this allocation (17 numeric, nine literal).

Fifteen guardrange-* programs pin 204 observations, four load rejections and
60 pre-native hashes in temp/m4bn-final-prepared-inputs.json. Four Go hashes
are frozen in temp/m4bn-production-inputs.json. Do not rerun the author script
or derive expectations from Go. The earlier contact-contaminated guarddist-*
draft is archived, unpromoted, under temp/m4bn-draft-fixtures. Corrected inputs
were frozen before their native execution; no native values/tolerances changed.

Focused Match races pass 41.153s (15 new plus 43 recent fall/bounce programs);
full Character 12.156s and core engine/evaluator/CNS/combat races
1.503s/1.022s/1.013s/1.013s pass. Two native reproductions and two apps/three
captures pass source/config/binary/PNG/runtime provenance in
temp/m4bn-evidence-audit.json. The stock 7s capture was visually inspected.
Binary SHA256: 70191f00bcd89cadb8a963794fb03e24a2c2f9889892983fe242fb57d2d56e5c.
temp/run-m4bn-validation.py exited 0. Root module/executable checksums pass.
Full scoped races are intentionally deferred to the planned integration gate.

M4bo: 32 hitliteral-* source sets/128 hashes frozen in
temp/m4bo-prepared-inputs.json. Native runner temp/run-m4bo-native.py is active
on the private display. Await that session and make a separate process check
before launching another native/app/display-dependent test. Source inputs are
frozen; accepted/rejected classification follows native evidence. Numeric and
literal-schema accounting must remain separate. Other engine/mode/asset/
renderer gates remain open; continue until all 27 keys are handled or blocked.

## M4bm verified checkpoint

Predecessor/reversal 8779869a. Compiles down.bounce as a native integer flag,
with default/coercion/bottom/error and full relative order. Nonzero down Y
with bounce disabled resets fall X to its preserve sentinel and fall Y to
zero, for both fall flags. Zero down Y and standing/airborne controls retain
fall vectors. Mixed-profile vectors convert once. Stock common bounce and
get-up paths now match. PosFreeze is retained across same-tick transitions;
common idle turning preserves the first negative-state heading observation.
Other auto-turn/PosFreeze edge cases remain open. Routing is 54/81 (66.7%);
27 keys are literal or absent, not a whole-engine completion count.

Twenty-one programs pin 324 native observations, two load rejections and 84
pre-native hashes. Manifests: temp/m4bm-prepared-inputs.json,
m4bm-prepared-extra.json, m4bm-prepared-last.json and m4bm-prepared-trace.json.
Do not rerun author-m4bm-downbounce.py or derive expectations from Go. Five pre-integration Go hashes are frozen in
temp/m4bm-production-inputs.json; final hashes are in
temp/m4bm-final-production-inputs.json. The only final source delta is Character
contact initialization.
The evidence audit also verifies M4bl's six authored source hashes against
commit 8779869a and both native families: 43 programs/564 observations/172
source hashes. No native expected values or tolerances changed.

The predecessor source overlay reproduces the default-vector and stock
no-bounce failures (temp/m4bm-before-fix.log). Initial focused replays found
bottom handling and the stock PosFreeze/idle phase differences. The additional
native trace was frozen before capture. Refined races pass all 21 programs
and both stock sweeps (Match 16.775s). Separate physics/landing/recovery/
SuperPause races pass Character 1.450s/Match 1.871s. Core races engine/
evaluator/CNS/combat pass 1.465s/1.025s/1.013s/1.013s.

Before the final correction, three fresh native reproductions and three
apps/six captures passed provenance in temp/m4bm-evidence-audit.json.
The recovered bounce 7s capture was visually inspected; magenta fixture stage
fill is intentional. These are smoke checks, not full raster parity. Shared
application binary SHA256:
fe85074f062eb5d505372c617d47473e9fcb894f5d7cc2fc567ceb8e3cfb9040.
The one full scoped race run completed with exactly one failed subtest:
hitkill-fall-override-idle reported fall=1 rather than native 0. All other
Match cases and packages passed. Match took 719.837s; Character 11.686s,
System 4.693s, storyboard 1.786s and stock/resources 6.677s; core engine/
evaluator/CNS/combat 1.443s/1.021s/1.012s/1.011s. The initial full log remains
at temp/m4bm-full-race.log with its failure; it is not a passing full run.

The narrow correction clears fall when contact enters a non-H override;
StateTypeSet changing an existing hit state to I still retains its flag, as
measured by M4bl. No native value or tolerance changes. Final affected Match
replays pass 136.768s (196 native programs plus both stock sweeps); full
Character races pass 11.907s. The fresh native idle-override reproduction and
three rebuilt apps/six captures pass. temp/run-m4bm-final-validation.py exited
0; logs are temp/m4bm-final-validation.log and m4bm-final-match-race.log.
Final source/config/binary/PNG/runtime audit passes in
temp/m4bm-final-evidence-audit.json; five final Go hashes and M4bl's six
committed hashes match. Final application binary SHA256:
3a89203832ebf5662566a567600499f4d75db84595b90bd75d36d9c5e7d7dc91.
The final recovered bounce and mixed-stock 7s captures were visually inspected.
No GUI client remains; root module and original executable checksums pass.
The full suite was not repeated after this isolated correction: the combined
record is one full run with a single diagnosed failure, then passing affected
Match/full Character/native/application checks. No third family or push is
authorized. Full recovery-command routing, non-fall
recovertime/liedown/mashing, juggle/chains, broad foreign/entity/contact/pause,
absolute KO/round clocks, modes/assets and strict alpha/spark gates remain open.

## M4bl verified checkpoint

Predecessor/reversal 915f40d7. HitFallSet now preserves omitted/-1 value,
retains other raw int32 values and the fall clock, and compiles supported value
expressions before effects. Native beta ignores documented xvel/yvel (including
invalid/side-effect expressions); alternate spelling controls also do nothing.
This is a documented target limitation, not a missing velocity implementation.
GetHitVar(fall) exposes retained/raw flags even outside MoveType H in the
measured control. Existing HitFallVel consumers agree. Routing stays 53/81.

Twenty-two programs pin 240 observations, two native load rejections and 88
pre-native input hashes in temp/m4bl-prepared-inputs.json and
m4bl-prepared-extra.json. Do not rerun author-m4bl-hitfallset.py or derive
native expectations from Go. Six authored Go hashes are frozen in
temp/m4bl-production-inputs.json. Initial native velocity-tuple acceptance
changed rejection classification only. The initial broad Go replay matched
all numbers; two rejection metadata state numbers were corrected to native
5000. No source input, native observation or tolerance changed.

Focused races pass Match 17.114s/Character 1.041s (22 new programs, both stock
sweep directions and recovery eligibility); initial broader replay also passed
all fall/vector/trip numbers. Core races engine/evaluator/CNS/combat pass
1.435s/1.021s/1.012s/1.011s. Two fresh native reproductions and two apps/three
captures pass input/config/binary/PNG/runtime provenance; stock 7s visually
inspected with intact fighters/stage/HUD. Binary SHA256:
3ed21faaf78458bc458af6029f8c9e267c736ef3fe4f2295dbd30528f0ded023.
temp/run-m4bl-validation.py exited 0; audit temp/m4bl-evidence-audit.json.
Full scoped races are deferred to the authorized M4bm boundary. Full recovery,
liedown/mashing, juggle, broad entity/contact/pause cases, round clocks and
strict alpha/spark gates remain open. M4bm native probes run sequentially on
the private display; await their session before launching any other GUI client.

## M4bk verified checkpoint

Predecessor/reversal 3db8b057. Three compiled float fields bring expression
routing to 53/81 (65.4%); 28 keys remain literal or absent. Other README
metrics are unchanged and are not overall compatibility percentages.
Omitted fall X preserves velocity; explicit zero/bottom sets zero. Fall Y
and yaccel default to -4.5/0.35 in 320-wide coordinates, scaled per host.
Explicit expressions retain source units and convert into receiver units
once. Floating literals, negative gravity, mixed/reversed profiles, repeated
HitFallVel consumption, source isolation and full relative order are measured.

Twenty original programs pin 276 observations, three tuple rejections and
80 source hashes in temp/m4bk-prepared-inputs.json. Do not rerun
temp/author-m4bk-fall-vectors.py or derive expectations from Go. Four authored
Go source hashes are frozen in temp/m4bk-production-inputs.json. The final
audit also checks M4bj's five source hashes against commit 3db8b057, and both
native families: 33 programs/432 observations/132 input hashes. No native
expectations or tolerances changed.

Focused races pass 20 vector, 13 trip, 63 earlier velocity controls and both
real-input sweep directions (Match 73.453s). Character recovery-command and
SuperPause checks pass 1.121s. Core development races pass engine/evaluator/
CNS/combat 1.442s/1.020s/1.012s/1.012s. Three fresh native reproductions and
three apps/five captures pass source/config/binary/PNG/runtime checks in
temp/m4bk-evidence-audit.json. The stock 7s capture was visually inspected:
intact fighters, stage and HUD; application checks are smoke, not full raster
parity. Shared application binary SHA256:
399865d0af49a3421c9e875ab3c3aa8a759399908b75de28b16a0164d60187ec.

Final pipeline temp/run-m4bk-validation.py exited 0 (temp/m4bk-validation.log).
One combined full scoped race run covers both authorized checkpoints:
Match 693.752s, Character 11.396s, System 4.735s, storyboard 1.790s,
stock/resources 6.520s; engine/evaluator/CNS/combat
1.453s/1.023s/1.012s/1.011s. Log: temp/m4bk-full-race.log. No native/app/test
GUI client remains. Root baseline temp/upstream-post-merge-module.sha256
passes; original native executable SHA remains
d5508f77f2a86f12358e669f91c64ba5e4a789d4b71262e8a758814384303a4e.

Next work needs another allocation: recovery-command state/velocity/alive
conditions, down.bounce, non-fall GetHitVar(recovertime)/mashing clocks,
HitFallSet variants, juggle/chains and broader contact/entity/pause cases.
Absolute KO/round clocks, all-mode/asset conformance and the six alpha/two
spark raster differences remain open. M4bj's one original stock-fixture
trailing space is deliberately frozen and documented in TRIP-RECOVERY.md;
the M4bk diff passes whitespace checks.

## M4bj verified checkpoint

Predecessor/reversal 449ce7af. The sweep loop came from ea3599303's recurring
Trip heuristic, not M4bi. It forced 5110/custom/override states back to 5070.
Trip now selects contact entry only. Native grounded/crouching Trip selects
5070 independently of fall; airborne Trip uses air.type (default ground.type),
downed entry stays 5080 and custom/override destinations keep their own flow.
Type queries expose High=1/Low=2/Trip=3; the entry/late legacy type query is 0
in the measured beta controls. Numeric expression routing remains 50/81.

Thirteen original text programs pin 156 observations/52 pre-native hashes in
temp/m4bj-prepared-inputs.json. Do not rerun temp/author-m4bj-trips.py or derive
expectations from Go. Stock common recovery observations at 0/13/29/89/103
ticks match exactly. Both real down+B mixed-profile directions fail before
the fix and now complete 5070/5071/5110/5120/0 with 72 damage and restored
control; baseline log temp/m4bj-before-fix.log.

Focused Match races pass 58.555s (13 new, two stock sweep directions, 38
forced-standing and 30 fall controls). Core races pass engine/evaluator/CNS/
combat 1.464s/1.024s/1.014s/1.013s. Two fresh native reproductions and two
apps/four captures pass provenance; temp/m4bj-evidence-audit.json also verifies
five authored Go source hashes. Binary SHA256:
58ce59cc00276489949717ea027b8a7ebec2cdd7da8d5dfa4aa11b9c3cfb4ad1.
The 6s recovered fixture capture was visually inspected; magenta diagnostic
stage fill is intentional. temp/run-m4bj-validation.py exited 0; no GUI client
remains. Full scoped races are deliberately deferred to the authorized M4bk
integration boundary. Legacy recovery-command heuristics and broader gates
remain open; see TRIP-RECOVERY.md.

## M4bi verified checkpoint

Predecessor/reversal ce6b7a50. Four new compiled fields bring expression
routing to 50/81 (61.7%); 31 documented keys remain literal or absent. The
other README metrics remain unchanged and do not measure engine completion.
Native controls establish fall/air-fall boolean selection, repeated source
defaults, runtime float truncation, bottom handling, default delay 4 and
negative-result fallback 4. Fall time starts at 0, advances during defender
shake and survives H-state continuation. CanRecover gates on that clock and
the enabled flag. This is not full recovery-state/physics compatibility.

Thirty original text programs pin 372 observations, five native load
rejections and 120 source hashes. Do not rerun temp/author-m4bi-fall.py or
derive native expectations from Go. All sources match
temp/m4bi-prepared-inputs.json; all 12 authored Go sources match
temp/m4bi-production-inputs.json. The first focused run found missing typed
selectors, an order transcription error and a missing unit-fixture controller.
They are fixed; temp/m4bi-initial-focused-race.log is retained. No expected
values or tolerances were changed.

Focused races replay all 30 new, 25 air/down, 21 ground/guard and 38
forced-standing programs (Match 71.220s). Character recovery-command/Pause/
SuperPause checks pass 1.386s. Three fresh native reproductions and three
actual apps/four captures pass source/config/binary/PNG/runtime provenance
checks in temp/m4bi-evidence-audit.json. The stock 7s image was visually
inspected: intact fighters, stage and HUD; these are smoke checks, not raster
parity. Shared binary SHA256:
79ea936dec430cfdd4318ca3e7a83543421e56040af6e7e8ac224c7a561bd596.

Final pipeline: temp/run-m4bi-validation.py, log temp/m4bi-validation.log;
exit 0. Full scoped races pass in temp/m4bi-full-race.log:
Match 640.740s, Character 11.000s, System 4.652s, storyboard 1.778s,
stock/resources 6.510s; engine/evaluator/CNS/combat
1.434s/1.020s/1.013s/1.012s. No native/app/test GUI client remains.
Root module baseline temp/upstream-post-merge-module.sha256 passes; native
executable SHA remains
d5508f77f2a86f12358e669f91c64ba5e4a789d4b71262e8a758814384303a4e.

Next recovery work needs a new allocation: fall x/y velocity and yaccel
schemas, down.bounce, non-fall GetHitVar(recovertime), HitFallSet reset
variants, and removal/replacement of legacy recovery-command state heuristics
with native common-state conditions. Juggle/chains, broad contact/entity/pause
combinations, absolute KO clocks and full mode/asset gates remain open. The
six alpha and two spark raster failures remain unwaived.

## M4bh verified checkpoint

Predecessor/reversal e37dd7da. Read HIT-AIR-DOWN-TIMING.md. Both omission
defaults are 20, explicitly zero/negative/bottom become 0, and runtime floats
truncate. Ground/down vertical launches select the air timer independently
of reaction state number. Ordinary down hits select DownHitTime. Slide time
is 0 for the measured non-ground reactions. Fall controls keep counting their
exposed timer; ground KO selects air time after its vector adjustment. Full
recovery and absolute KO clocks remain outside this allocation.

Twenty-five original text programs pin 336 observations, three native load
rejections and 100 source hashes. Do not rerun temp/author-m4bh-times.py or
record expectations from Go. temp/m4bh-prepared-inputs.json matches all 100
inputs. Initial MAF-only down controls never connected: source/goldens are
retained under temp/m4bh-before-down-hitflag/, excluded from acceptance. The
corrected D-flag controls have new -contact names and hashes; recording also
checks life 990 (0 for KO) to reject a missing-contact measurement.

Focused replay passes 25 flight, 21 ground/guard, 19 pause and four fatal-guard
pause programs (Match 42.414s). Character reference/pause/continuation checks
pass 1.585s. Engine/evaluator/CNS/combat races pass 1.426s/1.021s/1.014s/1.011s.
Final pipeline: temp/run-m4bh-validation.py, log temp/m4bh-validation.log.
Fresh native default/order/rejection, air-zero/down-clock/mixed-stock apps,
and one full scoped serial-package race run are included. Fresh native checks
pass. Three actual apps/four captures pass prepared-source/config/binary/PNG/
runtime audit in temp/m4bh-evidence-audit.json. Binary SHA256:
7c1b14ae5e12f3a73324b0ba2ff732fac999e21c316f428f4b5a54833879a062.
The stock 7s image was visually inspected: two fighters, intact stage/HUD,
no persistent effects. These are smoke checks, not full-frame parity.
Full scoped races pass in temp/m4bh-full-race.log:
Match 648.043s, Character 11.516s, System 4.843s, storyboard 1.879s,
stock/resources 6.625s.
Final pipeline exited 0. No test/native/application GUI client remains.
Root baseline remains
temp/upstream-post-merge-module.sha256; original native SHA remains
d5508f77f2a86f12358e669f91c64ba5e4a789d4b71262e8a758814384303a4e.
Existing six alpha/two spark failures stay open. Next work is a new allocation,
not a continuation into recovery or round-clock logic during this checkpoint.

Historical sections below do not override the current allocation.

## M4bg verified checkpoint

Read HIT-GROUND-TIMING.md. Predecessor/reversal b1b8864c. Twenty-one original
text programs pin 348 numeric observations, four native load rejections and
84 source hashes. Do not rerun either temp/author-m4bg-* script or regenerate
expected values from Go. Both pre-native input manifests still match all files.
Ground defaults are 0; guard hit/slide/control and air guard control re-execute
their inherited source independently. Negative/bottom become 0; runtime floats
truncate; literal floats and tuples reject. All 44 routed keys now have native
relative order, with source-forward/reverse controls.

Focused Match races pass 21 new, 19 M4be and four M4bd pause programs (29.907s).
Engine/evaluator/CNS/combat races pass 1.454s/1.020s/1.013s/1.012s. Three fresh
native reproductions (guard source defaults, reversed order, float rejection)
pass, recorded in temp/m4bg-replay-*.log. Initial validation then hit a stale
pause-family directory name in a disposable app recipe before launch. This is
retained in temp/m4bg-validation.log; it is corrected in the final pipeline.

Final pipeline: temp/resume-m4bg-validation.py, log m4bg-final-validation.log.
Four apps/five captures pass prepared-source/config/binary/PNG/runtime audit
in temp/m4bg-evidence-audit.json. Shared binary SHA256:
d5efec4146a6083cd46c4d260d0244f2bba9aa2b1a1b377de05541cc6a36724d.
Apps exercise ground expressions, air guard inherited defaults, mixed stock
fighters at 3/7s and KFM intro at 640x480. Stock 7s was visually inspected: two
fighters, stage/HUD intact, no persistent effects. These are smoke checks, not
full-frame parity. Full scoped races pass in temp/m4bg-final-full-race.log:
Match 614.394s, Character 11.208s, System 4.601s, storyboard 1.789s,
stock/resources 6.528s.
Final validation pipeline exited 0. No GUI test/native/application client remains.
Root module baseline upstream-post-merge-module.sha256 passes; native original
executable SHA remains d5508f77f2a86f12358e669f91c64ba5e4a789d4b71262e8a758814384303a4e.
Existing six alpha/two spark failures remain open. No new binaries are tracked.

## M4bf storyboard canvas

Read STORYBOARD-CANVAS.md. Predecessor 99b4605c, the completed upstream merge.
Six native controls and all six app comparisons pass 12 marker rectangles/counts
and 24 edge samples. Tests now render the real Player on the GPU and compare
native measurements. Parser/storyboard races 1.013s/1.787s, System/resources
4.613s/6.530s pass; real KFM intro and mixed KFM/KFM720 fight smokes pass.
Stock intro at 640x480 was visually inspected. Match was unchanged and was not
rerun at that checkpoint; M4bg's full suite above covers both changes.
Binary resources/captures
stay in ignored temp/. Root module baseline upstream-post-merge-module.sha256
passes. No GUI client remains after validation. Existing alpha/spark failures
remain open. Evidence: temp/m4bf-validation.log; tests/storyboard_probe.py.

The older handoff sections below are historical; their stop instructions do
not override the user's current allocation described above.

## Verified M4be checkpoint

Predecessor/reversal `e2addf06`. Production source is frozen after adding
engine/hit_pause.go and its routing in hitdef.go. HitDef expression routing is
now38/81 (46.9%);43 literal/absent fields remain. Controller/trigger/stock
compilation percentages keep the README's narrow meanings. COVERAGE.md is the
refreshed subsystem ledger; historical inventory status columns are not current.

Nineteen original hittiming-pause-* programs record252 observations, four load
rejections and76 hashes. All source hashes match m4be-prepared-inputs.json.
Never rerun author-m4be-pause.py or regenerate expected values from Go.
Full forward/reverse tags establish pausetime after GivePower/before NumHits,
guard.pausetime after guard.velocity/before guard.cornerpush, both Y-before-X.
Omitted guard components re-execute hit source defaults; a trailing empty second
component inherits, while leading empty/three-component tuples reject. Runtime
float truncation, bottom0 and negative0 are measured. See HIT-PAUSE.md.

Focused Match races pass all19 plus four M4bd controls in14.466s; pure engine/
evaluator/CNS/combat races pass. The pre-fix host/atomic compilation failures
are retained in m4be-before-pause-compilation.log. Pipeline96878 exited 0:
temp/m4be-validation.log. Three fresh native programs (default, full reverse
order and float rejection) and three actual apps/four captures pass. The single
full scoped race suite passes in m4be-full-race.log: Match607.027s,
Character11.118s, System4.730s, stock tests6.567s. No test/native/app GUI client
remains. Root module and original native executable integrity checks pass.

Latest user steering: do not add binaries to Git. Audited all tracked MUGEN
files: none is an ELF/PE compiled executable; `game/mugen/mugen.sh` is the Bash
launcher. The compiled executable is ignored under temp/bin/. The user added
`mugen` to .gitignore during this run; that edit is preserved alongside explicit
compiled-output ignore rules. All M4be additions are text; do not add new binary
fixture resources in later checkpoints. Audit staged content before committing.

App audit temp/m4be-evidence-audit.json passes source/prepared inputs, config,
binary, PNG dimensions/hashes and runtime logs. Binary SHA256:
0daa6080198b869d3e8d806c8f4646149267a9864c5b6c0566d50030ad179da8.
Two app programs exercise expressions/default inheritance, plus mixed stock
KFM/KFM720 at 3/7s. The stock 7s image was visually inspected: two fighters,
stage/HUD intact, no persistent effects. These are smoke/provenance observations,
not full-frame parity. The first audit script assumed kfm720.snd; it was fixed
to the actual stock kfm720/kfm.snd before the final audit. No runtime/data change.

The feature checkpoint is `f27496f9`. Its source/text-only audit passed before
commit; .gitignore was deliberately left for the separately requested follow-up.
Existing six alpha/two spark raster failures
stay unwaived. Other timing/fall/round-clock/renderer/mode work is outside this
allocation. No remote push or new feature batch.

## Launcher reconciliation and upstream integration

The historical audit checked4,134 unique Git blobs under game/mugen across all
local refs: no ELF/PE/Mach-O executable found, no blob over1MiB. The launcher
has five recorded revisions, all Bash, from05fbb2aa37 through d972c8d1. The user
accepted that clarification and requested the .sh extension. No history rewrite
is needed. Evidence: temp/committed-executable-audit.json.

Launcher is renamed to mugen.sh with active README/build/match/test/environment
commands updated. Historical checkpoint command receipts retain the prior path.
The user's `mugen` ignore entry is preserved; explicit mugen-go/*.exe/*.test
ignore rules keep build outputs out. Bash syntax/help and the expression runner
pass. Focused renamed-launcher stock/production-flow checks also pass in
temp/launcher-rename-flow.log. No Go runtime changed.

Rename checkpoint6fb25376 is complete. Fetch found two upstream commits:
bac65e96 adds filename/date/microsecond logging plus the already scoped font
dependencies in root go.mod;3d638348 includes the invalid value in sound-volume
errors. The requested no-rebase merge is clean and preserves both local
checkpoints. Root go.mod matches upstream exactly; no authored root edits.
The user's untracked go.sum and other unrelated files are preserved.

Post-merge configuration/System/stock races pass in
temp/upstream-integration-race.log (1.041s / 4.803s / 6.497s). Production build
passes; temp/upstream-logging-smoke.log verifies source filename/date/microseconds
on an intentional missing-data failure before audio/window creation. No full
Match rerun for these diagnostic/dependency declarations: the runtime engine
is unchanged from the full607.027s race checkpoint.

The module integrity baseline is now temp/upstream-post-merge-module.sha256.
The older before-font baseline is historical and will intentionally differ for
go.mod after upstream's declared dependency additions. Do not undo those
incoming changes. Complete the merge commit with these findings, then stop.
No remote push is authorized; await a new feature allocation.

## Latest documentation checkpoint: project README

Predecessor/reversal: `637c3a0d`. Adds requirements, data setup, launcher and
standalone build/run commands, stock controls, test commands, progress/remaining
work and narrowly scoped coverage percentages. No production source change.
Fresh source audit: 87/89 documented controller names registered (97.8%),
127/127 trigger names recognized by the static validator (100%), 36/81 HitDef
keys routed through expression compilation (44.4%). Unregistered controller
names: ModifyExplod and ExplodBindTime. Registration/recognition includes
partial/stub behavior and is not a semantic compatibility percentage.

The README standalone build command passes with the scoped module overlay.
Fresh focused stock compilation passes: 108/108 states per KFM variant,
216/216 loaded state programs (100%), in temp/readme-stock-compilation.log
(tests0.079s). This counts compilation, not every execution path. The full suite
was not repeated for documentation changes. Source audit denominators are the
documented controller/trigger name sets and current HitDef schema audit.
The standalone/launcher help commands and documentation link checks pass.
temp/readme-coverage-audit.json records the scoped counts. Root module integrity
checks pass; the build output and validation logs remain ignored under temp.

AGENTS.md now records bounded feature batches, an initial concise current
behavior ledger refresh, focused checks during development, full suites at
integration boundaries, dated metrics and documented local checkpoints. Apply
that workflow when the user resumes after pushing. Do not begin the ledger
refresh or a native probing campaign during the present documentation turn.

## M4bd checkpoint: cue/pause evidence, with open raster failures

Predecessor/reversal: `54d8c86e`. M4bd changes only authored fixtures, tests and
docs; there is no production behavior change. Read FATAL-GUARD-CUES.md.
Twelve native programs (four each audio/spark/pause) record 144 numeric
observations and 40 source hashes. Fatal guard matches ordinary fatal hit for
sound, spark selection and pause; safe/protected guard retain guard behavior.
All original sources/goldens are frozen; never rerun their authors.

All new semantic replays pass: four PCM programs (20 windows / 160 stereo
amplitudes), four spark programs (11 total cases) and four Match pause programs.
Focused logs are m4bd-audio-integrated.log and m4bd-cues-integrated.log.
Full Character race regression passes in m4bd-character-race.log (11.149s).
The latest full scoped suite remains M4bb (Match 618.564s); M4bc production/stock
scoped checks also pass. No full Match rerun was needed for this test/data-only
checkpoint.

Validation pipeline93682 exited 0: m4bd-validation.log. Fresh native fatal audio,
protected spark and fatal pause reproduce the pinned goldens. Four final apps /
four captures pass prepared-resource/config/binary/PNG/runtime provenance audit.
Two private-device PCM outputs match all 16 selected stereo amplitudes exactly
at the recorded precision. Binary SHA256:
48624d81792daa306e8c9b4af585877014a51e7f581d9f46d85ed7cb659d1b9e.
The source/production audit is temp/m4bd-final-app-audit.json.

**Two production spark raster gates fail and remain unwaived.** Selection and
anchors match, but each native 276-pixel core differs at all 276 pixels, max
channel delta8; Go has an extra left core column (288 pixels). Native hash
5b077cf1947ce2dbe811076af1f3549be4013f1becd5a8148108fc8237a1c238;
Go hash2c55c6b0e790d626fa7c7a6ff42ae51c0fa5969efc45a19b843be1974ef94753.
Cause not investigated. These add to M4az's six alpha precision failures.
Final audio apps end -go-m4bd-verified (5s), spark apps -go-m4bd-cue (3s).
Longer fatal pilots reach missing winner state180 in the minimal fixtures and
are retained only as diagnostics. Shorter captures do not prove their full
lifecycle. Stock lifecycle evidence is separate. Fatal spark was visually read.

No GUI test/native/app client remains. Root module integrity and original native
executable SHA checks pass. Native SHA remains
d5508f77f2a86f12358e669f91c64ba5e4a789d4b71262e8a758814384303a4e.
All disposable scripts/captures/reference copies stay ignored under temp.
The private display/audio infrastructure may be reused only after checking it
is still live and that no GUI/audio clients overlap; never affect host focus or
routing. The environment is documented in BUILD.md/AUDIO.md.

## Review and resume decision

PROGRESS-ASSESSMENT.md distinguishes working stock flow from full compatibility,
relates progress to M0-M8, lists known failures and suggests bounded allocations.
Do not infer a whole-engine percentage from fixture or checkpoint counts.
The current HitDef audit is 36 expression-routed keys out of 81 documented keys;
45 literal/absent keys include enums and require appropriate schemas. That is
not a complete-feature count. KO/round clocks, remaining combat/language,
projectiles/reversals/teams, BGM, modes, rendering/assets and final conformance
remain substantial. The controller/trigger tables are dated baseline data, not
an updated completion ledger. Refresh a concise evidence ledger if the user
resumes for a later allocation, then stop at the agreed next batch boundary.
The M4be ledger refresh above supersedes this older proposed first step.

## Historical checkpoint evidence

The sections below preserve prior verification details. Their suggested next
actions are historical and do not authorize work during the current pause.

## Latest verified checkpoint: M4bc

Read STAGE-CLEAR.md. Reversal/predecessor9ef4c313 (M4bb). Stage loads DebugBG
with truncation toward zero before nonzero conversion, then clears magenta in
Stage.Draw before layer0. Production and standalone viewer share the path.
Eleven native programs record132 observations,44 source hashes, exact owned
black/magenta region metadata and viewport points. No original art/binaries or
window pixels are committed. Default fit keeps black side bars; stage0/stretch
fills edge samples. Omitted/0/.5/-.5/foo sample black;1/-1/2/1.5 magenta.

Final pipeline44482 completed exit0: m4bc-validation.log. Twelve final apps/
fifteen captures (suffix go-m4bc-verified), all11 color regions/88,000 pixels and
33 viewport points pass prepared-input/config/binary/PNG/runtime audit.
Binary48df6ff9c82e06c1f73b459cd3265a0ceaef0edd18ec9310eda8a22e6b2c5548;
results m4bc-final-app-audit.json. Four fresh native pixel/numeric replays pass
m4bc-replays.log. Scoped sequential races pass m4bc-scoped-race.log:
System4.713s, stock tests6.502s; stage builds. Root module checks pass. Stock 7s
was visually inspected: two recovered fighters/no persistent effects. No GUI
client remains. Native original executable remains unchanged.

Diagnostics are preserved: m4bc-before-fill.json proves8,000 original mismatch
pixels; stageclear-half-go-m4bc-before-truncation and m4bc-half-before-check.log
prove the initial float-boolean coercion error. Initial seven app runs use
suffix go-m4bc-final and are not the final binary. Native record/verify tool is
tests/stage_clear_probe.py; Go cannot record a golden. Sources are frozen.
No whole-frame, no-clear persistence, pause/tint/foreground or general renderer
completion claim. M4az's six alpha precision crops remain unwaived.

Next: continue with fatal-guard sound/effect/pause selection and KO/round clocks,
then remaining HitDef schemas/renderer/mode gates. Local commits remain
explicitly authorized; no pause/check-in or remote push. Keep all edits/data/docs
under MUGEN, temp ignored. M4bb9ef4c313 is the preceding full-race checkpoint;
all90 lethality cases passed there along with the rest of the corpus.

## Latest verified checkpoint: M4bb

M4bb is verified; reversal/predecessor is 3fa397a9 (M4ba). Read HIT-LETHALITY.md.
The accepted corpus is now 90 hitkill programs: 1,080 numeric observations,
four float-literal rejections and 360 CNS/AIR/CMD/stage hashes. Sources/goldens
are frozen; never rerun their authors. Final source hashes are audited against
the native JSON. Diagnostic killdiag sources moved to
`temp/m4bb-flow-diagnostic-sources/`; none belong in the final corpus.

Validation pipeline57076 completed exit0 in temp/m4bb-post-validation.log.
All phases pass: app audit, fresh native replays and full scoped races. Match
618.564s, Character11.012s, System5.027s in m4bb-full-race.log. Root module checks
also pass. No active GUI client remains.

Validation:
- All90 integrated race replays pass: m4bb-integrated.log, Match61.287s.
- Pure CNS/engine/combat/evaluator races pass: m4bb-precedence-pure.log.
- Thirteen app runs/sixteen captures share binary
  d354737083b645fc513029e69f441268af50869ed14c3dbed067b2d5048185bb.
  All provenance/runtime checks and all12 exact garment masks pass in
  m4bb-final-app-audit.json. The mask selects RGB247,239,231 in
  box160,180..1120,650, excluding grayscale round text. This is an actor check,
  never whole-frame parity. Stock throw7s was visually inspected: two recovered fighters, no lingering effects.
- Native airborne capture initially had a K.O. overlay hiding1,180 garment
  pixels. Retained as *-occluded diagnostics. A fresh unchanged source replay
  `m4bb-replay-ko-air-up-unoccluded` at8s verifies pinned numeric observations
  and matches all13,923 garment pixels exactly. No golden or tolerance changed.
- Root go.mod/go.sum and original mugen.exe checks pass; final check follows
  the full suite too. Native binary SHA remains
  d5508f77f2a86f12358e669f91c64ba5e4a789d4b71262e8a758814384303a4e.

Native findings implemented:
- Effective damage caps before entry to remaining life, or life-1 for
  kill/guard.kill0. Victim5/raw10 exposes5(fatal) or4(nonlethal), entryLife5;
  raw-derived power remains7/6. Entry LifeSet0 stays dead.
- Fatal guard retains chip/guard.kill selection but normalizes reaction,
  counters, power and context to ordinary hit. Fall is forced. Normal fatal
  contacts get KO-adjusted velocities before CNS/HitVelSet.
- Ground zeroY scales X by float32.66; nonzero X then gains signed2.5.
  Airborne skips.66. NonpositiveY loses2 with ceiling-6 ground/-3 air;
  positiveY stays. Constants scale by receiver width/320, after source vector
  conversion. Downed contacts retain the supplied vector. Thirty-eight controls
  cover signs, thresholds, falling/nonlethal, S/C/A/L and mixed coordinates.
- Typed kill/guard.kill/fall.kill/fall.damage follow the measured full31-tag
  component order (36 typed keys total). GetHitVar(fall.kill) defaults1 and
  returns int. Getter fall is0 outside H.
- Native HitFallDamage consumes raw signed fall.damage once, without attack/
  defence scaling, even outside fall/H and with fall.kill0. Repeated6 only
  applies once; negative6 heals6. The controller does not itself force a fall.
- CNS ordinary parameters now keep the first duplicate; repeated triggers stay
  separate. Seven airborne/downed probes exposed the former last-wins bug.
  Three explicit duplicate damage/kill/PowerSet controls confirm the fix.
  Metadata/Statedef duplicate rules remain outside these controls.

Earlier failed focused logs are diagnostic evidence: m4bb-first-integrated
(typed fall.kill selector absent), m4bb-ko-integrated (seven first/last parameter
mismatches), and prior empty-state/unreached snapshot pilots. Current sources
include their fixes. Pilot capture errors never count as expected load rejection.
The accepted snapshot protocol is positiveState700 last, Time>=8 one-shotVar59,
clipboard there at delay0, FVar8=100000+Var29 sentinel. KO motion programs freeze
positions after the first three physics steps; phase timing remains independent.

Next: continue with stage diagnostic fill, then remaining combat/schema/KO
and renderer gates. M4bb contains only authored MUGEN source/fixtures/docs;
never stage temp, captures, original art or binaries. The final native/app
artifacts stay ignored under temp. Keep future checkpoints scoped and local.

## Remaining gates and next work

Full engine/assets/modes remain unfinished. Known unwaived gaps include M4az's
six RGB alpha precision crops, absolute KO slowdown/round transitions, complete
literal HitDef schema, recovery/juggle/projectile/reversal/team/foreign/multiple
contacts, and broader modes/assets/rendering. Do not claim whole-engine parity.

M4bc closes the bounded diagnostic fill gap. Complete background controllers,
no-clear persistence applicability, pause/tint/foreground and renderer behavior
remain separate gates. Do not relax the strict pixel expectations.

Local fight.def also exposes Round slow.time=60 (alongside over.waittime45,
over.hittime10, over.wintime45, over.time210). Match currently has no KO slowdown.
Measure CNS/GameTime/phase boundaries deterministically before implementing it;
wall-clock screenshots alone do not establish the slowdown factor or consumers.

Previous verified checkpoint: M4ba3fa397a9, reversal83eb5bd9. HIT-POWER.md
records66 native programs,804 observations, three type rejections,264 hashes;
full races passed Match538.364s/Character12.143s. M4az83eb5bd9 closes bounded RGB
sampling/placement and retains six strict alpha failures. Older notes are in
archive/HANDOFF-2026-10-17-long.md; its active-task instructions are obsolete.

## Scope and environment

Branch go-rewrite-mugen. No agents, skills or goal tool active. Sandbox bwrap
startup is broken; scoped require_escalated calls have passed automatic review.
No approval blocker. Do not touch the host display/audio/keyboard/focus.

Private display DISPLAY=:97,
XAUTHORITY=/home/juvinious/projects/paintown/game/mugen/temp/virtual-display/Xauthority,
LIBGL_ALWAYS_SOFTWARE=1. XvfbPID2146490. Private PulsePID1393643:
PULSE_SERVER=unix:/home/juvinious/projects/paintown/game/mugen/temp/audio-reference/native;
PULSE_COOKIE points to that directory's cookie; ALSA_CONFIG_PATH to its alsa.conf.
Never print cookie contents. Native/app/audio/GUI tests run sequentially. Wait
for the prior session, then separately check
`ps -eo pid,stat,comm | rg 'mugen.exe|mugen-go|match.test|character.test'`.
Idle Wine service processes are fine.

Use scoped builds with GOCACHE=.../game/mugen/temp/go-build,
GOTMPDIR=.../game/mugen/temp/tmp,
GOFLAGS='-modfile=/home/juvinious/projects/paintown/game/mugen/mugen.mod -mod=readonly',
GOPROXY=off. Full suite uses private display/audio variables and
`game/mugen/mugen.sh --check -race -timeout=20m`; growing Match corpus needs the
explicit timeout. Never save disposable Go snapshots with a .go extension under
temp; recursive tests discover them even when gitignored. Root baseline is
`temp/root-module.before-font.sha256`. Free space last checked75G.
