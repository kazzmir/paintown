# MUGEN implementation instructions

## User-directed workflow — 2026-11-06

The user replaced exhaustive native matrices and repeated small-test batteries
with implementation-led progress and connected state-machine scenarios. These
instructions supersede old research plans and checkpoint test requirements.

- Use docs/PROGRESS.md as the SINGLE project completion ledger: the full project
  breakdown, checkmarks, partial/missing features and completed scenario coverage.
- Implement substantial, coherent mechanics batches from unchecked items. Use
  existing code, evidence and the supplied beta documentation. Do not create
  native parameter matrices, new per-piece documents or asset-copy experiments.
- Tests cover segments spanning entry/order/transitions, combat/recovery,
  helper/binding/retirement, effects/pause, or complete match flow. Consolidate
  existing scenarios through tests/segments.py. Do not add a test per parameter.
- Completed segments stay checked and out of default runs. Reopen only when
  their covered production source changes, a regression identifies that segment,
  or the user explicitly requests it. Run affected segments/end-state acceptance
  after a coherent batch, never broad suites after every edit.
- Update the small feature catalog after implementing its actual consumer and
  refresh the single progress page with mugen.sh --coverage --write. Routes alone
  do not establish complete semantics. Mark the stated scenario complete after
  it passes; do not call known partial features complete.
- Keep handoffs and plans short. Historical detailed research is optional
  background and Git history, not a queue of mandatory experiments.
- Do not add generated testing data, captures, PCM, copied assets, binaries or
  new fixture matrices to Git. Reuse the supplied assets and shared build cache.
  Segment scratch output is automatically deleted on success AND failure; keep
  only a concise failure explanation or explicit user-requested artifact.

## Scope and execution

The user authorized the remaining functional work on 2026-11-07: BGM/NoMusic,
teams/modes/AI, remaining stage/team context, assets and platform support. Defer
color/shadow/alpha and KO-echo precision until functional coverage is finished.
Continue autonomously with local source/text checkpoints. Author only game/mugen/.
No pushes, committed binaries, subagents or goals. Preserve unrelated root work.
The user renewed autonomous local commit authorization on 2026-11-07. Continue
through coherent checkpoints without individual commit approval; give a recap
about every five commits, or sooner for a blocker needing user input. This
supersedes the earlier pre-commit approval rule. Do not add user configuration
files to Git. Keep mugen.yaml local and ignored; document examples in README.md.

On 2026-11-08 the user redirected work back to engine state-machine coverage
and will supply another mod for compatibility testing. Do not repair further
mod scripts. Record defects that also reproduce in native as content findings;
they are not engine implementation tasks. Retain the existing optional SF2
playable copy/tool and original assets without further mod changes.

Use the readonly mugen.mod/mugen.sum overlay and preserve root module files.
The requested origin/go-rewrite merge at172d3fb02 and reconciliation of the
earlier remote dependency commit dc3423cf1 update root metadata and the overlay:
Go1.26, Ebitengine2.10.6, purego0.11.3.
For this host prefix private-wrapper commands with env GOTOOLCHAIN=go1.26.9;
that toolchain is cached. Both graphs retain MUGEN's audio/YAML dependencies.
Prefer rg/rg --files --no-ignore; historical ignores hide source. Do not recurse
into temp for source discovery. Temporary Go helpers belong under temp/_tmp/.

For this host use python3 game/mugen/temp/round-task-env.py with its authorized
elevated prefix for Xvfb:97/Pulse/Go. Native/app/display-dependent runs stay serial;
await the entire process and confirm owned clients exited before edits/new runs.
Never expose cookies or unrelated process arguments or reset shared servers.

The user authorized temp cleanup. Nine generated run directories were removed,
recovering126.58 GiB; native originals/captures/prepared copies in those directories
are intentionally no longer retained. This supersedes earlier raw-evidence
retention instructions. Canonical temp/mugen-1.1b1, build tools/cache and old
tracked fixtures remain. Never regenerate removed matrices just to pass legacy
pixel diagnostics. --check-all is explicit historical diagnostics only and some
artifact-dependent tests now require unavailable captures.

## Current checkpoint

All89 documented controller and127 trigger families have runtime routes. The
functional batches connect BGM/NoMusic/stage context, simul/turns and team queries,
authored CMD/AI, campaign/continue/ending/survival/training/watch, and asset
consumers (AIR1.1 interpolation, storyboard text/sounds/windows, victory quotes).
All five connected segments pass with race detection. Read docs/PROGRESS.md
for the current checkmarks; routes do not prove arbitrary native semantics.

Linux is the user-selected target. The standalone amd64 build and normal startup
passed, as did an owned-window KFM/KFM720 keyboard-driven KO/result run with
private audio output. app_smoke.py now shares canonical assets using symlinks,
copies only tiny writable config text, and deletes scratch by default. All
checkpoint captures/PCM/logs/overlays were inspected and removed. Keep the one
ignored temp/bin/mugen-go executable and shared build cache for reuse.

Remaining: physical CD (.da) audio is unsupported. The documented color/shadow/
alpha/KO-echo discrepancies are closed; arbitrary content/rules/AI compatibility
remains unverified. Turns, mixed-team health/round
budgets and Survival team recovery are calibrated against supplied beta flows.
Licensed Windows fonts are absent; the observable fallback needs a supplied
font for full CJK glyph coverage. Other platform builds are deferred. No pushes
or committed binaries/generated fixtures. See docs/HANDOFF.md.


## Latest asset checkpoints

ZIP character references use an engine-owned CharacterPacks store. Selection,
rosters and quick matches share expansions; Engine.Close stops sound readers
before removing them. On title return, retire expansions after title music has
closed packed streams. Preserve cleanup on startup errors and handled signals.
Do not copy canonical assets into ZIP tests: extend the tiny authored asset flow.
Selection uses selective SFF v1/v2 portraits and link dependencies; gameplay
still loads the complete archive. Connected ZIP/portrait/menu lifecycles pass
with race detection; rebuilt Linux ZIP startup and signal cleanup were observed.
Options Game Speed updates the live scheduler and subsequent match clock.

TrueType references use the shared resource resolver; invalid banks share
fallback glyph textures. Preserve trailerless shared-palette PCX pixels even
when a pixel resembles the palette marker; masked/opaque views share indices.
Storyboard clearcolor=-1 retains its canvas across ticks/scenes; compose layers
once per simulation tick so repeated Draw calls cannot accumulate alpha. Colored
fades use premultiplied RGB and a reusable one-pixel texture. The connected asset
flow covers these paths through scene exit, remapping and round reset. Match-flow
passes with race detection; Linux build and owned-window KO/rematch/KO passed.
All current captures/overlays/logs removed. No user configuration tracked.

## Binding checkpoint

Keep moving-owner self-binding coverage: cumulative helper Pause allowances,
frozen helper poses, late body contact, retirement and fresh round identities.
The same entity lifecycle now continues through an asymmetric hit-paused throw,
finite TargetBind expiry and controllable own-state recovery. A hit-paused anchor
still places its target but publishes zero velocity, preventing expiry from
launching the victim with the anchor's stored velocity. Normal velocity transfer
resumes with active movement. This matches the bounded beta observations; other
pause/hitpause and target-binding combinations remain precision work. Four
changed-source segments pass with race detection; state-flow stays checked.
Linux rebuilt; native scratch inspected and removed. Do not regenerate matrices.

## Prior collision checkpoint

PosFreeze blocks ordinary velocity movement, not shared body push. Keep frozen
actors eligible on both sides of the late body-push pass. A bounded beta flow
with stored velocity 2 toggles freeze on either actor and both; body separation
and CNS clocks continue. The existing mixed-coordinate body scenario retains
prior interior/wall/vertical/Width/air phases, then covers PosFreeze, release,
real Pause and round reset. Four changed-source segments pass with race detection;
native scratch removed. Shared pre-CNS clipping and vertical Clsn2 gating remain
covered. Projectile-owner corner push remains intentionally excluded. Local YAML
stays ignored; standalone music-file playback CLI remains deferred by user request.

## Keyboard checkpoint

CMD [Remap] is a character-level logical-to-configured-button lookup after the
keyboard adapter: x=a reads configured a for logical x; blank disables that
logical slot. Use simultaneous snapshots for shared mappings and swaps. AI
converts its logical plans back to physical inputs; key-controlled helpers use
the same map. The connected human/helper/AI flow passes with race detection.

YAML keyboard actions accept scalar names or lists of alternatives for either
player. Lists replace one binding; omitted actions inherit; none/disabled/[]
disable. Input handles alternatives in menus and gameplay, preserving KeyA(0),
disabled mappings and value-copy isolation. The connected menu-to-fight scenario
now loads a small override and exercises alternatives through two rounds,
rematch/exit, plus ownership/release and malformed binding behavior. Affected
segments and existing config/input regressions pass with race detection.
See README for the user-facing contract.

## Background checkpoint

Linked backgrounds now compose immediate-parent start/motion/delta without
double-counting velocity. Animated parallax, width vs legacy xscale geometry,
camera-following tiling, zoomed masks, AIR blends and BG disable/hide/action
clocks have real consumers. The shared controller loader groups by preceding
definition and no longer duplicates named stage groups. One connected stage
scenario spans this lifecycle and round reset; affected segments pass with race
detection. Linux rebuilt and a stock owned-window stage capture inspected;
all current run scratch/captures/overlays removed.

The ledger has32/34 implemented rows; only assets and precision remain partial.
Team/mode/Survival checkpoints were tested/pushed by the user. Stage/global/hires
scaling, moving floors, linked/animated backgrounds, reflections, legacy outfits
and file music have functional consumers. Exact raster/camera/push and asset
compatibility edges remain unverified; do not regenerate historical matrices.
See docs/HANDOFF.md and preserve the cleanup workflow above.

## Current compatibility checkpoint

AIR collision allocation is bounded; packed failures retain file/action/line
context and corrected assets reload. Moving zoom matches five beta samples;
return now matches through time120 after preserving the unclipped participant
midpoint. The user requested known camera/rendering/KO-echo closure before
compatibility and deferred work (2026-11-08). Do not regenerate
historical matrices. See the small ledger/catalog and handoff for remaining work.

Retain consumed-vs-read-ahead music cursors, shared integer/float WAV decoding,
reused Linux codec buffers and SFX two-stage quantization. SFF uploads use
sff.UploadPixels for native byte premultiplication; true-color BGs filter linearly,
indexed BGs use nearest and animated BGs stay masked. Background/storyboard
SpriteCache is lazy and shares linked views; parsed SFF metadata is indexed.
Palette textures use MappingRevision; local effects retain Revision/clocks.
Fighter/HUD pixels are shared with separate effects. AfterImage retains its AIR
element clock and mapping-only historical outfits. Prior bounded Linux scenes
matched40 beta RGBA samples and five trail positions, not whole-renderer parity.

Legacy PCX decodes directly into visible rows while consuming padding. V1 now
uses the V2 total pixel budget; links charge their source once and portraits
charge only decoded sources. FNT V1 accepts commented settings/section headers;
map suffixes preserve quote/hex-semicolon characters. Numeric errors retain
file/line context. The existing connected asset scenario covers font bank/scene
changes, bad font/packed sprite correction, outfits and reset. Existing SFF/font
checks and affected match-flow pass with race detection; other segments stay
checked. Linux rebuilt and displayed both authored font banks. The beta scene
showed no authored glyphs, so native font equivalence remains open. Current
helper/capture/overlay/log scratch was removed; no user configuration tracked.

## Known-difference closure — 2026-11-08

Camera return, indexed sprite/shadow calibration and RGBA overlap RGB differences
are corrected. Indexed geometry adds 3/8 game pixel with no source-grid snapping.
Linear byte interpolation rounds the weighted delta to even before adding its
base texel (the reference Linux GL sampler rule). OnTop Explods compose above HUD
before final global PalFX, retaining window fit and projected subimage origins.
The connected stage flow keeps native RGB digests and covers finite AIR blends,
pause/expiry, HUD coverage and letterboxes. All five segments and existing render
color regressions pass with races. Rebuilt Linux output matches all four native
rendering regions exactly. Ordinary KO echo now passes the existing frozen
seven-segment device acceptance in native and Go, including the quiet interval;
Go cue levels match exactly and relative-start differences stay within the
unchanged native capture criterion. The historical gap no longer reproduces,
so no KO scheduler edit was needed. The known-difference batch is complete.
The user subsequently supplied sf2_mugen for a compatibility assessment. Preserve
exact expectations; do not create new image banks/matrices or treat unverified
permutations as known failures. Owned scratch was inspected and removed.

## SF2 compatibility assessment — 2026-11-08

The user supplied temp/sf2_mugen (and its ZIP). Its executable is byte-identical
to the beta reference; the motif and many characters are legacy content. Preserve
that pack/configuration/assets and its bundled executable in ignored storage.
The first compatibility batch fixes Scene-prefixed storyboard backgrounds being
misread as scenes, explicit empty roster slots and missing final WAV pad bytes.
Named Scene identifiers still work; empty slots retain positions and cannot pick
random fighters; WAV sample data must still fit physical bounds. Existing asset
flows and SND bounds checks cover these paths; affected match-flow/SND pass with
races. Other four segments stay checked. app_smoke input recipes can now navigate
menus as well as quick matches, always targeting the owned window.

The follow-up fixes the reported Arcade selection failure: saved rosters survive
cursor changes, and automatic opponents do not depend on a hidden invalid P2
cursor. Retain first Select Info values, row,column startcells, blank skipping,
authored cell/cursor sprites, portrait scale/axes and first-portrait palette
bootstrap. Native selection confirms eight normal portraits; the pack itself
places four boss slots below its visible canvas. Do not change the pack layout.

Native victory text ATW confirms that explicit glyph coordinates override Fixed
font cell defaults. Bounds checks remain. Legacy CNS accepts StateTypeSet's
second Type parameter, AnimElem time intervals/counter comparisons, first-value
constants, bare numeric state headers, CMD dividers and scalar Explod scale.
The existing connected combat/entity/asset scenarios retain these cases rather
than adding per-character fixtures. All five segments and existing SFF/font/CNS/
parser/evaluator regressions pass with races. All 12 roster entries compile
1,743 state programs; Linux Arcade Ryu/Dhalsim and Guile/Honda fights run with
input, HUD and audio. Nonfatal expression/resource warnings remain; this is not
full-roster gameplay parity. Keep the partial asset row honest. Scratch was
inspected and removed; shared cache and one ignored Go binary remain.

## SF2 presentation and recovery follow-up — 2026-11-08

The reported stock portrait shrinkage required character-to-motif coordinate
conversion. Keep selection offsets/windows and rendered VS portraits, scale,
facing/windows, old pos/new offset aliases, name alignment/spacing, match text,
fades and background archive ownership. The connected menu flow now checks
actual VS pixels/clipping; its prior checkmark only covered transitions.
Optional 1.0 movement/recovery constants use native defaults, scaled into the
character's units except unitless multipliers. Native 320p/1280p observations
and the connected sweep/bounce/get-up/control flow cover the reported state5101
crash. Legacy projectile-time suffixes bind to typed ID queries. All five
segments pass with races; stock/SF2 screens and 60-second Honda/Dhalsim inspected.

The Ryu/Dhalsim round-two state5900 stall also reproduces in native. Do not add
an engine timeout or automatic state exit to hide that content bug. Preserve
the original pack. tools/sf2_playable.py creates temp/sf2_mugen_playable with all
matching initialization repairs (ten scripts) and small writable config/log
copies; all media is shared by links. --update extends older copies, preserving
configuration and refusing local script edits. The retained copy is updated.
All ten initialization programs exit on round two; Guile/Chun-Li also completed
intro, KO/reset, round-two control and movement in the full match adapter.
RETAIN this ignored working copy for user play, the original pack and shared
cache/single executable. Diagnostic overlays/captures/PCM/helpers were removed.
Broader roster behavior and nonfatal AnimElem/resource warnings remain open.

## Crossover facing — 2026-11-08

The user reports Paintown C++ turning never worked; use supplied Elecbyte runtime
observations and beta CNS as the reference, not that C++ behavior. Command
expressions read the old facing; a controlled ground transition turns before
destination entry expressions/velset. Common idle/crouch/walk turns run before
the positive pass, play action5/6 and retain the state clock. Preserve locked
attack/air headings, NoAutoTurn, stage disable and physical input on direction
rematch without appending another tick. Pause-release commands remain buffered
through the rematch. The existing contact scenario now continues through a
mixed-coordinate crossover/landing/attack/walk lifecycle with native heading,
animation and velocity observations. All segments pass with races. Linux
Ryu/Dhalsim jump-over and continued input were inspected; scratch removed.
Other air-cancel, binding/hitpause and charge-history combinations remain open.

Held guard must not fall through into automatic walking/crouch transitions: the
beta retains Ctrl=1 while common CNS owns guard posture. Keep forward release
and up-back jump available. The connected stock input/contact scenario now
continues through held high/low guard, blocked punch/recovery, release, re-guard
and jump/landing. Changed-source segments pass with races; Linux rebuilt and
visible guard flow inspected. Diagnostic scratch removed.

Explod creation accepts the legacy velocity spelling when vel is absent. The
shipped KFM intro uses it for both incoming wood and broken pieces; ignoring it
left the incoming block stationary offscreen. Native beta and rebuilt Go show
the pre-impact wood in the same observed scene. The existing title-to-match flow
checks actual incoming-block pixels before the kick, break, control and rounds.
All five segments pass with races; Linux rebuilt; diagnostic scratch removed.

Borrowed-state Explods carry their evaluation coordinate width through deferred
admission. Convert position, velocity, acceleration and random displacement into
the issuer's storage units once, then keep the creating state's private-sprite
pixel scale. Sprite/AIR/palette banks remain the issuer's. This matches the beta
mixed-coordinate scene and survives Pause, SelfState and expiry. The old unused
stateEnvironment.CreateExplod adapter is removed; do not reintroduce double
conversion. The connected entity flow covers creation gating, contact, pause,
own-state recovery and round reset. All five segments pass with races; Linux
rebuilt and its marker position/dimensions agree with native. Guard-distance
behavior in the related native observation matched the existing implementation.
Diagnostic scratch removed; no mod scripts or user configuration changed.

Automatic air jumps now select common state45 on a fresh raw up edge (including
upper diagonals), subject to control, airborne posture, minimum height and the
per-flight budget. A held/buffered low press must not activate later at height.
Common CNS owns the start delay, directional velocity and animation. Reset the
budget in S/C before held guard can return early. The existing mixed-coordinate
jump scenario spans denied low/held input, allowed forward air jump, exhausted
budget, landing and a second flight, then its prior body/pause/reset lifecycle.
Four changed-source segments pass with races; state-flow stays checked. Native
and rebuilt Linux input flows finish with two accepted jumps across two flights.
All diagnostic scratch removed. No mod scripts or user configuration changed.

Declared CMD labels are authoritative in Character.Command: after checking
ActiveCommands, return false for a declared but inactive label. Do not let raw
button fallback reactivate a normal press throughout a hold. Preserve explicit
/held, ~release and authored buffer windows. The connected human/helper/AI flow
now spans two held attack/recovery cycles, release/repress and those modifiers.
Four affected segments pass with races; state-flow stays checked. Native and
rebuilt Linux stock KFM each attack once while held, then again after repress.
Diagnostic scratch removed; user configuration and mod scripts unchanged.

Base attack/defence must reach contact damage, including guard and the shared
projectile/helper path. Negative nonabsolute TargetLifeAdd/LifeAdd use the
receiver’s base/runtime/SuperPause damage factor; positive and absolute additions
stay unscaled. Native TargetLifeAdd ignores issuer attack despite its manual.
The existing connected combat flow retains twelve beta observations, recovery
and fresh-round reset; stock CMD rewrites var(1), so instrumentation uses unused
slots. All segments are checked with races; rebuilt Linux observations match.
Diagnostic scratch removed; no mod or user configuration changes.

Compatibility work resumed at the user’s request on 2026-11-08. InGuardDist must
include enemy helpers and live projectiles while their root is idle. Projectile
defaults use the issuer’s size.proj.attack.dist converted into the projectile’s
coordinate profile; explicit guard.dist wins. Removal visuals are not attacks.
Keep the existing mixed-coordinate guard/contact/recovery/reset scenario. Native
and Linux fireball blocking agree; four changed-source segments pass with races.
State-flow stays checked. Shared assets were reused; diagnostic scratch removed.

Confirmed ordinary-bottom fallback now covers position/velocity, life/control/
power updates, state transitions and entry ctrl/velset. Keep omitted attributes
separate from explicit bottom and continue later argument evaluation. Check
Value.RuntimeError before zero coercion: bitwise-not-of-bottom remains a fatal
VM error, with source context and no partly applied vector. The existing combat
scenario covers the connected runtime/recovery path and fatal diagnostics. All
five segments pass with races; rebuilt Linux matches twelve beta observations.
Shared assets reused; diagnostic scratch removed. Other entry/controller
parameters retain their existing policies; do not assume uniform VM coercion.

Animation selection uses the first duplicate AIR action; later blocks are still
validated and included in allocation bounds. ChangeAnim/ChangeAnim2 restart at
element1 for invalid selectors and use ordinary-bottom argument fallback. Keep
Player.Reset strict for clock validation. The connected combat/runtime scenario
and rebuilt Linux match twelve beta observations and recover stand/control.
All five segments pass with races; diagnostic scratch removed.

Hold-only commands ignore buffer.time. /x+y holds the whole chord; ordinary
x+y still requires simultaneous per-button press edges. Held sequence endings
complete on a fresh down transition and retain their normal command buffer.
Preserve human/helper remapping, recovery and subsequent AI in the existing
connected command scenario. Rebuilt Linux matches twelve native keyboard
observations; four affected segments pass with races, state-flow stays checked.
Probe overlays/captures removed; shared assets reused.
