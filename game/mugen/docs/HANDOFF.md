# Handoff

Updated: 2026-10-16. State/language, sprite/font, and finite animation checkpoints
are tested. Full compatibility remains unfinished. The production one-on-one fight path is
connected; the complete playable M4 gate is still open.

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

Next finish playable match behavior with deterministic injected input,
then integrate selected character palettes, complete storyboard/rendering semantics,
extend the headless world/state runner, and compare authored
reference fixtures. The 10-transition loop limit is now diagnosed but remains
provisional. State ownership, match flow, font rendering parity, and many controller/world
behaviors are incomplete. Do not call M1 or full M0/M2 complete.

The reference quick match loaded successfully through match-loop initialization
under Wine. The binary identifies itself as 1.1.0 Beta 1 P1 (2013.08.11).
See REFERENCE.md; no Go gameplay parity or full semantic harness is claimed.

## Next action

Continue reference harness work and the typed expression and
simulation foundations plus asset support. Reach the first playable KFM match
at M4 and continue through the complete compatibility gates. Implementation
choices are delegated after approval; ask only about concrete blockers or
unresolvable scope/reference ambiguities. Keep this handoff and the evidence
inventory current.
