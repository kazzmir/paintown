# MUGEN Go port

## Current authorization

Implementation is authorized. The user approved `docs/PLAN.md` and authorized
local commits at each milestone or coherent checkpoint. Record findings,
validation, and remaining work with each checkpoint. Stage/commit only changes
under `game/mugen/`; preserve unrelated work and never push without instruction.

The user authorized completion of all 27 remaining documented HitDef keys
from the M4bm audit, with local checkpoint commits and continuous iteration
until complete or blocked. No two-checkpoint stop remains. Planned batches:
M4bn guard.dist (1), M4bo literal enums/flags (9), M4bp juggle/chains (3),
M4bq minimum/maximum/snap constraints (3), M4br hit palette effects (3),
M4bs contact shake (4), M4bt fall shake (4). Adjust boundaries if native
findings require it; document dependencies and actual behavioral limits.
Numeric expression routing and literal schema coverage are separate counts:
there are 54 expression keys initially, 18 remaining numeric and nine literal
keys. Do not force literal fields through numeric evaluation to report 81/81.
Acceptance for each batch requires typed schemas/defaults/errors, demonstrated
runtime consumers or measured beta limitations, frozen native controls,
focused regression checks and updated handoff/ledger. Use full scoped races
at the enum/combat integration boundary and final integration, broadening only
for failures or unresolved concerns. Commit each coherent batch locally;
continue automatically. Never push or add binaries. Other engine/mode/asset/
renderer gates remain outside this HitDef allocation unless a key depends on
them. All seven batches M4bn through M4bt passed their gates. The 27-key allocation
is complete: 72 numeric schemas plus nine literal schemas cover all 81 documented
HitDef keys, with bounded native consumers. The final full scoped race suite
passes (Match 942.741s), including post-M4bp corrections. All final native/app/
render/provenance checks pass. See FALL-SHAKE.md, HITDEF-KEYS.json and HANDOFF.md.
Whole-engine parity and broad interaction coverage remain unfinished. No next
code batch is active after this allocation. Prior checkpoints are 8779869a and 24b3ebd0; read HITFALLSET.md,
DOWN-BOUNCE.md and COVERAGE.md, including the corrected idle-override case.
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
