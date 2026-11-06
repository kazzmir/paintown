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

## Immediate state — M4dj complete; continue item 3

M4di committed80e076127e8c33c00358b44dcbe06aeca3621f03 (321scoped UTF8 paths).
M4dj acceptance complete:44native/528values/44configs,30exact world crops/
15,264,000RGBpixels,full4,918results/36packages(25tested11compiled-only),
Match2294.916s. Entire32712 completed/awaited0,all23gates0. Five oldpalette/order
apps,four masks and stock21win/26FIGHTrematch/45secondwin reviewed;82balancedkeys/
1,973,087privatePCMframes. ALL30pairs/sixsheets+old9+stock3 reviewed. Separate
final19445/matrix19442/revision19444/nested19442input/client guardPASS. Auditor
38573completed0:19,448unique/3,506originals/44configs/41resources/1,804copies/
109manifest. No live runtime. Records temp/m4dj-final-evidence-audit.json,
m4dj-final-visual-review.json,m4dj-final1-separate-guard.json. Localcommit record
will be temp/m4dj-commit-record.json; source/text only,no binary/push.

NO-KO.md records current-pass global NoKO, perentity nonreviving deathlatch,
Alive latched insideCNS/deathcommit afterownCNS+damage, frozenretention,
selfKO5030 gating/missing5030 retainedattributes, corpsecontactduringdamagegrace,
protectedLife0lethalmetadata/no powerfordead, andpausedmovingownerKO. No guessed
selfKOfall! Native sixentrycontrols prove originalfall1camefromlatercontact.
KOaudio leftunchanged pending independent nativePCM. All44promoted ONCE; original
14GameTime diagnostics UNPROMOTED/UNSTAGED, plus10directrenderer/3oldwalkrawsets.
OriginalinvalidPause60/99999scope explicitlycorrected byseparate60/60controls.
Allsource/native/recorders/configs/captures/goldensimmutable. Prior35HUD/allstrict
raster failures UNWAIVED; no tolerances or regions changed. No wholeengine%.

Next: independently measure native NoKO KOvoice/channel/echo behavior, then
remaining AssertSpecial/helper/global/timer/round/music consumers; systemeffects
GameMakeAnim/MakeDust (36unobserved drafts/helpers), clipboard/debug formatting,
VarSet/FVaraliases/StateTypeSet/HitAdd schemas/consumers and strict renderer gates.
See HANDOFF.md for details. Continue until allallocated item3complete or concrete
userdependency; item4BGM/modes/assets/font/platform remainsunallocated.

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
