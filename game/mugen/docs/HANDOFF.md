# Current handoff — M4cv validation

The user authorized **all item3 effects/controllers**, documented local commits
and continued iteration. Item4 is unallocated. No pushes/binaries/agents/goals
or authoring outside `game/mugen/`. See ../AGENTS.md for active constraints.
Historical snapshots are in AGENT-STATE-HISTORY-M4CU.md and
AGENT-STATE-HISTORY-M4CV.md, including superseded hypotheses; do not treat those
as current instructions.

## Immediate state

**M4cw frequency fix ACCEPTED; local commit next.** No runtime active. Native
8530finished/awaited0, separate49hash/clientguardPASS, all12table rows inspected
before implementation/promotion.12accepted/144values. SharedparserABSfrequency,
exactzero=>.001, tiny nonzero retained, defaultphase after normalization. Baseline
66979failedexpected oldfreq.001/phase0 vsnative90/90; parser verifiedM4cvcommit.
No oldnativegolden/source/capture changes. Read SHAKE-FREQUENCY.md.

Acceptance94455finished/awaited0; separateclient/sourceguardPASS15,035inputs.
CoreCNS1.011/Engine1.458/Character11.376/System9.431s;145nativeprograms pass152
results93.361s. Fouroldcontact/fallrender masks exact; stockwin/rematch/win
visuallyinspected82balanced events/1,915,680PCM. FinalauditPASS:
temp/m4cw-final-evidence-audit.json. Allsource/native/resource/binary/config/PNG/
audio/module/inventory guards pass. Currentappbinary:
764e2b505918cee8898ff0ba639beed990bc66a0dfeb73bf9aa888379b112f63.
LatestfullM4cv4,033/36 predatesparserfix; focusedchecks accepted dependency.

Next auditedlocalcommit, separateidleguard, execute prepared prep-m4cx-pilot.py
(12AfterImagefirstlookcontrols), run generated run-m4cx-pilot.py, await/guard,
inspect ALLnative table/images BEFORE implementation. NoAfterImagefixturesyet.
ContinueALLitem3; no checkpointstop. Originalm4cwauthor ambiguitycaughtbefore
native,24existingpaths preservedunchanged/separatecorrectedauthorcompleted;
recordm4cw-author-partial-inputs.json. Native originals always immutable.

M4cv **99a5f52c8** committed383UTF8scopedtextpaths,no binaries/push. Full4033/36,
Match1736.103s,14,998frozeninputs verified. Finalaudit:m4cv-final-evidence-audit.json.
GeneratedinventoryHUDsourcehash refreshed afterfull; code unchanged, exception
recorded.38/45RGBcrops exact,1,727/1,755samples;sevenprofile252/876worldpixelerrors
(max2steps)/four1-stepHUDsamples each stay UNWAIVED;12priorM4cq rasterfailsalso.
All10regressions/stock82events/1,975,603PCM pass. See ENV-SHAKE.md.

## Native EnvShake evidence and implementation

All five native batches65107/21338/49307/82206/64771 finished/awaited with separate
guards. Inputs/metadata:temp/m4cv-{native,extra,clock,layer,min}-{inputs,metadata}.json.
72 programs/289 unique original+probe hashes;66 accepted/792 values/six rejects.
Original authored files/captures/promoted JSONs are immutable. ALL72 native-only
rows inspected before Go implementation in m4cv-native-behavior-table.json.
45 static393,040-pixel crops/39samples each (1,755);21 phase-dependent scenes
remain numeric diagnostics without static goldens, selected from native only.

Typed engine/env_shake.go routes in parameters.go; defaults time0/amplraw-4/
freq60/phase0. Integer time/ampl literals; runtime ampl float32 fractions persist.
Ordertime/ampl/phase/freq1234 eveninactive. Frequencyabs;exactzero=>.001, tiny
nonzero magnitudes persist; omittedphase90 after normalizedfreq>=90. Character
converts state-owner amplitude once to320camera units. Timeline queues exposure
atframeend;positive duration consumes issuingtick. Nonpositive clears earlier
pending requests but preserves current;time1 replaces current with expired0.
Pause/SuperPause advance global phase. Contact/fall policy unchanged.

FightState world still shakes/exposes ordinary black gaps. Positive EnvColor
gets a fixedviewportGPUquad; AllPalFX-only/rawblack receives finalpalette pass
in gaps. Allsix composition scenes are byte-exact in first matrix. HUD stays
fixed and now consumes AIR alpha/Blend. Native stockAS208D48 panel used to be
opaque gray; now transparency is correct, precision bounds remain explicit.

Separate native1280x960 calibration fixtureviewport-profile640-source exists
(four authored source files) with m4cv-viewport-inputs.json. Capture clipped to
1280x947 and clipboard extraction failed, so no evidence.json; diagnostic ONLY,
not promoted accepted observations. Preserve capture/logs/source. CPU native-only
interpolation models fail remaining rounding; no sampler/golden change justified.

M4cw independently resolves the contact/fall negative/tiny frequency audit;
read SHAKE-FREQUENCY.md. All original expectations stay unchanged.

## Committed checkpoints and known limits

M4cu **b8654fca3**, EnvColor:76 native/68 accepted/816values/eight rejects/305hashes;
68 exact crops31,715,200pixels/2,516samples. Core/189native races,ten ordering/
palette apps and visually inspected stock82balanced events/1,972,229PCM pass.
All final provenance/module/inventory guards pass;402UTF8 scoped text paths
committed, no binaries/push. Audit:m4cu-final-evidence-audit.json. Read ENV-COLOR.md.

M4ct **a048effa3**, actor/owned-effect ordering:16native/192values/65hashes;
16exact151,800pixel crops and allfour original M4cs overlaps now pass unchanged
expectations. Core/99effect+14actor native races,four regressions and stock
82events/1,965,839PCM pass. Audit:m4ct-final-evidence-audit.json.

Previous full scoped integration was **M4cs cac8c6957**,3,857results/36packages
(25tested/11compiled-only),Match1606.364s,total1691.569s,14,164frozen inputs.
It predates M4ct/M4cu/M4cv; current full is M4cv4,033/36 above.
M4cq's12 fractional/nonquarter raster counterexamples remain UNWAIVED.
Source routing72typed/17fallback/5stubs/9partial/15bounded/60route-only is not
whole-engine coverage; no family marked variant-complete.

## Private environment and storage

Use temp/round-task-env.py with authorized elevated prefix, private DISPLAY:97/
Xauthority/softwareGL/Pulse/ALSA, local Wine32 shims/devheaders, readonly scoped
mugen.mod/sum, GOCACHE=temp/go-build/GOTMPDIR=temp/_tmp. Runtime strictly serial.
Never print private audio cookie. Root module guard:upstream-post-merge-module.sha256.
No unrelated root/untracked/module changes belong in commits. New engine files
may require explicit force-add; stage exact source/text paths only.

Completed private Go app assets share43,003 identical paths (~5.06GB) plus6,549
(~776MB), preserving byte/hash/path/permissions. Records:private-go-asset-sharing.json
and private-go-asset-sharing-m4cu.json. Do not rerun original output names.
Only completed Go assets/binaries can share identical immutable content; native
resources/captures/goldens never candidates. Disk last3.6GBfree before revision2.
Use a new preflight/log if another independent completed-Go sharing pass is needed.

Remaining item3:AfterImage/Time and projectiletrails/shadows; Explod creation/
ownership/remap/random/angles/shadows/capacity; all AssertSpecial consumers;
GameMakeAnim/MakeDust; clipboard formatting; haptics/device boundary; residual
schemas/literal validation/StateTypeSet/VarSet/FVar aliases/HitAdd HUD. See
ITEM-3-PLAN.md. Item4 modes/AI/BGM/assets/final renderer/font/platform remains
unallocated; controller-specific gaps remain documented unwaived dependencies.

Storagepass85843 completed/awaited:3,737more completedGoappassets shared,442,783,495uniqueinodebytesreleased;native/captures/goldens/frozeninputs untouched.
Record:temp/private-go-asset-sharing-m4cv.json;3.27GBfreeafterpass. Do notrerunexistingoutputnames.
