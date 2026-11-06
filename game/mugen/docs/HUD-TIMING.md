# Timed HUD native contract — M4dq

This checkpoint records **23 accepted native programs,34 native runs,528 original
images and6,336 numeric observations**. All images and numeric rows are reviewed
before port changes. It adds no production compatibility percentage: the HUD
implementation and Go comparisons follow this source/text evidence checkpoint.

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
