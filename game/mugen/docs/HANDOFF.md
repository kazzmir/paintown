# Current handoff — M4cu active

The user authorized continuing **all item3 effects/controllers**, with local
checkpoint commits and documentation; no two-checkpoint stop remains. Item4
is unallocated. No pushes, binaries, agents, goals or authoring outside
`game/mugen/`. Current constraints are in ../AGENTS.md. Historical working notes
are archived in AGENT-STATE-HISTORY-M4CU.md; they are not current instructions.

## Immediate state

**M4cu acceptance COMPLETE; commit the source/text checkpoint locally.**
All68 final m4cu4 applications pass31,715,200 exact RGB pixels and2,516 samples.
Ten old ordering/palette regressions and visually inspected stock win/rematch/win
pass,82 balanced events/1,972,229 private PCM frames. All source/native/resource/
binary/config/PNG/audio/module/inventory guards pass. All exec sessions57289,
44293,46817,23233 and99214 completed/awaited; separate client guards pass, no clients.
Final audit:temp/m4cu-final-evidence-audit.json. Stock review:m4cu-stock-review.png.
Source snapshot:m4cu4-check-inputs.json; regression snapshot:m4cu-regression-inputs.json.
Native expectations and all failed m4cu1/2/3 diagnostics remain unchanged.

Stage only explicitly scoped UTF-8 source/text (new engine files need `git add -f`),
audit no binaries/outside changes, commit, then continue43 prepared EnvShake
controls and all remaining item3. No pushes or new permission step needed.

## EnvColor implementation/evidence

Read ENV-COLOR.md.76 native programs:68 accepted,816 numeric values,eight load
rejects,305 unique source/probe hashes,2,516 RGB samples. Native manifests:
`m4cu-{native,extra,boundary}-inputs/metadata.json`; captures:`m4cu-envcolor-*`.
All three native batches87036/13521/70876 were awaited with separate guards.
Originals/captures/goldens remain immutable. `m4cu-native-layer-table.json`
inspects every accepted native consumer before the final layer revision.

Typed `engine/env_color.go` evaluates RGB -> time -> under (tag23415); defaults
white/time1/under0. Unmasked uint32 packing r|g<<8|b<<16 propagates carry/sign
bits, not clamping. Integer literals reject floats, runtime conversion truncates,
missing redirects become zero. Global lifetime continues through CNSPause;
issuing frame remains visible; zero cancels and negatives persist.

Base stage is suppressed for nonzero time. Negative durations draw no quad,
retain stage foreground/actors/HUD and expose raw black (only final AllPalFX).
Positive under draws the GPU color before actors, retaining stage foreground;
positive front draws it after stage foreground and before foreground effects.
Ontop remains above it. Positive quads receive source+final AllPalFX passes;
BGPalFX does not affect them. Positive EnvColor covers the sampled HUD for either
under flag. HUD Draw still runs offscreen to preserve lazy clocks/resources;
final HUD fitting is masked instead. No full HUD-layer/round/format conformance.

Passing core:Engine1.450/Character11.338/System9.430s.189 selected native programs
pass123.968s. Only system/fight_state.go changed after that core/native run;
final audit verifies controller/timeline/replay identity explicitly. Subsequent
System races pass; final9.365s. Preserve diagnostic m4cu1 (world exact/HUD fail),
m4cu2 (positive-under foreground fail),m4cu3 (66/68 exact; two negative foreground
fails). All original comparisons are unchanged. M4cu4 tests all five foreground
cases first, then remaining accepted programs on the same build.

## Latest committed / integration boundary

M4ct **a048effa3**: interleaved native actor/owned-effect priority ordering.
16 native programs/192 values/65 original+probe hashes;16 exact151,800-pixel
crops (2,428,800 pixels), all four unchanged M4cs overlap crops fixed. Core,
99 effect/global and14 actor-order native races, four palette/raster regression
apps and visually inspected stock win/rematch/win82 balanced events/1,965,839
PCM frames pass. Audit:m4ct-final-evidence-audit.json. Staged97 UTF-8 source/text
paths; no binaries/outside changes or push. Read EFFECT-ORDERING.md.

Latest **full** integration:M4cs cac8c6957,3,857 results/36 packages (25 tested,
11 compiled-only),Match1606.364s,total1691.569s,14,164 frozen inputs. It predates
M4ct/M4cu; do not relabel it as a current full run. M4cs palette/GPU evidence is
in GLOBAL-PALETTE-FX.md; its four ordering failures are now closed by M4ct.
The12 M4cq fractional/nonquarter raster counterexamples remain unwaived separately
(ANGLES-OFFSET.md). Current source audit:71 typed/18 fallback,6 stubs,9 partial,
14 bounded,60 route-only; no family variant-complete or overall engine percentage.

## Continue after the EnvColor local commit

Next `temp/prep-m4cv.py` is **prepared only, not executed/frozen**. It authors43
EnvShake programs:required time/schema/defaults/runtime floats/order, six
ScreenPosY frame traces, cancellation/replacement, Pause/SuperPause, profiles
and helpers. Inspect before execution. Freeze, run native serially, inspect an
all-case native-only behavior table, then implement typed requests through the
existing timeline shake consumer with actual render checks. Manual defaults
must be measured independently from prior HitDef/FallEnvShake behavior.

Then continue remaining creation/palette/random/shadow/capacity/afterimage/
special/debug/dust/haptics/schema consumers in ITEM-3-PLAN.md. Native VarSet
literal-float rejection and alias/version gaps are explicitly queued. Broader
round/foreign/pause/hidden-HUD/system-spark combinations remain interaction gates.

## Private host and storage

Use `temp/round-task-env.py` via the authorized elevated prefix. DISPLAY:97,
private Xauthority/software GL/Pulse/ALSA, scoped modules readonly, Go cache and
`temp/_tmp`, local Wine32 shims and eight dev-header packages are configured.
Servers metadata:item3-private-servers.json. No private cookie output. Runtime
and source mutation stay serial. Module guard:upstream-post-merge-module.sha256.

Completed private Go app assets share43,003 identical paths (about5.06GB unique
inode storage saved), preserving bytes/hash/permissions and leaving native files,
captures and goldens untouched. Record:private-go-asset-sharing.json. Do not
rerun that script without a new output-name preflight; its original log exists.
Private app binaries are also shared by equal verified hashes. No binaries are
tracked. Preserve unrelated root files/modules/untracked work.
