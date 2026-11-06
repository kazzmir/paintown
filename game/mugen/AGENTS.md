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

## Immediate state — M4dk complete; continue item 3

M4dj committed9c35b389 (251 source/text paths):44 native/528values/30exact world
crops, full4,918 race results/36packages. M4dk adds10 native voice programs/
120values/36segments. All ten CPU programs pass within889core race results;
NoKO44+4parents48pass. Final33638 completed/awaited1 solely for strict ordinary
KOecho audio comparison:9/10 production cases pass,all six NoKO cases pass.
Other42gates0. Original failure UNWAIVED; no sampler/tolerance/golden changes.
Five native phase shifts at50/10ms did NOT reproduce it; hypothesis unadopted.

All10pairs/fivesheets+old9+stock3 reviewed; oldpalette/order/masks and stockwin/
rematch/win82keys/1,972,873PCMpass. Separate19,478 frozen/clientguardPASS.
Passive auditor57293completed0:3,994original paths,390archive+Git versions,
10configs,41resources/410copies/109manifest. No live runtime. Currenttopic
NO-KO-AUDIO.md; records temp/m4dk-final-evidence-audit.json,final-visual-review,
final1-separate-guard. Localtextcommit record temp/m4dk-commit-record.json.
Latest full4,918/36 remains M4dj source boundary; M4dk focused checks only.

IMPORTANT nokovoice ledger authoring Go/parser/unused probe versions are checked
using nokovoice-authoring-source-archive.json against BOTH original Git9c35b389
and byte-identical private snapshots. Native-used recorders/programs/resources/
configs/captures/PCM still immutable ORIGINAL paths. Do not compare archived
versioned authoring hashes against current port source or change originalledger.
Future native ledgers should separate immutable native inputs from versioned
port/parser authoring dependencies; preserve exact versions before later edits.

Next: remaining special flags actual helper/global/timer/round consumers and
system GameMakeAnim/MakeDust (16aged+20early drafts/helpers prepared NOT observed),
clipboard/debug formatting, VarSet/FVaraliases/StateTypeSet/HitAdd and shared
strict renderer/HUD/audio/presentation failures. Item3 continues without approval;
item4 BGM/modes/assets/fonts/platform remainsunallocated. NoMusic depends on native
BGM evidence; finish independent item3 before concrete allocation question.
Original14nokotime diagnostics,10directrenderer and3invalidwalk rawsets stay
UNPROMOTED/UNSTAGED. All35HUD/priorraster/echo-window/heldKOtext failures UNWAIVED.
Source75typed14fallback2stubs9partial18bounded60route-only is not enginecoverage%.

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
