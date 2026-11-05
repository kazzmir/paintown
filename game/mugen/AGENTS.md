# MUGEN Go port — active instructions

## Authorization and scope

The user selected **complete remaining effects/controllers (item3)** on
2026-11-04 and repeatedly authorized continuation. Continue autonomously through
ITEM-3-PLAN.md with documented local checkpoints and commits. Do not stop after
two checkpoints. Stop after item3 or a concrete issue needing user input. Item4
(modes/teams/AI/BGM, assets, remaining triggers and final renderer/font/platform
conformance) remains unallocated. Earlier allocation stop conditions are history.

Author changes only under `game/mugen/`. No pushes, binaries, subagents or goals.
Preserve unrelated root work and incoming upstream font/module changes. Commit
source/text only; audit staged paths, UTF-8 content and absence of binary data.
The tracked `mugen.sh` is a Bash launcher. Reference resources and build outputs
remain ignored. New engine files may require explicit `git add -f` because of a
historical root ignore rule; never force-add the whole tree or temp assets.

Target Elecbyte **M.U.G.E.N1.1 beta1**, using immutable `temp/mugen-1.1b1/docs/`
and its stock data as primary reference. C++ `src/mugen/` is incomplete prior art.
Use the scoped mugen.mod/mugen.sum overlay and readonly module flags. Verify
`sha256sum -c game/mugen/temp/upstream-post-merge-module.sha256`.

## Immediate state — M4cx validated, local commit next

No runtime is active. Final chain83328 completed/awaited0 and a separate
client/source guard passed. Native numeric141 programs/1668raw values/1524stored
comparisons/nine rejects/566source-probe hashes. ALL native tables/images were
inspected before implementation/corrections. Every fresh132-app crop is exact:
67,161,600RGB pixels. Full scoped races PASS4193 test/subtest results/36packages
(25tested/11compiled-only), no failures/individual skips; Match1825.419s,total
1929.442s. Four old contact/fall masks pass. Stock win/rematch/win was visually
inspected:82balanced owned events/1,921,557private PCM frames. All15,751 final
source/fixture/launcher inputs and final provenance/module/inventory audits PASS.
Audit:temp/m4cx-final2-evidence-audit.json. Binary036ee456257b01d4ee9d3e45421e4e02acf7fc12765ca68c5e76c601b877c3cc.

AfterImage/AfterImageTime has typed schemas, recording after physics/bounds and
before AIR clocks, retained history and native palette/drawing consumers. Palette
age counts selected enabled entries, ignoring disabled slots but including
clipped enabled entries. Red brightness validates without evaluating; postbright
evaluates without observed draw effect. See AFTERIMAGE.md for defaults, native
quirks, phase/clipboard/authoring diagnostics and exact bounds. Original native
inputs/resources/captures/goldens remain immutable. Failed complete matrix118/120
and its hash-identical validator are preserved; both failures close unchanged in
the fresh132/132 matrix. Old12M4cq and sevenM4cv raster failures remain UNWAIVED.

Next:stage/audit the explicit731-source/text path proposal in
m4cx-proposed-commit-paths.json, commit locally, then CONTINUE ALLitem3. No push.
Source74typed/15fallback/3stubs/9partial/17bounded/60route-only is not an engine
percentage. Latest committed checkpoint is still f428cd5db until the M4cx commit.

Next native research:global trail capacity,drained context/helper lifetime,
palette/drawing/pause variants,then projectile trails/shared shadows. Independent
ignored builders prepare-m4cy-actor-draft.py and prepare-m4cy-projectile-draft.py
are syntax-checked but NOT EXECUTED; they write ignored draft directories only.
Inspect actual CNS/AIR/stage/controller intent before freezing/observing. See
m4cy-native-research-draft.md. Do not implement from speculative notes. Native
results precede Go; keep source/test/probe/fixture inputs frozen during runs.

## Native and validation workflow

1. Freeze separately named original fixtures/recipes before native observation.
   **Never edit/delete/move original native inputs, captures or goldens**, including
   failed diagnostics. Never relax comparisons/tolerances to make a test pass.
2. Inspect an **all-case native-only consumer/layer table before implementation**.
   Do not infer uninspected matrix branches from manual prose. Native beta quirks
   can differ from its manual. Check actual controller kinds per state.
3. Implement typed schemas/defaults/errors, evaluation order and real production
   consumers. Unknown controllers/invalid triggers must not silently succeed.
   Keep file/state/controller/expression context in diagnostics.
4. Test counterexamples/dependencies first. Collect every strict RGB mismatch
   in a batch, retain each public probe's failure status, and fail the complete
   matrix on any mismatch. Never describe a partially passing matrix as exact.
5. Use focused races/apps during revisions and full scoped races at meaningful
   integration boundaries and final allocation closure. Run required checks;
   broaden/repeat only for changes/failures/unresolved concerns. Full races use
   `--check -race -p 1 -count=1 -timeout=40m`.
6. Freeze source/test/probe/fixture/module inputs for each runtime run. **Native,
   actual-app and display-dependent Go runs are strictly serial.** Await every
   exec session, then a separate client guard before edits/another runtime.
   Never edit frozen inputs or the active driver while a run is executing.
7. Record findings, failures, exact limits, tests and remaining work at each local
   checkpoint. Update README, COMPATIBILITY, COVERAGE and HANDOFF as relevant.
   Full stock selection/fight/results/rematch and required private audio remain
   acceptance gates. Source routing and behavioral evidence are separate counts.

## Tools and private environment

Use `rg` / `rg --files --no-ignore` to discover source paths before reading them;
root ignores hide some engine files. Disposable Go packages/builds belong under
`temp/_tmp` (underscore), preventing recursive package discovery. No ordinary Go
experiments anywhere else under temp. Keep the Go module readonly.

Private runtime/Git wrapper from repository root:
`python3 game/mugen/temp/round-task-env.py ...` (elevated tool execution uses the
already authorized wrapper prefix). It checks clients and sets private display,
audio, Go cache/_tmp, scoped modules, Wine32 shims and local dev headers/libs.
Xvfb:97/Pulse are task-owned; metadata is temp/item3-private-servers.json. Check
selective process output before inferring a server died; full listings truncate.
Do not print private audio cookies. Do not restart/kill unrelated user services.

Byte-identical completed private Go app binaries and stock SFF/SND/font copies
may share storage with their paths/content/hash/permissions preserved; no native
resources, screenshots or expectations are candidates. Prior record:
temp/private-go-asset-sharing.json. Keep all diagnostics and evidence intact.

## Remaining allocation and historical record

AfterImage/AfterImageTime variants and projectile trails/shadows; remaining
Explod creation/ownership/remapping/random/angle/trans/shadow/capacity; all
AssertSpecial consumers; GameMakeAnim/MakeDust; clipboard formatting; force
feedback and actual target/device boundary; residual typed/literal schemas,
HitAdd HUD, StateTypeSet and FVar aliases. `-nojoy` alone cannot prove a hardware
limitation. VarSet literal7.9 must reject natively; fix conflicting old unit
inputs to7.9+0.0 expressions without changing native goldens.

Read current HANDOFF and topic notes. Old detailed status is preserved in
`docs/AGENT-STATE-HISTORY-M4CU.md` for historical lookup only; its old stop
conditions are superseded. No engine-completion percentage is currently justified.
