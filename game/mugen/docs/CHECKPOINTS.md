# Implementation checkpoints

The user approved PLAN.md on 2026-10-16 and authorized local commits for
milestones/checkpoints. No remote push is authorized. All source, fixtures,
documentation, and tooling stay under `game/mugen/`.

## M4bh: air/down timing

Predecessor/reversal e37dd7da. Adds compiled air/down timers, measured default 20
versus explicit 0, and native timer selection for air/ground launch/down hits.
Non-ground reaction slide time is 0. Twenty-five text programs pin 336 numeric
observations, three native load rejections and 100 source hashes. Focused Match
and Character races pass 42.414s/1.585s; core races pass. Three fresh native reproductions and three actual apps/four captures pass
provenance checks. Full scoped races pass in temp/m4bh-full-race.log:
Match 648.043s, Character 11.516s, System 4.843s, storyboard 1.879s, stock/resources 6.625s. Routing 46/81 (56.8%) leaves 35 literal/
absent keys. Stop at this local checkpoint for user testing; see
HIT-AIR-DOWN-TIMING.md and HANDOFF.md for results and remaining gates.

## M4bg: ground/guard timing

Predecessor/reversal b1b8864c. Adds compiled ground hit/slide times and their
four dependent guard timing fields, with re-executed source defaults, native
order, truncation/clamping/bottom and load validation. Twenty-one native text
programs pin 348 observations, four load rejections and84 source hashes. Focused
integrated races pass (29.907s); fresh native defaults/order/rejection and four
actual apps/five captures pass. Full scoped races pass in
m4bg-final-full-race.log: Match 614.394s, Character 11.208s, System 4.601s, storyboard 1.789s, stock/resources 6.528s. See HIT-GROUND-TIMING.md and HANDOFF.md. Routing is
44/81 (54.3%), with37 keys still literal/absent. Stop after this local checkpoint
for user testing; existing alpha/spark and broader engine gates remain open.

## M4bf: storyboard canvas placement

Predecessor/reversal99b4605c. Retains localcoord/default320x240, propagates it to
backgrounds and separates story-to-game fit from KeepAspect window presentation.
Six native text-only controls match12 marker bounds/counts and24 edge samples;
GPU regression and six actual application comparisons pass. Stock intro was
visually inspected and mixed stock fight smoke passes. Parser/storyboard races
pass1.013s/1.787s; System/resources4.613s/6.530s. Module integrity is unchanged.
Evidence: STORYBOARD-CANVAS.md and temp/m4bf-validation.log. No binaries added.
Next authorized batch: ground.hittime/ground.slidetime, then stop for user testing.

## Requested upstream integration

After rename checkpoint6fb25376, fetch/merge integrates bac65e96 and3d638348
without rebasing local checkpoints. Incoming changes add application log source/
timestamp flags, the invalid sound volume value in errors, and matching font
dependency declarations in root go.mod. Root metadata matches upstream exactly;
the user-requested incoming change is preserved and own edits stay under MUGEN.
Configuration/System/stock race checks pass (1.041s / 4.803s / 6.497s), as do
production build and an intentional missing-data logger smoke. The user's
untracked go.sum remains unchanged. The fresh module integrity baseline is
temp/upstream-post-merge-module.sha256. No new engine behavior or full Match
rerun; its M4be full race result remains607.027s. Stop after this merge checkpoint.

## Launcher source clarification

After M4be (`f27496f9`), the user requested reconciling an apparent added binary.
The history audit checked4,134 unique MUGEN blobs across local refs, finding no
ELF/PE/Mach-O executable. All five revisions of the extensionless launcher were
Bash source. The user requested the clearer `mugen.sh` name; active build/run
instructions now use it. Historical command receipts below retain the old path.
The user's ignore entry is preserved alongside explicit compiled-output guards.
Bash syntax/help, expression fixtures and focused stock/production-flow checks
pass. No Go runtime or compiled binary is added. The user also requested
configured upstream integration after this separate rename checkpoint.

## M4be — Compiled HitDef contact pauses and current coverage ledger

M4be adds pausetime/guard.pausetime expressions, repeated source defaults,
per-component inheritance, measured Y-before-X/field evaluation order, integer
coercion, bottom/negative normalization and source-context load validation.
Nineteen native programs compare252 observations, four rejections and76 source
hashes. All19 integrated programs plus four M4bd pause controls pass under race
checks (Match14.466s); core races pass. Three fresh native programs reproduce
defaults, full reverse order and a float rejection. Three apps/four captures
pass provenance/runtime checks; mixed stock fighters were visually inspected.
One full scoped race suite passes: Match607.027s, Character11.118s,
System4.730s, stock tests6.567s. Root module/native integrity and links pass.
See HIT-PAUSE.md. Reversal: `e2addf06`.

COVERAGE.md reconciles source/tests/topic evidence into a current subsystem
ledger. HitDef expression routing is38/81 keys (46.9%);43 remain literal/absent.
Other README name/compilation metrics retain their narrow meanings. Existing
six alpha/two spark raster failures remain unwaived. Stop this feature allocation
after its local commit; the user requested a separate binary/Git-history
reconciliation next. All new M4be files are text; no binary artifact is staged.

## Project README and bounded workflow

After reviewing M4bd and trying the semi-playable build, the user requested
build/run instructions and dated progress/coverage figures in ../README.md.
Fresh audits establish 87/89 controller registrations (97.8%), 127/127 recognized
trigger names (100%) and 36/81 expression-routed HitDef keys (44.4%). These are
narrow source metrics, not whole-engine behavioral coverage. The README build
command succeeds and focused stock compilation passes for 216/216 loaded state
programs (108 per character; tests0.079s). Documentation links/integrity are
checked; no full regression rerun for this documentation-only change.
AGENTS/HANDOFF record the bounded workflow and pause for the user's push/resume.
No new feature batch is started. Reversal: `637c3a0d`.

## M4bd — Fatal guard cue and pause verification; review pause

This test/evidence checkpoint adds no production behavior. Twelve original
native programs compare fatal/hit/safe/protected sound, spark selection and
pause: 144 numeric observations and 40 authored hashes. New semantic replays
pass; full Character races pass (11.149s). Three fresh native reproductions
pass. Four apps/captures pass provenance/runtime checks; two private PCM
captures match all 16 selected stereo amplitudes at recorded precision.
Two strict spark raster checks fail: anchors/actions match, but each native
276-pixel core differs at all pixels (max channel delta 8) and Go has an extra
left column. These remain open alongside the six M4az alpha cases. No golden
or tolerance was changed. See [FATAL-GUARD-CUES.md](FATAL-GUARD-CUES.md).
Reversal: `54d8c86e`. Root module/native integrity checks pass.

The user requested a pause after this checkpoint for progress/token review.
[PROGRESS-ASSESSMENT.md](PROGRESS-ASSESSMENT.md) records current working behavior,
remaining M0-M8 gates and suggested bounded allocations. Do not start the next
implementation batch until the user resumes.

## M4bc — Diagnostic stage background fill

M4bc implements native BGDef.debugbg clear before stage layer0, shared by
production and the standalone viewer. Numeric values truncate toward zero before
nonzero conversion; omitted/zero, signed fractions, integers and invalid text
are measured. Eleven original native programs record132 observations and44
source hashes. Twelve final apps/fifteen captures share one binary and pass
provenance/runtime checks; all88,000 solid-color pixels and33 viewport samples
match exactly. Default fit retains black bars; stage0/stretch expose magenta at
window edges. Four fresh native programs reproduce pixel/numeric goldens.
Scoped races pass (System4.713s, stock tests6.502s), as do root module checks.
See STAGE-CLEAR.md. Reversal:9ef4c313. No whole-frame/renderer claim; KO timing,
fatal-guard cues, remaining schemas/modes and six strict alpha gates remain open.

## M4bb — Lethal contacts, fall damage and KO vectors

M4bb adds pre-entry kill/guard.kill damage caps, fatal guard normalization,
one-shot raw fall damage, typed fall/kill fields and receiver-scaled KO velocity.
Ninety native programs compare1,080 observations, four float-literal rejections
and360 source hashes. Repeated ordinary CNS parameters now keep the first
value; explicit damage/kill/PowerSet controls confirm the rule. All90 integrated
race replays pass (Match61.287s), as do pure parser/compiler/combat races.
Thirteen apps/sixteen captures pass provenance/runtime checks and twelve strict
garment masks match exactly, including native KO poses. Nine fresh native
programs reproduce ten captures and one type rejection. Stock throw recovery
was inspected. Full scoped races pass in m4bb-full-race.log
(Match618.564s, Character11.012s, System5.027s). See HIT-LETHALITY.md.
Reversal:3fa397a9. Absolute KO/round timing, wider combat combinations,
remaining HitDef schema, stage diagnostic fill, six RGB alpha precision gates,
and full engine/assets/modes remain open.

## M4ba — Damage and power transfer

M4ba adds typed damage/GetPower/GivePower, repeated expression defaults,
configured zero/negative factors, shared root/helper pools and contact transfer
before entry. Sixty-six original programs compare804 observations, three type
rejections and264 source hashes. Full27-field source/reverse controls establish
the current typed evaluation order. Signed healing now adds.5 before truncation
and clamps life. All66 integrated programs/full scoped races pass in
m4ba-full-race.log: Match538.364s, Character12.143s. Eight apps/eleven captures
share one binary and pass provenance/runtime checks; seven clothing masks match
exactly. Four fresh native programs/six captures reproduce their observations.
Stock throw recovery shows two fighters; high configured factors visibly reach
both HUD level3 counters. Pure races and root module checks pass. See HIT-POWER.md.
Reversal:83eb5bd9. Whole combat/engine/assets/modes remain open, along with
M4az alpha precision and diagnostic background fill. M4bb subsequently covers the bounded lethal/fall-damage
behavior described above.

## M4az — RGB HUD and effect sampling

True-color fight HUD/effects now share the clamped linear sampler and preserve
caller blend/tint options. Cue coordinates truncate in signed common-game units
before stage/camera translation. Sixteen original programs record192 numbers
and80 input hashes; ten strict2100-pixel crops match exactly. Three AS192D64
cases each fail12 pixels max1; half/independent destination cases each fail117
pixels max1. These six alpha gates remain open with unchanged native goldens.
No precision allowance was added. All three HUD/world video-fit profiles now
match exact core bounds/hashes, closing the prior nearest-filter gap.

Five actor crops match byte-for-byte; three fresh native replays reproduce the
native goldens, including two alpha controls. Twenty-five app runs/28 captures
share binary7c40366876ea6cd4dee8a3b73eccd3581c8626b8fff4a017ef9751740f8f8394
and pass input/config/capture/runtime provenance checks. Stock throw recovery
shows two fighters without lingering effects. Pure races/root module checks
pass. The clean full scoped race check passes (Match489.826s,
Character10.368s), including all16 integrated consumer programs. The first
invocation found diagnostic Go snapshots in temp/; they were renamed to text
and its setup failure is retained separately. See RGB-CONSUMERS.md. Reversal:d572399f. Full RGBA/driver/effect/asset/
engine/mode gates remain open; combat power is the next functional checkpoint.

## M4ay — Fractional sprite sampling

M4ay matches fractional actor rendering with separate indexed/RGB samplers,
unsnapped quads, local UV precision, mirrored texel selection, measured boundary
bias and transparent edge coverage. Thirty-seven native programs compare 444
numbers and180 source hashes; all integrated comparisons pass. All 32 original
crops (2,176,000 RGB pixels) and28 legacy flipmark crops are byte-identical.
Six strict real-art masks match, closing the four M4ax failures and adding the
irregular400 control. Sixty-seven app runs/70 captures pass provenance/runtime
checks from one binary; stock throw recovery shows two fighters without lingering
effects. Six fresh native replays pass. Full scoped races pass in
m4ay-full-race.log (Match 461.719s, Character 10.938s). See SPRITE-SAMPLING.md.

The additional default HUD/effect ramp gate still fails color hashes:256 of288
pixels differ in each core, max channel 22, despite correct bounds. A pre-M4ay
binary produces exactly the same SHA as the final actor binary. Those consumer
files remain on nearest first-stage RGB filtering. This known pre-existing gap
is the immediate M4az task; its hashes must not be waived. Full RGBA/effects/
foreign/angle/zoom/background/driver/mode gates remain open. Reversal:931467ac.

## M4ax — Physical corner push

M4ax adds the separate physical corner-push impulse and its common-space
proximity/cutoff, context selection, double accumulation and float32 decay
coefficient. Fifty-one original programs compare 1,224 numbers and 204 authored
hashes. Direction/sign, zero/default/explicit, five hit/guard contexts, interior/
edge proximity, resolution, irregular precision, pause/freeze, state/target/turn
lifetime and deferred hit-facing match native. All 51 integrated comparisons
pass. Full scoped races pass in temp/m4ax-full-race.log (Match 439.097 seconds);
six fresh native replays pass. Ten actual app captures pass provenance/runtime
checks. Four fractional-position white masks differ by567/471/933/567 pixels;
the stationary control matches. This render gap is the immediate next task.
See CORNER-PUSH.md. Projectile-owner push and broader helper/foreign/bind/multiple/
zoom/camera combinations remain open, along with full combat/asset/mode gates.

The initial recorder's clock bug is retained as diagnostic evidence, not a final
expectation. Corrected22 baseline fails17. Large and irregular impulses reject
approximate arithmetic; threshold/proximity controls bracket exact common-unit
rules. Read-only beta call paths corroborate observed storage and comparisons.
Reversal:7af38317. Full/reference/runtime checks pass; the documented fractional
sprite mask gap remains open and will be investigated immediately next.

## M4aw — Hit velocity defaults and down-hit offsets

M4aw adds typed hit vectors and inherited corner-push expressions, separate
component defaults, stored down-contact selection and consumed float Y offsets.
Sixty-three original programs compare 1,020 numbers and 252 authored hashes with
no final rejected programs. Tagged assignments establish the native ten-field
order and repeated expression defaults; mixed profiles distinguish variable
storage from caller units. Down Y offsets are fixed/scaled and consumed after
CNS, before motion. All 63 integrated comparisons and pure races pass. Full scoped races pass in
temp/m4aw-full-race.log (Match 395.933 seconds). Six fresh native replays and
ten actual app captures pass; five stable sprite masks match exactly. Stock
recovery retains two fighters. See HIT-VELOCITY.md.
Physical corner-push movement remains missing; its evaluated/stored parameters
do not close that runtime gate. Full HitDef/combat/engine/asset/mode gates remain
open in HITDEF-SCHEMA.md and the compatibility inventory.

The initial36 comparison fails22. Additional assignment tags distinguish
re-evaluated source defaults from cached numeric values, including explicit
corner replacements and omitted vector components. Down phase/type/constant/
profile controls pin the offset without guessing a trip-constant relationship.
Two invalid constant authors were repaired before final expectations; their
missing-Velocity rejections remain diagnostic temp copies only. Reversal:
31795b0c. Final broad/reference/app verification passes for this checkpoint.

## M4av — Contact facing and shared entry phases

M4av adds typed P1Facing/P1GetP2Facing, contact-selected hit direction and
end-of-actor facing publication. Forty-eight original programs compare 552
numbers, two load rejections and 192 authored hashes. Miss/guard/override,
activation/contact headings, range/coercion/precedence, momentum, entry and
combined P2Facing controls match native. Shared entry fixes apply FaceP2 before
remaining expressions and defer ordinary get-hit Statedef until the receiver's
positive pass. All 48 integrated comparisons and pure/Character races pass. Full scoped races
pass in temp/m4av-full-race.log (Match 349.165 seconds). Six fresh native replays
and ten actual app captures pass; six sprite masks match exactly, and stock
throw recovery retains two fighters without persistent dust. See CONTACT-FACING.md.
The new HITDEF-SCHEMA.md audit identifies remaining parameter/consumer gates;
whole HitDef, combat, input, rendering, asset and mode compatibility remains open.

The initial20 fails8. The first34 implementation fails4 override/entry cases.
The deferred40 version retains two behind/receiver entry-expression failures;
ordinary/contact controls and written-order side effects isolate the two shared
entry corrections. Reversal point: dc90bb1e. No reference artwork/binary is
committed. Final verification is complete for this bounded checkpoint.

## M4au — Forced standing, dynamic knockback and velocity coordinates

M4au adds optional Forcestand, float32 ground.velocity expressions and correct
world-oriented GetHitVar X with facing-relative HitVelSet. Thirty-eight native
programs compare 540 numbers, two load rejections and 152 authored hashes.
Default/explicit posture, guard/override/entry phases, parameter side effects,
heading/motion and nonzero/coerced axis flags match. Full scoped races pass in
temp/m4au-full-race.log (Match 320.408 seconds); all focused/pure races pass.
Six fresh native replays and nine actual app captures pass. Five white sprite
masks match exactly, including moving/clipped receivers; stock recovery retains
two fighters without persistent dust. See FORCED-STANDING.md.
Remaining velocity schema, heading-change timing, full cross-field ordering,
binding/juggle/projectile/team/input/render/asset/mode gates remain open.

The initial 23-program run fails 14 cases. Five phase/order controls expose
Forcestand-before-ground evaluation despite source order, and the first typed
prototype retains two X-getter sign failures. Three facing controls independently
pin world X and initial/late facing-relative velocity/positions. Seven axis-flag
controls pin boolean selection, literal-float rejection and computed/bottom
zero. The old unit host's raw-X expectation is corrected to this native contract.
The registry adapter now delegates to the typed HitVelSet path; no duplicate
velocity implementation remains there. Reversal point: 8ab6f50d (M4at).

## M4at — HitDef flags and fall contact eligibility

M4at adds shared hitflag eligibility, explicit-empty distinction, downed state
5080 and typed ForceNoFall/fall continuation. Sixty-seven original programs
compare 792 numbers, one load rejection and 268 authored input hashes. Posture,
AF/MF, both modifier orders, non-H fall contexts and reciprocal priority controls
match native; all focused/pure races and the full scoped suite pass in
temp/m4at-full-race.log (Match 292.855 seconds). Six fresh native replays pass.
Nine actual app captures pass provenance/dimension/runtime checks; five stable
white masks match exactly (11,181 pixels each). Stock throw recovery retains
two fighters without persistent dust. See HIT-FLAGS.md. Complete down physics, guard/fall,
juggle, foreign/pause/projectile/team and remaining engine/asset gates stay open.

The initial pre-fix 49-program run fails 35 cases. Native accepts empty/Z values
as inactive, accepts quoted/lowercase flags, and requires A together with F for
falling airborne H targets. Independent AF/MF and non-H controls distinguish
posture from fall metadata. Both +/- orders exclude both idle and H; ineligible
attacks cannot suppress an eligible reciprocal attack. State L selects 5080.
Fall continuation, signed ForceNoFall, computed0.5 truncation, literal-float
rejection, bottom0 and own-fall precedence have independent controls.
Reversal point: 89b5a501 (M4as). No reference art/binary is committed.

## M4as — HitDef contact drawing priority

M4as adds typed HitDef p1sprpriority/p2sprpriority and assignment at contact,
before ordinary/custom state entry. Twenty-three original marker programs
compare 252 numbers, two load rejections, 21 native foreground identities and
115 authored input hashes. Defaults are 1/0; normal/guard/override/helper contact
and later destination entry match. Measured +/-99 ordering is retained; literal
floats reject, computed floats truncate and bottom becomes zero. Full scoped
races pass in temp/m4as-full-race.log (Match 247.463 seconds), as do all focused/
pure races and four fresh native replays. Twenty-two actual app captures pass
provenance/runtime checks. Six marker RGB/ID/rectangles match exactly; four
real-art white masks match exactly, resolving the bound-release 6,099-pixel gap.
See CONTACT-DRAW.md. Full HitDef, projectile/effect, pause/foreign/team,
renderer/asset/mode gates remain open.

The pre-fix run fails 14 programs with missing draw effects or accepted float
fields. Exact tie brackets distinguish defaults from inherited priorities.
Destination Statedef controls prove contact precedes own/custom/override entry;
the no-contact tie retains initial priorities. Helper/guard/override marker
colors identify the helper as foreground independently of Go's sorting.
The permanent actor probe now verifies actual Go colors/IDs/rectangles with
source/bank/stage/profile/binary/config/PNG provenance and forbids Go recording.
It also passes the prior reverse-helper capture. No projection change is made.
Reversal point: 8cae270d (M4ar). Root modules remain unchanged.

## M4ar — HitDef clash priority, hit counters and interrupted targets

M4ar adds typed HitDef priority/numhits, reciprocal Hit/Miss/Dodge arbitration,
HitCount/UniqHitCount credit and incoming-hit target release. Sixty-three native
programs compare 996 numbers, five load rejections and 252 authored input hashes.
Higher integer priority wins; equal no-hit ties remain armed. Helpers trade with
root receivers before helper receivers, independent of team. Guard/manual H
changes preserve targets; unguarded hits/overrides release them, sending valid
bound H victims to their own state 5050. Full scoped races pass in
temp/m4ar-full-race.log (Match 231.848 seconds), as do all focused/pure races
and six fresh native replays. Sixteen actual app captures pass provenance,
dimension and runtime/resource checks. Three stable white masks match exactly;
the bound-release overlap differs at 6,099 pixels and is the next drawing-
priority investigation. See COMBAT-PRIORITY.md. Full hitflags/protection/projectile/
pause/team combinations and remaining engine/asset/mode gates stay open.

Initial pre-fix comparison fails all 25 original controls, including unresolved
HitCount. The first 52 then pass; nine helper/multi-target controls and two counter
persistence controls expose three target-list failures. Reversed helper teams
pin receiver class independently of root order. Root-before-helper contact
mutation fixes them without changing clash winners, damage or hitonce selection.
The initial helper snapshot incorrectly used root-only state -3 and produced an
all-zero helper page; final native evidence publishes from positive states.
All final 63 pass. Source authors/catalogues and rejected intermediate runs remain
under ignored temp/. No reference executable or stock art is committed.
Reversal point: 6f90691f (M4aq). Root module hashes remain unchanged.

## M4aq — Actor flip/offset coordinates and native raster alignment

M4aq corrects actor AIR offsets, flipped-axis pixel phase and draw-axis flooring.
Twenty-eight native programs compare 336 numbers, 140 input hashes and 28
original-marker rasters across 320/640/1280 render widths, combined/cancelled
flips, clipping, fractional positions and helper sizing. Final full races pass
in temp/m4aq-final-full-race.log (Match 190.318 seconds); all focused cases and
four final independent native replays pass. All 28 actual Go crops match exact
geometry; 24 are byte-identical and four have quantified one-channel-level
rounding at one/four blended corners. Strict checking remains default; the
opt-in edge allowance preserves exact geometry/pure pixels. See
SPRITE-ALIGNMENT.md. No whole renderer/asset/engine compatibility claim.

Thirty-nine final-code 1280x720 captures pass source/binary/config/PNG/dimension/
runtime/resource checks, from one identical binary. Reverse-helper color/order/
rectangle matches. Real-art overlap and helper-fall clothing masks match exactly:
22,362 and 16,608 white pixels respectively, zero differences versus the previous
408/801 (overlap was 12,036 before M4ap ordering). Stock recovery has two fighters
without dust clones. Results: temp/m4aq-axis-raster-summary.json and
m4aq-final-app-checks.json. Background/debug/HUD, four tiny corner-precision
exceptions and broader render/asset/combat/mode gates remain open.

Facing multiplies AIR X offset independently of effective image flip. Y offset
keeps direction. Effective flipped axes add one game-surface pixel after sprite/
local scaling. Projected draw axes floor before combination with fractional
image axes/scales/frame offsets; CNS positions stay fractional. Negative clipped
axes distinguish floor from truncation, and a 1.1 helper plus AIR X=1 locates the
operation before local offset/scaling. Physical window overrides isolate game
resolution from the native default window-size change.

The initial three app failures are all corrected byte-for-byte. Expanded crop
checks reveal half-pixel axis failures, fixed by independent negative/fractional
controls. Four sparse blended-corner differences retain max one channel level;
strict SHA remains available. Explicit precision mode permits <=4 nonblack/
nonpure-red pixels, never altered bounds/counts, validates original native RGB
against its SHA, and still rejects the known shifted geometry. No color hack or
Go-derived oracle is introduced. Compressed crops contain only original marker/
black data; no reference art/binary is committed.

The earlier full log precedes axis flooring. Final full races, all 28 final-code
crops, 39 capture checks and four final native replays pass. No whole-engine
promotion; root module hashes remain unchanged. Reversal point: 8de8d697 (M4ap).

## M4ap — Global actor sprite order and native color calibration

M4ap adds global actor sprite ordering and native SprPriority boundaries.
Fourteen original programs compare 156 numbers, one load rejection, thirteen
foreground identities and 70 CNS/AIR/CMD/stage/SFF hashes. Higher priority wins
across root/helper ownership; lower world ID wins ties, including reversed
helper birth. Bottom becomes zero, computed floats truncate, literal floats
reject, and measured +/-99 values retain ordering. Full scoped races pass in
temp/m4ap-full-race.log (Match 172.121 seconds); all focused and pure races pass.
Three fresh native color/rectangle replays and the float rejection pass. See
ACTOR-ORDER.md. Complete effect/projectile/foreign/render/asset gates remain open.

Twenty real 1280x720 captures pass provenance/dimension/nonblack/runtime/resource
checks. All thirteen accepted actororder-*-go-final scenes match native foreground
RGB/identity; eight pure-color rectangles match exactly and five flipped scenes
retain the independently measured edge gap (native 9,120 versus Go 9,216 pixels).
The real-art scene at temp/app-runs/actororder-real-overlap-go/ (3/5/7 seconds)
reduces clothing-mask differences from 12,036 to 408 pixels, with correct
foreground facing. Native/Go white counts remain 22,362/22,626. Stock recovery at
actororder-stock-throw-go/ (2/3/4/7) retains two fighters without dust clones.
This is ordering/smoke evidence, not full framebuffer parity. Native debug magenta,
background/HUD/filtering and flipped edge sampling remain open. Measurements:
temp/m4ap-app-comparison.json.

The pre-fix real root/cross-helper windows both show green instead of native
red/blue. A shared OrderedActors list replaces family-recursive fight drawing;
standalone player drawing uses the same order and keeps each sprite-bank owner.
The native color golden maps solid marker colors to observed EntityIDs, checking
CNS/AIR/CMD/stage/SFF/executable/capture provenance independently of Go sorting.
The corrected birth control makes P2 A in state 0; the earlier draft changed
A too late and did not reverse creation. The original SFF generator needs no
reference art or external compiler. An initial incorrect version header is
retained only as a rejected diagnostic under temp.

Typed SprPriority fixes the only two boundary mismatches (bottom/float) after
12/14 first-pass programs. Thirteen accepted runtime programs and the rejection
pass; actual marker windows independently exercise the renderer. Native exact-
color rectangles expose a separate flipped edge sample, not a sort error.
No guessed raster offset or full controller/render/asset claim. Root modules
remain unchanged. Reversal point: fb344800 (M4ao).

## M4ao — HitDef one-target and team selection

M4ao adds typed HitDef hitonce/affectteam, single-contact consumption and old-
target dropping. Twenty-eight native programs compare 300 numbers, three load
rejections and 112 authored input hashes. All throw classes default to one;
guard consumes the contact. Independent root/root, root/helper and helper/helper
controls pin roots before helpers, enemy roots before friendly roots and helper
creation order regardless of team. Full scoped races pass in
temp/m4ao-full-race.log; all focused cases and four fresh native replays pass.
See HIT-SELECTION.md. Priority/reversal/override/projectile/team-mode and full
binding/geometry/render/asset gates remain open.

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

The initial fifteen-program comparison fails twelve cases. Typed argument and
candidate changes pass 22/24; bottom-as-zero and root ordering explain the two
remaining differences. Four further ordering controls separate root priority,
root team preference and helper creation order. All 28 final programs pass.
Literal float/invalid team/quoted team load errors validate with state/source
context. Computed floats truncate; bottom becomes zero only at the measured
hitonce argument boundary. Successful replacement drops previous target labels.
No change is made to root-only Enemy/NumEnemy or legacy projectile ownership.

Full race regression, four fresh native controls and seven actual window
captures pass; the overlap visual discrepancy remains a separate renderer gate.
No whole HitDef/helper/target/controller claim. Root Go module hashes remain
unchanged; no reference binary/art is committed. Reversal point: 0482a7b5.

## M4an — Helper defenders, target identities and birth contact timing

M4an adds helper defender candidates and measured incoming-contact activation.
Ten native helpercontact programs compare 204 numbers and 40 authored input
hashes. Normal/player/multiple helpers take damage, enter their own get-hit
state and acquire independent target IDs; same-team helpers remain unharmed
under the default enemy rule. Incoming contact begins two world ticks after
birth. Independent negative/positive-state controls confirm the existing
per-entity ordering. Full scoped races pass in temp/m4an-full-race.log; focused
races and four fresh native replays pass. See HELPER-COMBAT.md. Complete combat,
HitDef selection, projectiles, pause/foreign/geometry and rendering remain open.

Seven real 1280x720 captures under temp/app-runs/helpercontact-two-go/
(3/5/7 seconds) and helpercontact-stock-throw-go/ (2/3/4/7) pass provenance,
dimensions, nonblack and runtime/resource checks. Stock recovery retains two
fighters without permanent dust clones. The helper scene shows both victims;
its native/Go white clothing masks contain 19,993/19,692 pixels and differ at
1,507 pixels. This is a smoke check, not framebuffer parity. Existing mirrored/
clipped placement, native debug magenta versus Go black, background filtering
and HUD differences remain renderer obligations. Measurements:
temp/m4an-app-comparison.json.

The first implementation used root-only defenders. Adding live opposing helpers
fixes life/state/targets but exposes MoveHit/MoveContact=70 instead of native
69 when a root and newly created helper overlap. Early/late attacker creation
controls pin the helper's first incoming contact at birth+2; outgoing attacks
remain available earlier. Negative-state clock/variable controls disprove the
initial suspicion of separate negative/positive scheduling. Existing pending
GetHitVar damage consumption also already matches.

Combat candidates are separate from root-only Enemy/NumEnemy queries. Legacy
projectile opponent-list handling remains unchanged. Full Match races include
all previous binding/throw/contact/pause/asset/sound regressions. Native replay
and seven window checks pass; no full combat/controller/renderer claim. Root
Go module hashes remain unchanged. Reversal point: 5f69348a (M4am).

## M4am — Helper binding phases, retirement fall and deferred hit facing

M4am adds anchor-completion binding publication, idle-anchor constraints,
external 5050 entry on bound-helper retirement and deferred typed P2 hit facing.
Twenty-seven native programs compare 564 numbers, one load rejection and 108
input hashes. Third-entity/own-CNS flag controls, changing speed, own/borrowed
states and reversed headings distinguish the phases. Full scoped races pass in
temp/m4am-full-race.log; focused/pure races and five fresh native replays pass.
See HELPER-BINDING.md. Whole binding/combat/helper/render/asset/mode gates remain
open; the helper fall's late clipped right sprite still has a small raster gap.

Binding position/velocity/flag publication moves from target preparation to
anchor completion. A third actor can read the completed request before the
victim runs; an idle anchor still constrains movement while the victim's own
flag is zero. General H/A/I sorting remains unchanged. Helper retirement queues
external 5050 only for a valid bound H victim; borrowed unbound states survive.
Changing-speed snapshots pin current position/velocity, normal entry attributes
and same-tick/next-tick entry at GameTime 134/135. Typed p2facing rejects literal
1.5, keeps omission unchanged and applies +/-1 after the first victim CNS while
preserving world velocity. Reversed headings distinguish absolute from relative
turn assumptions; value 2 leaves facing unchanged.

All 27 focused programs pass in temp/m4am-helper-second-race.log. The first
integrated comparison has one idle-victim flag mismatch in the first log;
pre-fix failures are in m4am-before-binding.log. Pure engine/evaluator races
pass in m4am-pure-race.log; the full race suite includes every previous
throw/contact/binding/pause/camera/sound/asset regression. See HELPER-BINDING.md.

Seven real 1280x720 app captures pass hashes/dimensions/nonblack/runtime-log
checks: temp/app-runs/helperbind-idle-fall-go/ (3/5/7 seconds) and
helperbind-stock-throw-go/ (2/3/4/7). The stock throw recovers with two fighters
and no permanent dust clones. The helper probe takes the native fall path;
its source intentionally omits standard state 5000 and retains that native
warning. The stationary left sprite's clothing mask matches exactly. The late
clipped right sprite starts at x=1003 in Go versus native 1004, with identical
vertical bounds; white counts are 8316/8229 and the full mask differs at 801
pixels. Late numeric position, mirrored placement and clipping still need an
independent calibration. This is not full framebuffer parity. Native debug
magenta/Go black and minimal background filtering differences remain open.
Measurements: temp/m4am-app-comparison.json.

Five fresh native controls pass, including the float rejection after repairing
the wrapper's missing page argument. State ownership after forced fall with
independent opponent files, attacker-hit interruption, pause/foreign/multi-target
binding, opposing-helper contacts and all other full engine/asset/mode gates
remain open. Root module hashes remain unchanged; only authored in-scope data
is tracked. Reversal point: a3cadeab (M4al).

## M4al — Native helper sound ownership and retirement evidence

M4al adds native helper sound ownership controls: two programs, 24 numeric
observations and eight PCM tick checks (48 stereo tone amplitudes). Root/parent/
descendant voices coexist on channel 1. Both ordinary and recursive DestroySelf
retire entities while their sounds continue; root channel-1 StopSnd removes only
the root voice. Existing production behavior matches, so no stop-on-retirement
change is introduced. All 29 PCM programs/30 cases pass race comparison in
temp/m4al-pcm-first-race.log; both fresh native replays pass. See AUDIO.md.

The numeric packet pins retirement/counts and GameTime=430 at Time=300. The
state-only runner starts at the previously established control boundary 130,
checks CMD hashes/language versions and shares a timeline. PCM windows precede/
follow retirement and root StopSnd; capture-shutdown silence is excluded from
semantic checks. Tone generation and the quiet-HUD recipe are reproducible.

The real Go device path at temp/app-runs/helperaudio-recursive-go/ produces
native-equivalent tone plateaus before/after retirement. Four 1280x960 windows
at 3/5/7/9 seconds pass hashes/dimensions/nonblack/log checks; PCM peak is 11,307,
RMS 3836.95. Raw PCM and tone measurements remain task-private under temp.
Full scoped tests last passed at M4ak; only a production comment, test harness,
authored fixtures and documentation change here, with all PCM cases rerun.

Still open: native binding publication, helper/projectile combat, sound capacity/
stealing/round/pause/foreign banks and other engine/asset/mode gates. No full
sound/helper controller claim. Root modules remain unchanged; synthetic tones
are original data, and no reference art/binary is added. Reversal: f8e2176b.

## M4ak — Helper retirement, parent snapshots and orphan Explod clocks

Fourteen helperretire programs compare 288 numbers, two intentional load
rejections and 56 authored input hashes. Typed recursive/removeexplods arguments
validate before effects; negative/non-one/computed recursion and bottom/default
behavior match native. Nonrecursive descendants survive with retained Parent
metadata; live registry lookup, parent writes and family participation retire.
The retiring parent's final physics/time step remains observable. Recursive
removal retires the full descendant tree in stable creation order.

Retained Explods now advance through the world after helper removal instead of
freezing indefinitely. Flags select removal versus orphaning for the helper and
its descendants. Anchors to retired actors release, finite effects expire and
round reset clears orphan resources. Native default postype=none keeps the
visual scene positions in stage space; binding-release publication still needs
an executable control beyond the manual/direct lifecycle regression.

Focused comparisons and clock/expiry/binding/reset tests pass in
temp/m4ak-focused-race.log. The engine race regression rejects an invalid later
argument before earlier assignments (m4ak-engine-race.log). Pre-fix divergences
remain in m4ak-before-retirement.log. Full races include every prior helper,
contact/throw/guard/pause/ownership/effect/asset regression. See RETIREMENT.md.

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

Remaining: helper sound ownership, native binding publication, retired-state
assignments/remaining queries, helper projectiles, death/KO versus DestroySelf,
pause/foreign combinations and full multi-entity combat. No whole helper gate
is complete. Root module hashes remain unchanged; all authored edits stay in
scope and no original art/binary is tracked. Reversal point: 0672ee13 (M4aj).

## M4aj — Helper configuration, entity order and intro/camera corrections

Seventy-two native helpercfg programs compare 984 numeric observations, eight
intentional load rejections and 276 authored CNS/AIR/stage/CMD hashes. Helper
arguments compile before effects; per-host expression values and copied size
constants do not mutate shared source/parent constants. Birth defaults/entry,
missing/negative states, postype/facing, names/counts, normal/player negative
eligibility and separate paused-tick allowances match their controls.

Root/helper scheduling is one flat ordered pass, with new helpers appended.
Nested creation distinguishes this from recursive child updates; existing
attacking helpers precede idle roots on subsequent ticks. Normal/player helper
counts stay separate from enemy/partner counts. Player helpers clamp at birth;
camera participation starts on their first own pass. Camera easing observes
each root/helper completion, including excluded helpers.

Far-camera packets exposed separately rounded float32 endpoints and movement
deltas, maximum-span correction and tiny zoom rounding. ScreenPos/edge queries
keep double precision through division before their final script value. This
supersedes M4ag's single-left-edge model. Five intro packets exposed the early
RoundState 2/control grant: Intro now needs ten consecutive clear ticks and
RoundState 2 starts with the fight announcement. Empty/five/twenty active
assertions grant control at GameTime 130/135/150.

Validation: the full scoped race suite passes in temp/m4aj-full-race.log;
display-independent engine/evaluator races pass in m4aj-final-pure-race.log.
Earlier 43/65-program failures remain in m4aj-helper-first-race.log and
m4aj-helper-second-race.log; the corrected first 65 pass in the third log.
Final full races include all 72. See HELPERS.md for measured details/recipes.


Real 1280x720 windows at temp/app-runs/helper-size-go/ (2/4/7 seconds) and
helper-stock-throw-go/ (2/3/4/7) pass all seven capture hash/dimension/nonblack
checks, with no runtime/missing-resource errors. The scaled helper's white
clothing occupies exactly [643,401,943,499) in both native and Go, with 10,381
thresholded pixels in each. This pins the measured 2.5/.5 scale and nonzero Y;
it is not full framebuffer equivalence. Native debug magenta versus Go black
outside the minimal stage, background filtering and other rendering rules
remain open. The inspected stock recovery has two fighters and no dust clones.

Fresh native key-one, phase-attack, camera-enabled-far, camera-endpoints,
pause-two, flow-active-twenty, nested-order and size-scene replays use
m4aj-replay-* run directories and check all pinned values/input hashes.

Still open: opposing-helper contacts, complete destruction/orphan/effect/binding
ownership, positive input, ownpal/remappal RGB, foreign/paused creation, proper
projectiles and the full vertical/zoom/render/audio/mode/asset gates. No whole
helper/controller gate is promoted to complete. All authored files stay in
scope; reference art/binaries remain untracked and root module hashes are
unchanged. Reversal point: M4ai commit 31133c14.

## M4ai — Width offsets, body push and horizontal edge queries

M4ai adds typed Width, temporary edge/player offsets and eight horizontal edge
query bindings. Thirty-one native programs compare 336 numbers, three load
rejections and all 93 authored input hashes. Ground push, one-pass reset,
negative offsets, replacement/parameter precedence, nonzero-camera edges,
asymmetric margins and mixed-profile redirection match the controls. P2BodyDist
uses base CNS widths independently of Width and selects the opponent's nearer
edge. Full scoped races pass in temp/m4ai-full-race.log and pure engine/evaluator
races in temp/m4ai-pure-race.log. See WIDTH.md. Full helper/foreign/air/pause/
movement/zoom/zero-axis and all other engine/asset/mode gates remain open.

Four fresh native reset/negative-offset/replacement/mixed-redirect replays
reproduce their pinned observations and input hashes. Real 1280x720 windows
at temp/app-runs/width-push-go/ (2/4/7 seconds) and width-stock-throw-go/
(2/3/4/7) complete without runtime/missing-resource errors. All seven capture
hash/dimension/nonblack checks pass. The push scene shows the expanded spacing;
the inspected stock recovery retains two fighters and no permanent dust clones.
The minimal probe background and full filter/framebuffer equivalence remain
renderer obligations; these captures are smoke checks.

The first 26 native programs pass focused races in
temp/m4ai-width-first-race.log. Additional ignored-float, player-over-value,
asymmetric/reversed margins and mixed-profile edge redirection bring the final
corpus to 31; the full suite includes them all. Empty Width rejects despite the
manual; explicit edge/player causes value to be ignored, even its otherwise-
invalid float literal. Width adds to margins/CNS push widths, while body-distance
queries remain based on CNS sizes. No original art/binaries are tracked and root
module hashes remain unchanged. Reversal point: `2dbb1eed` (M4ah).

## M4ah — Screen confinement, stage margins and camera eligibility

M4ah adds typed ScreenBound, per-pass root/helper defaults, converted stage
screen margins and root camera eligibility. Forty-seven native programs pin
924 numbers, four controller rejections, three stage rejections and all 141
authored input hashes. Fixed/mixed/nonzero-camera edges, retained velocity,
PosFreeze, one-tick reset, stage bypass, ordinary helper confinement and camera
axis coupling match the measured controls. Full scoped races pass in
temp/m4ah-full-race.log; pure engine/evaluator races also pass. SCREENBOUND.md
records exact values and grammar, including accepted negative/fraction/empty/
word stage margins. Complete pause/bind/Width/helper-camera, vertical/zoom and
renderer obligations remain open; no whole-controller compatibility claim.

Six fresh native replays reproduce reset, constant coupled-axis freezing,
normal-helper confinement, float-literal rejection, 640-stage scaling and a
missing-margin stage rejection. Real 1280x720 windows at
`temp/app-runs/screenbound-motion-go/` and `screenbound-stock-throw-go/` run at
2/3/4/7 seconds. All eight PNG hashes/nonblack checks pass, with no runtime or
missing-resource errors. The stock throw recovers with two actors and no
permanent dust clones. The box-free motion scene eventually overlays the two
actors at the right limit; its minimal background still ends during scrolling.
These are window/asset smoke checks, not full native framebuffer parity.

The old no-op/default-edge failures are preserved in
temp/m4ah-before-screenbound.log. Additional helper controls expose the old
160-edge clamp and incorrect default confinement in temp/m4ah-before-helper.log;
the final helper defaults and inherited margins resolve both. Stage missing
attributes now report path/[Bound] errors; accepted zero/negative/fraction/
empty/word values retain measured beta behavior. Root module hashes remain
unchanged. The reversal point is M4ag commit `aa35d0a3`.

## M4ag — Horizontal camera tension, easing and root phases

Twenty native programs pin 720 numbers and 60 authored input hashes. The
horizontal camera now retains a tension-constrained goal, eases after each
completed root and in the shared late phase, and clamps to the goal/stage
limits. Float32 left-edge rounding reproduces native query values. A phase
program distinguishes earlier/later root observations; long/return controls
distinguish finite arrival and retained goals from recentering or stopping early.
See CAMERA.md for exact values and the bounded zoom-one/320-stage scope.

The first ten static/phase cases pass without proving motion. Added walking
cases exposed a missing speed floor imposed by the actor beyond the tension
edge. Interior-only movement leaves the static trace unchanged; extreme-only
movement and mixed 1280/320 profiles retain the measured physical trace. Speeds
.5/1/2/3 distinguish the floor from ordinary easing. Full scoped races pass in
temp/m4ag-full-final-race.log, retaining all earlier camera/combat/asset/flow
regressions. Four old-average failures are in temp/m4ag-before-camera.log.

Four fresh native phase/return/slow-motion/stage-limit replays reproduce their
pinned values and hashes. Actual 1280x720 motion and stock throw windows are
inspected at temp/app-runs/camerafollow-motion-go/ (2/3/4 seconds) and
stock-throw-camera-go/ (2/3/4/7). Seven capture hashes/nonblack pixels pass,
without runtime/missing-resource errors. The throw still recovers with two
fighters and no permanent dust clones. The minimal motion stage exposes the
remaining screen-edge clipping and limited background/debugbg rendering;
window smoke checks do not establish full native raster/geometry parity.

World exposes its existing root completion point to Match; nested child
adapters do not emit root observations. The camera goal resets with the round.
Vertical/1.1 edge tension, zoom, complete bounds/ScreenBound/movecamera,
helper/projectile/priority/pause effects, every stage/default/profile and full
renderer equivalence remain open. M4af's reversal point is `7306da2e`.

## M4af — Screen-centered positions with nonzero camera controls

Seven native programs compare 192 numbers and two original RGB anchors. Pos X
and PosSet X use the screen center; internal actor storage remains absolute.
Pos Y stays floor-relative, and P2Dist subtracts two positions in the same
basis. Positive/negative camera offsets, vertical displacement, mixed widths,
omitted axes, facing-relative PosAdd and borrowed state/SelfState conversions
are compared. The old getter/setter errors canceled in Pos-only samples;
ScreenPos exposes them. Four old-binary failures are preserved in
temp/m4af-before-camera.log. See COORDINATES.md for exact observations.

Authored fixed-camera stage sidecars have checked hashes, are loaded through
the production stage loader in Go, and are copied only into disposable native
runs. They control camera inputs without claiming camera-follow equivalence.
Own/common stage Explods using Pos X+CameraPos X both project to native anchor
(30,-10) on camera (80,-20). The real 1280x720 Go windows at
temp/app-runs/cameramark-{own,common}-go/ are inspected at 2/4/7 seconds. Their
PNG hashes match across banks. Position agrees, while native/Go filtered RGB
core extents remain different and are recorded separately.

Full scoped races pass in temp/m4af-full-first-race.log, retaining all prior
regressions; pure engine/evaluator/effect/coordinate races also pass. Four fresh
native negative-camera/borrowed-state/own-common RGB replays match. All 25 authored
inputs are hash checked. Root module
hashes remain unchanged. Initial native stage-path and clipboard-attachment
attempts were harness diagnostics only; final pinned programs use the corrected
stage invocation and CMD clipboard. The early edge-clamped spawn control is
also excluded. No screen-boundary or camera-follow fix is inferred from it.

Complete camera follow/easing/zoom, bounds, precision edges, all other controller
coordinate boundaries and remaining world/asset/input/render/audio/mode gates
remain open. M4ae's reversal point is `0ff0f321`.

## M4ae — Owned Explod resources, lifetime and calibrated placement

Twenty-seven native programs compare 636 distinct per-program observations,
two float-literal load rejections and nine RGB marker positions. Typed Explod
parameters replace the literal parser that interpreted common F(expression)
as own animation 0. Character-owned animation/lifetime instances are shared
with the renderer without double advancement. NumExplod and immediate removal
follow those same objects, and retirement includes queued owned instances.
Default IDs/negative IDs become zero; negative lookups/removals select all.

Native counters remain active through the last CNS pass before shared late
expiry. Zero removal retires in its creation tick; finite, positive, indefinite
and below--2 lifetime cases are distinguished. Missing own/common actions
create nothing. Despite the manual's required label, omitted anim is accepted
and creates no object. Common F60 uses the real common animation rather than
an authored infinite own action 60. See EXPLODS.md for exact limits/defaults.

Nine owned-window RGB calibrations pin own/common stage placement, P1/P2,
screen space, both front directions, back facing left and an explicit-stage
edge control. An early front+explicit-stage marker with Y=100 was off-screen;
the promoted visible control proves stage Y is retained while edge X converts.
Sprite bank selection and geometry are compared in the production simulation.
Full framebuffer, dynamic camera and complete priority equivalence are open.

Full scoped races pass in temp/m4ae-full-markers-race.log, including all prior
stock/contact/guard/pause/sound/effect/ownership packets. Pure tests pin shared
owned clocks/retirement and host-local, pre-effect parameter compilation. Eight
fresh native replays match lifecycle, omission/rejection, resources and RGB
space cases. Root module hashes remain unchanged; original resources are
untouched. Final repaired stock window checks accompany this checkpoint.

All final normal/forward/drop 1280x720 windows were inspected through 2/3/4/7
seconds at temp/app-runs/stock-throw-{base,forward,drop}-effects-go/. They recover
with two fighters and no permanent dust clones, without missing-effect warnings
or runtime errors. The ordinary throw was also repeated in
stock-throw-base-effects-recheck-go/. A black preview was a display artifact:
the stored original/repeat recovery PNGs have identical hashes and substantial
nonblack pixels. app_smoke.py now rejects entirely black quick-match captures.
These remain window/asset smoke checks, not full camera/filter/layer/audio parity.

The task-owned Xvfb server exited during this work; it was restored with the
original authenticated 1280x960/noreset configuration. A failed display init
ran no assertions and is not a test failure attributed to the engine. No
user desktop/audio settings changed. M4ad's reversal point is `e635ea8c`.
Full bindID (currently a contextual unsupported-binding error), ModifyExplod,
ExplodBindTime, motion/pause/palette/3D/transparency/ownership and all remaining
engine/asset gates remain unfinished.

## M4ad — Stock throw composition and one-tick contact poses

Eight native programs compare 228 numbers. Four load the original KFM throw,
common state and AIR files through hash-pinned Files overrides: normal throw,
forward branch, released-target escape and an independent GameTime start stamp.
The first throw-relative trace could match despite Go delaying the grab until
action 800 wrapped. The stamp exposes native one tick versus the old Go 22.
Four independent attack/hurtbox window programs distinguish first/second
one-tick elements. Contacts now use the preceding simulation pose saved before
AIR advancement; the direct collision API still uses current elements.

The stock TargetDrop escape also exposed assigning the omitted fall-X sentinel
as velocity. HitFallVel now preserves X for that omission. The victim stays at
native X=42, enters its own fall/get-up states and loses no life. Normal and
forward throws apply 78 damage, release their last bind, land, get up and drop
from the acquired list. See THROWS.md for exact samples, fixture controls and
remaining obligations. The forward wrapper sets the stock throw's var(2),
which exercises its branch without claiming command-input coverage.

Full scoped races pass in temp/m4ad-full-first-race.log, including all previous
contact/guard/pause/ownership/spark/sound cases. Every authored input and the
original CNS/AIR overrides are hash checked. The reference harness records
resolved Files hashes and checks them on replay; stock scheduler runs use 240
ticks. Four fresh replays match start/drop and independent first/second windows,
including the pinned original Files hashes. Simulation-only tests omit sound
banks; production asset/window checks are recorded below. Original reference
files remain unchanged.

The normal, forward and TargetDrop variants also run through real 1280x720 Go
windows at temp/app-runs/stock-throw-{base,forward,drop}-go/. Captures at 2/3/4/7
seconds show the stock lift in both directions, released fall and final recovery.
Original sprite and sound banks load; no runtime errors appear. These are owned
window/asset smoke checks, not framebuffer or complete audio parity.

The final normal/forward captures expose a remaining visual bug: common landing
Explods using anim=F(expression) fall through the legacy literal parser to own
action 0 and infinite lifetime. They leave extra standing KFM sprites after
recovery. State/health/target observations pass, but a clean visual throw is not
yet accepted. Next connect typed Explod resource lookup and lifetime to the shared
effect scene, then repeat these captures. The dropped variant and the scripted
state tests do not excuse the normal/forward rendering defect.

This does not complete HitDef facing/hit flags/priority, dynamic Width, new-hit
interrupted bindings, complete helper/projectile phases or the other engine/
asset/mode/input gates. M4ac's reversal point is `c9e70791`.

## M4ac — HitDef custom states, binding publication and infinite AIR clocks

Thirty-eight original native programs pin 1244 distinct per-program numeric
observations and three load rejections. HitDef custom-state fields compile as
host-local expressions before effects. Attacker/victim transitions select
numbers immediately, defer entry until after negatives and use the measured
state owner/coordinate profile. A custom victim supersedes the standard get-hit
entry attributes while retaining its number as PrevStateNo. Missing external
states also defer contact-counter reset. See CUSTOMHIT.md.

Native controls distinguish borrowed/own states, -3/-2/-1 eligibility, first-pass
life before pending damage, negative/omitted/missing fields, literal-float load
errors and computed fractional coercion. Matching HitOverride excludes custom
hits before contact; explicit/default borrowing flags matter even without an
effective P2 destination. Ordinary overridden H states do not acquire targets.

GetHitVar(isbound) is an integer published by the subject's last update; requests,
cancel/drop and expiry do not prematurely replace that last observation.
Duration/recovery programs distinguish publication from physical constraints.
Infinite AIR AnimTime advances as elapsed+1. An accepted nonfinal -1 has the
native finite clock/element starts rather than permanently holding that frame.
The old infinite/unreachable unit assumptions were not native evidence and are
replaced by three pure clock comparisons and production scheduler packets.

The full scoped race suite passes in temp/m4ac-full-final-race.log. Parameter
cache isolation, pre-mutation validation and untouched direct-hit adapters also
pass. The initial distinct-
animation control lacked Clsn2 and made no contact; its zero-valued run is
explicitly diagnostic. The corrected contact control has independent AIR hashes.
Seven fresh native replays match entry, overrides/sentinels, cancel publication,
nonfinal AIR, fractional flags and integer-literal rejection. A real 1280x720
Go window runs the mixed-profile custom-entry case through seven seconds at
temp/app-runs/customhit-entry-go/. Both actors render; the authored victim uses
320-art in a 1280 profile and is correspondingly small. This is process/render
smoke, not framebuffer or stock throw parity. Root module hashes are unchanged.

The stock KFM/KFM720 regression window also runs at 1280x720 through seven
seconds at temp/app-runs/customhit-stock-regression-go/, with both full-sized
actors and no runtime errors. This remains a smoke check, not pixel parity.

Full stock throws still need facing/priority/Width/interruption comparisons;
complete HitDef, bind-family/helper/world, input/render/audio/mode/asset gates
remain open. M4ab's reversal point is `4d586c69`. No whole controller is Verified.

## M4ab — Target binding, literal grammar and native hurtbox push gating

Fifty-three original native programs pin 840 numeric observations and ten load
rejections. Typed TargetBind queues actor constraints with duration/default/
negative rules, anchor-relative offsets, mixed units and inherited world velocity.
Binding applies before the target's negative pass; own CNS writes and shared
late push can change its position. Expiry keeps velocity; renewal/cancel,
TargetDrop, new hits, recovery and H-to-H retention are distinguished. See
BINDING.md for the exact scope and remaining controller/entity/pause cases.

Literal float integer parameters are rejected at load, including grouped/signed
literals; float-valued expressions are coerced at runtime. Tuple omission/arity,
redirect/function commas and bottom-zero boundaries have native packets and
contextual Go validation. Turn now preserves world momentum.

Independent H/I and hurtbox-present/absent controls disproved the first H-only
push inference: both move types push with hurtboxes, while missing Clsn2 excludes
push. The provisional H exclusion was removed. Horizontal extents still use CNS
widths; bound actors can be moved by the shared late push phase.

Full scoped races pass with every packet. Seven representative fresh native
replays match numeric/default/mixed/lifecycle and load-rejection observations.
The real 1280x720 Go with-box match runs through seven-second captures without
runtime errors; actors reach an edge under sustained scripted movement. Camera/
body-edge/framebuffer parity is not claimed. Root module hashes remain unchanged.
Full throws still
need HitDef state fields; other binding controllers, complete Pause/SuperPause,
foreign/helper/team/retirement and rendering/input/mode gates remain open.
M4aa's reversal point is `9b07b9af`. No whole controller is promoted to Verified.

## M4aa — Per-root CNS/damage/physics clocks and shared late push

Three independent native programs pin 48 numbers. A later root sees the earlier
root's completed movement and Time increment; attacker priority covers this
entire root update. A two-sided trade shows the second root's Time=0 CNS sees
the first root's reduced life, while its own life is still unchanged. The old
global post-CNS damage assumption is superseded. See WORLD.md.

Each root now completes CNS, pending damage, physics and clocks before the next
root's CNS. Body push stays in a shared late phase; the initial per-root-push
draft broke old native push/guard/viewport packets, and splitting it restores
them. The complete scoped race suite passes, including pause/audio/effect/target/
ownership tests. Fresh replays of all three native packets match. The real
1280x720 mixed KFM/KFM720 fight loads/runs through seven-second window captures
at temp/app-runs/root-phases-stock-go/ without runtime errors. Root module
hashes remain unchanged. This is a smoke check, not complete fight parity.

Binding probes exposed the gap but binding is still in progress. Helper/
projectile/team/neutral scheduling, complete KO/camera phases, binding and HitDef
custom-state parameters remain open. M4z's reversal point is `adae7dfd`.

## M4z — Borrowed states, animation sources and external entry timing

Eleven native programs pin 200 distinct per-program numeric observations (216
capture entries with overlapping pages). Independent opponent CNS/AIR/CMD files
make state, animation and constant ownership distinguishable. TargetState now
selects borrowed definitions; ChangeState retains them and SelfState restores
the actor's own definitions. The executor resolves definitions/contexts after
each transition. See OWNERSHIP.md for the measured boundaries.

External TargetState changes number/time immediately but defers entry attributes
until after the victim's negative pass. Own -2 continues in borrowed units;
-3/-1 are excluded. Pos/Vel and constants use the current state owner's width
while victim storage stays in its own width. Negative TargetState clamps to
zero; undefined positive destinations advance with a warning and no body.
Ordinary missing-state fallback rules remain unmeasured and retain errors.

ChangeAnim2 has typed value/elem selection and an independent AIR source.
ChangeAnim and entry anim use the actor's own AIR. AnimExist follows the current
action source; SelfAnimExist always queries own AIR. SelfState and a new hit
can retain a borrowed action if no animation change replaces it. A repeat hit
restores own state definitions. Collision/drawing resolve the active AIR.

Full scoped and pure engine/evaluator races pass. Fresh native replays for
entry phase, retained animation after rehit, missing state and elem selection
match. A real 1280x720 Go window draws the borrowed action and restores states
without runtime errors; filtering/full framebuffer parity is not claimed. See
temp/m4z-full-final-race.log, temp/m4z-core-final-race.log and
temp/app-runs/ownership-borrowed-air-go/. Root module hashes are unchanged. Full throws
still require HitDef custom-state fields, binding, all controller geometry
boundaries, borrowed AIR offset/box scaling and complete helper/world ownership.
M4y's reversal point is `fb84babc`; no complete compatibility claim is made.

## M4y — Target retention, effects and redirected scope/units

Forty-nine original native programs pin 744 numbers and three required-value
load rejections. Typed drop/facing/life/power/velocity controllers act on the
acquired ledger. HitDef IDs default/clamp to zero, survive attacker transitions,
relabel on later contact and release on receiver recovery. Guard acquires a
target; an I override does not. See TARGETS.md for the measured parameters.

Target life changes immediately; negative adjustments scale by receiver defence
and super factors, healing does not, and caller AttackMul is ignored. Facing
preserves world velocity, and velocity omission/mixed units are measured. The
damage kernel now multiplies runtime DefenceMul and clears idle adjustment on
new get-hit sequences. A contact trace confirms -3/-2/-1/positive passes all see
state 5000/damage 100/life 1000/time 0 before damage applies.

Redirection now separates ordinary eager origin arguments from the destination
outer trigger. Cond executes lazily in destination context/units. Direct geometry
returns caller units while scalar variables and declared constant kinds remain
distinct. Forty-eight scope values, plus mixed-controller traces, exercise the
conversion and assignment ownership. An upward draft revealed native camera
easing; the stable fixture isolates it instead of claiming camera parity.

Full scoped/pure core races and representative native replays pass. TargetState
only gains required-argument validation; actual custom ownership/binding and
full multiple-entity scheduling remain open. Native multi-target keepone/RNG,
base-stat/kill/rounding and complete schemas still need comparison. M4x's
reversal point is `9c8c29b6`. No root module/reference/desktop changes were made.

## M4x — Entity IDs, helper families and native bottom boundaries

Nine stable native programs cover thirteen cases: 240 numeric values and one
runtime malformed-expression rejection. InputSlot/EntityID/TeamSide/HelperID
and opponents/acquired targets are now distinct. The match-owned registry
supports live ID/parent/root/helper/enemy queries, helper families and leaf
retirement/non-reuse. HelperMax clamps to 4..56 and sets the initial root ID.
See ENTITIES.md for exact observations and remaining scheduling/schema limits.

The probes also disprove ordinary bottom propagation in the supplied beta:
measured operators consume integer zero, IfElse treats a bottom condition as
false, and Cond preserves it. VarSet/Add and VelSet/Add use their measured zero
boundaries. Bitwise NOT of bottom remains a fatal VM error, shared by headless
and production execution. See BOTTOM.md. Two exponentiation programs retain
layout-dependent native results as unresolved evidence, outside stable goldens.

Full scoped races and pure core races pass, with fresh root-limit/nested/operator/
fatal replays. The first full run found the old JSONL propagation expectation;
it is updated to native integer zero and final races pass. Round registry reset
has authored coverage; native round continuity, full target/HitDef IDs, helper
parameters/negative states, projectiles and global entity phases remain open.
M4w's reversal point is `ca067064`. Root modules/reference/desktop are unchanged.

## M4w — Shared fight HUD projection and native stage cropping

Ten native cases pin 240 numeric values and twenty original marker cores.
StageFit=0 changes script-visible game/screen height and the stage's top cut,
with native integer stage-unit truncation and high/low guideline allocation.
Fight HUD uses motif-width conversion and the world's physical transform/top
origin. SystemFit does not change these fight scenes; KeepAspect fits or
stretches the effective viewport into the actual window. See COORDINATES.md.

Match camera updates/reset preserve the crop. CFG/YAML flags load without
discarding explicit false or resetting absent video values. Window defaults
follow configured game dimensions; Fight Layout uses actual window dimensions.
The prior independent-HUD fit assertion is superseded. Full scoped/pure coord
races and fresh fractional-crop/stretch native replays pass. Three actual Go
windows show exact HUD marker cores and matching world marker centers. Filtered
world edges still differ and remain documented; no framebuffer parity is claimed.

M4v's reversal point is `4c1391e5`. The original distribution/root module/desktop
are preserved. Menu/SystemFit behavior, System/DirectX fallbacks, tall overdraw,
additional cut/zoom/camera schemas and raster fidelity remain open. Continue
world/entity/target identity and runtime redirection before full combat ownership.

## M4v — SuperPause, frozen contacts and presentation ownership

Thirty-eight native programs pin 600 numeric observations, ten static render
cases and three audio programs with seven stable segments/seven Go checks.
Typed SuperPause now owns duration/movetime/preemption, power, unhittable,
acquired-target defence adjustments, own/common cues, position and darkening.
See SUPERPAUSE.md for exact boundaries and remaining schemas/ownership gaps.

Frozen attack boxes may contact moving receivers. Frozen receivers enter their
hit state immediately and accumulate damage until resumption. New HitDef rearms
contacts; active-tick counter aging matches both new freeze traces and existing
hitpause traces. Positive p2defmul stacks on acquired H targets, nonpositive uses
configured 1.5, and recovery clears it. This contact ledger is deliberately not
claimed as the complete world/Target registry.

Super effects keep a spawn position and animate through either pause, including
when their caller freezes; finite actions retire after one pass. Infinite
frames survive pause expiry and anim=-1 replacement. Static scene/opponent/caller
tints and common/own screen-X-panned audio match the measured scope. Full races,
fresh clock/other-owner render and isolated audio replays pass. Real stock KFM
super integration checks 30 frozen ticks, common cues and 1000 power consumption.

Overlapping early capture/audio drafts were discarded; promoted evidence uses
serial owned windows/private audio. AGENTS.md records this workflow requirement.
No desktop/reference/root-module changes were made. M4u's reversal point is
commit `b7df46ff`. The captures expose remaining HUD projection/configuration
differences despite correct world margins; compare fit flags next. Full world,
input, combat and controller gates remain open.

## M4u — Shared GameTime and root Pause scheduling

Thirteen native programs pin 264 observations for duration, caller movement,
issuing/release boundaries, zero/missing/negative arguments, replacement,
cancellation and later-caller precedence. Native accepts omitted time as zero
and resets invalid movetime to zero, contrary to treating it as a clamped range.
The typed Pause controller and production root clock reproduce all packets.
GameTime advances through frozen local Time/position/animation. See PAUSE.md.

The Timeline has independent actor identities, one outer-frame clock for nested
entity adapters, queued requests that preserve the issuing tick, and pause
clocks shared with Match. Match/command-window authored tests exercise phase/
timer/background/effect freezing, pausebg, round reset, and end command buffering.
Those interaction/presentation/input checks are not yet native parity evidence.
All scoped races and pure timeline/engine races pass. A fresh pair-caller native
replay matches. Root modules and the reference remain unchanged.

One SuperPause pilot pins its disabled-presentation root clock for pure Timeline
tests; its production controller is still a stub. Implement the full SuperPause
schema/presentation/protection and native pause/contact/command/effect interactions
next. GameTime absolute origins and complete helper/projectile/world ownership
remain open. M4t's reversal point is commit `05dc5222`; this does not finish M2,
M4, or the whole Pause controller coverage gate.

## M4t — Landing resumption, physics order and shared tick budget

Eighteen native programs pin 336 numeric observations and one runaway rejection.
Movement precedes Physics A acceleration and S/C friction. Ground contact enters
52 without clamping position or zeroing velocity, including MoveType H, and runs
Time=0 CNS/physics again immediately. Negative passes and final time/animation
clocks run once per tick. PosFreeze stops movement while acceleration continues.
State 105 remains exempt from automatic Physics A landing. See PHYSICS.md for
the exact values, custom-state probes and limits.

The shared StatePassBudget now counts CNS transitions across engine/landing
resumes; engine entries and terminal passes do not consume it. Native completes
2510 successive engine landings in one tick, beyond the CNS loop budget. A chain
near the CNS limit can land/resume successfully; its additional transition
fails at pending state 706, matching both production and a headless budget test.

Native custom states 40/50/51/52/100/105 with Physics N retain their authored
states and clocks. Removed Go's forced jump/down/landing/run exits; common CNS
owns those transitions. The production mixed KFM/KFM720 input test now requires
both characters to jump, reach 52 and return to controlled idle on the ground.

Full scoped races and pure engine races pass. Fresh frozen-landing and shared-
budget overflow replays match. The first full race run exposed missing geometry
getters in the new headless test host; those were supplied, and final races pass.
The first transition capture requested a page past the 40-fvar bank; corrected
fixtures use page 28. Root module files remain unchanged. M4s's reversal point
is commit `697fd8e1`. Full world/body/physics/pause and ownership semantics remain
open; no complete state-machine or combat claim is made.
Actual 1280x720 mixed-stock windows were inspected at physics-stock-match/;
their idle scene/timer smoke is distinct from the injected jump test.

## M4s — Native state continuation limit and pending-state diagnostics

Fifteen native programs pin seven accepted packets (84 integer values) and
eight runtime runaway rejections. The former ten-state cap rejected a legal
24-iteration chain. Native self/alternating/terminal and negative-launched cases
establish the actual boundary: 2502 positive executions may complete, while a
further continuation fails with the displayed 2500-loop message naming the
pending state. Source/AIR/executable hashes and rejection text are checked.

Read-only inspection of the pinned executable confirms why the displayed and
execution counts differ: an initial call precedes a post-increment counter test
using greater-than 2500. No binary or reference assets are edited or committed.
See REFERENCE.md for addresses and the exact inference boundaries.

The shared headless executor now follows that boundary and retains source-file
context in runaway diagnostics. Pure engine races and the full scoped race suite
pass, with both hosts exercising all 15 native programs. Fresh terminal-overflow
and maximum negative-launched success replays match. Root modules are unchanged.
M4r's reversal point is commit `4d53ebd2`.

Remaining: the budget across separate engine/physics resumptions, landing/physics
phase timing, all world/entity ownership and Pause/SuperPause. The current
Character landing path enters 52 in finishTick without running its Time=0 CNS
until the next tick; compare that path next. This checkpoint does not establish
complete per-tick state scheduling or full M2/M4 compatibility.

## M4r — Empty-state rejection and validation before merge

Five minimal native cases distinguish empty positive/negative declarations,
explicit entry attributes, same-file duplicates and later-file definitions.
The beta rejects all four effective empty declarations during character loading,
including an unreferenced final state. A duplicate empty state after a valid
same-file definition is ignored, retaining marker 111. An empty state in st0
still fails even when st already owns that ID. This corrects M4q's unproven
assumption that empty declarations were accepted and only needed runtime work.

The loader now validates first definitions in every loaded st/stN/CMD/common
file before merge, preserving the native same-file duplicate rule. It reuses
those compiled programs in the player and includes DEF Files key, source,
state and line in load errors. The constants-only CNS role remains distinct.
The executor's existing empty-state rejection remains correct and unchanged.

Full scoped races pass, including all stock loads, the five source/hash-pinned
cases, and a fresh shadowed-state rejection replay. A final loader/resource race
run also passes after simplifying the file list. One earlier command inherited
the unavailable desktop display and failed at GLFW initialization; subsequent
runs explicitly use the task's Xvfb. No desktop configuration changed.
M4q's reversal point is commit `7a1b9ec9`. Next measure the provisional ten-state
continuation cap, then continue world/entity, pause and full combat semantics.

## M4q — Native viewport profiles, framing and version gates

Twelve native cases cover seven accepted stage/game/local profiles, same-pass
ScreenPos after PosSet, legacy GameWidth/Height/ScreenPos/CameraPos acceptance,
and legacy ScreenWidth/ScreenHeight/CameraZoom rejection. The profile matrix pins
168 float32 observations; all accepted packets match production Character ticks.
Stage aspect determines match height in these scenes, even with a declared 1.1
stage or a differing configured height. The shared viewport supplies contextual
coordinates and propagates through match resets/entity creation. Zoom-one/zero-
camera observations do not establish full camera behavior.

The renderer fits world/HUD separately and draws actors/effects at common game
resolution. The actual 1280x720 owned capture viewport-spark-hd/ confirms the
previously missing world margins. Filter edges still differ from native. The
first capture remained 320x240 because the source config comments its window
settings; the harness now supplies explicit YAML dimensions and validates them.
See COORDINATES.md for provenance and remaining rendering limits.

Character loading retains language version and compiles nonempty states before
gameplay. Three measured 1.1 trigger gates apply to typed parameters, entry and
triggers, including shared AST cache hits; controller source remains immutable.
Empty states are still deliberately skipped pending their runtime comparison.
Other language-version/legacy-handler schemas remain incomplete.

Full scoped races pass, including all stock resource/state loads, viewport
packets and loader/cache version regressions. Fresh stage11 and legacy rejection
replays pass. Root module hashes are unchanged. M4p's reversal point is commit
`253649ce`. Continue native empty-state execution/entry and complete combat/entity
semantics; this checkpoint does not finish M4 or full compatibility.

## M4p — Native spark geometry and production contact effects

Added a shared CPU effect queue/scene with independent AIR clocks, finite
one-pass expiry, explicit infinite frames and scene cleanup. Match binds own/
common resources before state execution. Typed HitDef sparkno/guard.sparkno/
sparkxy expressions preserve source maps; omitted defaults come from CNS Data.
Production drawing consumes actual frames, axes, facing/offset/scale/palette
state and AIR A/A1/S/ASnDm blend metadata, with retired-cache pruning. Other
HitDef fields and full Explod/AIR/world behavior remain incomplete.

Eight authored native programs cover own hit/guard, two offsets, common/default
selection, disabled/missing common effects and Data-prefix coercion. Five
persistent RGB markers and two confirmed absences are pinned with numeric/source/
asset/window/core/recipe hashes. Go frame anchors and resource choices match.
Native common-default and guard replays match the same geometry/core hashes.
The beta accepts Data sparkno=S9992 but exposes zero rather than an own default;
the parser now matches that observation. See EFFECTS.md for precise calibration.

Full races and pure engine/effect/AIR races pass, including independent clocks,
expiry/clear/errors, typed host isolation and actual KFM common spark emission.
Normal/additive markers were inspected through real owned Go windows at
spark-own-production and spark-add-production; additive RGB is visible on the
stage. This is GPU-route evidence, not native framebuffer/blend parity.

The controlled native scene exposes a framing discrepancy: its 1280x720 stage
fits height at scale three with horizontal margins. The provisional M4d Go
width/cut-suggestion path differs. Resolve world/HUD framing and screen/game
coordinate bindings next, then continue remaining combat/entity gates. Native
pause/clock, dynamic Width/mixed profiles, layers/palettes, full AIR/Explod and
capacity rules remain open. M4o's reversal point is commit `6ba7c9c1`.

## M4o — Root KO voices, echoes and global suppression

The shared post-damage root tick now plays own-bank sound 11,0 once on KO,
stopping numbered voice channel 0. KO.echo enables additional voices at native
KO counter values 20 and 50, with 50% and 25% volume requests. The counter begins
at one on the detection pass. Echoes use the same measured screen-X panning as
contact cues. A root's nokosnd assertion suppresses every root's scheduled KO
voice for that tick; suppressing the initial voice does not cancel later echoes.
Missing KO samples warn and continue. Full KO state/kill/revival, helper ownership
and Pause/SuperPause timing remain separate open obligations.

Four authored programs with complete numeric constants pin 21 native segments
and 25 Go tick checks: basic voice, echoes, continuous opponent suppression and
initial-only suppression followed by audible echoes. Short 100-ms Fourier
windows and six-tick Go windows isolate quarter-second original tones. Gain
matches integer mixer truncation, including 25% producing a ratio below exact
one quarter at stock SFX gain. Full races pass and a fresh initial-suppression
replay matches seven segments, source/numeric packets and recipe hashes.
The fixture windows establish plateaus; exact first-sample world-clock boundary
parity and pause interactions are not claimed.

Initial captures were contaminated by the stock spoken KO announcement. Setting
negative announcement IDs alone did not remove it in this distribution. The
final reproducible recipe routes only the disposable HUD to a valid original
empty SND bank; its bank/fight hashes are pinned. Original reference files and
desktop audio routes remain untouched. Earlier captures stay diagnostic-only
under temp/. Optional --ko generation preserves prior default bank bytes.

Next common hit/guard effects, then complete combat/entity and sound configuration
edges. Root KO audio does not close full match/KO or sound compatibility gates.
M4n's reversal point is commit `99372c03`.

## M4n — HitDef sound schema, contact cues and voice interruption

Hit/guard sound parameters now compile expressions, integer tuples, resource
prefixes and item-zero omission before execution. The shared source map stays
immutable across hosts. S selects attacker SND, F or omission selects common
SND. Native leading-prefix consumption also rejects raw Facing as acing;
parentheses protect expressions. Other HitDef fields still use legacy parsing
and require the planned complete schema audit.

A successful hit stops the defender's numbered channel 0 before get-hit CNS
can emit a replacement. HitOverride to an idle state still interrupts it;
guarding preserves it. Other numbered voices remain. Contact cues play their
chosen immutable bank and expire naturally. The beta passes defender screen X
measured from the left edge into its panner: -70 world X gives 90 in 240p space,
and -40 gives 120. Both symmetric scenes match native PCM. Complete camera/zoom,
mixed-coordinate and ownership edges remain to measure.

Nine authored audio programs pin 32 native segments and 45 Go tick checks. They
cover normal hit, guard, override, two positions, distinct common versus own
tones, group-only F syntax, initial omissions and a later HitDef omitting prior
choices. Omissions emit no cue in these scenes and clear the earlier choice.
Five grammar programs add three accepts (including assignments) and two load
rejections. An assignment expression's native fvar packet confirms execution.
Full races pass; fresh override-sound and contact-sound-common-single replays
match levels/relative onsets and the disposable common-bank recipe hashes.

The real stock KFM logical walk/punch/hit/recovery test now also requires a
common hit command and nonzero shared-mixer PCM. Source banks are bound by M4m
before state execution. The original bank generator optionally adds one-second
contact/common tones without changing its prior default output. Initial tests
ran before the last captures had tick-check metadata and failed on missing
fixtures/checks; after promoting complete evidence, every comparison passes.

Next: KO voice/echo and sound configuration edges, common hit/guard effects,
complete HitDef/protection/priority/reversal schemas, and full world/entity
ownership. None of this promotes a whole controller to Verified or closes M4.
M4m's reversal point is commit `b616a7e4`.

## M4m — Production sound stream, fight banks and HUD cues

Menus and fights now share the CPU mixer through one endless Ebitengine audio
player. Stock SampleRate, BufferSize, WavChannels, master/SFX/BGM volumes and
resampling metadata load from [Sound]; the device buffer uses 2^BufferSize
frames. YAML false/zero overrides retain presence. Live SFX gain changes affect
active voices, Sound=0 opens no player, and disabled playback drains commands.
Invalid device resource bounds have contextual errors; these limits are not
claims about native invalid-config fallbacks. BGM and native-quality resampling
are retained settings, not implemented playback features.

Each fresh match/round character binds its own SND and the fight's common bank
before entry controllers. Match resets and leaving a fight issue an ordered
scene clear that also retires decoded assets. This cleanup is a provisional
lifecycle policy; complete native round/custom-state behavior remains open.
HUD round/fight/KO/DKO/time-over and power-level cues use the fight SND, respect
sound delays, and fire once per event. Exact native announcement offsets and
complete layout variants still depend on the open round scheduler work.

A new authored HUD-tone recipe patches only disposable fight definitions. The
native round cue produces the same measured stereo gain as the Go HUD stream;
two native segments pin a one-second tone and expiry. Recipe/fight/source/bank/
config/PCM hashes are retained in hud-sound.json. Native replay verifies those
levels. No absolute announcement start-time parity is claimed.

Full races pass, including production title/select/versus/fight/two-round/result/
rematch/exit with PCM reads, bank binding, delayed/one-shot HUD cues, power cues,
live mute/unmute and looping-scene cleanup. Actual device playback is captured
at temp/app-runs/sound-match-playback through task-local ALSA -> private Pulse
null sink. The 44100-Hz stereo capture contains 433534 frames (peak 32767, RMS
1692.49); owned-window images at 2/5/10 seconds were inspected. This establishes
nonzero production device output, not waveform parity. ALSA's control-device
enumeration warning did not prevent the explicit private PCM route.

Next: contact/guard/KO voice and effect cues, complete HitDef sound expressions
and bank selectors, channel-0 interruption, entity/round lifecycle, BGM and
storyboard audio. The full playable M4 and full sound compatibility gates remain
open. M4l's reversal point is commit `dfe8ae1b`.

## M4l — Typed sound commands and native PCM mixer checks

PlaySnd, SndPan and StopSnd now compile typed parameters into an ordered CPU
command queue. Sound banks stay immutable; audio owners are separate from input
slots, including helper/projectile allocation. The thread-safe S16 stereo mixer
handles channel replacement, low-priority requests, automatic voices, owner
stops, global StopSnd -1, loops, finite sample expiry and PCM decoding/resampling.
Fragmented reads preserve frame alignment through reset. Production playback
and match resource binding are the next checkpoint; no fight audio claim yet.

Twenty-three authored programs cover 24 native cases: twelve audio cases and
twelve accepted/rejected grammar cases. Native capture uses only a private,
authenticated PulseAudio null sink. Fifty-five stable tone segments produce
63 Go tick checks, including stereo samples and both 1.0/1.1 character versions.
Source/AIR/original tone-bank/executable/config/PCM hashes and settings are pinned.
A fresh sound-channels-audio-replay matches amplitudes and relative cue onsets.
The full scoped race suite and display-independent mixer/evaluator races pass.
See AUDIO.md and tests/README.md for reproduction and evidence limits.

The beta's behavior differs from plausible manual interpretations: PlaySnd pan
uses common-game pixels while SndPan uses character-local units; pan supplies
the value when both offsets exist but abspan still selects absolute mode.
Deprecated volume is parsed/evaluated but ignored. Tested freqmul values do not
change pitch, even for 1.0 characters; nonpositive values still start a sample.
A leading F/S is stripped before expression parsing, including Facing/FVar;
parentheses protect identifiers. Unavailable samples warn and continue.
Measured integer panning/volume truncation and center-channel asymmetry match.

Open: production/HUD/hit cues, device and round lifecycle, channel-0 hit/guard
interruption, complete ownership, arbitrary-rate/libresample fidelity, capacity
fallbacks, extreme/bottom arguments, stereo/config edges, BGM and storyboards.
The queue cap and linear resampler are diagnosed provisional implementations.
Known-trigger validation is opt-in for sound, not complete language validation.
No whole sound controller or asset row is promoted to Verified.

## M4k — Guard distance, move-type order and body push

Guard range now belongs to the attacker, in its local units, and extends in the
attacker's facing. Defender facing and vertical separation do not affect the
query. The upper boundary is strict; the same axis is eligible. MoveType A is
sufficient without an active HitDef. Range starts from size.attack.dist, explicit
HitDef guard.dist/AttackDist changes it, an omitted later guard.dist retains it,
and ordinary nonpersistent state entry resets it. AttackDist now compiles its
required integer expression and updates this shared value.

The cross-player clock proves state passes prioritize attackers over idle/hit
players, retaining P1-first order when both attack. Priority is selected before
passes: the I-to-A tick still follows the idle order. Production now partitions
root passes accordingly. This refines M4g/M4i ordering; static contact/life
observations alone had not established cross-player script order. Complete
helper/projectile, pause and physics ordering remains open.

Native spatial probes distinguish guard range from push/turn side effects.
Custom states retain facing instead of turning every tick. PlayerPush now has
typed boolean control, a per-tick reset, and pair suppression when either player
disables pushing. Ordinary ground overlaps split equally. For axes less than ten
240p units apart, the beta adds spacing of ten minus their separation before
splitting the correction. Exact, small, signed, fractional and cutoff probes
pin this behavior; a 1280/320 pair confirms its coordinate scaling. Previous
pure geometry tests had not established this native near-axis correction.

Twenty-three source-pinned programs cover 26 native cases, including two mixed
range directions and mixed push, and all compare production ticks. Full races
pass and guard-distance-clock-replay independently reproduces its packets.
The harness adds explicit fixture-owned constants, preserving existing modes;
push fixtures pin numeric KFM defaults with ten-pixel body widths. Failed drafts
with incomplete/duplicate constant groups remain diagnostic-only under temp/.
The native life-only Data group rejection also needs a dedicated character
constant-loader validation audit; it is not a state-parser rejection rule.

Complete auto-turn eligibility, airborne/corner/pause push, dynamic Width,
projectile guard range, HitDef expression/default coverage and full world/entity
ordering remain open. This is a measured guard/push checkpoint, not the full M4
gate or a Verified whole-controller/trigger claim.

## M4j — HitBy/NotHitBy protection slots and attack posture

Replaced unused string flag maps with two shared timed protection slots. HitBy
sets allowed state/attack masks; NotHitBy complements each component separately.
All active slots must allow a contact. Partial posture-only, attack-only and
empty masks are distinct; HitBy SCA has no allowed attack classes, while
NotHitBy SCA blocks all postures. A blocked HitDef stays active and can connect
after protection expires. Time defaults to 1, time=0 clears, time=-1 persists,
and both controllers overwrite the same slot selected by value/value2.
Typed compilation evaluates duration before mutation and diagnoses malformed or
missing attributes. The old raw methods/handlers are removed.

Twenty-five native protection programs (24 accepted, one load rejection) match
production collisions, including last-tick/expiry, slot intersection/replacement,
empty/partial masks and parameter precedence. The beta accepts both value and
value2 and uses value, contrary to the manual's one-parameter rule. It rejects
omission of both. Attack posture comes from the attacker's current StateType,
including a change after HitDef execution; the authored attr posture does not
control protection matching. Two further HitOverride probes establish the same
rule, now shared by contact snapshots without mutating authored definitions.
GetHitVar(damage) exposes pending damage during the contact pass and clears
after damage processing; later native observations return zero.

Full tests/races pass; protect-dynamic-pose-replay independently reproduces the
native case. Native resource/source hashes remain pinned; all edits stay scoped.
Complete invalid mask/value fallbacks, pause/superpause aging, custom-state and
helper/projectile ownership, hit flags, priority/reversal and guard-entry distance
remain open. No entire controller/trigger inventory row is promoted to Verified.

## M4i — Contact activation, separate clocks and guard response

Twelve new source-pinned native programs distinguish contact activation and
clocks from the earlier static trade observations. A newly defined HitDef first
connects on the next tick, before negative/positive state passes. Shared roots
now decrement existing clocks, collect prior definitions, snapshot contacts,
enter get-hit states, age defenses, run state passes, deduct damage, then finish
physics/animation/time. This supersedes the state-before-contact assumption in
M4g. Its trade fixtures still pass but did not discriminate those schedules.

Attacker hitpause and receiver hitshake are separate. Hitpause suppresses normal
CNS/time/animation and releases when its pre-pass decrement reaches zero. Victim
CNS and Time advance during hitshake; common CNS decides when recoil begins.
GetHitVar(hittime) counts down after hitshake reaches zero, stops at -1, and
HitOver becomes true below zero. Timed HitOverride slots remain active through
contact before aging/renewal, preserving the measured last-tick behavior. This
supersedes the unmeasured M2b shared ShakeTime/last-paused-tick interpretation.

Guard response now chooses guard flags by posture, guard damage, guard-specific
pause/shake, velocities, hit/slide/control times and 150/152/154 states. Guard-end
140 remains eligible. Matching HitOverride suppresses guard response. Received
guard vectors scale without mutating attacker data; contact counters distinguish
MoveGuarded from MoveHit. GetHitVar damage/guarded/hitshaketime are typed integer
bindings. Air guard reports hittime=-1 and slidetime=0. Omitted guard velocity,
pause and control defaults match the executable. In this beta guard.slidetime
inherits ground.slidetime, contradicting the manual's guard.hittime default.

Full tests/races pass, including all new captures and the previous coordinate,
trade and override cases. A fresh pause-release replay reproduces the six victim
clock packets. Original guard-end fixtures and reference assets are unchanged.
Guard entry distance/input timing, multiple contacts, hit flags, damage scaling,
priority/reversal/HitBy, helpers/projectiles and complete GetHitVar remain open.
This closes measured clock/guard cases, not all combat or the M4 gate.

## M4h — Stock HitOverride and get-hit context

Registered/compiled HitOverride with typed state/slot/time/forceair values and
shared display-independent hit attribute filters. Eight timed slots survive
state entry, lowest matching slot wins, and blank/omitted attributes or missing
stateno disable the slot. The last two omissions are accepted by the beta despite
the manual calling those parameters required. Stock time=0 clearing works.
Both KFM variants now compile all 108 states; the three diagnosed missing-
controller gaps are replaced by a complete stock compilation gate.

Twelve source-pinned native cases match default duration, interval expiry,
nonmatching/projectile attributes, slots, forceair, clearing and omission forms.
Native override first enters the standard 5000/5010/5020 get-hit state without
executing its controllers, then the selected override: PrevStateNo exposes that
intermediate entry. Forceair selects air vectors/time while allowing the target
Statedef to remain S; it does not force fall when air.fall=0. Stored hit-air
context is now separate from current StateType/fall. Native deferred damage is
applied only if the resulting contact state remains movetype H; I destinations
retain life while H destinations take damage. These observations are matched by
the shared production tick. A fresh forceair replay matches; full races pass.

Full HitOverride/custom-state behavior is still partial: matching p1stateno /
p2getp1state exclusions, invalid-value fallbacks, hitpause duration edges,
helper/projectile scheduling and ownership remain open. HitDef defaults/flags,
priority/guard/reversal and complete GetHitVar semantics are also incomplete.
No full controller/trigger inventory row is promoted to Verified.

During this checkpoint all desktop outputs disconnected. The native captures
stalled at frame 5 and Ebitengine tests failed with no-monitor initialization.
Xvfb 21.1.12-1ubuntu1.8 was downloaded/extracted only under temp/virtual-display;
a private authenticated :97 server and software GL restored tests/probes. No
system package/display configuration was changed. See BUILD.md for provenance
and environment. The failed early override captures remain as diagnostic evidence.

## M4g — Shared root-player tick/contact phases

The state-before-contact schedule described here is superseded by M4i. The
original static trade observations did not distinguish the two schedules.

Production matches and the character diagnostic now use character.TickCharacters:
all root state passes run before contact collection, eligible contacts are
snapshotted before any victim transition disables another attack, then contact
entry/positive-state continuation runs before damage and physics/time. This fixes
the previous P1-update-before-P2-state-pass advantage. Positive continuation uses
the existing executor and does not rerun negative states; engine-driven builtin
transitions also run their new Time=0 positive state before timers advance.

Three native trade fixtures pin first-pass and next-tick Life/P2Life/state/contact
observations. Equal-priority hits trade, get-hit Time=0 sees 1000/1000 life before
deduction, and Time=1 sees 975/975 and both players in 5000. The same observations
hold with 8,8 hitpause: the first contact-state pass runs despite the newly started
pause, while HitShakeOver still sees pause state. A fresh paused-trade replay
matches. The older hit-coordinate/default fixtures now use the production group
tick too. P2Life/P2StateNo and type queries use the current opponent context.

Full tests/races pass, including a regression preventing repeated negative passes
and Time=0 entry effects across hitpause. This is root contact ordering evidence,
not complete world phases: priority/attribute/guard/overrides, entity identity,
multiple contacts, helpers/projectiles, push/physics ordering, KO accounting and
pause duration edges remain open. Character/asset imports still initialize
Ebitengine, so full display-independent Match extraction is not complete.

## M3i — Selected v2 palettes and compiled RemapPal

Added instance-local palette maps over immutable SFF data. RemapPal now compiles
both literal/expression tuples with documented -1,0 defaults and validates all
arguments before mutation. Its former raw no-op path is removed. Mapping applies
one lookup (non-transitive), supports identity/destination removal and wildcard
reset, enforces eight slots, and checks source/destination depth/existence.
Invalid resources/capacity failures warn without altering mappings. These rules
are backed by the supplied controller documentation and authored tests; no full
native RemapPal conformance claim is made yet.

Common 5900 now applies PalNo to both stock characters, including round reset.
Character and HUD portrait rendering use the same mapping-aware sprite cache;
revision changes refresh colors, idempotent remaps preserve cached images, and
retired round owners are pruned on draw. Current helper/projectile ownpal paths
share or clone palette state, while their full parameter/lifecycle behavior
remains open. Tests compare every indexed pixel using the selected palette in
both stock archives and preserve source pixels/masking.

Full tests/races pass. Owned-window capture selected-palette-fight/ shows P2's
alternate stock colors at the correct mixed-resolution size. SFF v1 external
palN selection still uses the old fallback and needs separate native fixtures;
custom animation ownership, Explod ownpal and the complete helper palette rules
are pending. This closes the stock v2 selection/rendering gap, not the M3 gate.

## M4f — Received hit coordinates and native float overflow

Received HitDef vectors now copy/convert into the defender's local units;
shared attacker slices stay unchanged. Current knockback is no longer assigned
with an invented reversed sign on contact: get-hit CNS applies stored vectors
through HitVelSet. Explicit acceleration/fall vectors, omitted defaults and
unchanged entry velocities match six native contact cases across 320/1280 player
spaces and common game widths 1280/640. Default yaccel is .35 in 240p units,
not the old .44 fallback; explicit zero remains distinct from omission.

Omitted fall.xvelocity has a native no-change sentinel in common game units.
Its reported float scales with local/game width and can become negative infinity
when the local width exceeds game width. A native edge capture initially failed
the numeric-only glyph reader; it now exactly recognizes Windows #INF output
and writes JSON-safe labels. A fresh smallgame replay matches. Character/helpers/
projectiles retain the common profile as well as their local profile.

Separate language probes confirm float arithmetic, Exp and exponent overflow
produce IEEE infinity, with ordinary infinity comparisons and reciprocal zero.
The evaluator no longer converts infinity to bottom; NaN/domain failures remain
bottom. Scientific literals are now tokenized. The beta accepts decimal-point
forms (1.0e2, .5E+1, 2.E2) and rejects 1e2; Go preserves that restriction.
Native-pinned Character tests and the JSONL runner cover the new semantics;
JSONL traces use a nonfinite label because JSON has no infinity literal.

Full regression/race checks pass. These cases do not complete HitDef schemas,
air/guard contact selection, no-change bounce application, hit/fall ownership,
world phase order or every GetHitVar field. Numeric overflow/domain/conversion
edges beyond the recorded cases remain open. No full combat row is Verified.

## M4e — Guard-state ownership and movement control

Removed the per-tick guard maintenance that re-entered 140 and skipped custom
120 logic. Common CNS owns guard start/hold/hit transitions. The documented
engine return from 140 now waits for AnimTime=0 and selects standing/crouched/
air states, replacing the fixed 20-tick delay. Guard entry, jump/crouch/walk and
crouch release require control.

Three hash-pinned native fixtures match 36 recorded state/time observations:
a five-tick guard-end animation returns to state 0, a custom 120 stays in 120
without an engine-imposed 130/140 transition, and an uncontrolled crouched 11
stays crouched. A fresh guard-end replay matches. Authored tests also hold each
movement direction with Ctrl=0. Full tests/races pass. This does not complete
native guard-entry eligibility, guard flags/damage/velocity/pause, or combat phase
ordering; those remain separate work.

## M4d — First production fight connection

Selection now retains DEF/name/palette/mode/music metadata, confirms P1/P2
independently and offers roster/extra stage choices before versus. Missing roster
resources report paths instead of silently emptying the selection. Versus starts
FightState rather than returning to title. A shared match.Match loads stock
players/stage/fight assets, owns round/intro/control/timer/KO/time-over/result
state, resets characters between rounds and retains wins. Character input can
come from an instance-local logical source. RoundNo, PalNo, AILevel, LifeMax and
PowerMax now have real instance data. The former per-tick forced 191/180 entry
is removed so scripted intro/victory states can finish.

Stage uploads are lazy on the drawing thread. The production fight draws mixed
character scales, a cropped stage, life/power bars, names/portraits, time, win
icons and announcements from fight.def. It offers result Start/rematch and
Esc/menu. A basic deterministic AI policy uses the logical input path in current
single-player routes; full arcade/team/mode progression is not implemented.
Quick-match CLI and owned-window smoke captures exercise the production state.

Full tests/races pass, including stock mixed spawn/intros, KO/reset/two-win
completion, life-fraction time-over, real logical walk/punch/hit/recovery, and an
injected-key title/select/stage/versus/fight/result/rematch/exit test. The first
attack assertion incorrectly used KFM's kick button; it was corrected to the
stock X punch binding. Captures first-mixed-fight show matching character sizes;
the portrait scale mismatch was corrected for the HUD coordinate space.

This is a connected fight checkpoint, not the completed M4 gate. Native round
phase/camera ordering, simultaneous combat phases, correct guard/recovery,
hit-vector scaling, selected palette application, channels/common effects/audio,
HUD alpha/interpolation and complete mode progression remain open. ResetBG,
max-draw handling and full aspect/overdraw/zoom rules are not implemented.
See MATCH.md for controls, validation and concrete limitations.

## M4c — Mixed-player geometry foundation

Two hash-pinned native cases use separate 320/1280-localcoord players. Their
Pos/P2Dist/P2BodyDist observations now match Character: starts -70/-280,
axis distances 140/560 and front distances 120/480. P2BodyDist formerly reused
axis distance; it now subtracts the appropriately converted body fronts and
retains axis-based Y distance. A fresh inverse-case replay matches.

Cross-player positions, facing/guard-distance checks, collision extents and push
displacement convert into the querying player's units. Authored tests check
symmetric mixed-resolution collision, separation, equal physical push movement,
and facing when raw coordinates give the wrong ordering. Collision/push phase
ordering, airborne/overlap/custom Width semantics, acquired targets, hit-vector
conversion and camera transforms remain incomplete. Full race tests pass.
This checkpoint does not yet connect a production Match.

## M4b — Coordinate profiles and scaled constants

Added width-ratio coordinate spaces, retained character/motif/stage/game profiles,
and contextual dimension validation. Fractional localcoord width probes show
320.5 and 640.5 truncate to 320/640; the parser accepts this coercion rather than
rejecting them. Config.GameWidth/GameHeight are distinct from video window size.
Helpers/projectiles inherit their creator's local profile.

Const240p/Const480p/Const720p now bind to that profile and return float32. Five
source-pinned native cases match all 60 captured fvars (30 nonzero conversion/
type observations); the division case confirms float return behavior. Bad arity
is rejected in compiled expressions, and bottom/missing context are tested.
A fresh fractional640 native replay reproduces every value.

Production menu layout honors motif localcoord, background default windows use
its size, and the drawing origin uses the actual logical horizontal center.
Physical window dimensions no longer double implicitly. The high-resolution
MUGEN title now fits and centers correctly in an owned-window inspection; the
first capture exposed and helped correct the remaining fixed 160-pixel origin.
Full regression/race checks pass; root module files remain unchanged.

Mixed-player position/collision conversion, game/stage camera/crop/overdraw,
custom ownership and full fight/storyboard spaces remain open. These are profile
and expression foundations, not a completed Match/Fight state. See COORDINATES.md.

## M3h — Fight/HUD resource definitions

Added fight.Load for the stock fight definition, main SFF/SND/fonts, embedded
animations, fightfx sprites/AIR and common sounds. It retains the raw layout AST,
attributes and source locations for the production HUD renderer rather than
inventing replacement graphics. Required and declared missing/malformed assets
retain the parent Files key/line and resource path. Font lookup follows the
same documented directory order as the motif; font-height keys are not filenames.
DEF extraction can now reuse an already-parsed AST.

Both stock fight definitions load their complete declared asset groups and
retain life/power/time/round layout attributes. All four motifs hand off their
resolved fight definition correctly; big shares mugen1/fight.def. Regressions cover missing/malformed
resources in all six file categories. Full tests/races pass; root module hashes
are unchanged. Loading a HUD definition does not render it or complete fight
flow: bars, portraits, round announcements/effects and audio consumption remain
M4 presentation work, and language/font-height variants are still pending.

## M4a — Stock keyboard symbols and explicit binding presence

Replaced the incorrect DOS scancode conversion with SDL 1.2 symbols used by the
supplied beta config. All 22 stock bindings now map correctly, including P1
arrows/punctuation and P2 letters. Keys retain explicit presence independently of
numeric value: KeyA=0 works in legacy config, YAML overrides and input polling.
Unknown/unbound keys cannot silently poll A. Joystick identifiers remain raw
instead of passing through keyboard conversion; the gamepad adapter is pending.

Named overrides use Ebitengine's key-name parser, including function/keypad keys;
none/disabled explicitly unbinds. The manager accepts an instance-local keyboard
source, supporting injected held/edge events without OS input. Menu confirmation
uses each player's Start binding. Device ID collection no longer duplicates
newly connected gamepads. See INPUT.md for defaults, APIs and limitations.

Tests cover the full stock binding corpus, zero/presence across value copies,
YAML KeyA/disabled overrides, unknown symbols, injected held/edge and isolated
player confirmation. Loaded stock keys reach the real facing-relative gameplay
input path. Full tests/race checks pass; root module hashes remain unchanged.
This is an input foundation: no first playable match, complete command timing,
keyboard-layout or gamepad parity is claimed.

## M0d — Production startup captures and diagnostic ownership

A repeatable app_smoke.py builds the actual application with the scoped module,
uses a disposable stock copy/explicit empty override, and captures only its owned
window at requested times. It sends no input or focus changes and stops only its
own process. Binary/config hashes and commit/dirty metadata accompany captures;
images/logs remain in temp/. This is visual inspection, not a native pixel test.

The KFM motif startup reaches the Elecbyte logo (2 s), animated intro (6 s), and
readable title menu with its correct separate title graphics (16 s). Captures
were inspected in kfm-startup1 and the isolated kfm-startup3 repeat. The earlier
kfm-startup2 repeat exited cleanly after two captures while other graphical tests
were running; its cause is unestablished and its failure/evidence remain recorded.
Do not treat this live desktop tool as deterministic input/timing conformance.

AIR/background parsers no longer warn about normal unrelated sections of shared
DEF ASTs or emit unconditional sprite/action debug prints. Unsupported attributes
inside their own sections remain diagnosed, with a regression proving both
properties. Full tests/races pass. Startup diagnostics also expose a separate
stock keyboard-number conversion issue (the current mapper is DOS-scancode based);
resolve it before relying on keyboard gameplay. No playable match or full visual
parity is claimed.

## M3g — Shared storyboard resources and application handoff

Storyboard.Load now parses once, constructs CPU sprite/animation/background
resources, loads declared SND/fonts, and delays GPU uploads until drawing. The
production storyboard state uses it and returns errors instead of printing and
skipping failed cutscenes. Runtime animation failures retain file/scene/layer
context. Motif logo/intro/select/fight paths resolve relative to the motif;
selection no longer prefixes an already-resolved path with data/ again.

All eight stock storyboards load and finish bounded CPU playback with their
configured scene durations. Their scene backgrounds are retained, including the
KFM intro/credits backgrounds previously discarded. Tests cover declared fonts/
sounds, layer 99 (the documented limit is 100 layers, not the former 10), scene
inheritance metadata, missing assets/actions/end.time, invalid start indices,
checked player construction, application failure propagation and selection paths.
The documented default BGM loop flag is false. Clearcolor=-1 is retained as a
no-clear flag; actual persistent-framebuffer rendering remains pending.

Full regression/race checks pass; root module hashes remain unchanged. This is
loading/playback integration, not complete storyboard parity: text, timed sound/
music playback, window/transform/blend behavior, zero-length scene boundaries,
first-tick animation ordering, clear persistence and reference screenshots still
need work. The public legacy Storyboard.Run stub is not used by production.

## M3f — Per-background sprite archives in production drawing

Motif TitleBGDef/SelectBGDef/VersusBGDef/OptionBGDef spr declarations now load
independent archives with contextual errors. Elements bind to that archive, and
all production title/select/options/versus draw phases select the same background's
images. A shared DrawLayer path retains masked/opaque variants, foreground order
and lazy GPU upload on the drawing thread. Backgrounds without an override still
use the motif's shared archive.

The KFM motif's separate kfmtitle.sff is loaded and its title elements reference
those sprites. Original tests cover archive ownership when sprite IDs collide,
layer selection, cache reuse, shared fallback, and missing/malformed overrides
for all four supported screen backgrounds. Full regression/race checks pass and
root module hashes remain unchanged. Screenshot parity, complete background
controller/transform behavior and storyboard resource handling remain open.

## M3e — Motif/stage resources and font directory integration

Motif main SFF/SND/font and stage SFF loaders now propagate declared-file
resolution and parsing errors with source path, section/key and line. Embedded
AIR errors propagate as well. They use the shared resolver for Windows separator
and case handling. Bitmap FNT v2 child archives resolve relative to the font DEF
and retain parent/child paths on failure. The legacy ResolvePath wrapper delegates
to that resolver; strict loaders use its error-returning API directly.

Font search follows the order documented in stock system.def: system directory,
program root, data/, font/. The old loader omitted font/, so it could silently
leave all standard motif fonts unavailable. fontN.height no longer overwrites
fontN's filename; applying the height override itself is still pending. Missing
Windows TTFs retain the explicit portable fallback described in BUILD.md.

All four stock motifs load their main archives and declared fonts, and all three
stock stages load sprites/backgrounds. Original regressions cover documented
font search priority, missing/malformed sprite/sound/font resources, exact error
locations and mixed-case Windows font-child paths. The old synthetic motif test
now supplies real temporary assets and asserts loading instead of tolerating
missing files. Full regression/race checks pass; root module hashes are unchanged.

This is resource integration, not rendering parity. Per-background SFF overrides
(such as KFM title graphics), storyboard asset/error handling, language-specific
motif sections, font height overrides, audio playback and full stage/background
behavior remain incomplete.

## M1d — CNS controller blocks and duplicate-state binding

The reference state-binding fixture records 111/222/0: controllers with cosmetic
header numbers 999 and 0 both belong to the preceding Statedef 0, and the later
duplicate Statedef 0 contributes no controllers. A separate orphan fixture shows
controllers before the first Statedef are ignored. The Go parser now attaches
to the current block, preserves the first definition, and parses duplicates into
detached blocks so their controllers cannot leak into the selected state. It no
longer invents virtual states from controller labels.

Headless and Character state passes match every recorded variable for both
fixtures. Parser assertions check the absence of phantom states, selected block
source/line and controller order. The full suite/race checks and a fresh native
binding replay pass; stock players retain their known 105/108 compilation
inventory. Malformed duplicate-block validation, unusual header spelling and
special negative-state ownership/eligibility remain separate reference cases.

## M2f — Reference-correct transition/entry ordering

An authored ordinary-state sequence records control and animation across seven
entries, with omitted/explicit transition parameters, conflicting Statedef
parameters and an entry expression reading the incoming control flag. All 24
recorded variables match the headless executor and production Character after
fixing the shared entry path. Transition ctrl/anim apply before destination
entry expressions; explicit Statedef ctrl/anim take precedence. This supersedes
M2b's incorrect assumption that controller parameters win. Entry remains a
single activation and is not reapplied during hitpause or repeated Time=0 passes.
A fresh native replay reproduces both pages. Full regression/race checks pass.

The three M3d common-state differences were isolated to the probe's round startup:
RoundState=2 becomes visible before the round's initial control grant. The probe
now waits for ctrl as well, then forces the transition. The Go runner begins at
that same controlled point. All twenty resource cases now compare directly,
without known-difference substitutions. No special ctrl default or state-0 hack
was added. Production round setup/control scheduling still belongs to M4.

The new evidence establishes these parameter interactions, not the complete
entry expression ordering, owner changes, self-transitions or all special states.

## M3d — Character resource loading and measured state-file precedence

Character loading now requires primary cns/st/cmd/sprite/anim files, propagates
all parse failures, and loads declared sound banks. Errors retain the DEF path,
Files key/line and failed resource; later state compilation/execution retains
its CNS/CMD filename. DEF metadata and raw filenames survive parsing. The shared
resource resolver accepts Windows separators and case-insensitive paths with
explicit directory order, exact-name preference and ambiguity errors.

Native probes establish first-definition precedence: st, then st0..st9 in numeric
order, then CMD states, with common states filling remaining IDs. DEF declaration
order does not change that precedence. The cns entry supplies constants only;
its states participate only if the file is also listed as a state file. Blank
numbered st entries are ignored, while primary st is required; blank sound is
optional. Blank/omitted stcommon uses standard common states. The process-global
mutable common cache is removed; independent loads see current files without
mutating earlier players. LoadOptions accepts explicit root/motif directories.

At this checkpoint, twenty pinned resource cases ran through the real loader
and state pass. Seventeen matched every recorded observation/rejection. Three
common cases retained a control difference that M2f above later isolated to the
probe's round startup. That temporary inventory was removed after the probe
waited for the native control grant and Go began at the same point. Fresh
reversed-order replay passes. Missing/malformed resources for all
seven supported resource groups, case/Windows paths, named runtime errors and
independent common loads have regression coverage.

Both stock players still load 281 sprites, 117 actions, 12 sounds and 108 states;
105 states compile, with the existing three HitOverride gaps. Palette selection
still uses the earlier ACT fallback and needs its own reference-backed integration.
Same-file duplicate-state/header semantics, exact common search precedence,
constant required fields and non-character resource validation remain open.
Full tests and race checks pass; root module hashes stay unchanged.

## M3c — Bounded sound archives, PCM decoding and resource probes

SND extraction validates counts, offsets, cycles, lengths and each embedded WAV,
retaining version/format/rate/channel/frame metadata. Both zero and EOF tail
sentinels are supported: every stock archive uses EOF, so requiring zero would
incorrectly reject them. RIFF chunk bounds, padding, PCM alignment and byte rates
are checked. CPU decoding supports integer PCM8/16/24/32 and float PCM32/64;
unrecognized codecs remain identifiable but decoding them returns an error.

All 45 sounds in the five stock banks parse and decode completely (the stock
payloads are mono PCM8). Original fixtures cover signedness, stereo, wider PCM,
float samples, odd padding, malformed chunks/links/counts and nonfinite samples.
A bounded sound-archive fuzz run completed 141173 executions without failure.
Full regression/race tests pass. Audio channels, mixing/resampling, runtime sound
controllers and non-PCM codecs remain M6 work. Per-sound/raw-bank allocation
budgets are 256 MiB/512 MiB; these are parser bounds, not measured native limits.

Reference file probes establish that st is required, missing declared sound and
common-state files are fatal, and numbered state files include st0 through st9
without requiring a continuous sequence. st10/st12 are ignored. Boundary cases
were checked for 1.1 character declarations as well. The authored resource case
matrix and error/numeric evidence are pinned for the next loader checkpoint;
the existing Go loader has not yet been changed to match these rules.

## M1c — Documented constant bindings and reference rejections

Added a typed catalogue for all 74 documented Const names. The supplied beta
rejects `movement.jump.changeanim.threshold`, including with mugenversion=1.1;
it is marked unavailable rather than assigned an invented value. All other 73
names bind through retained CNS fields, including previously discarded recovery,
friction and tuple parameters. Integer/float32 kinds follow their declared
types. The fall defence multiplier is computed from fall.defence_up. Data.power
defaults to 3000 as observed for stock KFM and initializes maximum power.

Compiled triggers, entry attributes and core controller parameters now reject
unknown/unavailable constant names before side effects. Numeric constant fields
fail with line/field context on malformed or out-of-range values. Player loading
retains the main CNS constant table when merging state files.

Validation: all 73 supported stock values match seven reference clipboard pages;
tests separately assert documented kinds, schema coverage, tuple projection,
malformed input and unavailable-name rejection. The reference parser rejection
is captured as a dedicated authored fixture and reproduced for 1.1 characters.
Fresh supported-value probe pages reproduce their assertions. Full tests/race
checks pass, and root module hashes remain unchanged.

The reference rejects omitted/incomplete Data groups with "Can't load Data
constants". The exact required-field set, other omission defaults, incomplete
tuple semantics and version/localcoord interactions remain loader/reference work;
the 73-value comparison is not a complete constants conformance claim. Added a
small Win32 helper to read only the private reference error dialog, built by
`mugen --reference-tools`; regular captures still require no input/focus changes.

## M2e — Reference state order and corrected persistence

Added authored negative-state and persistence probes to the repeatable capture
runner. The reference trace is 32145 for -3/-2/-1/positive/next-positive order;
post-transition controllers remain unexecuted, and the new positive state sees
Time=0 and the correct previous state. Character and pure tests now share
RunStatePass; both execute the authored programs against recorded variables.

Executable persistence observations **supersede M2a's documentation-based
interpretation**. Persistent N fires on the first true trigger, then suppresses
the next N-1 eligible passes before checking triggers again. False conditions
after that cooldown leave it ready. Suppressed controllers do not evaluate
triggers; a used persistent=0 trigger also stops evaluating. The intermittent
fixture fires at ticks 0,2,5,8,11, with eight trigger side effects; its one-shot
trigger evaluates once. Fixed the gate/order and replaced the earlier incorrect
success-count/one-shot tests. Pause/negative-state reset interactions still need
additional probes. The diagnosed 10-transition runaway guard is provisional.

The harness supports named CNS/AIR/CMD fixtures and records only disposable
evidence with --record; normal runs check pinned engine/fixture hashes and exact
observations. Full tests/race checks and fresh probe reruns validate this
checkpoint. No complete world/state-ownership coverage is claimed.

## M2d / M0c — Shared animation clock and executable reference trace

Replaced the background/storyboard and Character animation clocks with one
display-independent AIR player. Frame durations, zero-duration skips, infinite
holds, element selection, reset and zero-cycle diagnostics now share that path.
Character implements AnimElemTime/AnimElemNo and legacy AnimElem bindings;
ChangeAnim compiles value/elem expressions. Missing actions warn and retain the
last animation. Animation clocks freeze in hitpause. Animation ownership and
ChangeAnim2 remain pending.

An authored 2/3/4-tick action with LoopStart at element 2 runs in the supplied
engine. Twenty-four ticks / 48 encoded numeric observations were captured in
four two-line clipboard pages, using a neutral CMD state. Exact bitmap masks
read the numbers; screenshots were inspected and remain untracked. The trace
demonstrates initial AnimTime=-9, zero on the first displayed tick after wrapping,
then -6 on the next tick. Element elapsed time retains the old end value on that
boundary before resuming the loop clock. Both previous Go clocks were wrong.

The authored fixture, hash-pinned numeric observations and a repeatable capture
tool live under tests/. It uses a disposable distribution copy/private Wine
prefix, captures only its own process's window, and does not send input or
change focus. Full regression/race checks pass. Clock tests compare every
recorded element/time value; other tests cover pause, expression events, element
selection, infinite/zero schedules and query bounds. Nonzero AnimElemNo offsets,
zero/infinite edge cases and initial-element overrides still need reference
probes; their tests presently follow the documentation.

The stock player inventory now loads both characters: 281 sprites, 117 actions
and 108 states each. 105 states compile; three per character still fail explicitly
on the missing HitOverride. The test pins this known gap and fails on unexpected
compilation gaps. This is not a complete stock-compilation/playability gate.

## M3b — Font v2 bitmap banks and TrueType

Font loading now accepts FNT v2 definitions, palette and RGBA sprite banks,
invalid-bank fallback, ASCII bitmap restrictions and glyph offsets. Rendering
retains true-color/alpha glyphs and the default indexed palette instead of
turning them into empty paletted images. Added CPU glyph access and bank-aware
metrics. TrueType/OpenType/TTC uses x/image, supports UTF-8, optional antialiasing,
local/configured/system lookup and an observable Go Regular fallback. That
fallback does not reproduce Windows fonts or supply full CJK coverage.

All 19 runtime font definitions/binaries load, with numeric glyphs and metrics.
Bank/color/alpha, variable and missing-character widths, UTF-8, blend modes,
case-insensitive local lookup, missing/corrupt metadata and wrapped binary
offsets have regressions. Stock f-6x8f's last cell extends into row padding:
pixels clip to the PCX while its six-pixel advance remains. V1 bank count now
uses 256/colors instead of colors. Full regression/race tests pass; exact
reference font metrics, rasterization and placement remain unverified.

Dependency changes are confined to `mugen.mod`/`mugen.sum`, selected by the
launcher with `-modfile`; direct Go commands need that option. This avoids root
module writes. Root go.mod/go.sum hashes were checked unchanged. BUILD.md
records versions, invocation, lookup/fallback and the overlay maintenance cost.

## M3a — SFF v2/v2.01 decoding and sprite reference comparison

Added CPU-only SFF v2/v2.01 decoding for raw indexed/RGB/RGBA, RLE8, RLE5,
LZ5 and PNG8/24/32; ldata/tdata, signed axes, palette/sprite links and palette
identity are retained. Indexed images expose masked and opaque views; true-color
images preserve alpha. Optional palette selection shares immutable pixels and
does not mutate the archive. Linked/cyclic/out-of-range data and malformed
packets fail with context instead of producing empty sprites. Hardened v1 chain,
count, link and PCX validation, and retained transparent color RGB for opaque use.

Validation: all **1376 sprites across all 27 stock SFF archives** decode with
nonempty masked/opaque images. Every one of **281 KFM sprites** matches the
opaque RGBA pixel hash exported by the bundled SFF2PNG v0.2 tool. Hashes and axes
are recorded in `tests/testdata/kfm-sprites.json`; reference art stays untracked.
The bundled SprMaker 2.01beta encoded small original fixtures covering RLE5,
PNG8/24/32 and raw RGB/RGBA absent from the stock corpus. Pixel formulas, input
generator and definition accompany that fixture in `sff/testdata/`.

Full regression/race tests pass. A three-second packet fuzz run completed 121863
executions without failure. Focused packet/link/malformed-PCX tests cover
overlapping/recycled LZ5 references, masking and independent linked metadata.
This closes the initial v2 load blocker, not M3: selected character palettes,
font v2/TTF, sound/path hardening and AIR/render behavior remain unfinished.

## M2c — Compiled core controller parameters

Core movement, variable/range/parent-variable, transition, life/power/control,
angle/multiplier, HitAdd, HitVelSet, VictoryQuote, PosFreeze and RemoveExplod
parameters now compile once and evaluate as typed expressions. Missing required
parameters and malformed expressions fail before any state controller runs.
Bottom numeric parameters produce contextual runtime errors instead of silently
becoming zero. That strict runtime failure policy still needs executable probes.

The bundled CNS warning guidance permits float-to-integer truncation with a
warning. Corrected entry attributes, assignment indices, trigger integer
arguments and redirect IDs to follow that rule; integer-only operators remain
strict. Parent variable expressions evaluate in the helper while writes target
the parent. VarRangeSet evaluates its value once and supports float ranges.
Explicit transition overrides retain presence separately from negative values.
HitVelSet uses enable flags rather than multiplying velocity; the incoming-hit
x-velocity convention remains a combat/reference comparison obligation.

HitAdd now has a separate combo counter, with match-owned credit/reset still
pending. VictoryQuote supports its optional random-selection default, range and
helper restriction; the result screen does not consume it yet. SelfState state
ownership is still incomplete. Many other controllers retain legacy handlers.

Validation: full MUGEN tests pass. Added behavior regressions for expression
side-effect order, int32 overflow, float32 variable arithmetic, parent context,
range defaults/evaluate-once, movement omission/facing, defence/kill handling,
transition overrides, optional defaults and compile/runtime failures. These
expectations are documentation-backed; executable semantic parity is not claimed.

## Reviewed baseline

Assessment, plan, compatibility inventory, and working instructions captured
before engine changes. The original full test target fails with duplicate main
functions; the 11 aggregate parser/config/asset tests pass using external data.
Reference behavior has not yet been measured. The original distribution remains
untouched and is excluded from commits along with temporary build artifacts.

## M0a — Reproducible build and pinned reference baseline

Moved the console physics application into its own package, resolving the
duplicate-main build failure. Existing corpus tests now require the supplied
1.1 beta distribution, without external data paths or skip-on-missing behavior.
Pinned 109 reference content/spec/tool files by size and SHA-256; assets remain
untracked. Added UTF-8 BOM handling for the supplied font definitions and a
regression test. Configuration tests can disable ambient YAML lookup explicitly.
Diagnostics now default to stderr and accept a caller-owned writer instead of
creating a fragile working-directory-relative log file.

The launcher runs entire packages, adds `--check`, keeps build/test temporaries
under `temp/`, and holds the root module read-only. `tests/README.md` describes
the fixture setup and current Ebitengine/display coupling.

Validation: `bash -n game/mugen/mugen` and
`GOPROXY=off game/mugen/mugen --check` pass. The required reference manifest is
verified during the suite, and all 33 stock DEF files parse. No state-machine
compatibility claim is made by this checkpoint.

Reference experiment: an isolated Win32 Wine prefix can start the executable
far enough to write its status log. `mugen.exe -h` did not exit within 40 seconds;
it is not a successful gameplay verification. The log identifies the supplied
binary as **1.1.0 Beta 1 P1 (2013.08.11)**, while the readme labels the distribution
1.1 beta 1 (2013-07-27). Use the supplied, pinned executable as the oracle and
record this distinction in future comparisons. Quick-match startup is being
investigated. Headless semantic fixture execution and strict controller/trigger
diagnostics remain M0/M1/M2 work.

## M1a — Preserve expression source through container parsing

Replaced token reconstruction with a line-oriented container parser retaining
raw expressions and line/column locations. CNS/CMD now consume the preserved
text; Statedef entry expressions are retained separately for runtime compilation.
Added explicit tests for operators, quoted semicolons/commas, nested calls and
intervals, numeric spelling, Windows paths, duplicate triggers, AIR rows, select
options, malformed sections/strings, and reader failures. Required old corpus
checks and all 33 stock DEF files still pass.

Validation: new regressions demonstrated failures with the original parser;
`GOPROXY=off game/mugen/mugen --check` passes after the replacement. This fixes
source fidelity, not expression arithmetic or state execution. Those are next.

The isolated Wine quick match reached both game-loop and match-loop initialization
with the stock character/stage/assets loaded. See REFERENCE.md for the observed
evidence and limits; no Go/reference semantic comparison is claimed yet.

## M1b / M0b foundation — Typed language and headless expression replay

Added distinct int32/float32/bottom values, numeric promotion, integer division,
strict integer bitwise/modulo operations, bounded exponentiation, assignments,
all logical operators, math calls/constants, and bottom propagation. IfElse
evaluates all arguments; Cond evaluates its selected branch. Replaced the
process-global custom evaluator with environment-local optional interfaces for
trigger bindings, redirects, animation queries, and warnings. Existing game
call sites use a numeric facade; preserving bottom through controller parameter
application is still M2 work.

The expression parser now rejects unknown characters, malformed numbers/strings,
invalid assignments/intervals, and excessive nesting instead of silently dropping
tokens. It preserves literal types, implements target operator precedence and
legacy nullary clauses, and parses redirects of a single trigger. The stock
corpus revealed `AnimElem = 3, -1`; the signed legacy argument now has a
regression. All 1522 stock trigger lines parse. This is syntax evidence, not
verification of their world-bound behavior.

Added `internal/exprfixture` and the `tests/expressions` JSONL runner. Six fixtures
produce 15 deterministic observations with kind/value/warnings/variables; typed
results and side effects are asserted. Fixture errors and mismatches exit
nonzero. It uses the production evaluator and works without DISPLAY.

Validation: the full `mugen --check` suite passes. Display-independent race tests
pass for evaluator, parsers, CNS, and the expression runner. A three-second
single-worker expression fuzz run completed 108219 executions without failure.
The mathematical/precedence expectations come from bundled CNS/trigger docs;
numeric edge cases and legacy/redirect behavior still need executable reference
probes. No semantic inventory entry is yet promoted to reference-Verified.

Remaining: complete constant/GetHitVar/world trigger bindings; compile/validate
controller parameters and reject invalid trigger groups; correct state entry,
persistence, pause and ownership; extract a headless state/match core. M1 is not
declared complete by this checkpoint.

## M2a — Headless state executor and compiled trigger groups

Moved the shared executor/handlers into `engine/`, retaining public aliases in
`character/`. No Ebitengine/input/render dependency enters the new core. Trigger
programs compile once from immutable source; synchronized caches allow distinct
hosts to share programs without sharing variables/persistence.

Unknown controller types, malformed expressions, missing trigger1 and invalid
persistent/ignorehitpause literals now fail before executing any controller in
the state, with state/controller/name/type/line context. Compilation errors stay
errors on repeated execution. Added the explicit Null controller. Triggerall and
numbered groups execute in documented order, stop at number gaps, and preserve
repeated-condition AND semantics. Bottom is false at the trigger boundary.
At this checkpoint N counted successful conditions and one-shot triggers kept
evaluating. **M2e supersedes both behaviors after executable probes disproved
them.** Positive one-shot state re-entry reset remains supported.

Validation: headless `go test -race ./game/mugen/engine` passes. Regressions cover
trigger ordering/gaps/AND/OR, invalid-state atomic validation, persistence across
intermittent conditions/re-entry, hitpause filtering, self-transition abortion,
Null, and eight concurrent hosts sharing one compiled state.

Remaining: production Character.Update still skips all paused state passes;
state entry is still applied in two places, and handlers still need typed
parameter schemas. These are next, followed by world/entity ownership and the
full state runner. This checkpoint does not declare M2 complete.

## M2b — Single state entry and production hitpause/timer integration

Hitpause/shake clocks and release boundaries below are superseded by the native
M4i frame traces. Entry precedence is separately corrected by M2g.

Added compiled entry expressions and an explicit EnterState operation. CNS
defaults now match the documented S/I/N entry defaults and omission sentinels.
U preserves prior enums; omitted control/animation/velocity/priority/juggle stay
unchanged. Entry applies power, juggle, facing and hitdef/movehit/hitcount
persistence, and keeps transition control/animation overrides through Time=0
passes. Entry never runs implicitly from ExecuteState. Character initial entry
and transitions use this same path.

Production state passes now run during hitpause so ignorehitpause can work,
including the last paused tick; hit-stun decrements once per active tick.
Contact counters persist and advance on unpaused ticks, with new-contact serials
preventing immediate extra increments. MoveHitReset clears status rather than
rearming already-hit targets. PosFreeze resets each active tick; Gravity adds
acceleration once per activation. Hitboxes require an active HitDef, and default
state entry disables stale hit definitions. Incoming hits no longer mark the
defender's own attack as contacting.

State/entry failures stop the affected Character and are returned by
Player.Update; the combat application propagates them. Expression warnings are
recorded with player/state context, deduplicated and bounded. Empty states are
rejected. The old silent transition-loop cutoff now records an error; its
10-transition cap is provisional and needs comparison with the reference limit.

Validation: full `mugen --check` and focused engine/character race tests pass.
New regressions cover entry defaults/U/omission/expressions/overrides, tuple
evaluation order, no repeated entry during hitpause, combat persistence flags,
last-paused/first-active ticks, contact counters, one-tick freeze, one-shot
gravity, active-hit gating and stopped execution on missing states.

Reference obligations remain: entry attribute/override evaluation ordering,
negative-state edge cases, animation boundaries, physics/collision phase order
and the transition limit. State ownership and world-level Pause/SuperPause are
still incomplete. Typed controller parameters and animation/constant bindings
are next; these changes are unit/regression-tested, not executable-parity claims.
