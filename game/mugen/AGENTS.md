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

## Current state and boundaries

The ledger has 89 controller routes, 127 trigger routes and 32/34 implemented
project rows. All five connected segments are checked with race detection;
Linux is rebuilt. These are scoped checks, not arbitrary native conformance.
The two partial rows cover broader asset/font/media acceptance and native AI/
rules/fidelity combinations. Physical CD audio, absent licensed/CJK fonts and
other platforms are deferred. Standalone music-file CLI playback was deferred
by the user. Do not turn unknown permutations into mandatory test matrices.

Use supplied Elecbyte runtime observations and beta docs/CNS as the reference.
Paintown C++ turning is not normative. Original SF2 later-round state5900 stalls
also reproduce in native: do not add an engine timeout or repair more mod scripts.
Retain temp/sf2_mugen_playable and its existing ten-script repair tool/copy,
original SF2 pack/ZIP/executable, canonical temp/mugen-1.1b1, shared cache and the
single ignored temp/bin/mugen-go binary. Other old run banks stay removed.

## Implementation invariants

- Keep first character/motif Info/Files, CNS constants, Statedef attributes,
  persistent settings and ordinary controller parameters. Preserve repeated
  trigger conditions and legacy StateTypeSet's second Type parameter.
  Duplicate AIR actions keep the first lookup but still undergo validation.
- Localize complete groups with English fallback, original source lines and
  stable scene order. Commands/states and individual BG elements are exempt.
  Standalone AIR stays undecorated; preserve runtime victory-quote banks.
  Motif-relative select.def precedes shared data.
- CMD defaults apply globally; first settings/remaps/command attributes win.
  Keep time >=1, buffer 1–30, nonempty full-character command sets and original
  resource context. A disabled remap stays disabled; remaps read simultaneous
  physical snapshots. Declared command labels cannot fall back to held buttons.
- /x+y holds the entire chord; normal chords require button press edges. Held
  sequence endings complete on a fresh edge. Preserve release/charge windows.
  Histories size lazily from CMD durations; AI uses timed input steps, not one
  allocated frame per charge tick. Player/helper buffers remain independent.
- YAML key alternatives apply to menus and gameplay; omitted actions inherit,
  lists replace, and none/disabled/[] disable. Keep KeyA(0), shared-key ownership
  and value-copy isolation. User YAML remains ignored and untouched.
- Variable destination/range admission precedes value evaluation. Invalid IDs
  and invalid/reversed ranges are no-ops; valid ranges evaluate once. Target
  controllers check existence then ID, then apply numeric fallback; TargetBind
  checks ID before time/pos.
  ParentVarSet/Add require a live parent before evaluating their arguments.
- Helper bank admission precedes expressions. Forward deferred creation through
  stateEnvironment: convert position once after admission, leave size overrides
  in helper units, and retain issuer profile/banks/state programs. DestroySelf
  parameters still evaluate for roots in native; do not generalize no-op gates.
- Confirmed ordinary-bottom zero fallback covers movement, life/control/power,
  transitions/entry, animations, variables/ranges, Pause/SuperPause, protection,
  sound/palette commands and calibrated control/effect/combat scalars. Preserve
  omitted defaults and later argument evaluation. Real VM failures remain fatal
  with source context and no partly applied vector/power/pause effects. Other
  parameter policies require evidence; do not blanket-convert controller values.
- Command reads see the old heading; controlled ground destination entry sees
  corrected facing/velocity. Walk/crouch turns use action5/6 without resetting
  Time. Preserve locked attack/air headings, NoAutoTurn, stage disable and paused
  commands; rematch current input without appending another history tick.
- Held guard keeps Ctrl=1 and its clock; it must not fall into walking/crouching
  each pass. Forward release and up-back jump remain available. InGuardDist
  includes enemy helpers/live projectiles after their owner goes idle; removal
  art is not an attack. Preserve explicit guard.dist and profile conversion.
- Common state45 owns air-jump startup/motion/animation. Require a fresh raw up
  edge, control, height and budget; low held input cannot activate later. Reset
  the per-flight budget in S/C before held guard can return early.
- Base attack/defence reach contact damage. Negative nonabsolute LifeAdd and
  TargetLifeAdd use receiver defence, including SuperPause; healing/absolute
  additions stay unscaled. Nonzero negative scaled additions have a one-point
  minimum; explicit zero stays zero. Native TargetLifeAdd ignores issuer attack.
- Hit-paused binding anchors place victims but transfer zero velocity. Preserve
  self-binding/Pause allowances, expiry, recovery and fresh round identities.
  PosFreeze suppresses velocity movement, not late body push. Keep pre-CNS
  clipping and vertical Clsn2 gating; projectile-owner corner push is excluded.
- Borrowed Explods retain creating-state geometry/private-pixel units and issuer
  banks through deferred admission, Pause/SelfState/expiry. Convert storage once;
  do not restore the obsolete extra CreateExplod adapter. Legacy velocity is an
  alias for vel, keeping KFM's incoming intro blocks visible.
- Keep ZIP expansions engine-owned: stop audio readers before deletion, close
  title music before retiring packs, and clean startup errors/signals. Reuse
  canonical media by links in scenarios. Portraits decode selectively; gameplay
  loads the bank. Keep resource bounds/context and corrected-content reloads.
- Preserve PCX padding/shared-palette handling, SFF pixel budgets/link charging,
  commented FNT maps and explicit glyph coordinates. TrueType uses the resource
  resolver and reported fallback. Lazy sprite caches share pixels, while fighter
  and HUD effects stay separate; palettes use MappingRevision for outfit changes.
- Retain linked/animated BG clocks and geometry, moving floors/reflections and
  camera return based on the unclipped participant midpoint. Storyboards retain
  clearcolor=-1 canvases and compose once per tick. OnTop/HUD/global PalFX order
  and window/subimage origins stay intact.
- Preserve indexed 3/8 game-pixel calibration without grid snapping, byte
  premultiplication, nearest indexed/linear true-color sampling and delta-first
  round-to-even interpolation. Keep AfterImage AIR clocks and saved mappings.
  Existing bounded RGB/KO-echo acceptance is not whole-renderer/audio parity.
- Audio uses consumed cursors, shared WAV decoding and reusable Linux codec
  buffers. Retain SFX two-stage quantization and existing KO-echo scheduling.

Details and native observations remain in docs/STATE-MACHINE-FEATURES.json,
PROGRESS.md and Git history. Keep future handoffs short; do not append a full
checkpoint narrative to this file. Verify root modules with
`sha256sum -c game/mugen/temp/upstream-post-merge-module.sha256`.
