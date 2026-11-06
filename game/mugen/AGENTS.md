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

## Immediate state — M4dm validated; commit then continue MakeDust

M4dl remains committed1e71278833c19ccee4d070dde416b008399fa31d. M4dm production now
separates Game admission from ordinary Explods (pending/active/standalone), warns
and resolves negative actions to0, and integerizes with absolute half-game-width/
stage-ground origins before random displacement and later camera subtraction.
No SysExplodMax restriction is guessed. New checked full common-AIR replacement
replay/probe support is separate from the old append recipe. All old goldens,
regions, samplers, tolerances and native inputs remain unchanged.

All54 new native programs accepted/648 values, four independent native/config
ledgers; all11native sheets/rows reviewed before promotion/fix. Native12154,
18510,55813,91360 complete0; separate499/199/159/159original/client guardsPASS.
Native origin +/-half/zero scenes EXACTequal with negativecamera; +fullunit differs
on BOTHaxes. Worldzero and leftedge originals retained; leftedge samebbox doesNOT
mean sameRGB. Visible AIR9999 belongs to BOTHfighters. Perentitynoshadow suppresses
P1shadow only; P2shadow remains. Once-Time8 globalnoshadow resets after pause and
original namedno-shadow cases stillcast shadows. All scope corrections documented.

Baselines57559/63474/59631 complete/guarded, all12pairs reviewed. Revision96964
ENTIREcomplete0; separate20,375frozen/client guardPASS. Core890results/4packages
and205native results (138Game+53oldpool+14parents)PASS. All8 targeted revision
worlds EXACT4,070,400RGBpixels; bothsheets reviewed. Production Go/test/probe bytes
UNCHANGED after revision. Final37394 ENTIREcomplete1; separate20,378outer/20,375
matrix frozen-input/client guardPASS. New39/54worlds EXACT19,843,200RGBpixels;
15strictmaximum-oneRGB failures UNWAIVED. Oldfraction/single failures unchanged
41/18pixels maximum1. All56pairs/14matrixsheets +old9pairs/3sheets +stock3screens
FULLYVIEWED. Old5palette/order+4shake gatesPASS. Stock82balancedkeys,
1,974,462PCMframes/4311fee96f646b78d6f2d0f2d39d85ca29a714f9996f4d15b87eaba3cbe60de2.
All66apps samebinary6cd51c6be3b39fd8f33f1cd094042adc908756f73b405402841ae97c9c4e4a2d.
No binary added to Git. Allpriorrenderer/HUD/echo/heldKOtext obligations UNWAIVED.

Finalinventorygate realexit1 preserved: onlythree changedproduction source SHAs,
classifications identical. AFTERwholeguard exact oldreport archived under
temp/_tmp/m4dm-final-run-source, validatedagainstGit1e,
versionmap m4dm-final-run-source-versions.json. Regeneratedreport/currentinventory
PASS; separate61,128-entry/source/client guardPASS. No validatedsource/test/probe/
fixture/module edits. Passiveaudit7609 ENTIREcomplete0: all6,096original paths,
390Git/archiveversions,138configs,65AIRappendices/54fullreplacements,5,658native
resourcecopies/109canonicalmanifest, seed/capture/measuredRGB/binary/stockPCM/module
checksPASS. Receipts m4dm-final-evidence-audit.json and-final-visual-review.json,
-final1-separate-guard.json and-current-metadata-separate-guard.json. Docs/README/
COVERAGE/COMPAT/CHECKPOINTS/ITEM3/topic/index/test/sourceinventory updated. Source
counts76typed/13fallback/1stub/9partial/19bounded/60route-only,85.4%routingONLY.
Latest FULLrace remains historicalM4dl5,028/36; do not repeat merelyfor this batch.

NEXT explicit UTF8/noNUL/exactindex source/text path audit, LOCALcheckpointcommit
(no push). If gitlog already shows M4dm don't redo commit/checks. Then continueALL
item3: MakeDust17oldnative-only raw controls plus new complete typed/globalspacing/
interleavedaxisRNG/commonresource/facing/geometry/helper/pool/palette/clock consumers.
Detailed unmeasured design in SYSTEM-EFFECTS.md. Static Dust reads double positions
at0x1f8/0x200, unlikeGamefloatpresentation1434/1438+origins; doNOTsharecoordinate
helper without independent native proofs. Then clipboard/residualschemas/aliases/
StateTypeSet/HitAdd/widerflags and strict obligations. No approval/checkpoint pause.
Old179raw (MakeDust17,nokotime14,direct10,invalidwalk3) remain UNPROMOTED/UNSTAGED.
No active runtime. No push/binaries/agents/goals/outside edits. Item4 unallocated.

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

Current staging audit correction: actual held raw count is227, not the abbreviated
179 tracked in the preceding notes. The extra48 are older unpromoted Reversal11
programs (44sidecars) and rejected ExplodBindRuntime1 (four sidecars). All227 are
excluded from this checkpoint and untouched. Staging67857 validated all351 source/
text blobs before its held-count assertion failed; corrected manifest and review
are recorded in m4dm-staging-held-path-correction.json. No production/runtime change.
