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

## Immediate state — M4dn source/text commit next; no runtime active

Last local commit is977bee612a98d8085328b7ca05f809296146e150 (M4dm).
M4dn typed MakeDust, ordered actual-position/jitter producer and aggregate round
spacing clock are implemented. Full details/results/corrections: docs/MAKE-DUST.md
and latest docs/HANDOFF.md. Continue ALL item3; no checkpoint approval/pause.

166valid native programs144accepted/22rejects/1728values, eight publicledgers and
configfinalizations. All independent full intents/native runs/separate guards and
native-only rows/images reviewed. Spacing advances underPause/SuperPause; actual
animation freezes. Common120 only; ordinary sharedpool/noNumExplod; unflipped
sprite/facing offsets; helper retirement; measured palette/layer/lifetime bounds.
ClockGameTime-origin+1 resets onRoundState, NOTRoundCall/startActive.

Final82981 ENTIREcompleted/awaited1; SEPARATE elevated21,503outer/21,500matrix
frozen/client guardsPASS. All144apps run;116worldexact80.6%/59,020,800RGB pixels,
28strictfailures UNWAIVED. ALL144pairs/24sheets+threeenlargeddiagnostics VIEWED.
Core890pass; all185productionGo bytes identical. Native357cases354pass/3initial
CameraPos80vs20fail;378testresults373pass5failinclparents. Allold138Game+53pool
PASS. ReplayharnessStageGroundY refreshed/earlyPause respected; no goldenedits.
Old9render controls andstockwin/rematch/win PASS/VIEWED:82balancedkeys and1,973,590
privatePCMframes. All23finalgates realstatuses native/matrix1,others21zero,outer1.
Sourceinventory77typed/12fallback/0stub/9partial/20bounded/60route-only,86.5%routing
ONLY. LatestFULL historicalM4dl5028/36; no newfullsuite claim. Futuremeaningful
sharedintegration mayneed50m ratherthan40m (oldMatch2346.7sec+newnativeprograms).

Passive finalaudit-r292515ENTIRE0 verifies3584original paths,166configs,149common
AIRreplacements,6806nativebinaryresource copies/109canonicalfiles,allseeds/caps/
recomputedRGB/shared154appbinary/input/PCM/productionbytes. Initialpassiveaudit
receipt omitted; completeoriginalreport preserved/exitunclaimed, R2observed0.
CPU source-sprite review12input/clientguardPASS. Rootmodules unchanged. Seeprivate
m4dn-final-evidence-audit-r2,final-visual-review,final1-separate-guard receipts.
No runtime active. Do not rerun completed tests merely to make another checkpoint.

Ready: explicit stage/audit UTF8/noNUL/exact index bytes; ONLY valid166 corpus+16
ledgers/sidecars, source/test/probe and docs. No binary/temp/outside/invalidraw.
Expectedheldraw354=old159+invalid115+camera80; verifyactual. No commit yet.
Then localcommit and CONTINUE noapprovalpause. Neverpush. Do not edit canonical
native resources/oldrecorders/inputs/captures/goldens or relaxstrictcomparisons.

Native21empty5000setup and2continuity-7-28/29unreached observers stayunpromoted.
Promotion abbreviated-exclusion mistake caughtbyfinalcountBEFOREruntime; archived
two derivedJSON/twoledgers unchanged, r3checkedexisting19exactbytes beforecorrect
ledgers. Originalsnativeneverchanged. ActualKOwarning5080not5000, ownAIR9100not
common9996; correction receipts preserve earlierinterpretations. Inert5080Null
is notdeathpose implementation. Do not count failedsetups/missed observers.

16camera-only Null programs duststartup8/startupunit8 are RAW/UNPROMOTED, notdust
coverage; allnative/intent/guards/rows/foursheets complete. Initialraw cameraG1:
stage40->10,-80->-20; local640/1280->40/80 forstage80, thenproperconversionG2.
Disproves20-step/easing. Issue-framePause retains native prioractorposes/positions.
Requireboot/multi-entity publication+pausedpose controls beforecamera/cache edits;
no specializedcue screenmode. Current28worldfailures:7startup/pose,12heldHUD,
2max1colour,one654pix/max8overlap,6finite360–526pix/max32. Enlargedcolourmasks
match silhouette, causeunassigned. NativeCueTTL needs independentdefaults/delays/
loop/infinite/explicit/SuperPause controls. HUDUpdate alreadyclocks duringPause;
Draw lacksdisplaytime/onepass expiry. NoHUD/camera/samplerproductionedit yet.

Next measured sharedpresentation/HUD/finiteoverlap gaps, thenclipboard andremaining
item3 schemas/flags/strictobligations. CLIPBOARD-PLAN.md ispreparationonly. Native
manualPause/ScrollLock framestepping documented, NOTyetexercised; verifyactual
counters in aNEWrecorder beforeusing it. Originaldebugfont94characters/68masks
casealiases; fullglyphobserver mustpreserveequivalences/positions, neveroverwrite
testedclipboard witholdnumeric injector. No clipboardprobes/codeyet. Item4 BGM/
modes/assets/fonts unallocated; finishindependentitem3 beforedependencyquestion.

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
   `--check -race -p 1 -count=1 -timeout=50m`. Required stock selection/fight/
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
GameMakeAnim and MakeDust have bounded native implementations with retained strict gaps.
ForceFeedback is native Null-kind, not a haptics stub. Broader modes/teams/AI/BGM/assets/triggers remain item 4.


