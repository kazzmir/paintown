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

Use the readonly mugen.mod/mugen.sum overlay and preserve root module files.
The requested origin/go-rewrite merge at172d3fb02 intentionally updates root
metadata and reconciles the overlay: Go1.26, Ebitengine2.10.5, purego0.11.2.
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


## Latest background checkpoint

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
