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
Whole-engine parity and broad interaction coverage remain unfinished. The
subsequent round-lifecycle allocation below is also complete. The current
trigger/clock allocation follows it. Prior checkpoints are 8779869a and 24b3ebd0; read HITFALLSET.md,
DOWN-BOUNCE.md and COVERAGE.md, including the corrected idle-override case.
Read `README.md`,
`docs/COVERAGE.md`, `docs/PROGRESS-ASSESSMENT.md` and the current handoff.

## Previous round-lifecycle allocation (complete)

The user approved three more checkpoints after M4bt (2026-10-18):
M4bu KO/time-over clocks (native slowdown, timer freezing, pause interactions,
control removal and win/lose entry); M4bv results/reset (double KO/draws, wins,
match completion, persistence and entity/effect cleanup); M4bw complete-match
validation (actual inputs/damage, multiple rounds, results/rematch, sustained
runs). Proceed through all three with local commits and documentation; stop
only at completion or a concrete issue requiring user input. Keep one-on-one
scope; other modes remain separate. The 27-key allocation above is historical.
Freeze original fixtures/recipes before native captures and never edit them
once observed. Use focused checks while developing and a full scoped race
suite at the final integration boundary. Private native/app runs stay serial.

M4bu is committed as 7fae8ec4; M4bv as ddb48b18. M4bw completes eight
full-life stock matches / 18 rounds through logical/keyboard input, including
natural KOs, time draws and repeated rematches. An owned-window 2%-life replay
also reaches two actual win/rematch screens. The final full scoped race suite
passes: 2,707 test/subtest results, 24 tested and 12 compiled-only packages;
Match 968.597s, total 1,037.599s. Source/config/binary/PNG/private-PCM audits and
root module checks pass; no clients/sessions remain. M4bw is committed as
218e3106. See docs/COMPLETE-MATCHES.md, docs/ROUND-RESULTS.md and docs/HANDOFF.md. Legacy HitVel binding, engine-wide
GameTime handoff and other documented engine gates remain open. Typed HitDef
coverage stays 81/81; no whole-engine completion percentage is justified.
No pushes or binaries.

## Previous trigger/clock allocation (complete)

The user validated M4bw and authorized proceeding. M4bx measures and binds
legacy HitVel X/Y, preserving the three existing counterexamples and adding
frozen native controls for units, heading, hit/guard/air/down contexts and
redirection. M4by separately wires cumulative GameTime through production
menus, matches and rematches. Each checkpoint requires focused regression
checks, updated behavioral limits/evidence and a local source/text commit.
Continue through these two bounded checkpoints unless user input is needed.
Earlier completed allocations are historical. No pushes or new binaries.

M4bx is committed as e1299891: 15 controls / 180 values / 60 frozen inputs, plus three
unchanged projectile counterexamples / 180 integers. Focused Match races
227.327s, Character/System/core races, two fresh native runs and a four-capture
stock input/results/rematch replay pass. Source/PNG/private-PCM/module/native
audits pass. See LEGACY-HITVEL.md and HANDOFF.md. M4by final integration below
also covers this checkpoint. Preserve all native originals and diagnostics.

M4by adds process-owned GameTime, native Config.GameSpeed + 5*Options.GameSpeed
rate, typed TicksPerSecond and bounded Versus MatchNo. Six programs/84 native
values/29 authored input hashes plus System/native-program/keyboard/wrap checks
pass. Final scoped integration PASSED against 408 frozen hashes: 2,735 test/
subtest results, 24 tested and 12 compiled-only packages; Match 956.321s, total
1,028.682s. Fresh native clock and four-PNG stock win/rematch/win with private
PCM pass source/input/config/PNG/executable/audio/root-module audits. All
sessions were awaited; no clients remain. M4by is committed as 0414d54e and was pushed by the user. This allocation is
complete; the current allocation below supersedes its stop condition. Keep renderer/mode/asset/remaining binding gates explicit; no
overall engine completion percentage is justified. No push or binaries.

## Latest binding/recovery allocation (complete)

The user authorized the next few checkpoints after 0414d54e (2026-10-18).
Proceed through three bounded checkpoints: M4bz audits all 127 trigger families
against actual runtime routes, records selector/variant gaps and adds a
reproducible inventory; M4ca implements missing GetHitVar combat metadata and
combo/impact counters with frozen native controls; M4cb implements the remaining
liedown recovery timer/input consumer and corrects recovery routing as supported
by native observations. Split dependent selectors between M4ca/M4cb if needed;
document the reason and keep the original inputs/goldens immutable. Each batch
gets focused checks, evidence/limits and a local source/text commit. Use a full
scoped race run at the final integration boundary, not at every small revision.
M4bz is bd018c65; M4ca is b258eaf3. M4cb completes this group. Final full scoped
races PASS: 2,774 results, 24 tested and 12 compiled-only packages, no individual
skips/failures; Match 1,009.698s, total 1,083.286s. All 470 frozen hashes, fresh
native reproductions, four-PNG stock win/rematch/win, owned events, private PCM
and root-module audits pass. All sessions were awaited; no clients remain.
Read LIEDOWN-RECOVERY.md and HANDOFF.md. Stop for user testing/new allocation;
no further implementation is active. Historical continuation instructions below
applied only until these three checkpoints completed.
Continue automatically through these three; stop at completion or an issue
requiring user input. No pushes, binaries, changes outside game/mugen or agents.

## Current scalar/projectile/reversal allocation

The user tested/pushed d251549d and on 2026-10-19 authorized items 1–2 of the
assessment, with checkpoints and local commits throughout. Proceed automatically
through HitPauseTime, Random/VarRandom, projectile lifecycle/cancellation/contact
queries, then ReversalDef/MoveReversed. Suggested coherent checkpoints: M4cc
HitPauseTime; M4cd random stream/controller; M4ce projectile schema/spawn/motion/
lifetime/ownership; M4cf multi-hit/removal/cancellation; M4cg projectile query
families and legacy forms; M4ch reversals. Split or reorder dependent slices if
native evidence requires it and document the actual boundary. Each gets frozen
native originals, focused checks, evidence/limits and a local source/text commit.
Use full scoped races at meaningful integration boundaries, including final
integration; do not repeat the entire suite for every small correction. Complete
these items without asking for routine approval. Stop after their final checkpoint
or a concrete issue needing user input. Items 3–4 (other controller/effect stubs,
assets/modes/AI/BGM and final renderer/font conformance) await later allocation;
record dependencies rather than silently expanding into those workstreams.
Keep projectile/reversal-specific consumers in scope, with honest variant limits.
No pushes, binaries, sub-agents or authored changes outside game/mugen/.

M4cc is committed as 750f0076. M4cd completes Random/VarRandom: 21 native
programs, 192 values, five rejections and 84 frozen inputs. Core/Match/57-chain
races, fresh seeded sequence/helper reproductions, four-PNG stock win/rematch/
win, 82 balanced owned events/private PCM, 436 frozen hashes and module audits
pass. Read RANDOM.md/HANDOFF.md. Continue M4ce automatically; 42 projspawn-*
programs / 168 hashes are frozen but not yet observed and not in M4cd's commit.
Move basic NumProjID counting into M4ce as needed for native lifetime controls;
keep the remaining query forms in M4cg. No clients/sessions remain at M4cd.

M4ce completes typed projectile setup/motion/lifetime/bounds/root ownership:
104 native programs, 1,192 distinct slots, nine load rejections, 416 authored
hashes and 12 sprite calibrations. Match races PASS 135.338s, core races pass;
fresh tagged native, three actual projectile apps/exact body masks, four-PNG
stock win/rematch/win with 82 owned events/private PCM, 819 frozen hashes and
module audits pass. Read PROJECTILES.md and HANDOFF.md. Continue M4cf contact/
multi-hit/removal/cancellation, including capacity/config and helper birth
ordering, then M4cg queries and M4ch reversals. No sessions/clients remain at
M4ce; no further implementation has started. Full suite at final integration.

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
