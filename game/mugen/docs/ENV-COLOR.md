# Environment color — M4cu

EnvColor now has typed parameters, a global lifetime and production drawing.
The focused checkpoint acceptance gates pass; this document separates measured beta
behavior from the broader interaction matrix that remains open.

## Measured parameters and clocks

Defaults are time1, RGB255,255,255 and under0. RGB is an exact three-component
integer tuple; time and under are integer scalars. Direct float literals and
unknown triggers reject before controller effects. Runtime integer conversion
truncates, and missing redirects become zero per component. Tagged expressions
pin RGB -> time -> under evaluation (23415), including time0. Nonzero under
values, including2 and-1, preserve actors above a positive color quad.

The beta packs channels as `uint32(r) | uint32(g)<<8 | uint32(b)<<16`, without
component masks. This allows carry/sign propagation instead of saturation:

| Input | Native RGB |
| --- | --- |
| 256,34,56 | 0,35,56 |
| -256,34,56 | 0,255,255 |
| -1,34,56 | 255,255,255 |
| 12,-1,56 | 12,255,255 |
| 12,34,-1 | 12,34,255 |
| -0.9+0.0,34,56 | 0,34,56 |

Positive durations are visible on their issuing frame and expire while CNS
Pause freezes actors. The shared timeline advances once per outer frame, so
nested entity passes cannot consume extra ticks. The current policy also
advances during skipped KO simulation frames; direct native KO/slowdown
interaction remains a separate gate. Zero restores normal drawing. Negative
durations persist with the distinct beta suppression behavior below.

## Native layer behavior

The manual's blanket foreground wording does not describe every measured branch.

| Request | Base stage | Stage foreground | Actors / ordinary owned effects | Sampled HUD |
| --- | --- | --- | --- | --- |
| time0 or expired | Normal | Normal | Normal | Normal |
| Negative time, either under | Black | Normal | Visible | Visible |
| Positive time, under0 | Color | Covered by color | Covered by color | Covered by color |
| Positive time, under nonzero | Color | Normal | Visible | Covered by color |

Negative time, including-1 and-2, suppresses the base stage but draws no color
quad. Its requested RGB is still evaluated. This differs from the documented
indefinitely colored overlay. Positive under quads draw before actors; positive
front quads draw after stage foreground. Ontop Explods remain visible above the
front quad. Unowned foreground cues retain their existing stage order outside
front overlays and draw above the front quad; the wider system-spark matrix
belongs to the remaining dependent controller work.

HUD drawing still runs internally while covered, preserving lazy animation
initialization and resource consumption. Positive EnvColor masks final HUD
fitting; negative durations retain it. The native evidence samples lifebars,
portraits and font pixels. Round-announcement and other HUD-layer combinations
remain separate variants.

The quad uses the ordinary GPU AllPalFX source transform and the existing final
AllPalFX viewport transform. Complex AllPalFX over12,34,56 yields59,35,111. Raw
black from negative-time suppression receives only the final AllPalFX pass and
yields138,225,255. These distinguish the quad from the CPU debug-background byte
path. BGPalFX does not change the quad.

## Evidence and verification

76 frozen native programs provide68 accepted snapshots,816 numeric values,eight
load rejections and305 unique original/probe hashes. Native-only render crops
are200,140..1080,670 (466,400 RGB pixels). Each accepted program also pins37
actor/stage/HUD/clear/outer samples,2,516 total. No Go output determined expected
colors, coordinates or crop hashes. All originals and failed diagnostics remain
unchanged, with no tolerance adjustments.

Later revisions change only fight drawing; the audit verifies
controller/timeline/replay source identity explicitly. All68 final m4cu4
applications match every crop pixel (31,715,200 pixels) and all2,516 color
samples. Ten ordering/global-palette regression apps pass. Stock win/rematch/win
is visually inspected, with82 balanced owned input events and1,972,229 private PCM
frames. No clients remain. The latest full integration remains M4cs3,857
results/36 packages; a new full integration follows the EnvShake boundary.

The read-only `tests/env_color_render_probe.py` checks original source/native
executable/capture, prepared resources, application binary/config/PNG, every
crop pixel and all37 samples. It cannot create expectations. Failed comparisons
retain a nonzero exit status. The private batch collector reports every scene
and fails if any strict comparison fails.

Preserved application history: m4cu1 exposed nine HUD failures despite an exact
world crop; m4cu2 exposed the positive-under foreground exception; the complete
m4cu3 matrix passed66/68 and exposed the two negative-time foreground branches.
All native expectations remain unchanged. The final layer revision follows the
complete native-only table rather than inferring uninspected branches from the
manual. Future checkpoints use this all-case inspection before implementation.

Global round/reset, foreign-state, super-pause/slowdown, mixed formats, hidden
HUD clock variants and system-spark combinations remain explicit interaction
gates. Source routing and these scoped observations are not whole-engine or
platform conformance percentages. Continue EnvShake and all remaining item3.

Completed private Go app stock assets share identical immutable SFF/SND/font bytes
to conserve storage.43,003 paths released approximately5.06GB of distinct inode
storage.

Native files/captures/goldens remain untouched. Earlier verbose agent notes are
archived in AGENT-STATE-HISTORY-M4CU.md; active AGENTS/HANDOFF now contain only
current scope, constraints, evidence and next work, reducing repeated historical
context reads.
