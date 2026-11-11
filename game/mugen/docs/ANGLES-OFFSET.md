# Actor angle, scale and display offsets — M4cq

AngleSet/Add/Mul now update the persistent native float32 angle register.
AngleDraw enables rotation and scale for the current active frame. Offset changes
the current frame's drawing position without moving collision/simulation axes.
These fields previously had no player-rendering consumer.

## Measured state and evaluation

Native beta accepts omitted AngleSet/Add/Mul values and uses zero, including for
multiplication. AngleDraw's omitted value preserves the angle register; explicit
values update it. Its exact -MaxFloat32 sentinel also preserves the register.
AngleSet/Add/Mul alone do not enable drawing. A later angle update changes an
already enabled frame: AngleDraw then AngleSet90 renders at90. The register
survives active frames; drawing activation/scales expire each active frame and
frozen worlds retain the displayed frame.

AngleDraw defaults scale to1,1 and multiplies scales cumulatively within a frame.
A partial scale keeps the missing component's default1. Its evaluation order is
value, scaleX, scaleY. Angle addition/multiplication rounds stored results to
float32, independently of the renderer's trigonometry. Positive angles rotate
counterclockwise when facing right and reverse direction when facing left.

Offset evaluates X then Y. Omitted axes preserve an earlier same-frame offset;
a new active frame starts at0,0. X is screen-oriented even when facing left.
The separate size.draw.offset constant follows facing in X. Both offsets add to
the drawing axis after angle/scale, so AngleDraw scale does not multiply either
translation. Local profile conversion and fractional offsets retain their own
coordinate boundary. Collision boxes and Pos X/Y remain unchanged.

Numeric expressions validate at load time, including unknown trigger rejection.
Missing redirects become zero at evaluation. Three-component scale tuples reject.
Both integer and float scalar values are accepted for the float drawing fields.
Typed programs retain filename/state/controller diagnostics.

## Evidence and setup diagnostics

The promoted corpus has62 programs:58 accepted/696 numeric readings and four
controller load rejections.278 unique hashes pin authored originals, the stock
constant source and the unchanged reference probe. Whole native RGB crop hashes
and white masks/bounds are recorded under drawing_render. The original region160,110 to1120,690 remains a broad diagnostic: it includes
native debug text and the final viewport border. It is not an isolated actor
RGB gate. An independently captured empty native world and its matching Go app
calibrate the separate region200,140 to1080,670, excluding those overlays/borders.
actor-drawing-crops.json pins58 native-only crop hashes plus the unchanged
original JSON/capture hashes and empty-world calibration. No original golden,
region or tolerance was edited. This bounded crop does not claim a full frame
or every clipped actor pixel. No Go image was used to generate these goldens.

Seven initial constant-offset fixtures had an incomplete Data block and failed
before their controllers loaded. They remain unpromoted diagnostics under
actordraw-extra- constant*. The separate actordraw-constants-* controls use full
stock constants and all seven capture successfully. No original input or native
capture was changed.

Read-only native disassembly corroborates these controls: AngleDraw parser
4466ce/runtime44a37f, float32 angle storage at14bc and cumulative scale149c/14a0;
Offset runtime449883 stores converted double offsets. Frame reset43c575 retains
the angle register while clearing activation/scales/offsets. These addresses are
supporting evidence, not a replacement for the captured programs.

## Verification

The initial62-program Match races pass40.770s; core/GPU races pass before the
final sampling/loader revisions. Final-source full scoped races pass3,703 test/subtest results across25 tested
and11 compiled-only packages, with no failures or individual skips. Match takes
1490.670s; summed package time1547.931s. All13,467 frozen source/fixture/module
hashes pass. This integrates the sampling/BOM changes and all earlier item3 work.
HANDOFF.md records final stock/provenance and continuation details.

All58 final drawing applications use one binary.46 RGB crops match exactly
(21,454,400 pixels), and54 white masks match exactly. Empty native/Go scene
calibration also matches. The strict full matrix intentionally FAILS for these
12 unwaived raster counterexamples; no per-pixel allowance is applied:

| Native fixture suffix | RGB pixels differing | White-mask XOR | Max channel error |
| --- | ---: | ---: | ---: |
| add-after | 3032 | 110 | 171 |
| angle-45-left | 2986 | 106 | 148 |
| angle-45 | 3060 | 110 | 168 |
| angle-90point5 | 514 | 1 | 142 |
| constants-scale | 876 | 0 | 2 |
| draw-twice | 1626 | 0 | 1 |
| local1280 | 180 | 0 | 1 |
| local480 | 1189 | 0 | 2 |
| offset-fraction-left | 951 | 0 | 1 |
| offset-fraction | 1703 | 0 | 2 |
| scale-fraction | 1022 | 0 | 2 |
| scale-half | 876 | 0 | 2 |

Quarter turns, including mirrored facing, now match native RGB exactly. Indexed
source addressing follows each local axis's dominant screen direction, and the
flipped-axis pixel correction remains in screen coordinates after rotation.
A36-candidate private GPU grid reproduces the production error and isolates the
correct mirrored-quarter-turn model. Inverse projection was tried, made180 worse
and was reverted. m4cq1/2/3 outputs remain diagnostic; final apps/audit use m4cq4.
The four nonquarter masks and eight fractional RGB precision cases remain final
raster-conformance gates. This checkpoint consumes controller state/geometry;
it does not advertise full framebuffer compatibility.

The full-constant native resources expose a BOM following the preparer's leading
newline. Go originally rejected that accepted native arrangement. The parser now
accepts a BOM before its first section after leading blank lines, with a targeted
regression.

```bash
python3 game/mugen/tests/drawing_render_probe.py \
  --fixture actordraw-stored --go-run YOUR_APP_RUN --name YOUR_CHECK
```

Use --native-run when the prepared native run has a nondefault name. The same
probe accepts actortrans fixtures. --original-region rechecks the preserved
wider diagnostic; it does not replace its golden with the calibrated crop. Native/app/display-dependent Go runs remain
serial. Final raster conformance, broader foreign-state/helper/pause/RGBA
combinations and all platform backends retain explicit evidence boundaries.

Final stock win/rematch/win is visually checked;82 balanced owned events and
1,995,181 private PCM frames pass. Sessions were awaited and separate client
checks are empty. Continue remaining item3; source counts67 typed/22 fallback/ 10
stubs are not an overall engine percentage.
