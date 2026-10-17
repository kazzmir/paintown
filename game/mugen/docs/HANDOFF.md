# Handoff

Updated: 2026-10-17. State/language, sprite/font, and finite animation checkpoints
are tested. Full compatibility remains unfinished. The production one-on-one fight path is
connected; the complete playable M4 gate is still open.

Latest M4ak adds typed DestroySelf recursion/effect flags and world-driven orphan
Explod clocks. Fourteen native programs compare 288 numbers, two load
rejections and 56 authored input hashes. Nonrecursive descendants retain parent
metadata while the live registry/parent writes retire; recursive removal covers
the full descendant tree. Effects are removed or orphaned according to the flags.
Focused native/lifetime/binding/reset checks and the full scoped race suite pass
in temp/m4ak-focused-race.log and m4ak-full-race.log. See RETIREMENT.md. Complete
binding publication, sound, retired-state queries, projectile and pause/foreign/
combat lifecycle combinations remain open.

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

Previous M4aj adds compiled Helper arguments, helper-owned size overrides and a shared
root/helper scheduler. Seventy-two native programs pin 984 numbers, eight load
rejections and 276 authored input hashes. Birth/negative-state eligibility,
creation order, postype/facing/defaults, names/counts and separate paused-tick
allowances match the controls. Far helper cameras expose independently rounded
endpoints/deltas and tiny zoom rounding. Intro now waits ten consecutive clear
ticks; the measured empty/active assertion flows grant control at 130/135/150.
Full scoped races pass in temp/m4aj-full-race.log; display-independent
engine/evaluator races pass in temp/m4aj-final-pure-race.log. See HELPERS.md.
Opposing-helper contacts, complete retirement, positive input, palette/foreign/
paused-creation and full projectile/vertical/zoom/mode/asset gates remain open.

Real 1280x720 windows at temp/app-runs/helper-size-go/ (2/4/7 seconds) and
helper-stock-throw-go/ (2/3/4/7) pass all seven capture hash/dimension/nonblack
checks, with no runtime/missing-resource errors. The scaled helper's white
clothing occupies exactly [643,401,943,499) in both native and Go, with 10,381
thresholded pixels in each. This pins the measured 2.5/.5 scale and nonzero Y;
it is not full framebuffer equivalence. Native debug magenta versus Go black
outside the minimal stage, background filtering and other rendering rules
remain open. The inspected stock recovery has two fighters and no dust clones.


Previous M4ai adds typed Width, temporary edge/player offsets and eight horizontal edge
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

Previous M4ah adds typed ScreenBound, per-pass root/helper defaults, converted stage
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

Previous M4ag: horizontal camera tension/retained goals, easing with float32 left
edges, speed floors and per-root/shared late update points. Twenty native
programs compare 720 numbers; all 60 authored input hashes are checked. Static
inside/boundary/edge/spread, phase, long/return, stage limits, speeds .5/1/2/3,
one-sided movement and mixed widths pass through real Match scheduling. Full
scoped races pass in temp/m4ag-full-final-race.log. The static-only draft passed
ten programs but failed walking; the final extreme-actor speed floor resolves
the additional eight motion cases. See CAMERA.md. M4af is `7306da2e`.
Four fresh native phase/return/slow-motion/limit replays match. Actual 1280x720
windows at temp/app-runs/camerafollow-motion-go/ (2/3/4 seconds) and
stock-throw-camera-go/ (2/3/4/7) are inspected; all seven capture hashes/nonblack
pixels pass, with no runtime/missing-resource errors. The stock throw recovers
with two fighters and no dust clones. The minimal motion stage still exposes
screen-edge clipping and background/debugbg limits. Full boundary margins and
ScreenBound/movecamera should be compared next; this is not full raster parity.
Vertical/1.1 edge tension, zoom, full bounds/ScreenBound/movecamera, helper/
projectile/attack-priority/pause and all renderer/mode/asset obligations remain.

Previous M4af: screen-centered Pos X/PosSet with absolute internal storage and
floor-relative Y. Seven native programs compare 192 numbers and two own/common
RGB positions on fixed nonzero cameras. Mixed widths, Enemy/P2Dist, omitted
axes, facing-relative PosAdd, borrowed state/SelfState and the stock stage-effect
expression Pos X+CameraPos X pass. See COORDINATES.md. Full scoped races pass in
temp/m4af-full-first-race.log; pure engine/evaluator/effect/coordinate races pass.
Four fresh native negative-camera/borrowed-state/own-common RGB replays match;
all 25 authored input hashes are checked.
Four pre-fix failures are in temp/m4af-before-camera.log. Authored .stage.def
inputs are hash checked; the fixed controls do not assert camera follow/easing.
Real own/common 1280x720 windows at temp/app-runs/cameramark-{own,common}-go/
were inspected at 2/4/7 seconds, with matching PNG hashes across banks. Positions
agree; native RGB core [707,570,730,582] differs from Go [708,572,728,580], so full
filter/framebuffer equivalence remains open. Root module hashes are unchanged.

Previous M4ae: typed Explod/NumExplod, owned shared effect instances and resource/
lifetime rendering repair permanent landing-dust clones. Twenty-seven native
programs compare 636 numbers, two load rejections and nine RGB marker placements.
All lifecycle/position/source cases pass, along with full scoped races in
temp/m4ae-full-markers-race.log. Eight fresh native lifecycle/RGB replays match.
See EXPLODS.md. Missing anim is accepted despite the manual's required label;
missing actions create none; IDs clamp/default zero, negative count/removal
select all, and expiry is a shared late phase after root CNS. Explicit stage
space preserves stage Y even with a deprecated screen-edge origin.
All final normal/forward/drop 1280x720 windows were inspected through 2/3/4/7
seconds at temp/app-runs/stock-throw-{base,forward,drop}-effects-go/. They recover
with two fighters and no permanent dust clones, without missing-effect warnings
or runtime errors. The ordinary throw was also repeated in
stock-throw-base-effects-recheck-go/. A black preview was a display artifact:
the stored original/repeat recovery PNGs have identical hashes and substantial
nonblack pixels. app_smoke.py now rejects entirely black quick-match captures.
These remain window/asset smoke checks, not full camera/filter/layer/audio parity.
No complete Explod/controller/render parity claim.
Explicit bindID is a contextual runtime error; remaining parameter/entity/
pause/geometry/renderer obligations are recorded in EXPLODS.md.

Private display :97 was restored after the old task server exited. Current
Xvfb PID 2146490, exec session 19354, uses the original 1280x960, -noreset,
-auth and task-local framebuffer settings documented in BUILD.md. Its brief
1920x1080 replacement was stopped before assertions/captures resumed. Old
session 15800 is complete. Private Pulse PID 1393643 remains running. Never
print/commit authentication cookies, change the desktop, or overlap native/app/
full Ebiten/audio clients. The failed display init executed no assertions.

Previous M4ad: actual stock throw/common CNS and AIR composition has eight native
programs/228 numbers. An initial-GameTime stamp disproves an otherwise passing
throw-relative trace: Go formerly grabbed on tick 22; native grabs on tick one.
Root contacts now use completed simulation poses before AIR advances. Independent
first/second attack and hurtbox windows confirm both sides. HitFallVel preserves
omitted X instead of assigning its numeric sentinel, so TargetDrop escape stays
at native X=42, falls, lands and gets up with full life. Normal/forward throws
apply 78 damage and recover with target cleanup. See THROWS.md. Full scoped
races pass at temp/m4ad-full-first-race.log, retaining all previous regressions.
The harness pins/checks original Files override paths/hashes; stock replays use
240 ticks. Wrappers declare 1.1 and the forward wrapper sets stock var(2) after
its input sample; no command or complete legacy-version parity claim follows.
Four fresh native replays match the start/drop and independent contact-window
packets, including original Files hashes. The normal/forward/drop Go windows are inspected at 2/3/4/7 seconds under
temp/app-runs/stock-throw-{base,forward,drop}-go/, with actual sprite/sound banks
loaded and no runtime errors. This is smoke coverage, not full raster/audio parity.

The final normal/forward captures expose a remaining visual bug: common landing
Explods using anim=F(expression) fall through the legacy literal parser to own
action 0 and infinite lifetime. They leave extra standing KFM sprites after
recovery. State/health/target observations pass, but a clean visual throw is not
yet accepted. Next connect typed Explod resource lookup and lifetime to the shared
effect scene, then repeat these captures. The dropped variant and the scripted
state tests do not excuse the normal/forward rendering defect.

Previous M4ac: HitDef p1stateno/p2stateno/p2getp1state compile and select measured
custom state owners with deferred entry. Thirty-eight native programs pin 1244
distinct per-program numbers and three load rejections. Full scoped races pass
in temp/m4ac-full-final-race.log. See CUSTOMHIT.md. Matching HitOverride excludes
custom hits before contact; ordinary overridden H states acquire no target.
Missing external destinations reset contact counters after negatives. The
isbound trigger publishes the subject's last update, including expiry/cancel/
drop/recovery lag. Final infinite AnimTime is elapsed+1; accepted nonfinal -1
finite scheduling is separately compared. Native malformed literal fields fail
with source context, while computed .5 borrowing flags truncate to zero.
The first distinct-animation native control had no Clsn2 and made no contact;
only the corrected customhit-entry-contact-native input/hash is promoted.
Seven fresh native replays match. The real 1280x720 custom-entry Go smoke at
temp/app-runs/customhit-entry-go/ renders both actors through seven seconds.
The authored victim has 320-art in a 1280 profile and is intentionally small;
this is not a stock throw/framebuffer parity claim. Root hashes are unchanged.

Previous M4ab work: typed TargetBind, 53 native programs/840 values/ten load
rejections; full scoped races and seven fresh representative native replays
pass. Constraints apply before the target's negative pass, convert mixed
coordinates, inherit world velocity, and release on expiry/cancel/drop/new hit/
recovery while H-to-H changes retain them. Own CNS writes and shared late body
push remain effective. Literal float Time/ID rejects at load but float-valued
expressions coerce; tuple/bottom cases are pinned. Turn preserves world velocity.
See BINDING.md. The initial H-only push inference was confounded by missing
Clsn2 and is disproved: both H/I push with hurtboxes; missing Clsn2 excludes push.
The final code uses hurtbox gating. No complete TargetBind/world parity claim.
The real with-box Go match runs through seven-second 1280x720 captures at
temp/app-runs/targetbind-with-box-go/. Sustained movement reaches an edge;
camera/body-edge/framebuffer parity remains open. Root module hashes match.

Next compare full screen boundaries and ScreenBound/movecamera, then vertical
camera/1.1 edge tension/zoom, remaining horizontal camera and effect motion/
binding/pause/ownership/priority, then HitDef facing/priority/hit flags, Width
and attacker-hit interrupted bindings. Stock throw wrappers now cover the ordinary/forward/drop paths. Then
complete BindToTarget/Parent/Root and helper negative/keyctrl/world phases;
compare pause/foreign/multi-target/retirement binding edges, remaining hit
schemas and all input/render/audio/mode/asset gates. Native bottom-power
instability and camera/filtering gaps remain.

M4aa follows M4z: three native root-phase programs/48 values prove each ordered
root completes CNS, pending damage, physics and clocks before the next root's
CNS. Body push remains a shared late phase. The first per-root-push draft
failed old native push/guard/viewport cases; separating late push restores the
full scoped race suite. See WORLD.md. Two-sided trade life observations
supersede the old all-roots-post-CNS damage interpretation. TargetBind probes
exposed this gap; M4ab subsequently implements the measured root TargetBind path.

The prior M4z checkpoint adds eleven native ownership programs, 200 distinct per-program
observations (216 capture entries), compared through the production Character
scheduler. TargetState selects borrowed definitions with deferred entry after
negative states; ChangeState retains them and SelfState restores own definitions.
Own -2 runs in borrowed units, -3/-1 are excluded; position/velocity/constants
convert without rescaling victim storage. Negative destinations clamp to zero;
undefined external destinations advance with a contextual warning. Animation
source stays independent: ChangeAnim2 selects state-owner AIR, ChangeAnim selects
own AIR, AnimExist follows the action and SelfAnimExist always queries own AIR.
SelfState and normal hits can retain a continuing borrowed action. New contacts
restore own state definitions. See OWNERSHIP.md for exact coverage and limits.

Full scoped and pure engine/evaluator races pass; fresh entry/rehit/missing/elem
native replays match. Validation and local checkpoint are recorded in
CHECKPOINTS.md. A real 1280x720 Go game smoke draws both actors,
including the victim's borrowed action, without errors. This is a window smoke,
not a framebuffer-parity claim. Keep all native/app/Ebiten/audio clients serial
on private :97 and the private Pulse server. Early four-second ownership captures
included stale intro clipboard contents and are diagnostic only; promoted
packets use seven seconds and pinned independent opponent hashes.

M4ab/M4ac now cover the measured TargetBind and HitDef custom-state root paths.
Complete other binding clocks/geometry and finish helper typed parameters/
negative eligibility, owner retirement, all controller coordinate boundaries,
projectile/global phases and the remaining input/render/audio/mode/asset gates.
Borrowed AIR offsets/boxes and native camera easing remain unverified/different.
M4x's two unstable bottom-power programs remain unresolved outside stable goldens.
Do not mark full compatibility, M1 or the complete M4 acceptance gate finished.

## Current authorization

The user approved the assessment/plan, authorized implementation, and explicitly
authorized local commits at each milestone or checkpoint, with findings and
problems documented along the way. Do not push. The reviewed documentation is
the first checkpoint; engine work starts at M0. See CHECKPOINTS.md for progress.

All future project changes remain under `game/mugen/`. Preserve the reference
distribution and leave the root module and unrelated files unchanged.

## Completed assessment

- Inspected parsing, expression evaluation, state execution, input/commands,
  character combat and ownership, asset decoders, stages/backgrounds,
  storyboards, motifs, and production application flow.
- Compared selected corresponding implementations in `src/mugen/` and the
  bundled M.U.G.E.N 1.1 beta 1 documentation, stock characters and assets.
- Inventoried all 89 documented controllers: eight missing, 34 registered
  no-op paths, and 47 with some behavior. None have reference-backed semantic
  verification in the current test suite.
- Inventoried all 127 documented trigger entries: 86 without an evaluation
  implementation and 41 with some behavior. Numeric types/bottom and scheduling
  defects affect the existing paths as well.
- Counted 27 stock SFF files: 12 v1 and 15 v2/v2.01. Both characters and every
  stock stage require v2. Stock v2 records include raw indexed, RLE8, LZ5 and
  PNG32. The release history calls header version `2.0.1.0` v2.01.
- Found the stock character/common-state corpus exercises 46 controller types,
  so additional authored fixtures are required for complete feature coverage.
- Created [ASSESSMENT.md](ASSESSMENT.md), [PLAN.md](PLAN.md), and
  [COMPATIBILITY.md](COMPATIBILITY.md), with an index at [README.md](README.md).

## Baseline tests

Run from the repository root using Go 1.24.3, existing local module dependencies,
`GOFLAGS=-mod=readonly`, `GOPROXY=off`, and
`GOCACHE=/home/juvinious/projects/paintown/game/mugen/temp/go-build`:

- `go test ./game/mugen/...`: fails because `tests/character/` contains two
  main functions (`physics_check.go:12`, `main.go:180`). Other compiled packages
  report no unit tests; the existing aggregate test package passes.
- `go test -v ./game/mugen/tests`: all 11 existing tests pass on this machine,
  without skips, using `data-new/mugen/`. A missing-font warning and failed
  relative unresolved-log path occur in the passing motif test.

No new tests, coverage percentage, reference execution, or playable-game
verification was performed. Do not claim implementation or compatibility from
the passing parser tests.

## Environment and constraints

- Wine and Python 3 are installed; DISPLAY is `:0`.
- The command sandbox fails initialization with `bwrap: loopback: Failed
  RTM_NEWADDR: Operation not permitted`. Repository inspection/test commands
  used sandbox escalation. This is an environment issue, not an engine failure.
- Reference Wine execution remains unverified. Future attempts must use
  a distribution copy and a Wine prefix under `game/mugen/temp/`, because Wine
  can create configuration and the reference can write runtime files.
- Existing repository work includes many unrelated untracked assets/builds,
  including the reference distribution and root `go.sum`. Preserve them.
- No source edits or changes outside the approved project subtree were made.
  Baseline Go tests used their normal disposable temporary directories.

## Current implementation checkpoint

M0a: full test build now passes; tests use the pinned local reference and do not
skip missing required content. UTF-8 BOM regression, explicit configuration
override selection, stderr diagnostics, separate physics-check application,
and the launcher's `--check` are implemented. See CHECKPOINTS.md for validation.
Reviewed baseline documentation is commit `06cdd9c1`.

M0a is commit `982c6058`. M1a replaces source-damaging parser reconstruction,
retains entry/controller expressions and their locations, and adds regressions.
All tests pass. M1a is commit `0720070e`.

M1b adds typed numeric/bottom evaluation, strict expression parsing, operators,
assignments, math/special forms, legacy clauses and environment-local extension
interfaces. The process-global evaluator hook is removed. The headless JSONL
expression runner is `tests/expressions`; six fixtures assert 15 observations.
All 1522 stock trigger lines parse. Full tests and headless language race tests
pass; a bounded fuzz run completed 108219 executions without failure.

M1b is commit `8e18f717`. M2a extracts the executor into `engine/` with public
compatibility aliases in `character/`, compiles/validates trigger programs once,
corrects grouping/persistence, adds Null, and tests pause filtering, transitions,
validation before mutation and shared-state host isolation. Its headless race
tests pass. Definitions are immutable after compilation.

M2a is commit `d4d808e7`. M2b implements explicit compiled state entry and
production hitpause/timer integration, with entry/default/persistence and timing
regressions. Full tests and engine/character race tests pass. Player.Update now
returns runtime errors; its existing statement callers still build. See
CHECKPOINTS.md for details and remaining reference-ordering obligations.

M2c compiles core typed controller parameters, adds ParentVarSet/Add and float
VarRangeSet paths, fixes integer argument conversion to warn/truncate, and tests
core expression/default/error behavior. See CHECKPOINTS.md. Many complex
controllers still use legacy handlers, and no controller is reference-Verified.

M2c is commit `d5d9ac0f`. M3a adds SFF v2/v2.01 and all required encodings,
strict decoding, and CPU palette selection. All 1376 sprites in 27 stock archives
load, and all 281 KFM sprite pixel hashes match Elecbyte's extractor. Original
fixtures were encoded with its compressor to cover absent formats. Full race
tests pass. Asset parity evidence is in CHECKPOINTS.md and REFERENCE.md.

M3a is commit `b7094832`. M3b adds bitmap/TrueType font loading and bank/alpha
render support. All 19 runtime font files load; full race tests pass. The scoped
`mugen.mod`/`mugen.sum` overlay is required for direct Go commands (the launcher
selects it). Root module files stay unchanged. BUILD.md explains font fallback.

M3b is commit `2f1ebe84`. M2d unifies animation clocks and adds actual Character
animation trigger bindings. A hash-pinned authored reference probe records 24
ticks and the clock matches every observation. Its runner uses process-window
captures and exact bitmap-font matching without keyboard/focus automation.
See tests/README.md and REFERENCE.md. Both stock characters load, with 105/108
states compiling; HitOverride remains an explicit three-state gap per character.

M2d is commit `2f82750b`. M2e adds named state-order/persistence probes and moves
the complete state pass into engine.RunStatePass. Negative order/abortion matches
the executable. Persistence probes disproved the earlier M2a success-count and
one-shot-side-effect interpretation: fixed to fire immediately, apply a cooldown,
and skip triggers when suppressed. Headless and Character tests use the same
authored programs and pinned numeric observations.

M2e is commit `cf651a4a`. M1c binds the full documented Const catalogue: 73
supported values match the executable; one documented constant is rejected by
the beta and by compiled Go paths. Retained CNS fields cover new recovery and
friction values, and Data.power initializes maximum power. Incomplete Data
groups are rejected by the reference; exact required-field/default rules still
need measurement. The optional Win32 reference error reader is built by
`mugen --reference-tools`. See REFERENCE.md and tests/README.md.

M1c is commit `d972c8d1`. M3c hardens sound archives and adds CPU PCM decoding;
all 45 stock sounds decode, full race tests pass, and 141173 fuzz executions pass.
Reference resource cases pin required primary st, fatal missing sound/common
files, and recognized st0..st9 (including gaps); st10/st12 are ignored. The Go
loader still needs to apply these rules. See resource-load.json and REFERENCE.md.

M3c is commit `d796bb58`. M3d applies the measured character resource rules,
first-definition state-file precedence, constants-only cns handling, strict
file/parse errors, character SND loading and named state diagnostics. It removes
the global common cache and adds explicit root/motif options plus a shared
Windows/case-insensitive resolver. Twenty resource cases run through the real
loader; their initial common-state control difference was later isolated to
round-control startup by M2f. Blank numbered ST files and blank sound are accepted; primary st remains
required. Exact common search order still needs probes; M1d below fixes
same-file duplicate/header binding.
Full tests/races and fresh reversed-order reference replay pass.

M3d is commit `283f5bb5`. M2f corrects ordinary transition parameter ordering:
ChangeState ctrl/anim apply before entry expressions, then explicit Statedef
parameters take precedence. Both hosts match all 24 values from the new authored
entry-control trace; this supersedes M2b's override-wins assumption. The common
probe now waits for both RoundState=2 and the initial control grant, and Go starts
at the same controlled point. All twenty resource cases then match directly;
there is no special common-state ctrl default to invent. Actual production round
control scheduling remains M4 work. Full regression/race checks and native
parameter replay pass.

M2f is commit `7f3ab076`. M1d fixes controller binding to the preceding Statedef
(the numeric controller header is cosmetic), first-definition selection within
a file and ignored orphan controllers. Native fixtures and both execution hosts
match every observation; parser tests reject phantom states/controller leakage.
Full tests/races and native binding replay pass. Malformed duplicate blocks and
unusual header spellings still need probes; this is not complete CNS validation.

M1d is commit `a73d257e`. M3e makes motif main SFF/SND/font and stage SFF errors
fatal/contextual, integrates the resolver and bitmap font-child names, and adds
the documented motif font search order (system, program, data, font). Previously
font/ was absent and missing assets were swallowed. All four stock motifs and
three stages now have actual load assertions. fontN.height no longer replaces
the font filename, but height application remains pending. Full tests/races pass.
Per-background SFF overrides, storyboards, language sections and complete drawing
behavior remain open; do not claim full asset integration/rendering parity.

M3e is commit `bf03f1ab`. M3f loads per-background SFF declarations for the four
supported motif screens and routes production drawing through Background.DrawLayer.
This fixes KFM's separate title archive and preserves masked/opaque images and
layer ownership when sprite IDs collide. CPU binding and render-route/cache
regressions pass with full tests/races. Storyboards and full background semantics
still need work; no screenshot parity is claimed.

M3f is commit `219e931f`. M3g introduces Storyboard.Load and replaces the old
application-only converter. All eight stock cutscenes load and finish bounded
CPU playback. Scene backgrounds, declared fonts/SND, 100 layers, path handoff,
contextual errors and safe player start are covered. Motif logo/intro/select/
fight references are now absolute; selection preserves resolved paths. Tests
and races pass. Full storyboard rendering/audio/text semantics are still open;
clearcolor=-1 is metadata only until persistent-framebuffer drawing is implemented.
No native storyboard timing/screenshot parity is claimed.

M3g is commit `a4caa9ea`. M0d adds repeatable production owned-window startup
captures and removes false shared-AST diagnostics while retaining unknown owned
attributes. Isolated KFM startup captures show logo/intro/title with readable
fonts and separate KFM title graphics. Evidence is in temp/app-runs/kfm-startup1
and kfm-startup3. Run2 exited cleanly early with cause unestablished; see the
ledger. Tool records binary/config hashes and commit/dirty state, sends no input
or focus changes, and is not a deterministic parity harness. Full tests/races pass.
The stock input diagnostics show wrong keyboard mappings from DOS conversion;
fix/reference-check numeric key codes and valid KeyA=0 handling next.

M0d is commit `7478a922`. M4a fixes SDL 1.2 numeric keyboard symbols, explicit
zero-valued KeyA/presence and disabled bindings, and adds per-manager injectable
keyboard held/edge queries. All 22 stock keys match, YAML KeyA overrides work,
and loaded stock keys reach facing-relative gameplay input. Confirmation honors
P2 Start independently. Joystick config values stay raw; runtime gamepad support
and layout-specific symbols remain pending. Full tests/races pass. See INPUT.md.

M4a is commit `9b166ac2`. M3h adds fight.Load and source-preserved fight layout
metadata. Both stock fight definitions load main HUD archives/fonts,
embedded AIR, fightfx SFF/AIR and common sounds. Missing/malformed declared
resources have contextual tests; full tests/races pass. This is the asset side
of the fight connection; no actual Match/Fight state has been added yet. The
versus screen still returns to title and selection still drops chosen players.

M3h is commit `32c7aca8`. M4b retains local/game coordinate profiles, implements
width-ratio math and float Const240p/480p/720p, and matches five native cases.
Fractional widths truncate; all 60 recorded fvars match (30 nonzero observations).
Menus now honor motif localcoord, background windows/center scale correctly,
and physical window dimensions no longer double. The HD title capture is
inspected at temp/app-runs/hd-menu-centered/. Full tests/races pass. These profiles
still need wiring into mixed-player geometry/camera and the actual Match loop;
see COORDINATES.md. No fight-state implementation has landed yet.

M4b is commit `dea6624e`. M4c adds mixed-player coordinate conversion for
positions, body distances, collision extents, push and facing/guard checks. Two
separate-player native cases pin 24 fvars and match; full races pass. See
COORDINATES.md for remaining hit-vector/camera/ownership/phase obligations.

M4c is commit `5cb382a9`. M4d connects selection/stage/versus to a shared
match.Match and production FightState with loaded HUD assets, lifecycle, input
injection, basic AI and result/rematch/menu routes. Full tests/races pass,
including the complete production state flow and real walk/punch/hit/recovery.
See MATCH.md for verification and gaps. Round phases/camera/combat timing are
provisional; guard, hit scaling, selected palettes, audio/effects and mode
progression still prevent the full M4 gate.

M4d is commit `c7486a71`. M4e fixes guard-state ownership, animation-based
guard-end recovery and movement control gates. Three native fixtures match 36
state/time observations; guard-end replay and full race checks pass. Guard combat
flags/damage/velocity/pause remain open.

M4e is commit `c410e1b0`. M4f translates received hit vectors/defaults and
retains common game coordinates. Six native contact cases match, including the
no-change fall X sentinel becoming infinity. Three language programs confirm
scientific notation and float/Exp/power overflow; the beta requires a decimal
point before an exponent. Full races and a native smallgame replay pass. See
COORDINATES.md/REFERENCE.md. Complete combat/guard rules, selected palettes and
audio/effects remain next.

M4f is commit `5583b1a7`. M3i adds compiled RemapPal and instance palette maps,
stock v2 selected colors in character/HUD rendering, revision-based cache refresh
and retired-owner pruning. Documented non-transitive/removal/depth/eight-slot
rules have authored tests; both stock archives have selected-pixel checks. Full
races pass and selected-palette-fight captures show alternate P2 colors. Native
RemapPal edge probes, SFF v1 external palN and full effect/helper ownership remain
open.

M3i is commit `0b2f2d0b`. M4g uses shared root state/contact/damage/physics
phases in production and the character diagnostic. Three native trade fixtures
match first-entry pre-damage life and next-tick post-damage life/state/contact
fields, including newly-started hitpause. Native paused replay and full races
pass. The group tick also backs all hit coordinate/default fixtures. Full entity
registry, priority/guard/HitOverride and helper/projectile phase rules are next.

M4g is commit `183f40e0`. M4h adds HitOverride and shared attribute filters;
all 108 stock states now compile. Twelve native cases match slots/defaults/expiry,
omissions, forceair, intermediate get-hit entry and damage state behavior. Full
races and forceair replay pass. Remaining custom-state exclusions, invalid-value
fallbacks, ownership and pause edges prevent a full HitOverride parity claim.

M4h is commit `a852b04f`. M4i corrects the shared scheduler to collide prior-tick
HitDefs before state passes, separates attacker hitpause/receiver hitshake, and
implements guard damage/flags/vectors/timers/defaults. Twelve native programs
match; full races and pause-release replay pass. M2b/M4g timing interpretations
are explicitly superseded. Native guard.slidetime defaults to ground.slidetime,
contrary to the manual. Remaining protection/priority/entity/guard-entry and
complete GetHitVar behavior prevents a complete combat claim.

M4i is commit `5ed632a8`. M4j integrates HitBy/NotHitBy as two shared timed
collision filters with typed duration, mask intersection/complement and slot
replacement. Twenty-five native protection programs (one rejection) and two
current-posture HitOverride programs match; full races and a dynamic-pose replay
pass. Both value/value2 are accepted with value precedence, contrary to manual.
Current attacker StateType drives matching, including changes after HitDef.
GetHitVar(damage) clears after contact damage processing. Complete pause,
invalid-value, entity and custom-state behavior remains open.

M4j is commit `f8cb21e6`. M4k adds attacker-facing/local guard range, retained
HitDef overrides and typed AttackDist/PlayerPush, fixes attack-first root state
passes (stable ties), preserves custom facing, and matches native near-axis push
spacing. Twenty-three programs/26 cases include mixed range and push. Full races
and a guard-clock replay pass. See REFERENCE.md for spatial/order evidence and
failed draft constant setup. Audit native required Data constants separately.
Dynamic Width, airborne/corner/pause push, projectile range, full auto-turn and
world ownership remain open.

M4k is commit `c8993728`. M4l adds typed sound controllers, independent audio
owners/queues and a thread-safe CPU mixer. Twenty-three authored programs cover
24 native cases, 55 tone segments and 63 Go tick checks. Full races pass and a
fresh channel audio replay matches. See AUDIO.md for measured common/local pan,
channel replacement/global stop, ignored pitch/deprecated volume, prefix parsing
and private PulseAudio provenance. Production match/device/HUD/contact audio is
not connected yet: resetRound must bind each new character to its actual own
bank and fight common bank on the shared queue. Integrate that pipeline next.
The task-owned private Pulse server runs in exec session 77895, with all state
under temp/audio-reference/. Do not touch the desktop's audio server.

M4l is commit `dfe8ae1b`. M4m connects one real streaming device player, menu/
character/HUD queues, own/common fight banks before entry, live volume and scene
cleanup. Stock sound config and optional YAML volume/disable values now load.
Full races pass; a native HUD tone matches gain/expiry. Actual private-device
capture sound-match-playback contains nonzero PCM and inspected fight frames.
See AUDIO.md/BUILD.md. Contact/guard/KO voice cues, native lifecycle, complete
sound schemas, BGM/storyboards and resampling fidelity remain open. Continue
contact sounds/channel-0 interruption and common hit effects next, then full
HitDef/entity semantics. No desktop audio configuration was changed.

M4m is commit `b616a7e4`. M4n adds typed HitDef sound expressions/F/S/group-only
syntax and production contact/guard cues. Nine audio programs (32 native segments,
45 Go tick checks) plus five grammar programs match; full races and fresh override/
common-single replays pass. Normal hit and idle HitOverride stop defender channel
0 before entry CNS; guarding preserves it. Contact panning uses defender screen
X from the left edge in the measured scenes. A later HitDef with omitted sound
clears prior choices. Real KFM combat now requires a common cue and audible PCM.
Continue common hit/guard effects, KO voice/echo and full combat/entity semantics.
Other HitDef fields remain legacy and complete sound/ownership parity is open.

M4n is commit `99372c03`. M4o adds root KO sound 11,0, KO.echo half/quarter
voices at counter 20/50, and current global root nokosnd suppression. Four native
programs (21 segments/25 Go tick checks), full races and a fresh initial-only
suppression replay pass. Voice isolation uses only a disposable valid original
empty HUD bank; negative IDs alone retained the spoken announcement. Earlier
captures remain diagnostic. See AUDIO.md. Complete KO states/kill flags/revival,
helpers and Pause/SuperPause still need work. Common hit/guard effects are next.

M4o is commit `6ba7c9c1`. M4p connects typed spark parameters, shared effect
clocks/resources, finite expiry/clear and production contact drawing with AIR
blend metadata. Eight native programs cover five markers, two absences and a
Data S-prefix coercion packet; frame anchors/resource choices match. Full races,
pure core races, common-default/guard native replays and real normal/additive
Go window inspections pass. See EFFECTS.md. A concrete M4d viewport discrepancy
is now measured: native 1280x720 stage fits at scale three with horizontal margins,
while Go's provisional width/cut path differs. Investigate world/HUD framing and
screen/game coordinate bindings next. Full effect/AIR/pause/ownership is open.

M4p is commit `253649ce`. M4q binds viewport queries, stage-aspect match height,
independent world/HUD fits and common-resolution actors/effects. Twelve native
cases pin 204 accepted float observations plus three legacy rejections. Full
races and fresh stage11/rejection replays pass. The actual 1280x720 owned capture
viewport-spark-hd/ shows corrected margins; filter edges still differ. Earlier
viewport-spark-production/ stayed 320x240 and is diagnostic only. The smoke
harness now validates actual dimensions. The loader retains source language
version, compiles nonempty states and applies the three measured 1.1 viewport
gates through typed arguments/entry/triggers, including AST cache hits. Other
version rules/legacy handlers remain open. See COORDINATES.md.

M4q is commit `7a1b9ec9`. M4r corrects its empty-state assumption: native rejects
effective empty declarations during loading, including final/negative/entry-only
states. Same-file duplicates are ignored, but lower-priority later files are
still validated. Five native cases match; the loader now compiles each loaded
state file before merge and shares the compiler with the player. Full races and
a fresh shadowed-state replay pass. See CHECKPOINTS.md/REFERENCE.md. The executor
already rejected empty states correctly. A finite state-chain probe is drafted
as state-chain.cns/.air but not yet recorded or committed: measure the provisional
ten-transition cap next.

M4r is commit `4d53ebd2`. M4s replaces the ten-state positive-chain cap with the
measured native guard: 2502 executions can terminate; continuing beyond that
raises the displayed 2500-loop error and names the pending state. Fifteen native
programs (84 integer packet values/eight rejections), headless/full races and
fresh overflow/negative-launch replays pass. Read-only executable inspection
confirms its post-increment greater-than comparison. See REFERENCE.md. The budget
across separate physics/engine resumes and landing phases remain open. Current
finishTick enters 52 after movement without executing Time=0 CNS that tick;
compare native landing next, including shared continuation budget.

M4s is commit `697fd8e1`. M4t fixes native movement-before-forces, immediate
landing CNS/physics resumption with retained Y/VY, state-105 exemption and CNS
budget sharing across engine resumes. Eighteen programs pin 336 values and one
overflow. Engine landings do not consume the CNS counter; native accepts 2510
of them in one tick. Custom 40/50/51/52/100/105 retain their CNS-owned exits, so
Go's forced exits are removed. Full races, pure engine races, frozen/budget
replays and actual mixed stock jump/landing recovery pass. See PHYSICS.md.
Other builtin recovery/trip/KO/auto-turn/input rules, full phase/body/pause and
world ownership still need native comparison. No full M2/M4 gate is claimed.

M4t is commit `05dc5222`. M4u adds typed Pause, shared GameTime/actor Timeline and
Match freezing. Thirteen native programs pin 264 values; full/pure races and a
fresh pair-caller replay pass. Requests freeze following ticks, invalid movetime
falls back to zero, and missing time is zero. Replacement/cancellation/last-caller
behavior matches. Command buffering and presentation/reset integration have
authored tests, not native interaction parity. See PAUSE.md. The SuperPause pilot
only tests pure Timeline clocks; its production controller is still a stub.
M4v below supersedes that production-stub status. Absolute GameTime origins
remain unverified.

M4u is commit `b7df46ff`. M4v implements typed SuperPause through production
clocks, power, protection, acquired-target defence, audio, effect clocks and
darkening. Thirty-eight programs pin 600 numbers, ten render cases and seven
audio segments/Go PCM checks. Full races and fresh serial effect-clock/other-
pause/isolated-sound replays pass. A stock KFM super consumes 1000 power and
uses common action 100/sound 20,0 with 30 frozen ticks. See SUPERPAUSE.md.

Frozen attack boxes can hit moving receivers, correcting M4u's assumption.
Frozen receivers enter the hit state immediately, accumulating damage until
resumption. New HitDef rearms contacts; existing hitpause counter traces still
pass. Super effects animate through either pause with a frozen caller, retain
spawn position, and retire after one finite pass; -1 frames survive pause end
and later anim=-1. The acquired-target generation ledger is limited to the
measured defence behavior, not a complete entity/Target registry.

Earlier overlapping window/audio drafts are diagnostic only. Run all native
captures, Go window captures and audio probes sequentially on :97/private Pulse;
the final serial paths/hashes are recorded in SUPERPAUSE.md. Pure tests with no
DISPLAY can run alongside; full Ebiten tests must also wait for captures.

M4v identified StageFit/SystemFit/KeepAspect and HUD projection for comparison: native loads
data/mugen1/fight.def and places portraits near X=172/Y=14, while Go's independent
HUD fit puts them near X=16/Y=16. World margins/marker coordinates are correct,
but M4q did not establish native HUD/full framebuffer parity. Then continue
world/entity/target ownership, complete HitDef/input/effect/audio/mode gates.

M4v is commit `4c1391e5`. M4w fixes fight HUD projection and StageFit=0 cropping.
Ten native cases pin 240 numbers/twenty marker cores. Top cuts truncate in stage
units, honor high/low guidelines, and feed CameraPos/ScreenPos and rendering.
HUD uses motif-width conversion and the world's physical transform/top origin.
KeepAspect fits or stretches the effective viewport into the real window;
SystemFit=0 leaves these fight scenes unchanged. See COORDINATES.md.

Full races and pure coordinate races pass; fresh crop765/window-stretch native
replays reproduce packets/core hashes. Actual Go windows have exact HUD cores
and matching world centers, but filtered world edges remain smaller. Do not
claim framebuffer parity. Video flags and presence-aware YAML overrides are
retained, and default window size follows game configuration. Menus/SystemFit,
other renderers/forced-fit fallbacks, tall overdraw, other guidelines, camera/
zoom and full raster parity remain open. Next separate world/entity/input IDs,
opponents/acquired targets and runtime redirection before complete combat
ownership. Keep native/Go captures and Ebiten tests sequential on :97.

M4w is commit `ca067064`. M4x separates InputSlot/EntityID/TeamSide/HelperID and
Opponents/acquired targets. The match-owned registry supports live IDs and
runtime parent/root/helper/enemy redirection/counts. Nine stable programs cover
thirteen cases, 240 numbers and one VM rejection. Helper capacity clamps to
4..56 and determines initial root IDs. Nested helpers share their root family;
leaf DestroySelf aborts later controllers, retires the ID and does not reuse it.
See ENTITIES.md for exact scope and remaining global scheduling/parameter work.

M4x also corrects documentation-based bottom assumptions: measured operators
consume integer zero, IfElse selects false for a bottom condition, and Cond
preserves it. VarSet/Add and VelSet/Add zero boundaries match; bitwise NOT of
bottom remains fatal with source context in the headless/production executor.
Full races, pure engine/evaluator races and fresh capacity/nested/operator/fatal
replays pass. The old JSONL logical-bottom expectation is superseded. BOTTOM.md
records two unresolved native power programs returning layout-dependent values;
they are not Go conformance goldens or included in the stable numeric count.

Continue acquired-target/HitDef ID retention and Target controllers against
native fixtures, then complete helper typed parameters/negative states/ownership
and projectile/global phases. Registry round reset is authored-tested; native
ID continuity across rounds still needs a trace. Complete menu/SystemFit, raster
filtering, input, BGM and mode/asset/state gates remain open. No full compatibility
or complete state machine claim is made.

M4x is commit `9c8c29b6`. M4y adds 49 native programs/744 numeric values/three
load rejections for target retention/labels/recovery/guards, typed drop/facing/
life/power/velocity effects and redirection scope/units. Full scoped/pure races
pass, with representative native replays. See TARGETS.md for exact measured
boundaries and remaining schemas. HitDef ID defaults/clamps to zero. Direct
geometric redirects use caller units; eager call arguments use the origin,
while delayed Cond uses destination variables and units. Scalar variables and
declared constant kinds stay distinct.

Native target life changes are immediate; negative values use receiver defence/
super factors, healing is unscaled and caller AttackMul is ignored. The normal
damage kernel multiplies runtime DefenceMul, clearing an idle adjustment for
a new get-hit sequence. A -3/-2/-1/positive trace confirms all four see pending
damage before Life changes. The initial upward coordinate draft moved the native
camera; the promoted fixture isolates camera at zero. Native camera easing is
still different from Go and remains open.

Next implement actual TargetState/ChangeAnim2/SelfState state/animation ownership
and TargetBind/BindTo* geometry/clocks, then complete helper typed parameters,
negative eligibility and multi-target/projectile/world phases. TargetState only
has required-argument validation in M4y. Native multi-target keepone/RNG, base
stats/rounding/kill and full configuration/modes/assets/input/audio/render gates
remain open. The two M4x bottom-power cases remain unresolved, outside stable
conformance goldens. Keep captures and Ebiten tests sequential on :97.

The desktop now reports no connected monitors. Tests/probes use private Xvfb
:97 with XAUTHORITY=game/mugen/temp/virtual-display/Xauthority and software GL;
see BUILD.md. The server was started by this task (exec session 15800), with
all extracted files under temp/. Do not reconfigure the desktop.

Continue native pause/input/contact/effect interactions, builtin-state/recovery rules and complete
HitDef/protection/priority/entity and effect semantics. Continue complete storyboard/rendering
semantics and world/entity ownership against authored reference fixtures.
Selected stock v2 palettes and the deterministic one-on-one flow are integrated.
M4s supersedes the provisional ten-transition limit. State ownership, match flow,
font rendering parity, and many controller/world
behaviors are incomplete. Do not call M1 or full M0/M2 complete.

The reference quick match loaded successfully through match-loop initialization
under Wine. The binary identifies itself as 1.1.0 Beta 1 P1 (2013.08.11).
See REFERENCE.md; no Go gameplay parity or full semantic harness is claimed.

## Next action

Compare helper sound ownership and native bound-target release publication,
then opposing-helper contacts. Continue positive input, helper palettes/foreign/paused creation,
remaining facing/priority/hit flags and attacker-hit interrupted binding controls.
Continue
remaining effect motion/bind/pause/owner semantics and the other binding/helper/world and
engine/asset gates. The selection-to-fight path runs, but the complete M4 and
whole 1.1 beta compatibility gates remain open. Implementation
choices are delegated after approval; ask only about concrete blockers or
unresolvable scope/reference ambiguities. Keep this handoff and the evidence
inventory current.


customhit-* packets use their pinned independent opponent and profile. Most
specify --p2-localcoord 1280,960; customhit-bound-* do not. Use each JSON's exact
pages, including supplemental page 28 for cancel/drop. TestNativeCustomHitStates
replays all 38 programs; pure TestNativeInfiniteAnimationClocks compares three
infinite/nonfinal schedules. JSON capture run fields identify supplemental
native captures. See CUSTOMHIT.md for boundaries and diagnostic exclusions.


The stock KFM/KFM720 regression window also runs at 1280x720 through seven
seconds at temp/app-runs/customhit-stock-regression-go/, with both full-sized
actors and no runtime errors. This remains a smoke check, not pixel parity.


M4ad throw-stock-{base,drop,forward,start} use --p2-fixture throw-stock-victim,
--file st0=kfm.cns --file stcommon=common1.cns --file anim=kfm.air, version 1.1,
fvar/CMD clipboard and seven-second captures. Use pages from each JSON. Their
actual original CNS/AIR inputs are additionally pinned by file_sha256. The
contactframe-* programs use their same-name -victim fixtures and pages 0 12.
TestNativeStockThrows and TestNativeContactFrames replay all eight packets.
The initial throw-relative base draft is not proof of first-grab timing; the
start stamp must remain. No original state/art files were committed.


M4ae TestNativeExplodLifetimes compares 18 explod-* packets; these same-fixture
opponents do not need --p2-fixture. Use each packet's fvar/CMD/version/pages;
two float grammar packets have expected_load_error. Missing anim is accepted.
TestNativeExplodMarkerGeometry compares nine explodmark-* packets and the own/
common sprite archive. spark_probe.py supplies the existing RGB calibration;
its historical JSON key is spark although these controllers are Explod. Common
marker recipes replace only disposable fightfx files; no original art is tracked.
Run --fixture NAME --name UNIQUE --action 9100 for marker replay. Marker byte
hashes and constant groups are required. See EXPLODS.md for remaining limits.
