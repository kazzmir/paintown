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

## Immediate state — M4df validated; local checkpoint next

No active runtimes. M4de364916511e3e46e36bc650491a95e9b99624cb18 committed255UTF8
source/text paths. M4df revision1 passes53native programs (45accept8reject540values),
869core/236selectednative focused races andALL45new apps exact/22,896,000RGBpixels.
Full scoped races PASS4,743results/36packages (25tested11compiled-only), Match
2211.607seconds. Five oldpalette/order, quarterturn,fourmasks,stock82inputs/
1,971,594PCMframes PASS; all images reviewed. Old45 still534RGB/max148 UNWAIVED.

Focused34141 complete0; final57254 complete1 solelyold45strictcomparison. SEPARATE
18495focused and18496final input/clientguardsPASS. Audit38432/review80547 complete0;
18498currentfrozenpaths/73973hashchecks/2036originalinputs/53configs/41archives/
2173nativecopies/109manifestfilesPASS. Records temp/m4df-final-evidence-audit.json
andm4df-final-visual-review.json. Read HANDOFF andEXPLOD-CAPACITY-RANDOM.md.

Typed random tuple/ordered component draws, world-axis offsets, globalroot/helper/
orphan/live+queuedcue admission andcorrectExplodMaxclamp8 binding implemented.
Hitsparks share slots butdrawNO RNG; Explodsdraw2evenzero. --match-seed supports
strict reproducibleactualapps. No source changes since passing integration,
except MD. Local source/text checkpoint next; then continue ALL item3. Keep ten
unpromoted direct native raw sets unstaged. No automatic pause/reapproval.

Prepared but UNOBSERVED next20ModifyExplodenum,32AssertSpecial(rev2),16aged+20early
system-effect drafts underignoredtemp; fullintent/native-only review required.
Remaining clipboard/debugsink andresidualschemas/HitAddHUD continue. Item4unallocated.
Prior strict renderer failures stayUNWAIVED; no sampler/tolerance changes. Source
74typed15fallback2stubs9partial18bounded60route-only isnotbehavioralcoverage.
Task-owned Xvfb98large-displaymetadata temp/m4de-large-display-r2/metadata.json;
authprivate. Original97/Pulse30218/30219 unchanged. Default sandboxps hideshost
clients; useelevatedwrapperguards. See HANDOFF for retained failed/lost-session
records; never claim observed exits that were not actually observed.

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
