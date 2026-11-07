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
Before every local commit, present the complete change list and wait for the
user's approval. This supersedes earlier authorization to commit automatically.
Keep mugen.yaml local and ignored; document configuration examples in README.md.

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

Remaining: physical CD (.da) audio is unsupported. Exact native color/shadow/
alpha/KO echo and AI policy remain precision work. Turns, mixed-team health/round
budgets and Survival team recovery are calibrated against supplied beta flows.
Licensed Windows fonts are absent; the observable fallback needs a supplied
font for full CJK glyph coverage. Other platform builds are deferred. No pushes
or committed binaries/generated fixtures. See docs/HANDOFF.md.


## Latest camera checkpoint

Camera fitting includes temporary Width edge bars, excluding permanent screen
containment margins. Clip the tension/Width fitting region to the fixed stage
edges before choosing zoom and center; keep a clipped edge anchored when the
zoomout limit cannot fit that region. This does NOT constrain actual actors:
ScreenBound=0 intentionally permits offstage roots/helpers. A bounded beta flow
establishes disabled tracking, offstage tracking and Width-driven fit. The
existing mixed-coordinate camera scenario covers those poses, Pause and reset;
four changed-source segments pass with race detection. Native scratch removed.
Prior startup, cropped modern/legacy drawing and tension/zoom observations remain
covered. Intermediate easing/push and other raster edges remain open. Preserve
local ignored YAML; the standalone music-file CLI is deferred by user request.

## Keyboard checkpoint

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
