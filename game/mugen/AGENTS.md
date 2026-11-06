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

## Immediate state — M4dg acceptance complete, commit next

M4dg corrects quoted ModifyExplod transparency modes; 20 native programs pass
(19 accepted / one rejection / 228 values), all 19 new app crops exact across
9,667,200 RGB pixels. Focused races: 869 core / 277 selected native results.
Five older palette/order apps, four masks and visually reviewed stock/audio
pass: 82 balanced inputs / 1,972,972 PCM frames. All 24 per-gate exits record zero.
Outer session 20518 became unrecoverable; do not claim its exit was observed.
Separate elevated 37,195-hash/client guard and passive audit PASS. All images
reviewed. See MODIFY-EXPLOD-ENUMS.md and HANDOFF. Commit source/text, then continue.
Latest full suite: M4df e1c07662bbcd94cba06bfdc7569d62bce8effa66, 4,743 results /
36 packages at the preceding source boundary. No active runtime. No reapproval.

Next: 32 unobserved AssertSpecial rev2 drafts, then 16 aged + 20 early system-
effect drafts, clipboard/debug sink, residual schemas/HitAdd HUD and renderer
gates. Finish all independent item 3. Item 4 remains unallocated. Keep ten
reviewed direct-render source sets UNPROMOTED/UNSTAGED. Larger Xvfb:98 metadata
is temp/m4de-large-display-r2/metadata.json; auth private. Original :97/Pulse
30218/30219 unchanged. Default sandbox ps hides host clients; use elevated wrapper.

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
GameMakeAnim and MakeDust remain two stubs. ForceFeedback is native Null-kind,
not a haptics stub. Broader modes/teams/AI/BGM/assets/triggers remain item 4.
