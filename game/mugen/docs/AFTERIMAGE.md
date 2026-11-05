# AfterImage / AfterImageTime — M4cx

The implementation now has typed parameter compilation, entity history and real
trail drawing, based on Elecbyte M.U.G.E.N 1.1 beta1 observations. The completed checkpoint is a bounded implementation, not whole-family
compatibility.

## Evidence and comparison

The three public native ledgers contain **141 programs:132 accepted and nine
rejected**. They retain1,668 captured numeric values;1,524 compare against final
Go storage.566 original source/probe hashes cover every CNS/AIR/CMD/stage input
and the two native recorder scripts. All native case tables and images were
inspected before their corresponding implementation or correction. Native inputs,
resources, captures and original expectations remain unchanged.

The incomplete C++ engine uses a length-1 history, but passes raw history indices
into palette aging and applies palpostbright. Its draw selection also starts at a
different offset. Those formulas do not establish beta compatibility. The original
Go methods were stubs; the native controls determine the new consumer. No C++
source is changed. See src/mugen/character.cpp around2776,3497 and3923.

The first study used stock full-body sprite0,0. Later controls isolate stock KFM
portrait9000,0 in AIR9100, disable velocity/bounds/camera motion and separate the
recorded poses. At PosX160 that portrait is completely off the right viewport
edge; the blank current sprite is expected and does not justify asset changes.
The strict world crop is508,800pixels per accepted program and excludes clipboard
text. All its native clipboard pages have identical world crops.

## Parameters and observed beta quirks

Defaults are time1,length20,timegap1,framegap4,palcolor256,palinvertall0,
palbright30,30,30,palcontrast120,120,220,palpostbright0,0,0,paladd10,10,25,
palmul.65,.65,.75 and ordinary transparency. Nonpositive length,timegap and
framegap select their defaults. The documented length maximum60 is applied;
the short length61 observation alone does not establish the long-history cap.

Evaluation order is time,length,timegap,framegap,palcolor,palinvertall,
palbright.G/B,palcontrast.R/G/B,palpostbright.R/G/B,paladd.R/G/B,palmul.R/G/B.
Red brightness is parsed and validated, but never evaluated and remains30.
Palpostbright evaluates but has no observed draw effect. Literal floats in
integer fields and unknown triggers reject with source/state/controller context.
Unknown or quoted trans strings load and select ordinary drawing; unquoted
add,add1 and sub select the corresponding shared blend paths.

AfterImageTime chooses time over value regardless of declaration order. Only
the selected field is parsed; an unused unknown/float value alias is ignored.
Missing duration selects0. Reissuing AfterImage clears the history and restores
omitted defaults; modifying its time preserves retained history and can restart
recording before that history drains. Restart after complete drainage is a
remaining lifetime/capacity control, not an established native behavior.

## Recording and palette age

Recording occurs after movement and bounds correction, before the AIR animation
clock advances. The history holds length-1 slots, including disabled entries.
Sampling occurs immediately and then once per timegap ticks. Finite time
decrements before capture:time0/1 records no enabled entries,time2 records the
first frame,time3 the first two. Minus1 records indefinitely;minus2 records none.

Frame selection starts at history age framegap-1, then2*framegap-1 and so on.
Palette age counts only **selected, enabled entries**, newest first. Disabled
slots do not age the palette; enabled entries still count when clipped offscreen.
Drawing proceeds oldest first. This distinction matters for framegap>1, finite
recording, stopping and resuming through disabled history slots.

Each palette age n adds n*paladd. Its multiplicative factor is first quantized as
trunc(256*pow(palmul,n)); contrast then quantizes as trunc(contrast*factor/256).
Color/grayscale and inversion precede brightness and the shared GPU blend path.
For source RGB49,49,49 the default palette gives37,37,68 at n0,27,27,67 at n1,
and19,19,62 at n2. The independent all-color default study explains34,272 opaque
portrait pixels across eight ages. The final application matrix verifies GPU
rounding instead of substituting a CPU preview for production rendering.

The tested aligned AfterImageTime calls with timegap2 preserve positions. They
did not show the manual's claimed position reset; other phase combinations remain
open rather than inheriting an unobserved reset rule.

## Preserved diagnostics and validation history

Six original24-field controls omitted their final positive clipboard publisher.
Their negative-state clipboard predates final VarSets and freezes during Pause.
Original values and images are retained as render-only diagnostics. Short Time7,
short Time8 and long Time7 controls independently establish real storage/order;
the stale values are not compared against final Go variables.

Two velocity-labelled phase controls retain zero velocity; separate motion
controls explicitly use30,0. The bounds-body authoring label uses portrait AIR;
the original full-body pilot remains the actual body counterexample. Accurate
promotion notes preserve these original labels and bytes without inferring
behavior from them.

The first8 Go applications passed six crops and exposed recording-phase and
palette-quantization errors. Both were corrected from native controls. The first
complete120-app matrix passed118 crops and retained two palette-age failures:
4,284 and41,679 colored pixels, with zero silhouette differences. Its failure
status, captures and hash-identical validator are preserved. Twelve new native
aging controls preceded the selected/enabled aging correction. All141 native
replays and nine focused application crops now pass, including both old failures.

The fresh132-app matrix passes every crop exactly:67,161,600 RGB pixels.
Full scoped races pass4,193 test/subtest results/36 packages (25tested and
11compiled-only), no failures or individual skips; Match1825.419s. Four old
contact/fall render masks and the visually inspected stock win/rematch/win pass.
All82 owned input events balance; private PCM contains1,921,557 frames. All
15,751 final source/fixture/launcher inputs and native/resource/binary/config/
PNG/audio/module/inventory guards pass. Final audit:
temp/m4cx-final2-evidence-audit.json. Source/text only is committed locally.
No expectation or comparison tolerance was relaxed to obtain a passing result.

## Remaining consumers and limits

AfterImageMax1/4/32/64 produced the same single-entity trail in M4cx. The
separate [M4cy actor context study](AFTERIMAGE-ACTORS.md) now measures shared
capacity, multiple owners, drained contexts, helper destruction, palette/drawing
changes and Pause/SuperPause budgets. The complete 48-case matrix has 46 exact crops and two unwaived shared
45-degree renderer failures; full races and stock/audio integration pass. Broader
combinations remain bounded.
Projectile afterimage fields and shared shadows are still separate consumers.
These continue under item3; source routing does not close their behavior gates.

The earlier12 angle/raster and seven EnvShake precision failures remain unwaived.
Nothing in this checkpoint establishes an overall engine coverage percentage.
