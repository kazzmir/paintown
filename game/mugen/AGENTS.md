# MUGEN Go port

## Current authorization

Implementation is authorized. The user approved `docs/PLAN.md` and authorized
local commits at each milestone or coherent checkpoint. Record findings,
validation, and remaining work with each checkpoint. Stage/commit only changes
under `game/mugen/`; preserve unrelated work and never push without instruction.

The user authorized the next two checkpoints after M4bk (915f40d7).
M4bl completes native HitFallSet value/default/raw integer behavior and
records that the beta ignores its documented velocities; focused/core/native/
application checks pass.
M4bm is active and covers down.bounce's schema and native down-hit consumers. Freeze original
text controls before native capture, implement only demonstrated behavior and
record remaining differences. Use focused checks for M4bl and one full scoped
race run at the M4bm integration boundary. Commit each checkpoint locally, then
pause for user testing. Do not push or start a third feature family. Full
recovery-command routing, liedown/mashing clocks, juggle, broad entity/pause/
contact combinations, round clocks and renderer gates remain open. Read
TRIP-RECOVERY.md, FALL-VECTORS.md and COVERAGE.md. Storyboard scaling is
b1b8864c; upstream integration is 99b4605c.
Read `README.md`,
`docs/COVERAGE.md`, `docs/PROGRESS-ASSESSMENT.md` and the current handoff.

## Scope

All project changes for this effort belong under `game/mugen/`. Do not modify
the root Go module, other engines, or existing data trees. Keep working notes,
compatibility evidence, and handoffs in `docs/`.
The explicitly requested upstream merge adds font requirements to root go.mod;
preserve those incoming changes. Continue authoring port changes only here.

The user explicitly prohibits adding binaries to the repository. Commit source,
text fixtures and documentation only; keep compiled executables, generated
binary artifacts and reference resources in ignored `temp/`. The tracked `mugen.sh`
file is a text Bash launcher. Existing binary test resources are historical;
do not add more binary files. Verify staged content before each local commit.

## Compatibility target

Target Elecbyte M.U.G.E.N 1.1 beta 1. The local reference distribution is
`temp/mugen-1.1b1/`: use its `docs/`, `data/`, `chars/`, and `stages/` as the
primary specification and regression corpus. The C++ implementation in
`src/mugen/` is useful prior art, but is incomplete and is not the specification.
Do not modify the reference distribution. Do not commit its binaries/assets.

## Implementation and verification

- On resumption, refresh a concise current behavior-coverage ledger from source,
  existing tests and topic documents before estimating broader work. The old
  inventory status columns remain baseline evidence. Keep percentage metrics
  dated, scoped and linked to their numerator/denominator sources.
- Work in bounded feature batches with explicit acceptance and stopping points.
  Use focused checks during development and full regression suites at meaningful
  integration boundaries; repeat broad checks only for a concrete reason.
  Record native evidence, unresolved differences and the next scope at each
  local checkpoint. Stop at the agreed boundary unless more batches are
  explicitly authorized; token-budget decisions belong to the user.
- A registered controller or a successfully parsed file does not demonstrate
  behavioral compatibility. Track partial and missing behavior honestly.
- Prefer deterministic, display-independent tests for expression and state
  semantics. Add corpus tests for the bundled stock content and malformed files.
- Errors must identify the file/state/controller/expression. Never treat an
  unknown controller or invalid trigger as a successful no-op.
- Exercise the complete selection-to-fight path as well as test applications.
- Run native captures, app smoke captures and audio probes sequentially on the
  private display/audio server. Overlapping windows can invalidate OpenGL
  captures, and other audio output invalidates tone provenance. Pure tests
  without DISPLAY may run alongside those probes.
- Run Go with `GOFLAGS=-mod=readonly` so the root module stays unchanged. Build
  artifacts and disposable tools belong in `temp/`, which is not source.
- Use the launcher or `-modfile=game/mugen/mugen.mod` for the scoped dependency
  overlay; see `docs/BUILD.md`. Keep it synchronized when dependencies change.
- Use non-Go extensions for disposable source snapshots. The recursive check
  also discovers ordinary packages below `temp/`; ignored files still compile.
- Full race checks use `--check -race -timeout=20m`; the growing native Match
  corpus approaches Go's default ten-minute package deadline.
- Update `docs/COMPATIBILITY.md` and `docs/HANDOFF.md` when behavior or verified
  coverage changes. Do not call the port complete without tests and reference
  evidence covering the advertised compatibility.
