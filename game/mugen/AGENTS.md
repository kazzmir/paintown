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

## Immediate state — M4dp validated; checkpoint then HUD controls

M4dp source/text checkpoint is ready to commit, or already at HEAD. Previouslocal
checkpoint e8de0b8e415eb7cf8625b41644eae8d589ea7596(M4do). Newproduction ONLY
character/actor_order.go rootdrawStartXY inRoundState0/1(currentanimation),helpers/
projectilesactual,body+shadow shared; match/match.go firstRoundState2 resetsactual
BOTHrootX/Y,retainsanimation/time. No genericcache/phasefreeze or inferredVelocity/
facing/Zreset. INITIAL-PLACEMENT.md hasfullnativeproof, corrections andlimits.

Final16701 ENTIREawaited1; SEPARATE21,476frozen/client guardPASS. ALL40appsrun same
binary8b7cb6ae1a7032d7f229cfee297ca4419f1c091eacecf3175ac4781eb5647d5b. ALL39pairs/
13sheets+stock3 VIEWED. All7originalstartupworlds EXACT3,561,600RGBpixels. New16/23
worlds EXACT7,680,000pixels/69.6%;7strictfailuresUNWAIVED(6HUD incl1mixedshadow,
1pure464pix/max1shadow). All23newnativeprograms/276valuesPASS. Combinednative
481/482cases(506pass/2failincloldnegativecamera parent), additional16matchresults
PASS. Core890PASS atsame185productionGo bytes. Legacy9PASS;stockwin/rematch/win
82balancedkeys +1,928,719stereo44.1kHzPCMframesSHA9fe4a1286f32abe773db6987a95ea7e83583c2dbf2b809a9f8bba40471129df4.
82gates73zero/9exit1(native,7newworlds,inventory). Oldexactinventory generator/report
archivedGit e8 +privatecopies AFTERguard; updateddescriptions/currentcheck0. Source
routing77/89typed86.5%,12fallback/0stub/9partial/20bounded/60route-only unchanged,
not overall compatibility. LatestFULLhistoricalM4dl5028/36; no newfullclaim.

Passiveaudit21612 ENTIREawaited0:710nativeoriginalpath/2553preparedpath checks,
943native/1640Goappbinaryresourcecopies,23exactconfigs,42appcaptures/sharedbinary,
seeds/recomputedRGB/185productionGo/ownedkeys/PCM/sourceversionsPASS. Native author
contexts verifyrecordedGit e8 or originalprivateinputpaths, notmutableGo. Final
m4dp-final-evidence-audit.json,final-visual-review.json,final-metadata.json. No runtime.

ALLnative studies awaited0+SEPARATEguardsPASS andallrows/imagesreviewedbeforecode:
31687/138(5activecontrols55frames660values),72606/158(10phase),66707/158(10context),
76586/126(2boundary),15158/122(1Y). 23stableprogramspromotedONCE aspreintropose10,
posecontext10,poseboundary2,posevertical1;123publicsource/evidencepaths+4native/4config
ledgers. Fiveactivecontrols remainPRIVATE, disprovegenericPausecache/schedulerfix.
Native-usednew frame_seek_reference_probe.py IMMUTABLE. Alloldrecorders/inputs/
resources/configs/caps/goldens unchanged. Firstderivedinitial_placement_render_probe
failedobsoleteconfigschema; preserveoriginal/excludecommit. Successful separately
named initial_placement_compare.py reusedbaselinecaptures withstrictprepared/final
hashes. Baseline98119/guard21475 andpassive27503/guard21508 retained1; all6pairs
reviewed. Revision83510/guard21476 retained1;all6pairsviewed/core890+new24resultspass.
No productionedits afterrevision/final. Originalsamplers/regions/tolerances unchanged.

Complete explicitstagedUTF8/noNUL/exact-index/scopeaudit andLOCALsource/textcommit;
excludeold354raw +failednewderivedcomparator. Neverbinary/temp/outsidepaths orpush.
Then CONTINUEALLitem3 autonomously, NOapprovalpause: HUD-LIFETIME-PLAN.md specifies
new nativeKO400/seek399 +fullfight.defsidecars/positiveSprites-fonts/defaults/delays/
loops/infinite/Pause/SuperPause/KOslow controls BEFOREHUDedits. Nativeprepareconfig
motif verified data/mugen1/system.def; fightloader in fight/definition.go. Existing
HUDUpdate clocksduringCNSPause butDrawignoresdisplaytime/startdelays; skipsKOslow
viaSimulationSkipped. Measurebeforechanging; staticbarTTL andtimedRound separate.
Nextfinite/overlapcolour,clipboard/debugdisplay,residualschemas/aliases/flags and
oldstrictobligations. Item4 BGM/modes/assets/fonts unallocated; finishindependent3
beforeconcretedependencyquestion. Userreclaimed144GiB, no currentdiskblocker.

## M4do camera publication validated

M4do source/text checkpoint is ready to commit, or already committed at HEAD.
Previous checkpoint a868f654bcf14def5b64e608806e1d14af1df742 is M4dn.
Do not rerun completed verification merely to make a checkpoint.

Cold-start X publication now matches all24 camera fields in4 independent boot/
first-two-pass programs.94/96 total fields match; negative80 P2 tick1 screenpos
360vs305 andPos200vs145 stay strict. No constraint-phase fix inferred. All166 dust,
138 Game,53pool,98oldercamera/boundary/width cases PASS: combined458/459 cases,
482pass/2fail test/subtest results (one new case plusparent). Core890PASS.

Final44675 ENTIRE completed/awaited1; SEPARATE21,349 frozen/client guardPASS.
All16apps run samebinary96a0047f577b9ce874799d94af0770611be7ab2143a1c7e1a8e459623c221702.
Six targeted startup worlds stillfail UNWAIVED, all15pairs/5sheets+stock3 VIEWED.
ThreeG1images differ93069/93114/93069 (previous77212/77139/77212); G4same73164.
Native displays prior actor positions during issuingPause; Go draws currentpose.
This does NOTprove later roots skipCNS. Need state/counter/own/opponent probes
before pausedcache/scheduler changes; no specialized cue-screen workaround.
Old9 rendercontrols PASS; stock win/rematch/winPASS82balancedkeys/1,928,880PCMframes.
35gates27zero/eight1(native,sixworlds,inventory). Exact pre-regeneration inventory
script/report archived Git a868 +privatecopies in m4do-camera-final-source-versions;
current inventorywrite/check0. No validated productionGo changes after final.
Passiveaudit0:155originals,109canonical,656appbinaryresources,18appcaps,sharedbinary,
seeds/RGB/185productionGo/input/PCM. m4do-camera-final-evidence-audit.json.

Native Pause/ScrollLock calibrated6277 entire0/separate485 guardPASS/all6frames
reviewed; exact1/2/3steps. R1/R2 intent failures preserved, omittedLoopStart=-1.
Resetframe sequence finalized/separate233 guardPASS/all6reviewed; F4 RESUMES pause,
so NOTexact bootsteps. Outer reset receipt unrecoverable; no exit claimed.
Both new tests/frame_*reference_probe.py recorders are native-used IMMUTABLE.
These calibrations are not controller coverage. Bounded authoring archive below.
Boot native42029 entire0/separate133guardPASS; all96fields/8frames reviewed before
promotion/Go. Initial baseline hash walk finished after test; diagnostic preserved,
R2baseline21501 against completefrozenmap1 before implementation. Attempted hash-
walk cancellation matched itself/exited130; originalwalk96251 finished0 unchanged.
Revision63528 failed fourboot+threeolddust; revision45115 fixeddust, leavesnewbound.

Continue ALL item3: independent screenconstraint and issuing-frame CNS/pose
controls, measuredHUD displaytime/defaults/delays/loops/Pause/SuperPause, finite/
overlapcolour isolation, then clipboard/residualschemas/flags/strictobligations.
STARTUP-CAMERA.md/CLIPBOARD-PLAN.md preserve measured vs proposed scope. No approval
pause. M4dn full144world matrix116exact is HISTORICAL, not rerun inM4do;28prior
failures allunwaived. Older max1/max32 colour causes unassigned; no sampler/tolerance
changes. M4dn invalid21empty5000 +2unreachable observers remain raw, old354paths
excluded. KOwarning5080not5000; ownAIR9999/9100notcommon9996; corrections retained.
LatestFULL historicalM4dl5028/36; currentfocused. Routing77/89typed86.5%,zero stubs,
12fallback/9partial/20bounded/60route-only, not overall compatibility. No runtime.

## Native source provenance exception

For nokovoice-native-ledger.json ONLY,390 versioned authoring Go/parser/unused
probe hashes are verified through nokovoice-authoring-source-archive.json against
BOTH Git9c35b389 blobs and byte-identical private snapshots. Do not compare these
archived versioned hashes against later mutable port source. Native-used
recorders/programs/resources/configs/captures/PCM remain immutable ORIGINAL paths.
Later system-effect ledgers hash the authoring map JSON, not live Go paths.
For m4do-frame-step-control-native-inputs.json ONLY,362 authoring context paths
are archived by m4do-frame-control-authoring-archive-r2.json:361 tracked Go/module
files verified against Git a868 and byte-identical snapshots; untracked root
go.sum verified against upstream-post-merge-module.sha256 and snapshot. Preserve
this exact bounded exception;123 native-used paths remain ORIGINAL live checks.
The failed first archive and corrected scope note remain private. New native maps
hash the authoring-input-map JSON rather than mutable Go authoring context.


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


