# MUGEN Go port — active instructions

## Authorization and scope

The user selected **complete remaining effects/controllers (item 3)** on
2026-11-04 and repeatedly authorized continuation. Continue autonomously through
`docs/ITEM-3-PLAN.md` with documented local checkpoints and commits. Do not stop
at a checkpoint. Stop after item 3 or a concrete issue needing user input. Item 4
(modes, teams, AI, BGM, assets, remaining triggers and final renderer/font/platform
conformance) remains unallocated.

Author changes only under `game/mugen/`. No pushes, binaries, subagents or goals.
Preserve unrelated root work and incoming upstream font/module changes. Audit
staged paths and UTF-8/no-NUL content before every source/text-only commit. New
files may need explicit `git add -f` because of a historical ignore rule; never
force-add a directory or temp assets. `mugen.sh` is a tracked Bash launcher.
Use the scoped readonly mugen.mod/mugen.sum overlay; verify
`sha256sum -c game/mugen/temp/upstream-post-merge-module.sha256`.

Target the supplied Elecbyte **M.U.G.E.N 1.1 beta1** distribution and its docs in
`temp/mugen-1.1b1/`. The incomplete C++ `src/mugen/` is prior art, not authority.

## Immediate state — M4dl source/text checkpoint

No active runtime. Read current bottom HANDOFF and SYSTEM-EFFECTS.md. Continue ALL
remaining item3 autonomously after the source/text commit; no checkpoint pause.

GameMakeAnim typed compiler/common cues/Under pass and sprite-only BG/FG scene
tint are implemented. Original84programs promoted ONCE:75accept9reject900values,
5native/configledgers; all84pass. All101native research rows/18sheets reviewed
before implementation. Final72fixedworld observers70exact /35,616,000pixels;
position-fraction41pixels/max1 and position-single18pixels/max1 UNWAIVED. Three
finite repeats remain dynamic diagnostics. ALL72pairs/24sheets+old9/stock3 reviewed.
Dust17 remains UNPROMOTED/UNSTAGED. Source76typed13fallback1stub9partial19bounded60route-only.

Full82340 completed/awaited1; separate20,046/20,043 guardPASS. All5,028 production
race results pass across36packages (25tested11compiled), Match2346.712s. Original
recursive command also selected36 historical source-snapshot packages, producing
520 failures from missing resources/empty fixture sets/propagated parents. Real
exit1 retained, not called a successful outer run. Other21 gates pass; matrix1
fails ONLY two strict RGB cases. Stock82keys/1,972,096privatePCM and old5palette/
order+4masks pass. Auditor71972 completed/awaited0;5,241originals/84configs/
65commonAIR/41resources/3,444copies/109manifest/source/module guardsPASS.

Launcher --check now filters temp package paths using read-only metadata, leaving
all source snapshots unchanged. Fresh scope80797 completed/awaited0 with exactly
36production packages; separate20,044current frozen/clientguardPASS. Production
Go/test hashes are UNCHANGED from full5028 execution. Only launcher package
selection and inventory annotation/report changed afterward. Exact run-specific
versions retained in temp/_tmp/m4dl-full-run-source, mapped by
m4dl-full-run-source-versions.json; original launcher also matches Gitc88d2a77.
When auditing historical full-run maps, use their recorded versions for these3
paths, not later mutable launcher/inventory paths. Current scope map checks
current files. This is separate from native-original hashes, which are unchanged.

Final evidence: m4dl-final-evidence-audit.json, m4dl-final-visual-review.json,
m4dl-archive-failure-final-review.json and both separate guards. No binary/push/
agent/goal/outside changes. Explicit UTF8/text staging still required. Include
only84promotedGame programs,10ledgers, changed source/docs; exclude Dust17,
old14nokotime/direct10/invalidwalk3 rawsets and all temp assets. The local hash/path audit is recorded in
temp/m4dl-commit-record.json after committing; do not repeat completed validation.

Next measured system-effects boundary: independent GameMakeAnim pool admission,
negative-action clamping and sub-game-pixel coordinate quantization; then MakeDust
schema/global clock/interleaved per-axis RNG/resource/lifetime/layer/ownership/
capacity. Static supplied-executable guidance is documented, NOT native behavior
coverage or permission to change old expectations. New complete intent + native
all-case/image review before promotion/implementation. Existing17Dust originals
stay intact. Continue clipboard, remaining special flags and residual schemas,
then allocated strict rendering/HUD/audio obligations. Item4 remains unallocated.

## Native source provenance exception

For nokovoice-native-ledger.json ONLY,390 versioned authoring Go/parser/unused
probe hashes are verified through nokovoice-authoring-source-archive.json against
BOTH Git9c35b389 blobs and byte-identical private snapshots. Do not compare these
archived versioned hashes against later mutable port source. Native-used
recorders/programs/resources/configs/captures/PCM remain immutable ORIGINAL paths.
Later system-effect ledgers hash the authoring map JSON, not live Go paths.

## Native and validation workflow

1. Freeze separately named source/recipes before native observation. Never edit,
   delete, move or replace original inputs, captures or goldens. Never relax
   comparisons or tolerances. Preserve failed diagnostics too.
2. Verify actual controller kinds and full parameter/state intent, then inspect
   ALL native-only consumer/color/geometry/layer rows and review images BEFORE
   implementation. Manuals guide authoring; native quirks decide behavior.
3. Implement typed schemas, defaults, errors, ordered evaluation and production
   consumers. Unknown controllers/triggers must not silently succeed. Preserve
   source file/state/controller/expression context in diagnostics.
4. Test counterexamples and dependencies first. Keep each strict comparator's
   real exit status and collect all failures. Do not call a partial matrix exact.
5. Focused races/apps during revisions; full scoped races at meaningful shared
   integration boundaries and final item-3 closure. Full races use
   `--check -race -p 1 -count=1 -timeout=40m`. Required stock selection/fight/
   results/rematch and task-owned private PCM checks remain acceptance gates.
6. Native, actual-app and display-dependent Go runs are STRICTLY SERIAL. Await
   every exec session and run a SEPARATE client/frozen-input guard before edits
   or another runtime. Freeze source/test/probe/fixture/module inputs per run.
7. Document findings, failures, limits, tests and remaining work at each local
   commit. Update README, COMPATIBILITY, COVERAGE and HANDOFF as relevant. Source
   routing and behavioral evidence are separate counts; no overall completion
   percentage is justified by routing counts alone.

## Tools and private environment

Use `rg` / `rg --files --no-ignore`; root ignores hide engine source. Disposable
Go experiments belong ONLY under `temp/_tmp/` so recursive discovery skips them.

From repository root use `python3 game/mugen/temp/round-task-env.py ...` with the
already authorized elevated wrapper prefix. It checks live clients and supplies
task-owned Xvfb:97/Pulse, Wine32, local Go cache/modules and development headers.
Private server metadata: `temp/item3-private-servers.json`. Do not print cookies,
restart unrelated services or infer server death from truncated process output.

Byte-identical completed private GO app binaries and stock SFF/SND/font copies
may share storage with paths, bytes, hashes and permissions preserved. Native
resources, screenshots and expectations are never candidates. Retain evidence;
the user reclaimed 144 GiB to allow continuation.

## Remaining allocation

Read docs/HANDOFF.md and docs/ITEM-3-PLAN.md. Prior 12 M4cq, seven M4cv, two M4cy,
60 M4da and six M4de strict renderer failures remain UNWAIVED. Do not change
samplers or tolerances without independent native evidence and old exact controls.
The native VarSet literal 7.9 rejects; fix conflicting unit setup with 7.9+0.0
expressions without changing native goldens. FVar aliases need native acceptance.
GameMakeAnim is under final validation; MakeDust remains the sole stub. ForceFeedback is native Null-kind,
not a haptics stub. Broader modes/teams/AI/BGM/assets/triggers remain item 4.
