# Reference execution evidence

The pinned distribution is labeled M.U.G.E.N 1.1 beta 1 in readme.txt
(2013-07-27). Its executable reports **M.U.G.E.N ver 1.1.0 Beta 1 P1
(2013.08.11)**. Compatibility experiments use that supplied executable and the
109-file hash manifest. The engine runs from a copy. Read-only extraction and
compression tools write only to disposable output directories; the original
distribution is unchanged.

## M4ap original RGB actor-order calibration

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

## M4ao HitDef target/team selection

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

## M4an helper defenders and birth contact timing

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

## M4am helper binding and deferred hit facing

M4am adds anchor-completion binding publication, idle-anchor constraints,
external 5050 entry on bound-helper retirement and deferred typed P2 hit facing.
Twenty-seven native programs compare 564 numbers, one load rejection and 108
input hashes. Third-entity/own-CNS flag controls, changing speed, own/borrowed
states and reversed headings distinguish the phases. Full scoped races pass in
temp/m4am-full-race.log; focused/pure races and five fresh native replays pass.
See HELPER-BINDING.md. Whole binding/combat/helper/render/asset/mode gates remain
open; the helper fall's late clipped right sprite still has a small raster gap.

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

## M4al helper channel ownership and retirement

M4al adds native helper sound ownership controls: two programs, 24 numeric
observations and eight PCM tick checks (48 stereo tone amplitudes). Root/parent/
descendant voices coexist on channel 1. Both ordinary and recursive DestroySelf
retire entities while their sounds continue; root channel-1 StopSnd removes only
the root voice. Existing production behavior matches, so no stop-on-retirement
change is introduced. All 29 PCM programs/30 cases pass race comparison in
temp/m4al-pcm-first-race.log; both fresh native replays pass. See AUDIO.md.

## M4ak helper retirement and orphaned effects

M4ak adds typed DestroySelf recursion/effect flags and world-driven orphan
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

## M4aj helper configuration and shared scheduling

M4aj adds compiled Helper arguments, helper-owned size overrides and a shared
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


## M4ai Width and edge queries

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

## M4ah screen confinement and camera eligibility

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

## M4ag horizontal camera following

Twenty camerafollow-* programs compare 720 numbers through the production
Match scheduler. All 60 authored CNS/AIR/stage hashes are checked. Static
tension/threshold/direction/spread, per-root phase, long/retained-goal, stage
limits, speeds .5/1/2/3, one-sided movement and mixed-profile motion distinguish
the measured zoom-one behavior. The old center-average failures and the initial
static-only draft are retained as diagnostics; the latter does not establish
walking until the extreme-actor speed floor is applied. Full scoped races pass
in temp/m4ag-full-final-race.log. See CAMERA.md for values and remaining scope.
Four fresh native phase/return/slow-motion/limit replays match all packet values
and input hashes. Seven real motion/stock-throw Go window captures were hash
checked and inspected; remaining clipping/background/filter limits are recorded
in CAMERA.md. No complete framebuffer or screen-boundary claim follows.

## M4af fixed nonzero-camera coordinates

Seven authored programs pin 192 numbers and two RGB positions. Five camerapos-*
packets compare positive/negative horizontal offsets, vertical displacement,
mixed/borrowed state units, Enemy/P2Dist and omitted PosSet axes. Two cameramark-*
packets compare own/common stage effects using Pos X+CameraPos X. Stage sidecars
are authored, hashed, copied only into disposable native runs and loaded through
the real Go stage loader. Equal camera start/bounds isolate the coordinate
contract from still-incomplete native camera following. See COORDINATES.md for
exact samples, old-binary failures, final window inspection and filter limits.
Four fresh negative-camera/borrowed-state/own-common RGB replays match, including
stage and marker core hashes. All 25 authored input hashes are checked.

Initial stage-path/clipboard setup failures and the first edge-clamped start
are diagnostic runs only. Final inputs use -s probe-stage, CMD clipboard output,
and player starts inside the fixed view. No original distribution files changed.

## M4q viewport evidence

The viewport catalogue has eight cases: seven accepted profile combinations
and one 1.0 ScreenWidth rejection. Four additional programs cover same-pass
PosSet/ScreenPos, accepted legacy game/camera/position queries, and rejected
legacy ScreenHeight/CameraZoom. Together they pin 204 accepted float observations
plus three load errors. Fixture source/executable/AIR hashes and query packets
are checked; stage11 also pins the disposable stage-version edit. Fresh stage11
and legacy ScreenWidth replays match.

The observed stage-aspect match dimensions and independent world/HUD fit are
recorded in COORDINATES.md. Owned Go capture viewport-spark-hd/ is actually
1280x720, with binary/capture hashes and inspected margins. Filter edges remain
different from native. Earlier viewport-spark-production/ was inadvertently
320x240 due commented window config; the harness now supplies explicit dimensions
and rejects a capture-size mismatch. Camera displacement/zoom and whole-frame
image equivalence remain unverified.

## M4r empty-state loading evidence

Five minimal source/AIR/executable-hash-pinned cases establish load behavior:
empty-default (unreferenced final 700), empty-explicit (700 with entry attributes),
empty-negative (-3), and empty-shadowed (st0 state 0 after st already defines 0)
are rejected with “Need at least one state controller.” Empty-duplicate places
an empty 0 after the valid same-file 0; native ignores the duplicate and retains
marker 111. A fresh empty-shadowed replay reproduces rejection. Each loaded
file's first definitions therefore require validation before the files merge.
The loader now does so, retaining contextual errors and compiled programs.

Draft captures empty-default-native and empty-default-positive-native contained,
respectively, an empty -3 before other declarations and an invalid Physics query.
They are diagnostic, not the minimal pinned positive cases. Final minimal runs
are empty-load-default/explicit/negative, empty-duplicate-native and
empty-shadowed-native. M4q's supposed native acceptance of empty declarations was
an assumption, now disproved; no change to the executor's rejection was needed.

## M4s positive state-chain boundary

Fifteen original programs cover a 24-iteration alternating chain, total positive
execution lengths 2499..2503, larger alternating chains, a self loop, and chains
launched from -2. Seven successful packets contain 84 integer observations;
eight programs reach the native runaway error. A terminal positive execution
number 2502 succeeds; if that execution transitions again, native rejects the
pending state before its controllers execute. The 2503 terminal-chain error names
704, the negative-launch overflow names 703, and other rejected chains name
701/702 as pinned. All accepted chains complete at Time=0 with zero fixture-frame
delta; state entry power additions also match. The fixture resets its local
counter at launch to exclude unrelated round-control timing.

Read-only objdump inspection corroborates the counter interpretation in the
same hash-pinned executable. At VA 0x43c6cb an initial state-execution call precedes
the loop. The check at 0x43c6dd copies the old counter, increments it, compares the
old value with 0x9c4, and uses signed greater-than to branch to the error at
0x43ce5c. The next execution is called at 0x43c701. Thus the displayed 2500 is
not an execution count; it permits 2501 follow-ups after the first execution.
This explains the probes' 2502 boundary. No disassembly/binary is bundled.

The shared executor and production Character both replay every pinned success/
failure, with source/AIR hashes checked in Character. Fresh state-chain-overflow-
replay and state-chain-negative-replay match. The early state-chain-native capture
included absolute intro-frame values; the final state-chain-boundary fixture
resets that counter and is the pinned successful base program.

The inspected native function can return to positive execution after engine/
physics processing without resetting its counter. Current Go RunPositivePass
calls have separate budgets. Native landing/physics phase probes and shared
budget integration remain necessary; this checkpoint is limited to each
positive chain and its negative-state launch behavior.

## M4t physics and resumed state execution

Eighteen programs pin 336 numbers and one native runtime error. Landing simple/
transition/hit/frozen cases establish immediate 52 CNS, retained velocity and
overshoot, resumed movement and one final clock update. Physics standing/
crouching establishes position integration before friction; backhop and Physics
N probes distinguish state 105 and ordinary state 50. Six custom-state cases
show that 40/50/51/52/100/105 retain their scripted state through Time=5. See
PHYSICS.md for the values and limits.

Landing-budget and its overflow variant establish that the CNS counter is
retained after engine landing: terminal 705 succeeds, while the extra transition
fails at pending 706. Repeated engine-entry programs finish after 3 and 2510
landings in one fixture tick, so engine entries do not consume the CNS counter.
The shared Go budget now preserves this distinction. Fresh frozen-landing and
budget-overflow replays match their pinned packets/error.

The initial landing-transition capture request exceeded the 40-fvar bank and
was rejected by the harness before launch; final source/captures use page 28.
Focused early observations suggested clamping, but the frozen case disproved
that: movement-before-acceleration explains the normal Y=0 and overshoot cases.
No clamp or velocity reset is applied by the engine landing path.

## M4u root Pause clock packets

Thirteen programs pin 264 values. Pause-three/one/zero/move-one/move-all/other
distinguish duration, caller movement and caller ordering. Pause-missing/negative/
overmove/negmove establish accepted omission and invalid numeric fallbacks.
Pause-replace/cancel/pair prove replacement, cancellation and the later idle
root's precedence. A fresh pause-pair replay reproduces both packets. Every
source/AIR/executable hash is checked by Character clock tests.

GameTime keeps advancing while local Time, position and animation freeze. The
issuing tick finishes; time=3/default movetime gives the first Time=1 observation
at relative GameTime=4. Movetime=1 gives Time=1 at GameTime=1, Time=2 at GameTime=4.
Visit counts for ordinary/ignorehitpause controllers remain one during the freeze.
Missing time is accepted as zero; movetime greater than time falls back to zero
rather than the duration. See PAUSE.md for runtime integration and limits.

Superpause-three pins 24 additional values with presentation/defence effects
disabled. It backs the pure Timeline clock, not the still-stubbed production
SuperPause controller. End-command buffering, backgrounds/effect clocks and round
reset have authored Go tests; they have no native input/presentation proof yet.

## Wine startup, 2026-10-16

Environment: Wine 9.0, a private Win32 prefix at `temp/wine-reference`, and a
full distribution copy at `temp/reference-runs/stock`. Invocation from that copy:
`wine mugen.exe kfm kfm -s kfm`, with a 25-second timeout. Wine debug output was
captured to `temp/reference-runs/quickmatch.log`; the executable wrote
`temp/reference-runs/stock/mugen.log`. The private wineserver was stopped after
the experiment. Temporary logs/prefix/copies are excluded from commits.

Observed in the engine's own status log:

- Graphics initialized in its OpenGL mode with a 1280x720 game coordinate space.
- The default motif, fonts, sound, fight definition and Lua initialization loaded.
- Quick VS entered the gameflow loop and loaded stage, KFM CMD/CNS/common states,
  SFF, AIR, SND and palettes.
- The engine reported 3012 expressions (979 on trigger lines) for stock KFM.
- Startup reached `Game loop init` and `Match loop init` before the timeout.

This establishes that the supplied executable can load a stock quick match in
this environment. It does not establish any Go parity, a completed round,
deterministic input replay, or a semantic reference result. The help invocation
`wine mugen.exe -h` remained open until its 40-second timeout; help exit behavior
is not used as a success signal.

## Sprite extraction and compression, 2026-10-16

The bundled `sff2png.exe` identifies as SFF2PNG v0.2 (2010-07-12). Invoked with
`chars/kfm/kfm.sff` and an output prefix in `temp/reference-runs/extract-kfm`,
it exited successfully and wrote 281 indexed PNGs. The Go decoder matches all
281 opaque RGBA pixel hashes; axes/group/item are checked against source headers.
The recorded oracle is `tests/testdata/kfm-sprites.json`, including source/tool
hashes. Transparency masking is tested separately because the tool exports
opaque palettes. This is decoder evidence, not animation or fight parity.

The bundled `sprmake2.exe` identifies as SprMaker 2.01beta (2010-06-16). It
successfully encoded six original patterned sprites to exercise RLE5, PNG8/24/32
and uncompressed RGB/RGBA. The fixture is `sff/testdata/formats.sff`; its
definition and standard-library Python input generator accompany it. The Go
decoder matches every visible pixel/alpha against the input formulas. RLE5
runs encode length minus one, unlike the incomplete C++ reader's implementation.
Raw RGB/RGBA bytes are in channel order without a decoded-size prefix; compressed
formats include that prefix. These findings have focused regressions.

## Animation loop trace, 2026-10-16

`tests/reference_probe.py` prepares a disposable distribution copy with the
authored `tests/testdata/reference/animation-loop.cns` and `.air`. Stock KFM
constants/assets/commands remain available; the command state is replaced by a
neutral Null controller so it cannot overwrite trace variables. The 2/3/4-tick
action loops at element 2. CNS records its first 30 ticks in player variables,
then displays two six-parameter lines per capture (the documented printf limit).

Four captures from `temp/reference-runs/animation-loop-clean2` supplied 48
encoded observations covering 24 ticks. Exact matching against the bundled
f-4x6 debug glyph masks reads the numbers, and captures were inspected visually.
The fixture JSON records source/engine/capture hashes and decoded tick values.
The shared Go clock matches all of them, including three loop-boundary ticks.
This establishes the finite-loop timing used by those assertions, not complete
animation or rendering parity.

The finalized runner reproduced pages 0 and 30 in a fresh
`temp/reference-runs/animation-loop-verified` run with all assertions passing.

The automatic runner checks hashes and numeric observations. It requires
Wine, DISPLAY, xdotool, ImageMagick import, and Pillow, and captures only its own
spawned process window. It sends no keyboard/mouse events or focus changes. The
private prefix is shared with these reference runs and must not be used by an
unrelated Wine process. Each invocation creates a fresh named run directory.

Exploration found that Lua player query exports exist, but the bundled Lua
wrapper scripts do not expose a usable per-tick capture hook at initialization.
Input/console attempts did not produce a reliable trace; they are not part of
the runner. Clipboard captures provide the reproducible route used here.

## State order and persistence, 2026-10-16

The named `state-order` fixture records 32145, showing -3/-2/-1 followed by a
two-state positive chain. Counters after each ChangeState stay zero; the final
state observes StateNo=31, PrevStateNo=30 and Time=0. This resolves the earlier
C++ comment ambiguity for ordinary self-owned negative states. Ownership and
helper eligibility are separate outstanding cases.

The `persistence` fixture records intermittent conditions true except at ticks
1,4,7,10. Persistent=2 activates at 0,2,5,8,11 and evaluates its side-effecting
trigger eight times. Persistent=0's side-effecting trigger evaluates once. This
disproves the earlier interpretation that N counts only successful conditions
and that one-shot trigger expressions continue after activation. The executor
now applies a pre-trigger cooldown/one-shot gate. Negative-state re-entry and
pause interactions still need probes.

Numerical evidence and engine/source/capture hashes are committed alongside the
authored fixtures. Fresh `state-order-verified` and `persistence-verified` runs
reproduced every observation. The production Character and headless state-pass
host execute the same programs against those numerical assertions.

## Constant catalogue, 2026-10-16

The bundled trigger docs list 74 Const names. The two authored `constants-a/b`
fixtures query them through fvars, and seven debug clipboard pages capture values
with nine significant digits. All 73 supported names match Go's retained fields
and computed fall defence multiplier; Go tests separately check declared kinds.
Stock KFM omits data.power and receives 3000, which the port now preserves.

`Const(movement.jump.changeanim.threshold)` is rejected at load with
"Invalid cnstname movement.jump.changeanim.threshold in: Const". This was observed
for stock mugenversion=1.0 and explicitly for 1.1. The dedicated
`constant-unavailable` fixture pins the rejection; a fresh verified run matches
it. The numeric B fixture leaves that slot unavailable, without claiming a value
for it. This is an actual binary/documentation mismatch in the supplied beta.

Attempting empty constants groups or Data containing only life/attack/defence
fails with "Can't load Data constants". The exact required-field set is not yet
established, so no invented defaults are derived from those failed experiments.
The successful numeric pages use the original stock KFM constants. Fresh
`constants-a-verified` and `constants-b-verified` runs reproduce selected pages.

The optional `tests/reference_text` helper runs as Win32 inside the same private
Wine prefix and reads only the known MUGEN error dialog through Win32 text APIs.
It makes no input/focus/clipboard changes. Build it with `mugen --reference-tools`;
expected-error probes require it. The runner stores complete error evidence in
its temporary run directory and checks the stable expected rejection text.

## Resource file rules, 2026-10-16

The `resource-load` fixture records a state-file marker with its clipboard in
the neutral CMD state, allowing tests to change state-file declarations. The
pinned case matrix shows these load failures: omitted/blank primary st, missing
declared st8/st9, missing sound, and missing stcommon. Missing st10/st12 files
are ignored and the character reaches its marker state. The st9/st10 boundary
also holds with an explicit mugenversion=1.1 declaration.

Cases and engine/fixture/capture hashes are in `resource-load.json`; errors were
read directly from the private MUGEN error dialog. The runner accepts --file and
--omit-file when authoring, and --case when verifying a pinned matrix entry.
This evidence corrects the Go loader's arbitrary scan/break behavior: all ten
recognized extra state-file slots must be considered, even after gaps. Additional
merge precedence, motif/common fallback and palette behavior still need probes.

## State resource precedence and blank files, 2026-10-16

`resource-order` records marker 111 from primary st with st/st0 declarations in
either order, and marker 222 from st0 over st9 with reversed declarations and
sparse slots. Its main cns marker 900 never runs when only listed as cns.
`resource-priority` records primary marker 111 over CMD marker 333. These establish
fixed first-definition precedence across state files. Same-file duplicates are established separately by the state-binding fixture
below. The harness hashes and copies a
fixture's authored .extra directory and pins --state-file-order on replay.

Additional resource-load cases show blank st9 and blank sound succeed. Common
cases for blank, omitted and explicit stcommon reach common state 0. The initial
probe entered 5900 as soon as RoundState=2; the round's later control grant made
its ctrl observation differ from an already-running Go host. Requiring ctrl as
well before the forced transition removes that interference. Go begins at the
same controlled point, and all twenty resource cases compare directly. Earlier
known_go_differences were removed. Production round-control timing is still open.

The real character loader consumes these cases and reports missing/malformed
resources with DEF key/line plus resource/state source. Shared-resource search
now has explicit roots, but the exact native character/motif/data priority needs
separate conflicting-file probes. Selected external/internal palettes and
non-character load failures remain reference work.

## Transition parameters and entry precedence, 2026-10-16

The authored entry-control sequence runs ordinary states 700..706 and records
24 variables in two pages. Conflicting transition ctrl=0 / Statedef ctrl=1 yields
1, and the reverse yields 0. Statedef anim=9990 wins over transition anim=9991.
Omitted destination parameters preserve transition values. An entry animation
expression conditional on ctrl sees the transition's incoming control=0 and
selects 9991, even though the preceding state had control=1. Both the pure host
and Character match every recorded value after applying transition values before
entry evaluation. This supersedes the M2b override-wins interpretation. The
fresh ordinary-entry-parameters-replay run reproduces both pages. These probes
compare animation IDs, not additional animation-clock boundaries or owner rules.

## State block binding, 2026-10-16

The state-binding fixture defines state 0 twice. In the first block, a controller
labeled State 999 sets var(0)=111 and State 0 sets var(1)=222. The duplicate block
attempts 333/444. The reference records 111/222/0, demonstrating cosmetic header
numbers and first-definition selection. The separate state-orphan fixture puts
a controller before any Statedef and records zero, showing it is ignored.

CNS parsing now uses the preceding definition instead of indexing by controller
labels, and keeps duplicate blocks detached. Pure engine and Character tests
match all recorded variables. The state-binding-replay run reproduces the trace.
These cases do not establish whether invalid expressions in discarded duplicates
are diagnosed at load, or every accepted header spelling/range.

## Go production startup inspection, 2026-10-16

The Go application (not the reference executable) now has an owned-window smoke
runner. KFM startup captures show the Elecbyte logo, intro background, and title
menu with readable fonts and separate KFM title sprites. The successful isolated
runs are temp/app-runs/kfm-startup1 and kfm-startup3. A concurrent-test repeat,
kfm-startup2, exited with code 0 after two captures; no cause is established.
The runner records failures rather than accepting missing windows. These images
are inspection evidence, not native pixel/timing comparisons. Original stock
content remains unchanged, and captures/binaries/copies are untracked.

## Keyboard symbol evidence, 2026-10-16

The supplied mugen.cfg contains SDL symbols such as 273 for Up, 44 for Comma and
97 for A. Their meanings are checked against the primary
[SDL 1.2 header](https://github.com/libsdl-org/SDL-1.2/blob/main/include/SDL_keysym.h).
The former DOS lookup misdecoded them. All 22 configured stock actions now match
the intended keys and flow through injected-key gameplay tests. This is config/
adapter evidence, not reference hardware or command-timing parity. SDL WORLD and
shifted-only symbols, device mappings and full command semantics remain open.

## Coordinate-scaled constants, 2026-10-16

The coordinate-constants fixture records Const240p(3), Const480p(6), Const720p(12),
float division, negative and fractional arguments. Standard local widths 320,
640,1280 produce the documented ratios. Fractional width inputs 320.5 and 640.5
produce the same values as 320/640, establishing truncation rather than rejection
or unconditional default fallback. The three trigger types return float; the
Const240p(5)/2 result is 2.5 at width 320. Height coercion is not isolated by these
width-based numeric queries. All five cases/60 fvars are pinned and match Go at
float32 precision. A fresh coordinate-constants-replay run verifies fractional640.

The reference runner records --localcoord in case metadata and validates it on
replay. The Go HD title inspection at temp/app-runs/hd-menu-centered/ verifies
that the declared motif dimensions, default clip and horizontal origin are
consumed by production drawing. It is not a native screenshot comparison.

## Next reference work

Add authored diagnostic characters/animations in disposable run copies, fixed
input/tick scenarios, and observable assertions for expression types/evaluation
order, negative-state transitions, animation timing, and ownership. Record each
fixture's source/hash, invocation, observation and interpretation. Use exact
discrete comparisons and justified numeric/visual tolerances. Do not assume
control of the reference's RNG: the startup log reports session/match seeds but
no equivalent seed-setting mechanism has been established.

## Mixed-player distance probes

`player-distance.cns/.air/.json` uses separate probe opponents through
`--p2-localcoord`. Runs player-distance-720-240 and player-distance-240-720
record starts -280/-70, P2Dist X 560/140 and P2BodyDist X 480/120; all 24
recorded fvars match Go. Native replay player-distance-replay checks the inverse
case. Only disposable copied characters are modified. No hit/camera/timing
parity is inferred from these static distance observations.

## Guard state and control probes

`guard-end`, `guard-start` and `control-gating` each pin 12 state/time values.
Native runs of the same names with -native suffix show a five-tick action in
140 ending in state 0, a custom 120 remaining under CNS control, and a crouched
state 11 retaining Ctrl=0 without auto-standing. All 36 values match Character;
guard-end-replay reproduces the native capture. No guard damage/velocity or
keyboard-driven entry parity is inferred from these fixtures.

## Received-hit coordinates and overflow

`hit-coordinates` pins explicit received vectors for 320/1280 and 1280/320
players. `hit-defaults` adds four omitted-default cases, with common width 1280
and 640. All six contacts compare their captured fvars in Character. Explicit
X velocity/acceleration/fall vectors convert by defender/attacker width, while
entry velocity stays zero in the authored idle fixture. Default yaccel=.35 and
fall Y=-4.5 use 240p units. Omitted fall X returns a common-space no-change
sentinel; local width greater than game width can expose negative infinity.
The first smallgame run saved an image but its finite-only reader failed;
smallgame2 records exact #INF glyphs and hit-defaults-replay reproduces them.

`float-overflow` and `float-math-overflow` demonstrate stored positive/negative
infinities, comparisons, reciprocal zero, Exp(1000), Exp(90), and float power
overflow. These supersede the old unmeasured exp(1000)-is-bottom expectation.
`scientific-accepted` records 1.0e2, 1.0e-2, .5E+1, 2.E2, 3.0E+2 and -2.5e-1.
`scientific-notation` is a pinned load rejection for 1e2; native requires a decimal
point. Original source hashes accompany all observations. Remaining numeric
NaN/domain/coercion cases are not inferred from overflow observations.

## Palette mapping integration

The supplied sctrls.html RemapPal section specifies non-transitive source/dest
maps, wildcard/removal behavior, same-depth resources and the eight-map limit.
These have authored CPU/compiler tests. Both stock SFF archives have selected
palette pixel tests and production selected-palette-fight captures show distinct
P2 colors. These are Go rendering/asset checks, not native controller edge or
screenshot parity. V1 external palettes and exact helper/effect ownership still
need authored executable fixtures.

## Shared contact phase probes

`hit-trade`, `hit-trade-phases` and `hit-trade-pause` pin equal-priority contact
observations. First get-hit positive-state execution sees Life/P2Life=1000/1000;
at Time=1 both are 975 and both states are 5000. Initial contact counters reset
through get-hit state entry. These values are unchanged with 8,8 hitpause,
these snapshots alone do not determine pause duration or contact ordering. All three
programs match character.TickCharacters, used by production; hit-trade-pause-replay
reproduces native evidence. Full hitpause duration, priorities, multiple contacts,
guard/overrides, helper and projectile order are not inferred from these probes.

## HitOverride cases

The twelve override-* fixtures cover basic/slot/default-time/forceair/clear/
expire/lasttick/hitstate/missing-state/missing-attr/nonmatch/projectile cases.
Runs use -virtual3 for the first six and -virtual for the remaining cases; the
first basic -virtual capture predates the final entry-counter fixture. All final
source/hash-pinned observations compare through TickCharacters. Native omissions
of attr or stateno yield inactive overrides rather than the manual's advertised
required-parameter rejection. Standard get-hit entry precedes override entry,
without executing standard-state controllers, as PrevStateNo/entry-counter show.
Forceair uses air values but respects air.fall=0. Damage applies after first
state execution only when movetype remains H. override-forceair-replay reproduces
the final case. This does not establish custom-state exclusion or complete pause,
helper/projectile/ownership behavior.

The first override-basic-native/native2 captures stalled in initial fade-in as
all desktop monitor outputs disconnected; they contain no interpreted numeric
observations. A task-owned Xvfb restored native and Go execution. Original
reference assets remain untouched; see BUILD.md for the temporary display setup.

## Contact clocks and guard response

The twelve contact-activation, hit-countdown, pause-release, attacker-pause and
guard-* contact programs have default case JSON with executable/CNS/AIR/capture
hashes. Runs use NAME-virtual on private :97; pause-release-replay independently
reproduces all clock packets. Guard-exit-contact is distinct from the original
M4e guard-end animation fixture, which remains unchanged.

contact-activation observes negative-state clock=2 on first get-hit entry:
HitDef defined on one tick connects before the next tick's negatives. In
pause-release, negative-state packets (Time, hitshaketime, hittime, pass count)
are (0,3,13,0), (1,2,13,1), (2,1,13,2), (3,0,12,3), (4,0,11,4),
(5,0,10,5). Victim CNS runs during hitshake. Attacker guard pause=2 instead
holds (Time, pass count, MoveContact)=(1,1,1) for three observations including
contact, then advances (2,2,2), (3,3,3), (4,4,4). Existing pause decrements before
pass eligibility. hit-countdown records hittime=2,1,0,-1,-1,-1 and
HitOver=0,0,0,1,1,1. Earlier M2b/M4g clock/schedule assumptions are superseded.

Ground/crouch/air guards enter 150/152/154, take 3 chip damage from damage=25,3,
and use guard vectors/pause/timers. Wrong-height standing guard hit by L receives
normal 25 damage in 5000. Guard end 140 still guards. Matching HitOverride enters
710 with normal hit data and guarded=0, retaining life because its destination
movetype is I. Ground guard hittime=13/slide=7/ctrl=11 while air guard uses
hittime=-1/slide=0/ctrl=11. Native omitted guard.slidetime inherits ground.slidetime
(5), despite the manual's advertised guard.hittime (11); omitted control inherits
5. Air guard omitted velocity is (-18,-10) from air.velocity=(-12,-20).

These compare through production TickCharacters. They do not establish all
GetHitVar fields, damage scaling, input/distance entry, protection filters,
priority/reversal, multiple contacts or helper/projectile/world ownership.

## Protection masks and current attack posture

Twenty-five protect-* programs have pinned default cases. Twenty-four execute;
protect-missing records the beta's load rejection for omitting both value and
value2. Tests verify matching compile rejection with source hashes. All accepted
programs compare production state/life/time/pending-damage observations; a fresh
protect-dynamic-pose-replay verifies the original capture.

HitBy allowed masks intersect across two active slots. NotHitBy complements
posture and attack masks individually: absent components remain empty before
complementing. Thus HitBy SCA and HitBy ,NA block all contacts, NotHitBy SCA
blocks all, and NotHitBy ,NA,AP excludes normal attacks/all projectiles while
allowing a special attack. Blank NotHitBy allows contacts. Slot replacement is
shared across the controllers; time=0 clears, time=-1 persists. Default one-tick
protection blocks the first attempt and the still-active HitDef connects next
tick; the delayed Life/Time observations distinguish expiry from no protection.

Both parameters are accepted; protect-both-first/second show value wins over
value2, despite the manual's exclusive rule. The attacker, defender and authored
attr postures are varied independently: actual current attacker StateType is
used. protect-dynamic-pose changes posture after HitDef execution and permits
the contact on the next tick. override-current-pose/authored-pose confirm the
same posture source for HitOverride (710 versus 5000). The authored CurrentHitDef
remains unchanged. Later GetHitVar(damage) is zero after processing, unlike its
pending value seen by contact-state controllers.

These establish the listed masks/clock edges in ordinary root-player ticks;
complete malformed-value fallbacks, pause/superpause, custom state ownership,
helpers/projectiles and the remaining hit/guard/priority rules are not inferred.

## Guard range, state-pass priority and push geometry

The guard-distance family, attack-distance and two state-order-* programs pin
strict 140/141/139 boundaries at axis separation 140, zero/negative ranges,
omission/entry reset, no-HitDef attack posture, height and independent facing.
Ranges use the attacker's facing; turning the defender away still returns 1,
turning the attacker away returns 0. Heights -500 through +100 still return 1
with the same horizontal positions. Later omitted guard.dist retains an earlier
explicit value; entry with hitdefpersist omitted resets size.attack.dist. The
AttackDist integer expression changes the same value. Named clock cases add
1280/320 and 320/1280 pairs. No projectile range behavior is inferred.

Guard-clock packets include P2Life as a published Time counter. P2 A executes
before P1 I/H, whereas both A execute P1 first. When P2 begins a tick I and turns
A in its pass, P1 observes the prior published value on that tick; the next tick
returns to attack-first order. These source-pinned observations match the shared
root scheduler and guard-distance-clock-replay. Global entity/physics/animation
ordering and ordering among unmeasured mode/team combinations remain open.

The original cross-over first seemed to contradict horizontal-only range.
guard-distance-cross instead reveals P1 ending at -91 and axis separation 161
after exact overlap, beyond range 160. Custom state 700 retains facing=1.
Dedicated push-* fixtures pin complete numeric KFM constant groups with front/
back widths 10, using the harness's --fixture-constants option. Initial failed
-virtual/-virtual2 drafts of push-standing/disabled lack interpreted values:
empty/duplicate or life-only Data constants fail native loading. Final cases use
-virtual3; later push variants use -virtual. Original distribution is unchanged.

For ordinary overlaps, gap 12 resolves to 20 with P1=-74, and gap 11.5 resolves
to 20 with P1=-74.25. Near axes have additional spacing: gaps 0, .5, 4, 8, 10
resolve to 30, 29.5, 26, 22, 20. The negative .5 case mirrors displacement.
These imply additional separation max(0,10-abs(gap)) before equal splitting,
within the measured ground-overlap cases. Mixed 1280/320 exact overlap produces
P1=-325 and P2Dist X=90, confirming 240p-scaled spacing. Always-disabled push
keeps coincident axes; a Time=0-only controller expires before the later overlap.

All 23 programs/26 cases compare source hashes and captured fields in production
TickCharacters. Airborne/height collision gating, corner transfer, pause,
dynamic Width, complete auto-turn eligibility and entity ownership are separate
open obligations; no complete body-physics parity is inferred.

## Sound command and PCM evidence (M4l)

See AUDIO.md and `tests/sound_probe.py`. Twenty-three original authored programs
cover twelve audio cases (including legacy character version) and twelve grammar
cases. Pinned JSON records include 55 stable native tone segments and 63 Go tick
checks. Original tone banks are reproducible with generate_sound_bank.py; no
reference sound assets are committed. Native capture uses only the task-owned
private PulseAudio null-sink monitor. Configuration/source/AIR/SND/executable/PCM
hashes are retained, with amplitude tolerance 8 signed-16 units and relative
onset tolerance 0.35 seconds. Absolute startup and full waveforms are not compared.
A fresh sound-channels-audio-replay reproduces channel isolation, low priority,
replacement, automatic overlap and global stop with matching measurements.
Full scoped races pass. Device/frontend cues and lifecycle remain open.

M4m adds hud-sound: the harness patches only a disposable reference's fight DEFs
to use the authored one-second tone bank. Modified fight hashes are pinned with
source/executable/settings/audio hashes. Native gain and finite expiry match the
production Go HUD PCM test; absolute announcement timing is outside this check.
The private Go device smoke sound-match-playback records nonzero PCM through an
explicit task-local ALSA/Pulse route and inspects only its owned window.

M4n adds nine contact audio programs and five HitDef sound grammar programs.
Thirty-two native segments back 45 Go tick checks. Original one-second own/common
tones distinguish resource selection, guard versus hit, channel-0 interruption,
HitOverride and cue expiry. Two symmetric geometries pin defender screen-X
panning; omitted/later-omitted definitions emit no cue. F/group-only selection and
assignment execution have dedicated captures. Fresh override-sound-replay and
contact-sound-common-single-replay match numeric packets, levels, relative onsets
and disposable common-bank recipe hashes. Full races and actual KFM hit-sound
command/PCM integration pass. Whole HitDef and camera/ownership parity remain open.

M4o adds four KO voice programs (21 native segments, 25 Go tick checks). Original
quarter-second group-11 tones isolate initial/half/quarter voices and current
global nokosnd behavior. Native echo/initial-only suppression replay passes with
100-ms analysis windows and a disposable original empty HUD bank. Its bank/fight
hashes are pinned. Earlier captures with spoken announcements are diagnostic-only.
Full races pass. Exact audio onset/world-clock boundaries and complete KO/pause/
revival/entity state behavior remain open.

M4p adds eight spark programs and spark_probe.py. Five persistent original RGB
markers, two confirmed absences and one numeric Data-prefix program isolate
geometry/resource/default behavior. Character/scene frame anchors and resource
choices match; fresh common-default and guard replays reproduce the native
pixel-core hashes. Calibration is limited to static KFM scenes; EFFECTS.md gives
bounds and the newly measured world/HUD viewport discrepancy. Full races and
real Go normal/additive marker window inspections pass. No full framebuffer,
blend, pause/lifetime or ownership parity is claimed.


## SuperPause and frozen contacts (M4v)

See SUPERPAUSE.md for 38 native programs/600 numeric observations, ten static
render cases and three audio programs with seven segments/seven PCM checks.
The suite includes the adopted M4u super clock pilot and an ordinary Pause
contact comparison. Clock preemption/arguments/power, caller protection, frozen
contact/damage, acquired-target defence stacking/recovery, effect lifecycle and
common/own cues are exercised by production Character. Full races, stock KFM
super integration and fresh serial effect-clock/other-pause/audio replays pass.

The original RGB marker is reused without copying reference art. Super render
analysis pins resource/core hashes and static sample coordinates; it is limited
to fixed-camera KFM scenes. Scene/opponent tints are inferred within one byte,
not full framebuffer parity. HUD projection still differs and fit flags are
next. Early overlapping clock windows/audio drafts are diagnostic only; final
measurements use serial owned captures and isolated private audio. AGENTS.md
records the ordering rule. The optional --clipboard-delay is retained in recipe
metadata for frozen P1 observations, with existing default 30 unchanged.


## Video projection and stage crop (M4w)

video-fit.json pins ten cases/240 numeric observations and twenty original RGB
marker cores. The HUD marker replaces only disposable fight SFF references and
Face settings; both fight-file hashes and original marker hash are recorded.
Optional --video flags change only allowed CFG settings in the disposable copy;
--hud-marker and case replay retain recipe/config hashes. No reference art is
committed. video_render_probe.py compares native HUD/world cores and hashes.

Fresh crop765/window-stretch replays match; Character tests assert all packets,
and pure coordinate tests compare projected boundaries within one pixel for
native fractional filtering. Full races pass. Owned Go default/crop/stretch
windows have identical HUD cores and matching world centers; their smaller
world solid cores remain an acknowledged filtering issue. See COORDINATES.md
for crop truncation, exact cases, diagnostic-only oversized capture and limits.


## Identity, helper families and bottom operands (M4x)

Nine stable original programs cover thirteen cases: 240 numeric values and one
VM malformed-expression rejection. identity-roots and identity-helper use named
cases with --helper-max recipe/config hashes. Root allocation follows clamped
HelperMax 4..56, separate from input/helper labels. Leaf retirement/non-reuse,
nested families, Parent/Root timing and missing-selector behavior have focused
programs. Fresh limit32/nested/operator/bitnot replays pass. Character compares
every stable packet; pure evaluator replays all 24 bottom-operator values.
Full scoped and pure core races pass. See ENTITIES.md/BOTTOM.md.

Native ordinary operators consume bottom as integer zero in the measured cases,
superseding manual-based propagation assumptions. IfElse/Cond differ for a bottom
condition; measured variable/velocity argument boundaries also use zero. Bitwise
NOT of bottom is a fatal VM error, with matching source-context diagnostics.
Bottom exponentiation remains unresolved: raw identity-bottom-power and -layout
programs record different non-mathematical values with different expression
layouts. Their explicit status excludes them from stable Go goldens; do not
claim parity or hardcode those observations. Native memory-level cause and
applicability remain open. Root round continuity and complete target/helper/
projectile/state ownership are separate remaining comparisons.


## Targets, defence and redirection scope (M4y)

Forty-nine original programs record 744 values and three required-value load
rejections. Target acquisition/labels/recovery/guard/override, immediate life/
power, facing momentum and velocity omission/mixed units are compared in
Character. Typed controller defaults and valid required fields are distinguished
from accepted omitted TargetFacing. Negative-state source/CMD hashes establish
contact before -3/-2/-1/positive passes, with pending damage before life changes.
Runtime defence and new get-hit-sequence behavior correct prior assumptions.

Three scope programs distinguish direct caller-unit results, eager origin
argument evaluation and delayed Cond destination variables/units. A pure
evaluator test replays the twelve distinct-variable assignment/index values.
Full/pure races and representative fresh native replays pass. The first upward
coordinate draft exercised camera easing; only the stable zero-camera source
is promoted. See TARGETS.md for exact fields, calibration and remaining
TargetState/binding/multi-entity/complete-damage/schema obligations. No original
assets/binaries or root module/desktop changes are made.

## Independent custom state and animation ownership (M4z)

Eleven original two-character programs pin 200 distinct per-program numeric
observations (216 capture entries with overlapping pages). --p2-fixture chooses
independent opponent CNS/AIR/CMD files and records their hashes separately.
TargetState external-entry timing, negative exclusion/units, positive ownership
chains, mixed position/velocity/constants, animation-source predicates and elem
clocks, missing/negative destinations, preserved control and repeat hits are
compared. See OWNERSHIP.md for precise observations and remaining throw/HitDef/
binding/rendering obligations. Early four-second drafts retained stale intro
clipboard output and are diagnostic only; promoted captures use seven seconds.

## Root movement, clocks and two-sided trade damage (M4aa)

Three original independent programs record 48 values. A later root sees the
earlier root's completed movement/Time; attacker priority covers the whole
root update. The second root's get-hit Time=0 CNS sees already-reduced opponent
life while its own life is pending. The old single-sided trade evidence remains
valid but could not distinguish this from one shared post-CNS damage phase.
Shared late push is retained by the earlier push/guard/viewport packets. See
WORLD.md for the order, failed-draft evidence and remaining entity/world limits.

## TargetBind, literal typing and independent hurtbox controls (M4ab)

Fifty-three original programs pin 840 numeric values and ten expected load
rejections. Changing anchor velocities distinguish real duration/release from
an expired target continuing at inherited velocity. Mixed profiles, facing,
ID/no target, defaults, renewal/cancel/drop, new hit/recovery/H-to-H retention,
negative timing and competing CNS writes are compared. Literal integer fields
reject float literals while float-valued expressions coerce; tuple/bottom and
comma-expression cases are pinned. Independent H/I and hurtbox-present/absent
controls disprove an initial H-only push inference. See BINDING.md.

Full scoped races pass. Seven representative fresh native replays reproduce
packets/rejections. A 1280x720 Go with-box match runs through seven-second window
captures; actors reach an edge under sustained scripted motion, so this does
not establish camera/body-edge/framebuffer parity. All original reference
assets, root module hashes and desktop settings remain unchanged.


## M4ac custom HitDef, binding publication and AIR clocks

Thirty-eight original customhit-* programs pin 1244 distinct per-program numbers
and three direct-float load rejections. Independent opponent CNS/AIR/CMD and
profile hashes distinguish owned/borrowed state definitions, animation clocks,
negative passes, pending damage and override exclusions. Most use P2 localcoord
1280,960; binding-flag variants use equal profiles. Exact pages/flags are pinned
per packet. Supplemental cancel/drop and computed-flag captures retain their
run names in capture entries. No original binary or art is committed.

The first customhit-entry-native control lacked Clsn2 on its distinct action,
so no hit occurred and all observations stayed zero. It is diagnostic only.
The promoted customhit-entry-contact-native control adds an authored hurtbox
and independently pins the new opponent AIR. It proves that standard get-hit
entry animation is superseded rather than resetting before a custom state.

Final -1 clocks and a nonfinal 2,-1,4 schedule expose native AnimTime and query
rules contrary to the old documentation-based assumptions. Three pure AIR tests
and the ten-tick detailed scheduler program compare those numbers. See
CUSTOMHIT.md for exact behavior and limits. Full scoped races pass in
temp/m4ac-full-final-race.log. Seven fresh replays match entry, override/negative
sentinels, cancel publication, nonfinal clocks, fractional flags and literal
rejection. The real custom-entry game window at temp/app-runs/customhit-entry-go/
is 1280x720 and inspected through seven seconds, with both actors rendered. The
authored victim uses low-resolution art in a 1280 profile and is small; no
framebuffer/stock-throw parity claim follows. Full world/engine gates remain open.


The stock KFM/KFM720 regression window also runs at 1280x720 through seven
seconds at temp/app-runs/customhit-stock-regression-go/, with both full-sized
actors and no runtime errors. This remains a smoke check, not pixel parity.


## M4ad stock throw and contact-pose evidence

Four original wrappers load the unchanged stock kfm.cns, common1.cns and kfm.air
through Files overrides. file_sha256 pins resolved copied paths and their input
hashes; fresh replay checks them, and Go reads those exact original inputs.
The stock bodies, sprites and sounds are not copied into tracked fixtures.
Normal/forward/drop/recovery observations contribute 132 numbers, including an
independent GameTime start stamp. Four minimal one-tick attack/hurtbox window
programs contribute 96 more. See THROWS.md.

An early throw-relative trace passed while Go grabbed 21 ticks late. The start
stamp measured one native tick versus Go 22 and must remain in regression
coverage. Saving pre-advance contact poses corrects the start and both minimal
window directions. TargetDrop also exposed omitted fall-X sentinel assignment;
the controller now preserves X, matching native's stationary escape at X=42.
The initial base draft observed get-up through 701, but its authored Ctrl=0
state stops at 0; final complete packets observe 0/previous 5120 explicitly.

All eight packets and prior scoped race regressions pass in
temp/m4ad-full-first-race.log. Full controller/world/engine parity remains open;
four fresh start/drop/first/second-window replays match, including the original
Files hashes. Production-window evidence accompanies the checkpoint.


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


## M4ae Explod lifecycle and RGB placement

Eighteen original numeric programs pin 528 values and two float-literal load
errors. Nine additional original RGB marker programs pin 108 values and static
position/facing/resource measurements. Total: 27 programs, 636 numbers and 63
authored input files, with no committed Elecbyte art. The marker is the original
formats.sff RGB ramp; common recipes replace only disposable fightfx files.
Source/AIR/extra/recipe/core hashes are retained. spark_probe.py and its spark
JSON field are reused for calibration, not contact behavior.

The first front/space=stage/Y=100 run was off-screen and remains diagnostic.
Promoted implicit screen-edge controls and the visible explicit-stage/Y=-30
case distinguish the coordinate modes. Both own/common (20,-30), P1/P2, screen,
front in both directions, back facing left and stage-edge cases compare through
the production simulation. Full moving-camera/filter/relative-priority parity
is not established by these static captures.

Full scoped races pass in temp/m4ae-full-markers-race.log; eight fresh native
replays match cross-root expiry, common resources, negative removal, omitted
anim, float rejection and common/stage/back RGB cases. The private X server
exited before one assertion run; the task-owned server was restored to the
same configuration, then the complete run passed. See EXPLODS.md for exact
semantics, rendering smoke and remaining work.


All final normal/forward/drop 1280x720 windows were inspected through 2/3/4/7
seconds at temp/app-runs/stock-throw-{base,forward,drop}-effects-go/. They recover
with two fighters and no permanent dust clones, without missing-effect warnings
or runtime errors. The ordinary throw was also repeated in
stock-throw-base-effects-recheck-go/. A black preview was a display artifact:
the stored original/repeat recovery PNGs have identical hashes and substantial
nonblack pixels. app_smoke.py now rejects entirely black quick-match captures.
These remain window/asset smoke checks, not full camera/filter/layer/audio parity.
