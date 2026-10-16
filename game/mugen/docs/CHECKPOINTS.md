# Implementation checkpoints

The user approved PLAN.md on 2026-10-16 and authorized local commits for
milestones/checkpoints. No remote push is authorized. All source, fixtures,
documentation, and tooling stay under `game/mugen/`.

## M3a — SFF v2/v2.01 decoding and sprite reference comparison

Added CPU-only SFF v2/v2.01 decoding for raw indexed/RGB/RGBA, RLE8, RLE5,
LZ5 and PNG8/24/32; ldata/tdata, signed axes, palette/sprite links and palette
identity are retained. Indexed images expose masked and opaque views; true-color
images preserve alpha. Optional palette selection shares immutable pixels and
does not mutate the archive. Linked/cyclic/out-of-range data and malformed
packets fail with context instead of producing empty sprites. Hardened v1 chain,
count, link and PCX validation, and retained transparent color RGB for opaque use.

Validation: all **1376 sprites across all 27 stock SFF archives** decode with
nonempty masked/opaque images. Every one of **281 KFM sprites** matches the
opaque RGBA pixel hash exported by the bundled SFF2PNG v0.2 tool. Hashes and axes
are recorded in `tests/testdata/kfm-sprites.json`; reference art stays untracked.
The bundled SprMaker 2.01beta encoded small original fixtures covering RLE5,
PNG8/24/32 and raw RGB/RGBA absent from the stock corpus. Pixel formulas, input
generator and definition accompany that fixture in `sff/testdata/`.

Full regression/race tests pass. A three-second packet fuzz run completed 121863
executions without failure. Focused packet/link/malformed-PCX tests cover
overlapping/recycled LZ5 references, masking and independent linked metadata.
This closes the initial v2 load blocker, not M3: selected character palettes,
font v2/TTF, sound/path hardening and AIR/render behavior remain unfinished.

## M2c — Compiled core controller parameters

Core movement, variable/range/parent-variable, transition, life/power/control,
angle/multiplier, HitAdd, HitVelSet, VictoryQuote, PosFreeze and RemoveExplod
parameters now compile once and evaluate as typed expressions. Missing required
parameters and malformed expressions fail before any state controller runs.
Bottom numeric parameters produce contextual runtime errors instead of silently
becoming zero. That strict runtime failure policy still needs executable probes.

The bundled CNS warning guidance permits float-to-integer truncation with a
warning. Corrected entry attributes, assignment indices, trigger integer
arguments and redirect IDs to follow that rule; integer-only operators remain
strict. Parent variable expressions evaluate in the helper while writes target
the parent. VarRangeSet evaluates its value once and supports float ranges.
Explicit transition overrides retain presence separately from negative values.
HitVelSet uses enable flags rather than multiplying velocity; the incoming-hit
x-velocity convention remains a combat/reference comparison obligation.

HitAdd now has a separate combo counter, with match-owned credit/reset still
pending. VictoryQuote supports its optional random-selection default, range and
helper restriction; the result screen does not consume it yet. SelfState state
ownership is still incomplete. Many other controllers retain legacy handlers.

Validation: full MUGEN tests pass. Added behavior regressions for expression
side-effect order, int32 overflow, float32 variable arithmetic, parent context,
range defaults/evaluate-once, movement omission/facing, defence/kill handling,
transition overrides, optional defaults and compile/runtime failures. These
expectations are documentation-backed; executable semantic parity is not claimed.

## Reviewed baseline

Assessment, plan, compatibility inventory, and working instructions captured
before engine changes. The original full test target fails with duplicate main
functions; the 11 aggregate parser/config/asset tests pass using external data.
Reference behavior has not yet been measured. The original distribution remains
untouched and is excluded from commits along with temporary build artifacts.

## M0a — Reproducible build and pinned reference baseline

Moved the console physics application into its own package, resolving the
duplicate-main build failure. Existing corpus tests now require the supplied
1.1 beta distribution, without external data paths or skip-on-missing behavior.
Pinned 109 reference content/spec/tool files by size and SHA-256; assets remain
untracked. Added UTF-8 BOM handling for the supplied font definitions and a
regression test. Configuration tests can disable ambient YAML lookup explicitly.
Diagnostics now default to stderr and accept a caller-owned writer instead of
creating a fragile working-directory-relative log file.

The launcher runs entire packages, adds `--check`, keeps build/test temporaries
under `temp/`, and holds the root module read-only. `tests/README.md` describes
the fixture setup and current Ebitengine/display coupling.

Validation: `bash -n game/mugen/mugen` and
`GOPROXY=off game/mugen/mugen --check` pass. The required reference manifest is
verified during the suite, and all 33 stock DEF files parse. No state-machine
compatibility claim is made by this checkpoint.

Reference experiment: an isolated Win32 Wine prefix can start the executable
far enough to write its status log. `mugen.exe -h` did not exit within 40 seconds;
it is not a successful gameplay verification. The log identifies the supplied
binary as **1.1.0 Beta 1 P1 (2013.08.11)**, while the readme labels the distribution
1.1 beta 1 (2013-07-27). Use the supplied, pinned executable as the oracle and
record this distinction in future comparisons. Quick-match startup is being
investigated. Headless semantic fixture execution and strict controller/trigger
diagnostics remain M0/M1/M2 work.

## M1a — Preserve expression source through container parsing

Replaced token reconstruction with a line-oriented container parser retaining
raw expressions and line/column locations. CNS/CMD now consume the preserved
text; Statedef entry expressions are retained separately for runtime compilation.
Added explicit tests for operators, quoted semicolons/commas, nested calls and
intervals, numeric spelling, Windows paths, duplicate triggers, AIR rows, select
options, malformed sections/strings, and reader failures. Required old corpus
checks and all 33 stock DEF files still pass.

Validation: new regressions demonstrated failures with the original parser;
`GOPROXY=off game/mugen/mugen --check` passes after the replacement. This fixes
source fidelity, not expression arithmetic or state execution. Those are next.

The isolated Wine quick match reached both game-loop and match-loop initialization
with the stock character/stage/assets loaded. See REFERENCE.md for the observed
evidence and limits; no Go/reference semantic comparison is claimed yet.

## M1b / M0b foundation — Typed language and headless expression replay

Added distinct int32/float32/bottom values, numeric promotion, integer division,
strict integer bitwise/modulo operations, bounded exponentiation, assignments,
all logical operators, math calls/constants, and bottom propagation. IfElse
evaluates all arguments; Cond evaluates its selected branch. Replaced the
process-global custom evaluator with environment-local optional interfaces for
trigger bindings, redirects, animation queries, and warnings. Existing game
call sites use a numeric facade; preserving bottom through controller parameter
application is still M2 work.

The expression parser now rejects unknown characters, malformed numbers/strings,
invalid assignments/intervals, and excessive nesting instead of silently dropping
tokens. It preserves literal types, implements target operator precedence and
legacy nullary clauses, and parses redirects of a single trigger. The stock
corpus revealed `AnimElem = 3, -1`; the signed legacy argument now has a
regression. All 1522 stock trigger lines parse. This is syntax evidence, not
verification of their world-bound behavior.

Added `internal/exprfixture` and the `tests/expressions` JSONL runner. Six fixtures
produce 15 deterministic observations with kind/value/warnings/variables; typed
results and side effects are asserted. Fixture errors and mismatches exit
nonzero. It uses the production evaluator and works without DISPLAY.

Validation: the full `mugen --check` suite passes. Display-independent race tests
pass for evaluator, parsers, CNS, and the expression runner. A three-second
single-worker expression fuzz run completed 108219 executions without failure.
The mathematical/precedence expectations come from bundled CNS/trigger docs;
numeric edge cases and legacy/redirect behavior still need executable reference
probes. No semantic inventory entry is yet promoted to reference-Verified.

Remaining: complete constant/GetHitVar/world trigger bindings; compile/validate
controller parameters and reject invalid trigger groups; correct state entry,
persistence, pause and ownership; extract a headless state/match core. M1 is not
declared complete by this checkpoint.

## M2a — Headless state executor and compiled trigger groups

Moved the shared executor/handlers into `engine/`, retaining public aliases in
`character/`. No Ebitengine/input/render dependency enters the new core. Trigger
programs compile once from immutable source; synchronized caches allow distinct
hosts to share programs without sharing variables/persistence.

Unknown controller types, malformed expressions, missing trigger1 and invalid
persistent/ignorehitpause literals now fail before executing any controller in
the state, with state/controller/name/type/line context. Compilation errors stay
errors on repeated execution. Added the explicit Null controller. Triggerall and
numbered groups execute in documented order, stop at number gaps, and preserve
repeated-condition AND semantics. Bottom is false at the trigger boundary.
Persistent N counts successful conditions and activates on each Nth success;
one-shot persistence resets on host re-entry. Trigger side effects continue to
evaluate even after a one-shot controller has activated, matching the prior C++
activation structure; executable reference confirmation is still required.

Validation: headless `go test -race ./game/mugen/engine` passes. Regressions cover
trigger ordering/gaps/AND/OR, invalid-state atomic validation, persistence across
intermittent conditions/re-entry, hitpause filtering, self-transition abortion,
Null, and eight concurrent hosts sharing one compiled state.

Remaining: production Character.Update still skips all paused state passes;
state entry is still applied in two places, and handlers still need typed
parameter schemas. These are next, followed by world/entity ownership and the
full state runner. This checkpoint does not declare M2 complete.

## M2b — Single state entry and production hitpause/timer integration

Added compiled entry expressions and an explicit EnterState operation. CNS
defaults now match the documented S/I/N entry defaults and omission sentinels.
U preserves prior enums; omitted control/animation/velocity/priority/juggle stay
unchanged. Entry applies power, juggle, facing and hitdef/movehit/hitcount
persistence, and keeps transition control/animation overrides through Time=0
passes. Entry never runs implicitly from ExecuteState. Character initial entry
and transitions use this same path.

Production state passes now run during hitpause so ignorehitpause can work,
including the last paused tick; hit-stun decrements once per active tick.
Contact counters persist and advance on unpaused ticks, with new-contact serials
preventing immediate extra increments. MoveHitReset clears status rather than
rearming already-hit targets. PosFreeze resets each active tick; Gravity adds
acceleration once per activation. Hitboxes require an active HitDef, and default
state entry disables stale hit definitions. Incoming hits no longer mark the
defender's own attack as contacting.

State/entry failures stop the affected Character and are returned by
Player.Update; the combat application propagates them. Expression warnings are
recorded with player/state context, deduplicated and bounded. Empty states are
rejected. The old silent transition-loop cutoff now records an error; its
10-transition cap is provisional and needs comparison with the reference limit.

Validation: full `mugen --check` and focused engine/character race tests pass.
New regressions cover entry defaults/U/omission/expressions/overrides, tuple
evaluation order, no repeated entry during hitpause, combat persistence flags,
last-paused/first-active ticks, contact counters, one-tick freeze, one-shot
gravity, active-hit gating and stopped execution on missing states.

Reference obligations remain: entry attribute/override evaluation ordering,
negative-state edge cases, animation boundaries, physics/collision phase order
and the transition limit. State ownership and world-level Pause/SuperPause are
still incomplete. Typed controller parameters and animation/constant bindings
are next; these changes are unit/regression-tested, not executable-parity claims.
