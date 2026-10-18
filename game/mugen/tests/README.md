# Tests and reference fixtures

From any working directory, run `game/mugen/mugen.sh --check` using the launcher's
absolute path if needed. It keeps build/test temporary files under
`game/mugen/temp/` and prevents changes to the root Go module.
It uses scoped `mugen.mod`/`mugen.sum`; direct Go commands need
`-modfile=game/mugen/mugen.mod -mod=readonly`. See [build notes](../docs/BUILD.md).

Required reference content is the supplied, unmodified M.U.G.E.N 1.1 beta 1
distribution at `game/mugen/temp/mugen-1.1b1/`. Set `MUGEN_REFERENCE_DIR` to use
another extracted copy. Missing files and mismatched hashes fail rather than
skip; `testdata/reference-manifest.json` pins 109 content/spec/tool files without
committing the distribution itself. No test reads the old `data-new/` tree.

The `physics_check`, `character`, `stage`, `air_player`, `snd_player`, and
`fetch-all-data` directories are executable diagnostic applications. Launch one
with `game/mugen/mugen.sh --test NAME`; pass its `--data-dir` explicitly when needed.
Their output is not a substitute for semantic assertions.

Pure parser/evaluator tests can run without DISPLAY. Packages importing
Ebitengine currently require a display even when their tests do not open a
window; the simulation extraction in M2 will remove that coupling from state
tests. Do not describe the current entire test tree as headless.

The original engine is never executed in place. Engine experiments use
copies under `temp/reference-runs/` and a Wine prefix under `temp/wine-reference`.
Reference-generated files, extracted images and captures stay under `temp/`.
Tiny original reference-compressed fixtures and numerical pixel hashes may be
committed; Elecbyte art remains untracked.

Run headless language fixtures with:

```
game/mugen/mugen.sh --test expressions --input game/mugen/tests/testdata/expressions.jsonl
```

Each JSONL fixture has a name, initial state time, and ordered expression steps
with expected kind/value and optional variable assertions. The runner emits
deterministic observations (including warnings and variable changes) and exits
nonzero on malformed fixtures or mismatches. These use the production typed
evaluator; world-bound trigger semantics require the forthcoming engine runner.

Run the authored reference animation probe with
`python3 game/mugen/tests/reference_probe.py --name UNIQUE_NAME`. It requires
Wine, DISPLAY, xdotool, ImageMagick `import` and Pillow; it copies the supplied
distribution and uses a private Wine prefix under `temp/`. It captures only the
spawned process's window, sends no input, and makes no focus/pointer changes.
Each capture reads two clipboard lines by exact matching against the bundled
debug font, rejects ambiguous/missing glyphs, and checks the pinned observations.
Captures/logs/hashes remain in its run directory. Each run name must be new.
Each clipboard page starts a fresh native process and waits the requested
seconds; pages are separate observations, not successive times in one match.
The Go clock regressions use the committed numerical observations without Wine.

Named state probes use `--fixture state-order --pages 0` or
`--fixture persistence --pages 0 12`, each with a new `--name`. `--record` saves
new observations only in that disposable run directory; it does not change the
committed expected data. A fixture may supply its own neutral CMD state program.
Engine and Character tests run both programs against the pinned variables.

Float-valued probes select `--bank fvar`. `constants-a --bank fvar --pages 0 12 24`
and `constants-b --bank fvar --pages 0 12 24 28` cover the documented constant
catalogue; the one reference-rejected name is an explicit unavailable slot.
The exact glyph reader accepts signed decimals/exponents, preserving nine-digit
float32 round trips. Kind assertions follow the documentation's declarations.

Build the optional error reader with `game/mugen/mugen.sh --reference-tools`.
It reads only the reference MUGEN error dialog inside the private Wine prefix,
without input/clipboard/focus operations. The runner uses it to record and check
expected load rejections. For example, `--fixture constant-unavailable --pages 0
--mugenversion 1.1` checks the pinned invalid-constant error. `--bare-constants`
is an omission/load-error experiment using empty constant groups; the supplied
engine rejects incomplete Data groups. It is not a source of valid fallback data.

Resource matrix cases run with `--fixture resource-load --case st9-version11`
and a fresh run name. Authoring options `--file key=value`, `--omit-file key` and
`--clipboard-in-cmd` are recorded in evidence and checked on replay. The character loader now exercises the pinned load/precedence rules.
`--fixture resource-order --case merge-numbered-first` verifies declaration-order
independence. An optional `.extra` directory contains authored auxiliary files;
its hashes are pinned. `--state-file-order st0 st` records reordered declarations.

`TestReferenceCharacterResources` compares all twenty loader cases directly.
Common probes wait for the initial round-control grant before forcing entry,
and Go starts at that same point. See REFERENCE.md for the earlier startup
difference and its isolation. Missing/malformed resources, source filenames,
Windows/case names and independent common loads have separate regressions.

`--fixture entry-control --mugenversion 1.1 --pages 0 12` replays transition versus
entry parameter precedence. Pure engine and Character tests compare all 24
recorded variables, including incoming-control reads by entry expressions.

`--fixture state-binding --mugenversion 1.1 --clipboard-in-cmd --pages 0`
checks cosmetic controller labels and first-definition selection; `state-orphan`
checks controllers before any definition. Both are replayed by pure engine and
Character tests.

Stock asset-loader tests cover all four motif main archives/fonts and all three
stages; they assert loaded resources, not merely successful DEF parsing. Separate
regressions exercise missing/malformed declared assets, source locations, the
documented motif font search order and Windows/case font-child names. These are
not screenshot/rendering parity tests or per-background/storyboard coverage.

Background archive tests check KFM's separate title archive, matching CPU element
bindings, sprite-ID collisions, masked/opaque caches and foreground selection.
All four supported motif background override declarations reject missing or
malformed archives with section/key/line context. Full screenshot parity remains
separate work.

Storyboard loader tests cover all eight stock cutscenes, CPU-only resource
construction and bounded playback, plus authored scene/index/action/resource
failures and layer 99. Application tests cover failure propagation and resolved
selection paths. Configured-duration assertions are regression checks, not
native timing or screenshot parity. Text/audio and full scene drawing remain
incomplete.

Run `python3 game/mugen/tests/app_smoke.py --name UNIQUE_NAME` for production
startup inspection. It builds with the scoped module, copies the reference into
temp/app-runs/, selects the KFM motif, excludes ambient YAML overrides, and
captures its own process window at 2/6/16 seconds. Go, DISPLAY, xdotool and
ImageMagick import are required. It sends no input or focus changes, records
hashes and commit/dirty state, and terminates its own process. Use --motif or
--seconds for other inspection points. Live keyboard/window timing can affect
results; it does not assert native image parity or deterministic gameplay.

Input regressions use the stock SDL key-number corpus and injected keyboard
sources; no OS key events are generated. They cover all 22 stock bindings,
explicit KeyA=0 and disabled presence, YAML overrides, per-player confirmation,
and the actual facing-relative gameplay input path. See INPUT.md for remaining
command timing, layout and gamepad work.

Fight resource tests cover both stock fight definitions, main SFF/SND/fonts,
embedded AIR, fightfx SFF/AIR, common sounds and retained layout attributes.
Missing/malformed declared files are checked with source context. HUD drawing
and live match behavior require separate M4 integration tests.

Coordinate tests retain all stock profiles, distinguish game space from window
size, verify width-based translation and compare float scaled constants against
five native cases. Replay with `--fixture coordinate-constants --case fractional640`
and a fresh run name. `--localcoord width,height` is an authored-probe override;
its value is pinned. Mixed-player geometry and complete camera/rendering parity
still require Match integration.

Mixed-player geometry: `reference_probe.py --fixture player-distance --case
240-720 --name UNIQUE_NAME` replays an independent opponent profile. The
`--p2-localcoord` override copies the authored probe player into a second private
character directory; the original reference is unchanged. Character tests pin
both coordinate pairings and compare all recorded float variables.

Production fights: see docs/MATCH.md. The system flow test injects keys through
title, selection, stage, versus, fight, result, rematch and menu. match tests use
the same lifecycle with stock KFM/KFM720 and logical walk/punch input. For owned
window inspection, app_smoke.py accepts --match CHAR_DEF CHAR_DEF and --stage.

Guard/control probes: --fixture guard-end, guard-start or control-gating with
--mugenversion 1.1 --pages 0 and a fresh --name replay native state/time traces.
These fixtures wait for round control before entering the measured state; the
Go runner starts at the corresponding control boundary.

Received hits: --fixture hit-coordinates --case 240-720 and --fixture
hit-defaults --case smallgame replay local/common coordinate cases. Infinity
observations use positive_infinity/negative_infinity strings in native JSON;
JSONL expression traces use a nonfinite field and value=0 as a JSON placeholder.
float-overflow, float-math-overflow and scientific-accepted are numeric native
probes (--bank fvar --mugenversion 1.1 --pages 0). scientific-notation instead
pins the native rejection of 1e2. Decimal forms such as 1.0e2 are supported.

Palette integration: palette/ tests cover mapping semantics and selected pixels
in both stock archives. Character tests cover cache refresh/removal; match tests
verify common CNS applies PalNo again after reset. selected-palette-fight is an
owned-window Go capture of the actual renderer; no native remap-edge parity is
claimed from it.

Contact ordering: --fixture hit-trade-phases or hit-trade-pause --case default
with a fresh --name replays same-frame/pre-damage and next-frame/post-damage
values. Character tests use the same TickCharacters entry as production. The
fixture tests do not open a window, but current Character imports still need
Ebitengine display initialization; only engine/evaluator are display-independent.

HitOverride: override-* fixtures are pinned cases replayed with --case default;
clipboard output is appended to the negative CMD state so every destination can
report. The Go suite now gates all 108 stock states compiling and compares twelve
native override cases. Full controller parity remains open. When desktop outputs
are disconnected, use the private display environment described in docs/BUILD.md.

Contact/guard reference regressions also cover prior-tick HitDef activation before
negative states, attacker hitpause versus receiver hitshake, HitOver's -1 edge,
guard posture/flags/damage/vectors/timers/defaults, guard-end eligibility and
HitOverride precedence. Twelve source-pinned programs compare production group
ticks; pause-release-replay repeats native clock packets. M2b/M4g timing
assumptions are superseded; see docs/REFERENCE.md. These do not establish full
priority/protection/helper/projectile or GetHitVar parity.

Protection probes protect-* cover two shared HitBy/NotHitBy slots, independent
mask complements, partial/empty masks, duration/expiry/clearing, replacement,
current attacker posture, both-parameter precedence and missing-parameter load
rejection. Twenty-four accepted programs plus two override posture cases compare
production group ticks; protect-missing checks compile rejection. A dynamic-pose
native replay verifies the captured case. Complete invalid-value, pause and
entity/ownership behavior remains open.

Guard-distance/order/push probes add 23 programs/26 cases for attacker-owned
range, direction/height/default retention, state-entry reset, AttackDist,
attack-first state-pass priority, custom facing, PlayerPush reset/suppression,
near-axis spacing and mixed coordinates. They all use production group ticks.
--fixture-constants selects complete authored CNS constant groups without a stock
prefix and is pinned in case metadata; --bare-constants retains its prior empty-
group mode. Push fixtures use complete numeric KFM defaults with explicit widths.
Native partial Data rejection still needs dedicated loader-validation coverage.

Sound controller/mixer fixtures are `testdata/reference/sound-*`. Twelve audio
cases replay real grouped Character ticks through the CPU mixer and compare
63 plateau checks against 55 native segments. Twelve additional grammar cases
pin accepted/rejected resource-prefix and parameter behavior. Sources, original
bank bytes and native executable/config/PCM hashes are retained. Regenerate an
original bank with `python3 game/mugen/tests/generate_sound_bank.py --help`.

Native sound replay requires the private Pulse server documented in
[../docs/AUDIO.md](../docs/AUDIO.md), plus the private display in BUILD.md:

```sh
DISPLAY=:97 XAUTHORITY="$PWD/game/mugen/temp/virtual-display/Xauthority" \
LIBGL_ALWAYS_SOFTWARE=1 \
PULSE_SERVER="unix:$PWD/game/mugen/temp/audio-reference/native" \
PULSE_COOKIE="$PWD/game/mugen/temp/audio-reference/cookie" \
python3 game/mugen/tests/sound_probe.py --fixture sound-channels \
  --name sound-channels-audio-replay-new --seconds 14
```

Use a new run name. `--record` records a selected case; default replay checks
source/numeric observations, settings, tone amplitudes and relative onsets.
It records only the private null sink, never a microphone or desktop audio.
This is controlled PCM evidence, not full-waveform or device output parity.

M4m adds `hud-sound`: a one-second original tone replaces only disposable native
fight announcement settings. sound_probe.py --fixture hud-sound replays the
pinned recipe automatically. The Go HUD test compares stereo amplitudes and
sample expiry, not absolute round timing. app_smoke.py --audio additionally
records real Go device PCM from the private null sink; set ALSA_CONFIG_PATH to
the task-local pulse route described in BUILD.md. Production-flow tests read the
same mixer without opening a device and verify announcements, bank binding,
round reset, live volume and cleanup.

M4n contact-sound*, guard-sound* and override-sound fixtures add 32 native audio
segments and 45 Go tick checks. Hit-sound-* grammar probes measure F/S prefixes,
group-only syntax, assignments and invalid names/expressions. Optional bank
generator --contact/--common adds original one-second tones; default bytes remain
unchanged. sound_probe.py --common-tone patches only disposable common.snd
references, pins the modified fight hashes, and automatically repeats the recipe
on replay. Both override-sound and contact-sound-common-single have fresh passing
replays. The normal KFM combat test also requires common hit-sound output.

M4o ko-sound* fixtures add 21 isolated native segments and 25 Go tick checks.
--fixture-constants uses their complete numeric groups. --quiet-hud routes only
disposable fight announcements to an original valid empty SND; bank/fight hashes
are recorded. --window-ms/--step-ms are pinned so short KO tones retain their
plateaus; Go checks can request WindowTicks (default 12). Fresh ko-sound-initial-
suppressed replay verifies global initial suppression with later echoes. Earlier
captures containing stock spoken announcements remain diagnostic-only in temp.

M4p spark-* and guard-spark-own programs use the original formats.sff RGB marker
with blank fighters. spark_probe.py --fixture spark-common-default --name NEW
--action 9992 replays numeric/source checks plus the calibrated native marker
geometry/core hash; common-bank patch recipes repeat automatically. Calibration
originally covered static zero-camera KFM scenes and flipped markers; M4af adds
authored fixed nonzero-camera controls. --expect-none
checks disabled/missing markers after confirmed contact. Data-prefix coercion
has a separate numeric-only case. See EFFECTS.md for the eight-case scope,
production GPU inspections and the viewport discrepancy still being corrected.


SuperPause native fixtures and tests are detailed in [SUPERPAUSE.md](../docs/SUPERPAUSE.md).
For example, tests/super_render_probe.py --fixture superpause-effect-clock
--name UNIQUE replays numeric packets and compares the original RGB marker core
and static colors. The continuously frozen other-pause fixture records
--clipboard-delay=0; reference_probe.py defaults to 30 and pins nondefault delay
in metadata. Audio fixtures use sound_probe.py and require the private null-sink
monitor. Run native/Go captures, audio and full Ebiten tests sequentially on the
private display; overlapping windows/audio invalidate capture provenance.


Fight viewport cases run with reference_probe.py --fixture video-fit --case
CASE --name UNIQUE, or video_render_probe.py --case CASE --name UNIQUE for both
numeric replay and original HUD/world marker comparison. Cases retain --video
StageFit/SystemFit/KeepAspect/Width/Height and --hud-marker recipes. The latter
patches only disposable fight definitions to use the original authored ramp.
All ten packets run through Character, and pure coord tests compare native
projection boundaries with a one-pixel fractional core tolerance. Actual Go
window inspections distinguish projection from the still-different filtered
world edges; see COORDINATES.md. No full framebuffer claim follows from them.


Identity cases use reference_probe.py --fixture identity-roots --case limit32
--name UNIQUE, or identity-helper --case default. Single identity-helper-nested,
identity-helper-retire, identity-bottom, identity-missing and bottom-operator
programs use --mugenversion 1.1 --clipboard-in-cmd --bank fvar and their pinned
pages. --helper-max changes only copied configuration and is restored by case
replay. identity-bottom-bitnot is an expected runtime rejection. Full Character
identity regressions and pure evaluator operator replay consume the stable
source/hash packets. See ENTITIES.md and BOTTOM.md. The two identity-bottom-power
programs are explicitly unresolved native behavior, not passing Go goldens.


Target/scope programs and bounds are detailed in TARGETS.md. For example,
--fixture targets-relabel --pages 0 12, target-life-super --pages 0, and
target-vel-add-mixed --p2-localcoord 1280,720 --pages 0 use the usual
--mugenversion 1.1 --clipboard-in-cmd --bank fvar recipe. The three redirect-*
programs separate eager origin arguments, delayed destination Cond, and
caller-unit direct geometry. target-negative-phase also pins its CMD source.
Required-value grammar rejections are expected. All 49 programs are compared
by TestNativeTargetsAndRedirectedCoordinates; pure scope replay has no DISPLAY
dependency. Full custom-state/binding and multi-entity cases remain open.


Ownership programs use --p2-fixture NAME for an independently authored opponent;
its CNS/AIR/optional CMD/extras hashes are pinned separately and capture never
overwrites its CNS. For example, --fixture ownership-entry-phase
--p2-fixture ownership-entry-phase-victim --pages 0 12 --p2-localcoord 1280,960
--mugenversion 1.1 --clipboard-in-cmd --bank fvar --seconds 7 --name UNIQUE.
Eleven ownership-* packets replay in TestNativeCustomStateOwnership, covering
200 distinct per-program observations, including positive/negative state and
animation ownership, external-entry timing, units, control, missing destinations
and repeat hits. Four-second draft captures sometimes retained intro clipboards
and are diagnostic only. See OWNERSHIP.md for limits; complete throws/binding,
borrowed AIR geometry and multi-entity behavior remain open.


phase-root-physics, phase-root-attacker-second and phase-trade-life use their
same-named -victim opponent fixtures and the independent-opponent recipe above.
The first two use --pages 0; the trade uses --pages 0 12. TestNativeRootUpdatePhases
compares all 48 numbers. These packets distinguish per-root movement/clocks/
damage from shared late body push; see WORLD.md. The initial intro-Time draft
and per-root-push implementation are diagnostic only, not passing evidence.


Binding cases use the independent-opponent recipe, for example --fixture
binding-target-2 --p2-fixture binding-victim --pages 0 12 --mugenversion 1.1
--clipboard-in-cmd --bank fvar --seconds 7 --name UNIQUE. The mixed fixture adds
--p2-localcoord 1280,960. Each packet records its own opponent fixture; use that
name for the recovery/mutation/negative/push controls. Grammar rejection packets
have expected_load_error and no numeric captures; replay with --pages 0.
TestNativeTargetBindings compares 53 programs/840 numbers/ten contextual load
failures. Early constant-speed duration runs could not distinguish release
from continued matching velocity; current packets use a changing anchor.
Independent hurtbox controls replace the disproved H-only push inference.
See BINDING.md for scope, exact defaults and remaining pause/entity/throw work.


M4ac adds 38 customhit-* packets with 1244 distinct per-program observations
and three required native load rejections. TestNativeCustomHitStates uses each
packet's independent CNS/AIR/CMD opponent and profile. Supplemental capture run
names identify cancel/drop page 28 and computed-flag detail pages. Most packets
use --p2-localcoord 1280,960; the binding-flag programs use equal profiles.
Pure TestNativeInfiniteAnimationClocks compares three schedules. The detailed
nonfinal -1 program also compares native frame selection and element clocks.
See ../docs/CUSTOMHIT.md for exact scope, diagnostic exclusions and remaining
stock throw/world obligations. All display/audio clients run serially on the
task-owned private servers; pure tests without DISPLAY can run alongside probes.


M4ad adds four throw-stock-* and four contactframe-* packets (228 observations).
Stock wrappers use --file st0=kfm.cns --file stcommon=common1.cns --file anim=kfm.air
and --p2-fixture throw-stock-victim. Resolved original file hashes are pinned in
file_sha256 and verified on native/Go replay. TestNativeStockThrows runs 240 ticks;
TestNativeContactFrames compares one-tick attack and hurtbox windows. Keep the
GameTime start stamp: throw-relative positions alone initially hid a 22-tick
Go grab versus native one tick. See ../docs/THROWS.md for limits and exact recipes.


M4ae adds 18 explod-* and nine explodmark-* packets: 636 numeric observations,
two load rejections and nine RGB placement/resource controls. Lifetime programs
use reference_probe.py with their version/fvar/CMD/pages, and no independent
opponent. Missing anim loads and creates no object despite the manual. Marker
programs reuse spark_probe.py --fixture NAME --name UNIQUE --action 9100; the
historical spark field stores calibration. Their constants/extra RGB SFF hashes
are required, and common recipes modify only disposable fightfx resources.
TestNativeExplodLifetimes and TestNativeExplodMarkerGeometry replay the native
state and position/source results. See ../docs/EXPLODS.md for scope and limits.

Quick-match app_smoke.py captures require Pillow and reject an entirely black
window; a live process and correct dimensions alone do not establish rendering.
Stored capture hashes and visual inspection still matter.

M4af adds five camerapos-* and two cameramark-* packets: 192 numbers and two RGB
positions with fixed nonzero cameras. --fixture-stage copies the authored,
hash-pinned .stage.def sidecar into the disposable stage directory. A replay is:

```
python3 game/mugen/tests/reference_probe.py --fixture camerapos-fixed-xy --name UNIQUE --fixture-stage --bank fvar --mugenversion 1.1 --clipboard-in-cmd --pages 0 12 24 --seconds 7
```

negative uses the same options; mixed adds --p2-localcoord 1280,960; wide adds
--localcoord 1280,960 --p2-localcoord 320,240. borrowed uses --pages 0 12 and
--p2-fixture camerapos-borrowed-victim --p2-localcoord 1280,960. RGB replays use
spark_probe.py --fixture cameramark-own (or cameramark-common) --name UNIQUE
--action 9100; pinned metadata supplies stage/common options. Its historical
frame_anchor_240p key reports screen-center/ground-200 coordinates, so nonzero
cameras must be subtracted from stage positions before comparison.
TestNativeCameraPositionCoordinates and TestNativeCameraExplodGeometry use the
production CNS, stage and effect paths. Camera follow/easing/zoom and native
filter-edge equivalence remain open; see ../docs/COORDINATES.md.

M4ag adds 20 camerafollow-* packets: 720 numbers and 60 authored input hashes.
Replay with reference_probe.py --fixture NAME --name UNIQUE --fixture-stage
--bank fvar --mugenversion 1.1 --clipboard-in-cmd --pages 0 12 24 --seconds 7.
motion-mixed additionally uses --p2-localcoord 1280,960. TestNativeCameraFollowing
loads the authored stage and CNS/AIR into a stock Match, adjusts the measured
profiles, resets the round and advances through control grant and 90 more ticks.
It compares all recorded fvars, including both roots' observations in phases.
Long/return traces include Time=60. Static success alone is insufficient: the
eight motion packets distinguish the extreme-actor speed floor. See CAMERA.md
for exact scope; vertical/zoom/full bounds/ownership/renderer rules remain open.

M4ah adds 47 screenbound-* packets: 924 numbers, four controller rejections,
three stage rejections and 141 authored input hashes. TestNativeScreenBoundaries
uses the real stage loader/Match runner and checks the full input set even for
rejected programs. Replay with reference_probe.py --fixture NAME --name UNIQUE
--fixture-stage --bank fvar --mugenversion 1.1 --clipboard-in-cmd --pages 0 12
--seconds 7. The three helper-* programs use only --pages 0. wide additionally
uses --localcoord 1280,960 --p2-localcoord 320,240. Programs with expected_load_error
must reject; those with expected_stage_error reject at the stage loader. Accepted
empty/word/negative/fraction margins are intentional native controls. See
../docs/SCREENBOUND.md for exact defaults, camera-axis coupling and remaining
pause/Width/helper/world obligations.

M4ai adds 31 width-* packets: 336 numbers, three load rejections and 93 input
hashes. TestNativeWidth uses the same production geometry runner as the camera
and ScreenBound programs. Replay with reference_probe.py --fixture NAME
--name UNIQUE --fixture-stage --bank fvar --mugenversion 1.1 --clipboard-in-cmd
--pages 0 --seconds 7. wide additionally uses --localcoord 1280,960
--p2-localcoord 320,240; query-redirect uses --p2-localcoord 1280,960. All use the
same fixture for both roots. Selected push cases have hurtboxes; edge/query
cases are deliberately box-free. Empty Width and active float/extra-component
fields must reject. The ignored-float packet must load: explicit edge causes
native to ignore value entirely. See ../docs/WIDTH.md for scope and limits.

## M4aj helper configuration

TestNativeHelperConfiguration compares 72 helpercfg programs, 984 numeric
observations, eight intentional load rejections and all 276 authored input
hashes. CMD sidecars are checked and merged after CNS state definitions.
Reproduce accepted controls with a fresh name, for example:

```
python3 game/mugen/tests/reference_probe.py --fixture helpercfg-nested-order --name UNIQUE_NAME --fixture-stage --bank fvar --mugenversion 1.1 --clipboard-in-cmd --pages 0 --seconds 7
python3 game/mugen/tests/reference_probe.py --fixture helpercfg-size-scene --name UNIQUE_NAME_2 --fixture-stage --bank fvar --mugenversion 1.1 --clipboard-in-cmd --pages 0 12 --seconds 7
```

Other representatives are key-one, phase-attack, camera-enabled-far,
camera-endpoints, pause-two and flow-active-twenty, all prefixed helpercfg-.
Use the pages/recipe recorded in each JSON. Full scoped races pass in
temp/m4aj-full-race.log; pure engine/evaluator races pass without DISPLAY in
temp/m4aj-final-pure-race.log. HELPERS.md separates measured coverage from the
remaining helper/world obligations.

## M4ak helper retirement

TestNativeHelperRetirement compares fourteen helperretire programs, 288 numeric
observations, two load rejections and all 56 authored input hashes. Rejected
programs identify expected_compile_state=800. Replay, using a new name:

```
python3 game/mugen/tests/reference_probe.py --fixture helperretire-orphan-parent --name UNIQUE_NAME --fixture-stage --bank fvar --mugenversion 1.1 --clipboard-in-cmd --pages 0 12 --seconds 7
```

The same recipe covers recursive-one/two/negative/computed/bottom, both float
rejections and five scene-* programs. RETIREMENT.md describes retained metadata,
parent writes, recursive effect removal and the finite/permanent orphan scenes.
TestOrphanExplodClockExpiryBindingAndRoundReset checks clocks, released anchors
and round cleanup without graphics resources. Full scoped races pass in
temp/m4ak-full-race.log; pre-fix failures remain in m4ak-before-retirement.log.

## M4al helper sound ownership

helperaudio-ordinary and helperaudio-recursive add 24 numeric observations,
eight PCM tick windows and 48 stereo amplitude checks. TestNativeSoundControllerPCM
passes all 29 programs/30 cases with race detection. Native replay uses
sound_probe.py --fixture helperaudio-recursive --name UNIQUE_NAME --seconds 9
(and ordinary), with the private display/audio environment. The JSON pins the
quiet-HUD/configuration and original synthetic tone bank. Capture-shutdown
silence is not a semantic stop event. See AUDIO.md for the measured retained
voices, initial GameTime alignment, real device evidence and remaining gaps.

## M4am helper binding phases and hit facing

TestNativeHelperBindingRetirement compares 27 helperbind programs: 564 numeric
observations, one deliberate rejection and all 108 authored input hashes.
Examples, always using fresh run names and the private display/audio setup:

```
python3 game/mugen/tests/reference_probe.py --fixture helperbind-flag-prepost --name UNIQUE_NAME --fixture-stage --bank fvar --mugenversion 1.1 --clipboard-in-cmd --pages 24 --seconds 7
python3 game/mugen/tests/reference_probe.py --fixture helperbind-fall-idle-speed --name UNIQUE_NAME_2 --fixture-stage --bank fvar --mugenversion 1.1 --clipboard-in-cmd --pages 0 12 24 28 --seconds 7
```

Use each JSON's page list for other cases, and explicit page 0 for facing-float's
expected state-800 rejection. HELPER-BINDING.md distinguishes anchor completion,
per-pass flags, idle constraints, external 5050 timing and delayed facing. Full
races pass in temp/m4am-full-race.log; the documented late right-edge raster
difference remains separate from passing numeric conformance.

## M4an helper defender contacts

TestNativeHelperDefenderContacts compares ten helpercontact programs: 204 numeric
observations and 40 CNS/AIR/CMD/stage hashes. Normal/player/multiple/same-team
cases pin damage and target identities; early/late pairs pin incoming birth+2
contact. Separate -3/-2/-1/positive controls distinguish scheduling from contact
activation. Reproduce without overwriting the recorded goldens:

```sh
python3 game/mugen/tests/reference_probe.py --fixture helpercontact-two-normal --name UNIQUE_NAME --fixture-stage --bank fvar --mugenversion 1.1 --clipboard-in-cmd --pages 0 --seconds 7
```

Use each JSON's recorded pages. Full scoped races, four fresh replays and seven
private-display app captures pass; HELPER-COMBAT.md records the limits and
remaining sprite-mask difference. No full HitDef/helper/renderer claim.

## M4ao HitDef target and team selection

TestNativeHitSelection compares 28 original hitselect programs: 300 numeric
observations, three load rejections and 112 authored CNS/AIR/CMD/stage hashes.
Controls cover hitonce consumption/defaults/replacement/guarding, affectteam
syntax and independent root/root, root/helper and helper/helper selection order.
To replay without changing goldens:

```sh
python3 game/mugen/tests/reference_probe.py --fixture hitselect-helper-pair-team-order --name UNIQUE_NAME --fixture-stage --bank fvar --mugenversion 1.1 --clipboard-in-cmd --pages 0 --seconds 7
```

All these programs use page 0; rejected inputs have no captures and retain the
expected state in expected_compile_state. HIT-SELECTION.md records grammar,
phase evidence and validation. Complete priority/override/projectile/team-mode
and binding-release rules remain open.

## M4ap actor sprite priority and ties

TestNativeActorDrawOrder compares fourteen actororder programs: 156 numeric
observations, one load rejection, thirteen native foreground identities and 70
CNS/AIR/CMD/stage/SFF hashes. Original solid markers identify the front actor
independently of the Go sort. Equal priorities prefer the lower world ID;
priorities cross root/helper ownership. Typed SprPriority boundaries include
bottom, literal/computed floats and accepted values outside -5..5.

```sh
python3 game/mugen/tests/actor_render_probe.py --fixture actororder-helpers-reverse-birth --name UNIQUE_NAME
python3 game/mugen/tests/generate_actor_markers.py game/mugen/temp/actor-markers.sff
```

The native pixel tool also verifies pure-color bounds/counts and all input,
executable, capture and numeric hashes/observations. --adopt-run can measure an
existing run; --record updates only the requested render metadata after these
checks. Rejected float inputs use reference_probe.py instead. All source sprites
are original raw RGB data, not reference character art. See ACTOR-ORDER.md for
pre-fix windows, full regression evidence and the remaining flipped-edge gap.

## M4aq actor sprite alignment

TestNativeSpriteFramePrograms compares 28 flipmark programs: 336 numbers and
140 CNS/AIR/CMD/stage/SFF hashes. Native crop goldens cover facing/AIR H/V/HV,
cancelled flips, offsets, fractional positions, three render widths and
nonuniform helper sizing. Stage debugbg=0 isolates the original marker over
black. Low-resolution runs explicitly pin the physical video size.

```sh
python3 game/mugen/tests/sprite_render_probe.py --fixture flipmark-left-320 --name UNIQUE_NATIVE
python3 game/mugen/tests/app_smoke.py --name UNIQUE_GO --reference game/mugen/temp/reference-runs/UNIQUE_NATIVE/stock --match chars/kfm/kfm.def chars/kfm/kfm.def --stage stages/probe-stage.def --motif data/mugen1/system.def --width 1280 --height 720 --seconds 5
python3 game/mugen/tests/sprite_render_probe.py --fixture flipmark-left-320 --name UNIQUE_CHECK --go-run UNIQUE_GO
```

Go verification checks binary/config/capture hashes, prepared source, AIR, marker
bank, stage and render profile, then requires identical bounds, color counts and
complete crop SHA-256 by default. Explicit --max-edge-channel-error 1 allows at
most four blended pixels to differ by one channel level. Pure red/black pixels
and geometry stay exact; the report retains exact versus differing-edge counts.
The compressed native crop validates against its SHA before comparison. The
precision allowance still rejects the known half-pixel position failure.
Go verification cannot record or overwrite native goldens. Native
--adopt-run/--record retain the input/executable/config/video/capture checks.
SPRITE-ALIGNMENT.md records the measured rule, validation and remaining scope.

## M4ar HitDef clashes, counters and interrupted targets

`TestNativeHitClashes` compares 63 original `hitclash-*` programs: 996 numbers,
five native load rejections and 252 CNS/AIR/CMD/stage hashes. The corpus covers
the Hit/Miss/Dodge tie table, higher priorities, defaults and numeric/parser
boundaries, one-sided attacks, no-hit reactivation, helper/reverse-team trades,
hitonce selection, simultaneous/staggered hit counters, signed numhits and
counter persistence. Hit/guard/manual-H/override controls distinguish target
release; bound interruption selects the victim's own state 5050.

```sh
python3 game/mugen/tests/reference_probe.py --fixture hitclash-helper-trade-reverse --name UNIQUE --fixture-stage --bank fvar --mugenversion 1.1 --clipboard-in-cmd --pages 0 12 --seconds 7
```

Use each JSON's pages; rejected programs use page 0. Helper snapshots publish
from positive states because normal helpers do not execute root-only state -3.
The initial all-zero helper diagnostic is excluded from final evidence. Native
and Go clients run sequentially on the private display/audio servers. See
[COMBAT-PRIORITY.md](../docs/COMBAT-PRIORITY.md) for contact phases, limits and
validation. Complete hitflags/protection/projectile/pause/team combinations
remain outside this checkpoint's claim.

## M4as HitDef drawing priority

`TestNativeContactDrawPriority` compares 23 original `hitdraw-*` programs: 252
numbers, two native load rejections, 21 foreground identities and 115 CNS/AIR/
CMD/stage/original-SFF hashes. Exact tie controls bracket defaults 1/0; normal,
guard, override, helper and custom/ordinary entry programs pin precedence.
Range pairs retain +/-99 ordering; literal floats reject, computed floats
truncate and bottom becomes zero. No-contact ties leave priorities unchanged.

```sh
python3 game/mugen/tests/actor_render_probe.py --fixture hitdraw-helper-override --name UNIQUE_NATIVE
python3 game/mugen/tests/app_smoke.py --name UNIQUE_GO --reference game/mugen/temp/reference-runs/UNIQUE_NATIVE/stock --match chars/kfm/kfm.def chars/kfm/kfm.def --stage stages/probe-stage.def --motif data/mugen1/system.def --width 1280 --height 720 --seconds 5
python3 game/mugen/tests/actor_render_probe.py --fixture hitdraw-helper-override --name UNIQUE_CHECK --go-run UNIQUE_GO
```

Go checking validates original/prepared inputs, bank, stage, profile, binary,
configuration and captured PNG, then requires exact foreground RGB/ID/count/
bounds. It cannot record or overwrite native goldens. Rejected fields use the
ordinary reference probe with `--file sprite=probe.sff`. See
[CONTACT-DRAW.md](../docs/CONTACT-DRAW.md) for reference evidence and open gates.

## M4at HitDef hitflag eligibility and fall continuation

`TestNativeHitFlagEligibility` compares 67 original `hitflag-*` programs: 792
numbers, one native load rejection and 268 CNS/AIR/CMD/stage hashes. Posture
matrices, independent AF/MF controls, both modifier orders, idle/attack/H fall
contexts and reciprocal priorities distinguish eligibility from subsequent
contact effects. Empty/Z flags are accepted inactive values; lowercase and
quoted flags work. Downed targets select own state 5080. Typed ForceNoFall
controls cover preservation, explicit clearing, signed/coerced/bottom values
and precedence over a hit's own fall setting.

```sh
python3 game/mugen/tests/reference_probe.py --fixture hitflag-force-computed --name UNIQUE --fixture-stage --bank fvar --mugenversion 1.1 --clipboard-in-cmd --pages 0 --seconds 7
```

All programs use page 0, including the float rejection. The accepted inactive
values must not become artificial load errors. Falling eligibility needs A
and F together while MoveType H; do not interpret the single-F miss as evidence
of a juggle-budget failure. See [HIT-FLAGS.md](../docs/HIT-FLAGS.md) for the
measured state rules and remaining guard/foreign/pause/juggle/down-state gates.

## M4au forced standing and get-hit velocity axes

`TestNativeForcedStanding` compares 38 original `forcestand-*` programs: 540
numbers, two load rejections and 152 CNS/AIR/CMD/stage hashes. The corpus covers
automatic/explicit Forcestand, guard/override/entry precedence, pre-force flag
eligibility and compiled ground.velocity integer/fractional/redirect/single/
bottom components. Side effects pin explicit Forcestand before ground.velocity
independently of written order. Facing controls pin world-oriented GetHitVar X
and HitVelSet's facing-relative assignment, initial velocity and actual motion.
Nonzero axis flags select once; computed 0.5/bottom become zero; literal floats
reject loading, including the HitVelSet error in state 5010.

```sh
python3 game/mugen/tests/reference_probe.py --fixture forcestand-facing-same-left --name UNIQUE --fixture-stage --bank fvar --mugenversion 1.1 --clipboard-in-cmd --pages 0 12 --seconds 7
```

Use each JSON's page list; rejected programs use page 0. See
[FORCED-STANDING.md](../docs/FORCED-STANDING.md) for exact rules, prototype
failures, native provenance and remaining coordinate/velocity/heading gates.

## M4av contact facing and entry phases

`TestNativeContactFacing` compares 48 original `hitfacing-*` programs: 552
numbers, two load rejections and 192 CNS/AIR/CMD/stage hashes. The corpus pins
P1Facing/P1GetP2Facing sign, precedence, integer coercion, bottom and fixed
evaluation order; combined P2Facing; before/after activation turns; guard,
override, miss, custom entry and momentum. Negative/positive/receiver-entry
controls distinguish selected hit direction from the later physical turn.
Ordinary/contact entry and side-effect controls pin FaceP2 before other entry
expressions and ordinary receiver entry at its positive pass.

```sh
python3 game/mugen/tests/reference_probe.py --fixture hitfacing-entry-order-after --name UNIQUE --fixture-stage --bank fvar --mugenversion 1.1 --clipboard-in-cmd --pages 0 --seconds 7
```

Every program uses page 0, including float rejections. Native source hashes must
match before replay; Go observations cannot overwrite the native expectations.
See [CONTACT-FACING.md](../docs/CONTACT-FACING.md) for timing, prototype failures,
verification and remaining contact/entry/velocity gates.

## M4aw hit velocity defaults and down-hit offsets

`TestNativeHitVelocity` compares 63 original `hitvelocity-*` programs: 1,020
numbers and 252 authored CNS/AIR/CMD/stage hashes, with no final load rejections.
The corpus covers air/guard/air-guard/down omission, literal/computed/single/
bottom/zero/redirect values; per-component inheritance; native evaluation order
and source-expression re-evaluation; corner-default suppression/replacement;
receiver snapshots, mixed profiles and down-hit Y-offset type/consumption.

```sh
python3 game/mugen/tests/reference_probe.py --fixture hitvelocity-down-offset-type1280 --name UNIQUE --fixture-stage --bank fvar --mugenversion 1.1 --clipboard-in-cmd --p2-localcoord 1280,720 --pages 0 12 --seconds 7
```

Honor each JSON's page list, localcoord/p2_localcoord and fixture_constants flag.
The trip7/back8 fixtures contain original numeric constant sections and require
`--fixture-constants`. Their early missing-Velocity diagnostic rejections are
not final expectations. Both-player1280 and true mixed1280/320 cases are
separate. See [HIT-VELOCITY.md](../docs/HIT-VELOCITY.md) for exact values and
remaining physical corner-push, pause/bind/foreign/down/HitDef gates.

## M4ax physical corner push

`TestNativeCornerPush` compares 51 original `cornerpush-*` programs: 1,224
numbers and 204 CNS/AIR/CMD/stage hashes. World-frame packets distinguish the
separate impulse from ordinary velocity, context selection, proximity versus
predicted movement, cutoff units, double precision/float32 coefficient, signs,
state/target/turn lifetime, deferred hit-facing, PosFreeze and ScreenBound.

```sh
python3 game/mugen/tests/reference_probe.py --fixture cornerpush-ground-unit-game640 --name UNIQUE --fixture-stage --bank fvar --mugenversion 1.1 --clipboard-in-cmd --game-width 640 --video Width=1280 --video Height=720 --pages 0 12 --seconds 7
```

All cases use pages 0/12. Honor localcoord/game_width/video metadata; the width 400
case is a deliberate precision discriminator. The first pre-round-clock captures
are diagnostic copies only; final expectations use native-controlled, native-math
and native-integration runs. See [CORNER-PUSH.md](../docs/CORNER-PUSH.md) for exact
rules, validated scope and remaining projectile/helper/foreign/bind/world gates.


## M4ay fractional sprite sampling

`TestNativeSpriteRasterPrograms` compares 37 original `raster-*` programs:
444 numbers, 180 authored CNS/AIR/CMD/stage/bank hashes and no load rejections.
Thirty-two original solid/patterned sprite crops separately test pixel centers,
RGB/indexed filtering, mirrored texel boundaries, fine masks, flips, offsets,
helper scale, character width 400, game width 640 and transparent guard coverage. Generate the
original RGB24/RLE8/LZ5 banks with `generate_raster_markers.py`; no external sprite
compiler or stock artwork is needed. Five real-art numeric controls separate
stationary positions from corner-push movement; their artwork is never recorded
as a committed raster golden.

```sh
python3 game/mugen/tests/reference_probe.py --fixture raster-pattern-indexed-helper --name UNIQUE_NATIVE --fixture-stage --bank fvar --mugenversion 1.1 --clipboard-in-cmd --file sprite=probe.sff --pages 0 --seconds 7
python3 game/mugen/tests/raster_render_probe.py --fixture raster-pattern-indexed-helper --name UNIQUE_CHECK --native-run UNIQUE_NATIVE
python3 game/mugen/tests/raster_render_probe.py --fixture raster-pattern-indexed-helper --name UNIQUE_GO_CHECK --go-run APP_SMOKE_RUN
```

Honor each JSON's game_width/video metadata. The game640 case pins Video Width
1280/Height720. `raster_render_probe.py` accepts only the exact original generated
banks, validates input/config/binary/capture provenance and compares every crop
byte. Go output cannot record a native golden. The old `sprite_render_probe.py`
remains the independent 28-case flipmark geometry/edge gate. See
[SPRITE-SAMPLING.md](../docs/SPRITE-SAMPLING.md) for measured rules, candidate
failures and current validation status.

## Storyboard canvas and ground/guard timing checkpoints

M4bf's `storyboard_probe.py` compares generated RGB markers against six native
canvas/window configurations. Sources and measured metadata are text; generated
SFFs and captures stay in ignored temp/. See [canvas evidence](../docs/STORYBOARD-CANVAS.md)
for sequential native/application commands and the GPU regression test.

M4bg adds21 `hittiming-ground-*` programs,348 numeric observations and four
native rejections for ground hit/slide plus their dependent guard timings.
The normal Go suite replays every pinned input through the shared Match code.
Fresh native reproduction requires BUILD.md's private display/audio environment:

```
python3 game/mugen/tests/reference_probe.py --fixture hittiming-ground-guard-side-effects --name UNIQUE_NAME --fixture-stage --bank fvar --mugenversion 1.1 --clipboard-delay 0 --pages 0 --seconds 6
```

Do not record expected values from Go or regenerate frozen source inputs.
[Timing evidence](../docs/HIT-GROUND-TIMING.md) lists defaults, field order,
validation and remaining gates. Neither passing these controls nor a successful
storyboard load establishes full engine/asset/rendering compatibility.

## Air, launch and downed timing (M4bh)

Twenty-five `hittiming-flight-*` programs pin 336 numeric observations, three
native load rejections and 100 source hashes. Match replay covers both typed
fields, timer/slide selection, omission versus zero, bounded countdown/shake,
fall/KO controls and source-forward/reverse evaluation order. Corrected downed
controls use -contact names and D flags; the initial non-contact diagnostics
remain ignored and are excluded from acceptance.

```
python3 game/mugen/tests/reference_probe.py --fixture hittiming-flight-down-omitted-contact --name UNIQUE_NAME --fixture-stage --bank fvar --mugenversion 1.1 --clipboard-delay 0 --pages 0 --seconds 6
```

Use BUILD.md's private environment and run native/application/GUI tests
sequentially. Do not regenerate frozen sources or record expectations from Go.
[Air/down evidence](../docs/HIT-AIR-DOWN-TIMING.md) records measured behavior,
verification and remaining recovery/KO/round/combination gates.


### HitFallSet and downed-hit bounce (M4bl/M4bm)

`fallset-*` has 22 native programs (240 observations/two load rejections);
`downbounce-*` has 21 (324 observations/two load rejections), including full
stock recovery and a bounce/idle phase trace. The shared Match replay checks
all frozen source hashes and exact native float32 results. See
[HitFallSet findings](../docs/HITFALLSET.md) and
[downed-hit bounce](../docs/DOWN-BOUNCE.md) for ignored native parameters,
metadata, measured timing, provenance and limits. Native expectations come
from the immutable beta executable, never the Go replay. The stock trace uses
pages 0/12/24; full parameter-order controls use 0/12/24/28. Native/app/display
checks run sequentially. Reproduce with a fresh run name; leave frozen inputs
and recorded values unchanged.

```bash
game/mugen/mugen.sh --check -race -p 1 -timeout=5m \
  -run '^(TestNativeHitFallSetPrograms|TestNativeDownBouncePrograms)$' -v
```
