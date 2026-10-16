# Handoff

Updated: 2026-10-16. Status: M0a baseline checkpoint implemented and tested.

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

Reference help timed out under isolated Wine, but wrote a log identifying the
binary as 1.1.0 Beta 1 P1 (2013.08.11). A bounded quick match is being checked.
No gameplay parity or complete M0 semantic harness has been verified yet.

## Next action

Continue reference harness work and the typed expression and
simulation foundations plus asset support. Reach the first playable KFM match
at M4 and continue through the complete compatibility gates. Implementation
choices are delegated after approval; ask only about concrete blockers or
unresolvable scope/reference ambiguities. Keep this handoff and the evidence
inventory current.
