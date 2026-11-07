# Timed HUD native contract — M4dq

M4dq records **23 accepted native programs,34 native runs,528 original
images and6,336 numeric observations**. All images and numeric rows are reviewed
before port changes. It adds no production compatibility percentage by itself. M4dr implementation
and Go comparisons are recorded below.

The programs use invisible roots, nonempty inert death states and a single P2
self-KO at GameTime400. A complete authored fight.def replaces only the selected
MUGEN1 motif's fight definition. Existing stock KO sprite200,0 and enter48 font
provide positive resources; no binary assets are added. The new immutable
hud_frame_reference_probe.py retains authored clipboard output, seeks exactly399
with calibrated owned Pause/ScrollLock events, then captures explicit frames.

| Setting/control | Observed native result |
| --- | --- |
| Sprite/font omitted displaytime | One visible frame at411; absent412. |
| Sprite/animation displaytime0 | No visible frame. |
| Sprite/font displaytime3 | Visible411–413; absent414 onward. |
| Sprite/animation displaytime−1 | Persists through the bounded window; finite animation repeats. |
| Finite animation, omitted displaytime | Frames411/412 at first offset,413/414 at second; absent415. |
| Same animation, explicit7 | Repeats after four ticks; visible411–417; absent418. |
| Explicit LoopStart, durations2+3 | First pass lasts five ticks411–415; absent416. |
| Final infinite element | Changes offset at413, then remains through422. |
| KO.time3, sprite displaytime2 | Visible414/415; absent before414 and after415. |
| Pause/SuperPause issued412, time8/movetime0 | CNS clipboard holds412 through420; animation changes413 and expires415, matching the unpaused control. CNS resumes421. |
| KO slow60, over.hittime0 | CNS passes at401,406,412,418,424…; message starts444 at the eleventh post-KO simulation pass. First sprite444/445, second446/447, absent448 despite skipped CNS passes. |
| over.hittime0/3/10/20 | KO.time0 message starts411 in all four. Do not derive presentation delay from damage grace. |

There are two clocks: the initial KO presentation gate follows simulation passes;
the visible animation and expiry advance on real frames even during Pause,
SuperPause and skipped KO simulation frames. A single unconditional early return
for SimulationSkipped cannot govern both. Static bars and portraits are separate
from timed messages and must not receive the one-frame default.

## Preserved corrections and provenance

The initial seventeen-program window ended412 and missed later expiry boundaries.
Its Pause/SuperPause at400 ended before the message appeared. These original runs
remain intact; longer separately named runs and overlapping412 controls fill the
measurement gaps. An early hypothesis tied411 to over.hittime10; independent
0/3/20 controls disprove that relationship. The extra early hit3 run is redundant
positive confirmation, not a replacement golden. Slowdown initially produced no
visible message in the short window; the later440–455 window measures its onset
and complete expiry. No original sources, captures or tolerances were rewritten.

Independent Go intent programs verify complete controller/state/CMD/AIR/stage
recipes, compile every controller, reconstruct full fight definitions, and verify
positive stock resources before native use. New variants reparse/compile exact
whole-byte deltas against those checked recipes. Native maps pin intent-map JSON;
versioned authoring source resolves to Git43145992, rather than later mutable Go.
Native-used originals remain immutable at their original paths.

All four native execution sessions were awaited completely with observed exit0:
58545(initial17),49723(extended14),53115(extra early hit3),50616(start/slow2).
Separate frozen-input/host-client guards pass after each:2,090;1,788;345;466 path
checks. The three independent intent checks and separate guards pass as well.
The34 runs contain5,736 balanced owned key events. Prepared resources, selected
motif/fight path, full fight replacements, exact config finalization, seeds and
every capture/world hash were checked before one-time source/text promotion.

Public recipes and expectations are hudduration-*,hudtail-* and hudstart-* under
tests/testdata/reference. hudtiming-native-ledger.json pins originals and prepared
paths; hudtiming-native-config-finalization.json pins all34 configurations. PNGs,
reference executable, assets and generated outputs remain private under temp/.

The strict world region[160,140,1120,640] was selected from native-only evidence
before any Go baseline. Later Go failures must remain strict, with no golden or
tolerance changes. Round/Fight/Win-specific delays, variants and phase overlap
need their own positive native controls; this KO corpus does not establish whole
fight-screen/theme/font compatibility. Recheck held startup overlays, prior
render controls and stock input/private PCM before claiming implementation closure.

Continue all allocated item3 after each local checkpoint without approval pauses.
Broader modes, teams, AI, BGM and assets remain item4.


## M4dr — timed messages and strict presentation follow-ups

Timed Round/Fight/KO messages now have separate start, animation and expiry
clocks; static bars retain their own clocks. The measured KO gate follows
simulation passes, while visible animation/lifetime advances on real frames,
including Pause, SuperPause and skipped KO frames. Five previously held overlays
now match their original native world crops exactly. All seven original startup
worlds still match. The mixed shadow case remains468 pixels/max-one strict.

The34 native-derived production replays compare460 world frames:357 exact
(**77.6% of this bounded image corpus**),95 visible sprite frames with max-one
RGB differences, and8 text frames with a one-pixel placement difference. All460
visibility observations and all95 visible sprite bounds match. The exact frames
are pre-start, expired or disabled blank worlds; all103 visible-message frames
remain strict failures. This is timing evidence, not complete renderer/font or
whole-engine compatibility. [Evidence and limits](HUD-TIMING.md).

The full scoped race suite was run at this shared integration boundary:
**5,292 passing test/subtest results,32 failures across36 scoped packages**.
There are23 passing packages,2 failing packages and11 with no tests. Failures are
only the existing negative-camera case/parent and29 new strict HUD runs/parent.
No new state-machine, flow, race or resource failure was observed. All23 actual
applications used the same binary. Nine older bounded renderer controls and
stock win/rematch/win pass, with82 balanced inputs and1,970,452 stereo44.1kHz
private PCM frames. This supersedes the historical M4dl full-suite boundary.

Final68376 was awaited completely with exit1; the separate26,345-input/host-client
guard passes.48 real gates:45zero,3exit1(full suite, mixed shadow, stale inventory).
The exact old inventory generator/report were archived against Git before their
metadata refresh; current write/check passes. Passive provenance/config/resource/
RGB/geometry/source/input/PCM audit passes. All178 runtime/internal non-test Go
files and all186 scoped non-test Go files including8 tools remain byte-identical
to the validated run. No original samplers, regions, expectations or tolerances
changed. Routing stays77/89typed86.5%,zero stubs; no overall completion percentage.

Continue [native Round/Fight/Win phase studies](HUD-PHASE-PLAN.md), then finite/
overlap colors, clipboard/debug display, residual schemas/aliases/flags and prior
strict obligations. Item3 continues autonomously after the local source/text
checkpoint; item4 remains unallocated. No approval pause.

The deterministic production tests run the unchanged native-prepared assets
through Engine.Update, FightState/HUD drawing and viewport projection inside the
public Ebitengine loop. Their frame labels count real ticks; stale paused/slow
clipboard values are not used as frame counters. Normal execution of this GPU
corpus requires the preserved native prepared/capture workspace as well as the
reference distribution; it is not a standalone fixture-only test after cloning.
All460 pairs are represented in12 reviewed full-window thumbnail sheets. Every
world belongs to one of four byte-identical native/Go pair classes, reviewed at
full crop detail:357 blank pairs,63 first-sprite pairs(1,448 max-one differences),
32 second-sprite pairs(1,529 max-one differences),8 text pairs(1,689 pixels/max236).
Text bounds are native[601,198,680,226], Go[600,197,679,225]; cause is unassigned.
All22 actual-application pairs/eight sheets and stock3 captures are reviewed.

The first Go baseline was a harness setup failure: the character DEF path needed
the absolute prepared root. Its exact old test source and separate guard remain
archived. The corrected focused baseline has0/42 exact frames. The first timing
revision has48/68 exact frames; all remaining sprite failures are max-one, and
the one text frame retains the same placement difference. These diagnostic runs
remain intact. No validated production Go changed between that revision and the
full integration run. The system TestMain now runs tests inside the public GPU
loop so readback exercises production drawing.

Inventory audit corrections are retained: the original355 excluded-path hash
manifest includes48 files hidden by Git ignore; visible-untracked alone is307.
Full native fight sidecars retain nine original stock trailing-whitespace lines
each, byte-for-byte. The failed raw whitespace audit and canonical-line evidence
remain private; non-sidecar whitespace checks pass. The final evidence auditor's
first scope assertion expected186 in a map excluding test tools. The corrected
scope verifies178 runtime/internal plus8 non-test tool sources,186 total; no
runtime source or native evidence changed to satisfy it.
