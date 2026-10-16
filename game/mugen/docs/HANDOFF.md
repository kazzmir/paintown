# Handoff

Updated: 2026-10-16. State/language, sprite/font, and finite animation checkpoints
are tested. Full compatibility and playable match flow remain unfinished.

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

Next integrate selected character palettes, harden complete resource loading,
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
