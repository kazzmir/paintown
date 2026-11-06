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

## Immediate state — M4cz validated, local checkpoint next

No active runtime. Last committed checkpoint is M4cy3270a56ee8be00d80742c0df9eeceb795c96509d.
M4cz final integration10305 completed/awaited1, with only two UNWAIVED missing-
shadow RGB cases. SEPARATE16,814frozen-input/clientguard PASS. Final evidence
and resource audits pass. Read docs/HANDOFF.md and docs/PROJECTILE-TRAILS.md.

Fresh final source:843engine/character and625old/new native Match results pass
under race detection.154apps run,152crops exact/77,337,600RGBpixels; shadow1/-1
remain294RGB/max32 each, strict matrix exit1 preserved. Four masks and visually
inspected stock21win/26FIGHT rematch/45secondwin,82balanced events/1,918,419PCMframes
pass. Module/inventory checks pass. Source audit counts remain74typed/15fallback,
3stubs/9partial/17bounded/60route-only. Native corpus161original programs,
154accepted/5target schema rejects/2preserved authoring rejects,1848values and646
original source-probe hashes. All161 rows/11sheets inspected before promotion.

NEXT finish scoped UTF8/noNUL staging audit and local M4cz commit, record it in
 temp/m4cz-commit-record.json (check if it already exists), then CONTINUE item3.
No pushes/binaries. Do not stop at checkpoint or ask for routine reapproval.
Docs/evidence are complete; all original inputs, goldens and failure artifacts
remain unchanged. Audit: temp/m4cz-final-evidence-audit.json,
m4cz-reference-resource-audit.json, m4cz-final-stock-visual-review.json.

Next shared shadows: independent temp/m4da-shadow-draft3 has62UNOBSERVED,
UNPROMOTED drafts. FULL CNS/AIR/STAGE intent parser verification is required
before public freezing/native observation. No shadow runtime or renderer work
has started. Inspect ALL native-only rows/images BEFORE implementation. Existing
294pixel shadow crops cannot establish full geometry/alpha/fade/ordering.
Native/app/display-Go always serial; await session and SEPARATEinput/clientguard
before source/test/probe/fixture edits or another runtime. Go experiments ONLY
 temp/_tmp. The incomplete older shadow drafts and all diagnostics are preserved.

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

Finish projectile trails, then shared shadow rendering and remaining Explod
creation/ownership/remapping/random/angle/trans/shadow/capacity. Continue all
AssertSpecial consumers, GameMakeAnim/MakeDust, clipboard formatting, haptics and
actual device/target boundary, residual typed/literal StateTypeSet/VarSet/FVar
schemas and HitAdd HUD. `-nojoy` alone proves no hardware limitation. Native
VarSet literal7.9 rejects; later fix conflicting unit inputs to7.9+0.0 expressions
without changing native goldens. Shared shadows need new fully visible native
controls; M4cz's two nonzero shadow crops expose only294pixels each.

Prior 12 M4cq, seven M4cv and two M4cy nonquarter-angle failures remain UNWAIVED.
M4cy's current-only45degree case differs534RGBpixels/17maskXOR; trail case2285/
65; max148. Actor matrix46/48exact; strict exit1 preserved. M4cy full scoped race
has4243results/36packages, four masks and stock82events/1,920,802PCM frames pass.
Historical status: `docs/AGENT-STATE-HISTORY-M4CU.md`; old stop rules superseded.
